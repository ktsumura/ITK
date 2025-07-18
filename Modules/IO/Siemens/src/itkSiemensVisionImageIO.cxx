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
#include "itkSiemensVisionImageIO.h"
#include "itksys/SystemTools.hxx"
#include <iostream>
#include <fstream>
#include <algorithm>

// From uiig library "The University of Iowa Imaging Group-UIIG"

namespace itk
{
// Default constructor
SiemensVisionImageIO::SiemensVisionImageIO()
{
  // Purposefully left blank
}

SiemensVisionImageIO::~SiemensVisionImageIO()
{
  // Purposefully left blank
}

bool
SiemensVisionImageIO::CanReadFile(const char * FileNameToRead)
{
  this->SetFileName(FileNameToRead);
  //
  // Can you open it?
  std::ifstream f;
  try
  {
    this->OpenFileForReading(f, FileNameToRead);
  }
  catch (const ExceptionObject &)
  {
    return false;
  }
  int matrixX = 0;
  //
  // another lame heuristic, check the actual file size against
  // the image size suggested in header + the header size.
  if (this->GetIntAt(f, HDR_DISPLAY_SIZE, &matrixX, false) != 0)
  {
    return false;
  }

  if ((HDR_TOTAL_LENGTH + (matrixX * matrixX * 2)) != static_cast<int>(itksys::SystemTools::FileLength(FileNameToRead)))
  {
    return false;
  }

  return true;
}

GEImageHeader *
SiemensVisionImageIO::ReadHeader(const char * FileNameToRead)
{
  if (!this->CanReadFile(FileNameToRead))
  {
    RAISE_EXCEPTION();
  }

  // #define DEBUGHEADER
#if defined(DEBUGHEADER)
#  define DB(x) std::cerr << #x << ' ' << x << std::endl
#else
#  define DB(x)
#endif

// #define GE_PROD_STR "SIEMENS"
#define TEMPLEN 2048
  auto * hdr = new GEImageHeader;
  if (hdr == nullptr)
  {
    RAISE_EXCEPTION();
  }
#if defined(DEBUGHEADER)
  std::cerr << "----------------------" << FileNameToRead << "----------------------" << std::endl;
#endif

  std::ifstream f;
  this->OpenFileForReading(f, FileNameToRead);

  snprintf(hdr->scanner, sizeof(hdr->scanner), "GE-ADW");

  // Set modality to UNKNOWN
  strcpy(hdr->modality, "UNK");

  strncpy(hdr->filename, FileNameToRead, itk::IOCommon::ITK_MAXPATHLEN);

  // Get VITAL Information from the header
  this->GetStringAt(f, HDR_PAT_ID, hdr->patientId, HDR_PAT_ID_LEN);
  hdr->patientId[HDR_PAT_ID_LEN] = '\0';
  DB(hdr->patientId);
  // fprintf(stderr, "Patient %s\n", hdr->patientId);a

  this->GetStringAt(f, HDR_PAT_NAME, hdr->name, HDR_PAT_NAME_LEN);
  hdr->name[HDR_PAT_NAME_LEN] = '\0';
  DB(hdr->name);

  int year = 0;
  this->GetIntAt(f, HDR_REG_YEAR, &year);
  int month = 0;
  this->GetIntAt(f, HDR_REG_MONTH, &month);
  int day = 0;
  this->GetIntAt(f, HDR_REG_DAY, &day);
  int hour = 0;
  this->GetIntAt(f, HDR_REG_HOUR, &hour);
  int minute = 0;
  this->GetIntAt(f, HDR_REG_MIN, &minute);
  int second = 0;
  this->GetIntAt(f, HDR_REG_SEC, &second);

  snprintf(hdr->date, sizeof(hdr->date), "%d/%d/%d %d:%d:%d", year, month, day, hour, minute, second);
  DB(hdr->date);

  this->GetStringAt(f, HDR_INSTUTE_NAME, hdr->hospital, HDR_INSTUTE_NAME_LEN);
  hdr->hospital[HDR_INSTUTE_NAME_LEN] = '\0';
  DB(hdr->hospital);

  this->GetStringAt(f, HDR_MODEL_NAME, hdr->scanner, HDR_MODEL_NAME_LEN);
  hdr->scanner[HDR_MODEL_NAME_LEN] = '\0';
  DB(hdr->scanner);
  for (unsigned int i = 0; i < strlen(hdr->scanner); ++i)
  {
    if (hdr->scanner[i] == ' ')
    {
      hdr->scanner[i] = '-';
    }
  }

  char tmpStr[TEMPLEN];
  this->GetStringAt(f, TEXT_STUDY_NUM2, tmpStr, TEXT_STUDY_NUM2_LEN);
  tmpStr[TEXT_STUDY_NUM2_LEN] = '\0';
  hdr->seriesNumber = std::stoi(tmpStr);
  DB(hdr->seriesNumber);

  this->GetStringAt(f, TEXT_IMG_NUMBER, tmpStr, TEXT_IMG_NUMBER_LEN);
  tmpStr[TEXT_IMG_NUMBER_LEN] = '\0';
  hdr->imageNumber = std::stoi(tmpStr);
  DB(hdr->imageNumber);

  this->GetStringAt(f, TEXT_SLICE_THCK, tmpStr, TEXT_SLICE_THCK_LEN);
  tmpStr[TEXT_SLICE_THCK_LEN] = '\0';
  hdr->sliceThickness = std::stoi(tmpStr);
  hdr->sliceGap = 0.0f;

  DB(hdr->sliceThickness);
  int tmpInt = 0;
  this->GetIntAt(f, HDR_DISPLAY_SIZE, &tmpInt, sizeof(int));
  hdr->imageXsize = static_cast<int>(tmpInt);
  DB(hdr->imageXsize);
  hdr->imageYsize = static_cast<int>(tmpInt);
  DB(hdr->imageYsize);

  this->GetStringAt(f, TEXT_ACQ_MTRX_FREQ, tmpStr, TEXT_ACQ_MTRX_FREQ_LEN);
  tmpStr[TEXT_ACQ_MTRX_FREQ_LEN] = '\0';
  hdr->acqXsize = std::stoi(tmpStr);
  DB(hdr->acqXsize);

  this->GetStringAt(f, TEXT_ACQ_MTRX_PHASE, tmpStr, TEXT_ACQ_MTRX_PHASE_LEN);
  tmpStr[TEXT_ACQ_MTRX_PHASE_LEN] = '\0';
  hdr->acqYsize = std::stoi(tmpStr);
  DB(hdr->acqYsize);

  this->GetStringAt(f, TEXT_FOVH, tmpStr, TEXT_FOVH_LEN);
  tmpStr[TEXT_FOVH_LEN] = '\0';
  hdr->xFOV = static_cast<float>(std::stod(tmpStr));
  DB(hdr->xFOV);

  this->GetStringAt(f, TEXT_FOVV, tmpStr, TEXT_FOVV_LEN);
  tmpStr[TEXT_FOVV_LEN] = '\0';
  hdr->yFOV = static_cast<float>(std::stod(tmpStr));
  DB(hdr->yFOV);

  double tmpDble = NAN;
  this->GetDoubleAt(f, HDR_PIXELSIZE_ROW, &tmpDble, sizeof(double));
  hdr->imageXres = static_cast<float>(tmpDble);
  DB(hdr->imageXres);

  this->GetDoubleAt(f, HDR_PIXELSIZE_CLMN, &tmpDble, sizeof(double));
  hdr->imageYres = static_cast<float>(tmpDble);
  DB(hdr->imageYres);

  this->GetStringAt(f, TEXT_ANGLE_FLAG1, tmpStr, TEXT_ANGLE_FLAG1_LEN);
  tmpStr[TEXT_ANGLE_FLAG1_LEN] = '\0';

  char tmpStr2[TEMPLEN];
  this->GetStringAt(f, TEXT_ANGLE_FLAG3, tmpStr2, TEXT_ANGLE_FLAG3_LEN);
  tmpStr2[TEXT_ANGLE_FLAG3_LEN] = '\0';

  std::string text_angle_len;
  {
    char tmpStr3[TEMPLEN];
    this->GetStringAt(f, TEXT_ANGLE, tmpStr3, TEXT_ANGLE_LEN);
    tmpStr3[TEXT_ANGLE_LEN] = '\0';
    text_angle_len = tmpStr3;
  }
  // An empty string implies an angle less than 45 degrees for backwards compatibility
  text_angle_len.erase(std::remove_if(text_angle_len.begin(), text_angle_len.end(), isspace),
                       text_angle_len.end()); // Remove all whitespace
  if (strcmp(tmpStr, "Cor") == 0)
  {
    if (text_angle_len.empty() || itk::Math::abs(std::stod(text_angle_len)) <= 45.0)
    {
      // hdr->imagePlane = itk::IOCommon::ITK_ANALYZE_ORIENTATION_IRP_CORONAL;
      hdr->coordinateOrientation = AnatomicalOrientation(itk::AnatomicalOrientation::NegativeEnum::RSP);
    }
    else
    {
      if (strcmp(tmpStr2, "Sag") == 0)
      {
        // hdr->imagePlane =
        // itk::SpatialOrientationEnums::ValidCoordinateOrientations::ITK_ANALYZE_ORIENTATION_IRP_SAGITTAL;
        hdr->coordinateOrientation = AnatomicalOrientation(itk::AnatomicalOrientation::NegativeEnum::AIR);
      }
      else
      {
        // hdr->imagePlane =
        // itk::SpatialOrientationEnums::ValidCoordinateOrientations::ITK_ANALYZE_ORIENTATION_IRP_TRANSVERSE;
        hdr->coordinateOrientation = AnatomicalOrientation(itk::AnatomicalOrientation::NegativeEnum::RAI);
      }
    }
  }
  else if (strcmp(tmpStr, "Sag") == 0)
  {
    if (text_angle_len.empty() || itk::Math::abs(std::stod(text_angle_len)) <= 45.0)
    {
      // hdr->imagePlane =
      // itk::SpatialOrientationEnums::ValidCoordinateOrientations::ITK_ANALYZE_ORIENTATION_IRP_SAGITTAL;
      hdr->coordinateOrientation = AnatomicalOrientation(itk::AnatomicalOrientation::NegativeEnum::AIR);
    }
    else
    {
      if (strcmp(tmpStr2, "Cor") == 0)
      {
        // hdr->imagePlane =
        // itk::SpatialOrientationEnums::ValidCoordinateOrientations::ITK_ANALYZE_ORIENTATION_IRP_CORONAL;
        hdr->coordinateOrientation = AnatomicalOrientation(itk::AnatomicalOrientation::NegativeEnum::RSP);
      }
      else
      {
        // hdr->imagePlane =
        // itk::SpatialOrientationEnums::ValidCoordinateOrientations::ITK_ANALYZE_ORIENTATION_IRP_TRANSVERSE;
        hdr->coordinateOrientation = AnatomicalOrientation(itk::AnatomicalOrientation::NegativeEnum::RAI);
      }
    }
  }
  else
  {
    if (text_angle_len.empty() || itk::Math::abs(std::stod(text_angle_len)) <= 45.0)
    {
      // hdr->imagePlane =
      // itk::SpatialOrientationEnums::ValidCoordinateOrientations::ITK_ANALYZE_ORIENTATION_IRP_TRANSVERSE;
      hdr->coordinateOrientation = AnatomicalOrientation(itk::AnatomicalOrientation::NegativeEnum::RAI);
    }
    else
    {
      if (strcmp(tmpStr2, "Cor") == 0)
      {
        // hdr->imagePlane =
        // itk::SpatialOrientationEnums::ValidCoordinateOrientations::ITK_ANALYZE_ORIENTATION_IRP_CORONAL;
        hdr->coordinateOrientation = AnatomicalOrientation(itk::AnatomicalOrientation::NegativeEnum::RSP);
      }
      else
      {
        // hdr->imagePlane =
        // itk::SpatialOrientationEnums::ValidCoordinateOrientations::ITK_ANALYZE_ORIENTATION_IRP_SAGITTAL;
        hdr->coordinateOrientation = AnatomicalOrientation(itk::AnatomicalOrientation::NegativeEnum::AIR);
      }
    }
  }

  /* fprintf(stderr, "Plane %d\n", hdr->imagePlane); */
  this->GetStringAt(f, TEXT_SLICE_POS, tmpStr, TEXT_SLICE_POS_LEN);
  tmpStr[TEXT_SLICE_POS_LEN] = '\0';
  hdr->sliceLocation = static_cast<float>(std::stod(tmpStr));
  DB(hdr->sliceLocation);

  /* fprintf(stderr, "Slice Location %f\n", hdr->sliceLocation); */
  this->GetDoubleAt(f, HDR_TR, &tmpDble, sizeof(double));
  hdr->TR = static_cast<float>(tmpDble) / 1000.0f;
  DB(hdr->TR);

  this->GetDoubleAt(f, HDR_TE + 8, &tmpDble, sizeof(double));
  hdr->TI = static_cast<float>(tmpDble) / 1000.0f;
  DB(hdr->TI);

  this->GetDoubleAt(f, HDR_TE, &tmpDble, sizeof(double));
  hdr->TE = static_cast<float>(tmpDble) / 1000.0f;
  DB(hdr->TE);

  this->GetStringAt(f, TEXT_ECHO_NUM, tmpStr, TEXT_ECHO_NUM_LEN);
  tmpStr[TEXT_ECHO_NUM_LEN] = '\0';
  hdr->echoNumber = static_cast<int>(std::stoi(tmpStr));
  DB(hdr->echoNumber);

  this->GetDoubleAt(f, HDR_FLIP_ANGLE, &tmpDble, sizeof(double));
  hdr->flipAngle = static_cast<int>(tmpDble);
  DB(hdr->flipAngle);

  this->GetStringAt(f, HDR_SEQPROG_NAME, hdr->pulseSequence, HDR_SEQPROG_NAME_LEN);
  hdr->pulseSequence[HDR_SEQPROG_NAME_LEN] = '\0';

  hdr->offset = HDR_TOTAL_LENGTH;
  return hdr;
}
} // end namespace itk
