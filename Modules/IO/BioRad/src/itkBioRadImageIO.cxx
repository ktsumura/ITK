/*=========================================================================
 *
 *  Copyright NumFOCUS
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         https://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
/*=========================================================================
 *
 *  Portions of this file are subject to the VTK Toolkit Version 3 copyright.
 *
 *  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
 *
 *  For complete copyright, license and disclaimer of warranty information
 *  please refer to the NOTICE file at the top of the ITK source tree.
 *
 *=========================================================================*/
#include "itkBioRadImageIO.h"
#include "itkByteSwapper.h"
#include "itksys/SystemTools.hxx"
#include "itkMakeUniqueForOverwrite.h"

#define BIORAD_HEADER_LENGTH 76
// #define BIORAD_NOTE_LENGTH 96
// #define BIORAD_NOTE_LENGTH 80
// #define BIORAD_RGB_LUT_LENGTH 768
#define BIORAD_FILE_ID_OFFSET 54
#define BIORAD_MAGIC_NUMBER 12345

namespace itk
{
union Aligned4ByteUnion
{
  float localFloatMagFactor;
  int   localIntNoteNext;
  char  localChar4Array[4];
};

struct bioradheader
{
  unsigned short nx, ny; // 0   2*2  image width and height in
  // pixels
  unsigned short npic;                 // 4   2    number of images in file
  unsigned short ramp1_min, ramp1_max; // 6   2*2  LUT1 ramp min. and max.
  char           notes[4];             // 10  4    no notes=0; has notes=non
  // zero
  short          byte_format;          // 14  2    bytes=TRUE(1); words=FALSE(0)
  short          image_number;         // 16  2    image number within file
  char           filename[32];         // 18  32   file name
  short          merged;               // 50  2    merged format
  unsigned short color1;               // 52  2    LUT1 color status
  unsigned short file_id;              // 54  2    valid .PIC file=12345
  unsigned short ramp2_min, ramp2_max; // 56  2*2  LUT2 ramp min. and max.
  unsigned short color2;               // 60  2    LUT2 color status
  short          edited;               // 62  2    image has been edited=TRUE(1)
  short          lens;                 // 64  2    Integer part of lens
  // magnification
  char          mag_factor[4]; // 66  4    4 byte real mag. factor (old ver.)
  unsigned char reserved[6];   // 70  6    NOT USED (old ver.=real lens mag.)
};

enum biorad_notestatus
{
  NOTE_STATUS_ALL = 0x0100,
  NOTE_STATUS_DISPLAY = 0x0200,
  NOTE_STATUS_POSITION = 0x0400
};

enum biorad_notetype
{
  NOTE_TYPE_LIVE = 1,       // info about live collection
  NOTE_TYPE_FILE1 = 2,      // note from image #1
  NOTE_TYPE_NUMBER = 3,     // number in multiple image file
  NOTE_TYPE_USER = 4,       // user notes generated notes
  NOTE_TYPE_LINE = 5,       // line mode info
  NOTE_TYPE_COLLECT = 6,    // collect mode info
  NOTE_TYPE_FILE2 = 7,      // notes from image #2
  NOTE_TYPE_SCALEBAR = 8,   // scale bar info
  NOTE_TYPE_MERGE = 9,      // # merge info
  NOTE_TYPE_THRUVIEW = 10,  // # thruview info
  NOTE_TYPE_ARROW = 11,     // arrow info
  NOTE_TYPE_VARIABLE = 20,  // internal variable
  NOTE_TYPE_STRUCTURE = 21, // again internal variable, as a
                            // structure.
  NOTE_TYPE_4D_SERIES = 22  // 4D acquisition information
};

struct bioradnote
{
  short level;    //  0 level of note -- no longer
                  //    used
  char next[4];   //  2 indicates there is a note
                  //    after this one
  short num;      //  6 image number for the display
                  //    of this note
  short status;   //  8 one of NOTE_STATUS_ALL,
                  //    NOTE_STATUS_DISPLAY,
                  //    NOTE_STATUS_POSITION
  short type;     // 10 type code for note
  short x;        // 12 x coordinate for note
  short y;        // 14 y coordinate for note
  char  text[80]; // 16 info, maybe not null terminated
};

BioRadImageIO::BioRadImageIO()
{
  this->SetNumberOfDimensions(3);
  m_PixelType = IOPixelEnum::SCALAR;
  m_ComponentType = IOComponentEnum::UCHAR;
  m_ByteOrder = IOByteOrderEnum::LittleEndian;
  m_FileType = IOFileEnum::Binary;
  m_NumberOfComponents = 1; // default
  this->AddSupportedWriteExtension(".pic");
  this->AddSupportedReadExtension(".PIC");
  this->AddSupportedReadExtension(".pic");
}

BioRadImageIO::~BioRadImageIO() = default;

// This method will only test if the header looks like a
// BioRad image file.
bool
BioRadImageIO::CanReadFile(const char * filename)
{
  std::ifstream     file;
  const std::string fname(filename);

  if (fname.empty())
  {
    itkDebugMacro("No filename specified.");
    return false;
  }


  const bool extensionFound = this->HasSupportedReadExtension(filename, false);

  if (!extensionFound)
  {
    itkDebugMacro("The filename extension is not recognized");
    return false;
  }

  try
  {
    this->OpenFileForReading(file, fname);
  }
  catch (const ExceptionObject &)
  {
    return false;
  }

  // Check to see if its a BioRad file
  file.seekg(BIORAD_FILE_ID_OFFSET, std::ios::beg);

  unsigned short file_id = 0;
  file.read(reinterpret_cast<char *>(&file_id), 2);
  ByteSwapper<unsigned short>::SwapFromSystemToLittleEndian(&file_id);

  itkDebugMacro("Magic number: " << file_id);

  file.close();
  return file_id == BIORAD_MAGIC_NUMBER;
}

void
BioRadImageIO::Read(void * buffer)
{
  std::ifstream file;

  // read header information file:
  this->OpenFileForReading(file, m_FileName);
  file.seekg(BIORAD_HEADER_LENGTH, std::ios::beg);

  if (!this->ReadBufferAsBinary(file, buffer, this->GetImageSizeInBytes()))
  {
    itkExceptionMacro("Read failed: Wanted " << this->GetImageSizeInBytes() << " bytes, but read " << file.gcount()
                                             << " bytes.");
  }

  // byte swapping depending on pixel type:
  if (this->GetComponentType() == IOComponentEnum::USHORT)
  {
    ByteSwapper<unsigned short>::SwapRangeFromSystemToLittleEndian(
      reinterpret_cast<unsigned short *>(buffer), static_cast<SizeValueType>(this->GetImageSizeInComponents()));
  }

  // closing file:
  file.close();
}

void
BioRadImageIO::InternalReadImageInformation(std::ifstream & file)
{
  // read .pic file (header)
  this->OpenFileForReading(file, m_FileName);

  // Find info...
  bioradheader h;
  if constexpr (sizeof(h) != BIORAD_HEADER_LENGTH)
  {
    itkExceptionMacro("Problem of alignment on your platform");
  }
  file.seekg(0, std::ios::beg);
  bioradheader * p = &h;
  file.read(reinterpret_cast<char *>(p), BIORAD_HEADER_LENGTH);

  // byteswap header fields
  ByteSwapper<unsigned short>::SwapFromSystemToLittleEndian(&h.nx);
  ByteSwapper<unsigned short>::SwapFromSystemToLittleEndian(&h.ny);
  ByteSwapper<unsigned short>::SwapFromSystemToLittleEndian(&h.npic);
  ByteSwapper<unsigned short>::SwapFromSystemToLittleEndian(&h.ramp1_min);
  ByteSwapper<unsigned short>::SwapFromSystemToLittleEndian(&h.ramp1_max);
  ByteSwapper<short>::SwapFromSystemToLittleEndian(&h.byte_format);
  ByteSwapper<short>::SwapFromSystemToLittleEndian(&h.image_number);
  ByteSwapper<short>::SwapFromSystemToLittleEndian(&h.image_number);
  ByteSwapper<short>::SwapFromSystemToLittleEndian(&h.merged);
  ByteSwapper<unsigned short>::SwapFromSystemToLittleEndian(&h.color1);
  ByteSwapper<unsigned short>::SwapFromSystemToLittleEndian(&h.file_id);
  ByteSwapper<unsigned short>::SwapFromSystemToLittleEndian(&h.ramp2_min);
  ByteSwapper<unsigned short>::SwapFromSystemToLittleEndian(&h.ramp2_max);
  ByteSwapper<unsigned short>::SwapFromSystemToLittleEndian(&h.color2);
  ByteSwapper<short>::SwapFromSystemToLittleEndian(&h.edited);
  ByteSwapper<short>::SwapFromSystemToLittleEndian(&h.lens);
  Aligned4ByteUnion localMagFactor;
  memcpy(localMagFactor.localChar4Array, h.mag_factor, 4);
  ByteSwapper<float>::SwapFromSystemToLittleEndian(&(localMagFactor.localFloatMagFactor));
  memcpy(h.mag_factor, localMagFactor.localChar4Array, 4);

  // Set dim X,Y,Z
  m_Dimensions[0] = h.nx;
  m_Dimensions[1] = h.ny;
  if (h.npic != 1)
  {
    this->SetNumberOfDimensions(3);
    m_Dimensions[2] = h.npic;
  }
  else
  {
    this->SetNumberOfDimensions(2);
  }

  // Check the pixel size:
  if (h.byte_format == 1)
  {
    SetComponentType(IOComponentEnum::UCHAR);
  }
  else
  {
    // sometime the file set an erroneous value for byte_format, check the size
    // of the file in this case, since byte_format = 1 seems to be the default
    file.seekg(0, std::ios::end);
    const SizeValueType gcount = static_cast<SizeValueType>(file.tellg()) - BIORAD_HEADER_LENGTH;
    const auto          hsize = static_cast<SizeValueType>(h.nx * h.ny * h.npic);
    if (gcount == hsize)
    {
      itkWarningMacro("File is declared as two bytes but really is only one byte");
      SetComponentType(IOComponentEnum::UCHAR);
    }
    else if (gcount == hsize * 2)
    {
      SetComponentType(IOComponentEnum::USHORT);
    }
    else
    {
      SetComponentType(IOComponentEnum::UNKNOWNCOMPONENTTYPE);
      itkExceptionMacro("Cannot read requested file");
    }
  }
  int          punt(0);
  unsigned int notes = 0;
  memcpy(&notes, h.notes, sizeof(notes));
  ByteSwapper<unsigned int>::SwapFromSystemToLittleEndian(&notes);
  if (notes != 0)
  {
    // do it the recommended way
    std::streampos pos = static_cast<std::streampos>(h.nx) * static_cast<std::streampos>(h.ny);
    if (this->GetComponentType() == IOComponentEnum::USHORT)
    {
      pos = pos * 2;
    }
    pos += BIORAD_HEADER_LENGTH;
    file.seekg(pos, std::ios::beg);
    bioradnote note;
    if constexpr (sizeof(note) != 96)
    {
      itkExceptionMacro("BIORadImageIO:Problem with structure alignment");
    }
    while (!file.eof())
    {
      file.read(reinterpret_cast<char *>(&note), sizeof(note));
      ByteSwapper<short>::SwapFromSystemToLittleEndian(&note.level);
      Aligned4ByteUnion localNext;
      memcpy(localNext.localChar4Array, note.next, 4);
      ByteSwapper<int>::SwapFromSystemToLittleEndian(&(localNext.localIntNoteNext));
      memcpy(note.next, localNext.localChar4Array, 4);
      ByteSwapper<short>::SwapFromSystemToLittleEndian(&note.num);
      ByteSwapper<short>::SwapFromSystemToLittleEndian(&note.status);
      ByteSwapper<short>::SwapFromSystemToLittleEndian(&note.type);
      ByteSwapper<short>::SwapFromSystemToLittleEndian(&note.x);
      ByteSwapper<short>::SwapFromSystemToLittleEndian(&note.y);
      note.text[sizeof(note.text) - 1] = '\0'; // make sure terminated
      if (note.type == NOTE_TYPE_VARIABLE)
      {
        punt = false;
        const std::string  note_text(note.text);
        std::istringstream ss(note_text);
        std::string        label;
        ss >> label;
        short type = 0;
        ss >> type;
        if ((type & 0x00ff) != 1)
        {
          continue;
        }
        double origin = NAN;
        double spacing = NAN;
        if (label == "AXIS_2")
        {
          ss >> origin; // skip origin
          ss >> spacing;
          spacing *= 1000; // move to millemeters
          m_Spacing[0] = spacing;
          ++punt;
        }
        else if (label == "AXIS_3")
        {
          ss >> origin; // skip origin
          ss >> spacing;
          spacing *= 1000; // move to millemeters
          m_Spacing[1] = spacing;
          ++punt;
        }
        else if (label == "AXIS_4")
        {
          ss >> origin; // skip origin
          ss >> spacing;
          spacing *= 1000; // move to millemeters
          m_Spacing[2] = spacing;
          ++punt;
        }
      }
    }
  }
  if (punt == 0)
  {
    // deprecated method for finding spacing
    // These are not specified by the format, but we can deduce them:
    // pixel size = scale_factor/lens/mag_factor
    m_Spacing[0] = m_Spacing[1] = localMagFactor.localFloatMagFactor / h.lens;
    if (m_NumberOfDimensions == 3)
    {
      m_Spacing[2] = m_Spacing[0];
    }
  }
}

void
BioRadImageIO::ReadImageInformation()
{
  std::ifstream file;

  this->InternalReadImageInformation(file);
  file.close();
}

bool
BioRadImageIO::CanWriteFile(const char * name)
{
  const std::string filename = name;

  if (filename.empty())
  {
    itkDebugMacro("No filename specified.");
    return false;
  }

  const bool extensionFound = this->HasSupportedWriteExtension(name, false);

  if (!extensionFound)
  {
    itkDebugMacro("The filename extension is not recognized");
    return false;
  }

  return true;
}

void
BioRadImageIO::Write(const void * buffer)
{
  std::ofstream file;

  this->OpenFileForWriting(file, m_FileName);

  // Check the image region for proper dimensions, etc.
  const unsigned int numDims = this->GetNumberOfDimensions();
  if (numDims != 3 && numDims != 2)
  {
    itkExceptionMacro("BioRad Writer can only write 2 or 3-dimensional images");
  }

  // Write the BioRad header information
  bioradheader   header{};
  bioradheader * p = &header;
  if constexpr (sizeof(header) != BIORAD_HEADER_LENGTH)
  {
    itkExceptionMacro("Problem of alignment on your platform");
  }
  // In particular `notes' needs to be set to zero to indicate there is no notes
  header.nx = static_cast<unsigned short>(m_Dimensions[0]);
  header.ny = static_cast<unsigned short>(m_Dimensions[1]);
  if (m_NumberOfDimensions == 3)
  {
    header.npic = static_cast<unsigned short>(m_Dimensions[2]);
  }
  else
  {
    header.npic = 1;
  }
  header.file_id = BIORAD_MAGIC_NUMBER;

  // Always say that image was not edited:
  header.edited = 0;

  // Default dummy values:
  header.lens = 1;
  // Set the if file is in byte format or not:
  switch (this->GetComponentType())
  {
    case IOComponentEnum::UCHAR:
      header.byte_format = 1;
      header.ramp1_min = 0;
      header.ramp1_max = 255;
      header.ramp2_min = 0;
      header.ramp2_max = 255;
      break;
    case IOComponentEnum::USHORT:
      header.byte_format = 0;
      header.ramp1_min = 0;
      header.ramp1_max = 65535;
      header.ramp2_min = 0;
      header.ramp2_max = 65535;
      break;
    default:
      itkExceptionMacro("Component type not supported.");
  }
  // write the actual header
  ByteSwapper<unsigned short>::SwapRangeFromSystemToLittleEndian(reinterpret_cast<unsigned short *>(p),
                                                                 BIORAD_HEADER_LENGTH / 2);
  // To be able to deduce pixel spacing:
  Aligned4ByteUnion mag_factor;
  mag_factor.localFloatMagFactor = static_cast<float>(m_Spacing[0]);
  ByteSwapper<float>::SwapFromSystemToLittleEndian(&(mag_factor.localFloatMagFactor));
  memcpy(header.mag_factor, mag_factor.localChar4Array, 4);
  // Set the filename
  // NOTES: This is not very clear what should be written here, some files
  // have either:
  // 1. FILENAME.PIC
  // 2. FILENAME.pic
  // 3. FileName.pic
  // or simply
  // 4. FileName
  const std::string filename = itksys::SystemTools::GetFilenameName(m_FileName);
  // The buffer is at most 32 bytes, but must be null-terminated.
  // Here we copy at most 31 bytes and terminate it explicitly
  strncpy(header.filename, filename.c_str(), sizeof(header.filename) - 1);
  header.filename[sizeof(header.filename) - 1] = '\0';
  file.write(reinterpret_cast<char *>(p), BIORAD_HEADER_LENGTH);

  // preparation for writing buffer:
  const auto numberOfBytes = static_cast<SizeValueType>(this->GetImageSizeInBytes());
  const auto numberOfComponents = static_cast<SizeValueType>(this->GetImageSizeInComponents());

  const auto tempmemory = make_unique_for_overwrite<char[]>(numberOfBytes);
  memcpy(tempmemory.get(), buffer, numberOfBytes);
  if (this->GetComponentType() == IOComponentEnum::USHORT)
  {
    ByteSwapper<unsigned short>::SwapRangeFromSystemToBigEndian(reinterpret_cast<unsigned short *>(tempmemory.get()),
                                                                numberOfComponents);
  }

  // Write the actual pixel data
  file.write(static_cast<const char *>(tempmemory.get()), numberOfBytes);
  file.close();
}

void
BioRadImageIO::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
}
} // end namespace itk
