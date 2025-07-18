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
#ifndef itkWatershedRelabeler_hxx
#define itkWatershedRelabeler_hxx

#include "itkImageRegionIterator.h"

namespace itk::watershed
{
template <typename TScalar, unsigned int TImageDimension>
Relabeler<TScalar, TImageDimension>::Relabeler()
{
  const typename ImageType::Pointer img = static_cast<ImageType *>(this->MakeOutput(0).GetPointer());
  this->SetNumberOfRequiredOutputs(1);
  this->ProcessObject::SetNthOutput(0, img.GetPointer());
}

template <typename TScalar, unsigned int TImageDimension>
auto
Relabeler<TScalar, TImageDimension>::MakeOutput(DataObjectPointerArraySizeType itkNotUsed(idx)) -> DataObjectPointer
{
  return ImageType::New().GetPointer();
}

template <typename TScalar, unsigned int TImageDimension>
void
Relabeler<TScalar, TImageDimension>::GenerateData()
{
  this->UpdateProgress(0.0);
  const typename ImageType::Pointer input = this->GetInputImage();
  const typename ImageType::Pointer output = this->GetOutputImage();

  const typename SegmentTreeType::Pointer tree = this->GetInputSegmentTree();
  typename SegmentTreeType::Iterator      it;
  auto                                    eqT = EquivalencyTable::New();

  output->SetBufferedRegion(output->GetRequestedRegion());
  output->Allocate();
  //
  // Copy input to output
  //
  ImageRegionIterator<ImageType> it_a(input, output->GetRequestedRegion());
  ImageRegionIterator<ImageType> it_b(output, output->GetRequestedRegion());
  it_a.GoToBegin();
  it_b.GoToBegin();
  while (!it_a.IsAtEnd())
  {
    it_b.Set(it_a.Get());
    ++it_a;
    ++it_b;
  }

  this->UpdateProgress(0.1);
  //
  // Extract the merges up the requested level
  //
  if (tree->Empty())
  {
    // itkWarningMacro("Empty input.  No relabeling was done.");
    return;
  }
  const ScalarType max = tree->Back().saliency;
  auto             mergeLimit = static_cast<ScalarType>(m_FloodLevel * max);

  this->UpdateProgress(0.5);

  it = tree->Begin();
  while (it != tree->End() && it->saliency <= mergeLimit)
  {
    eqT->Add(it->from, it->to);
    ++it;
  }

  SegmenterType::RelabelImage(output, output->GetRequestedRegion(), eqT);
  this->UpdateProgress(1.0);
}

template <typename TScalar, unsigned int VImageDimension>
void
Relabeler<TScalar, VImageDimension>::GenerateInputRequestedRegion()
{
  // call the superclass' implementation of this method
  Superclass::GenerateInputRequestedRegion();

  // get pointers to the input and output
  const typename ImageType::Pointer inputPtr = this->GetInputImage();
  const typename ImageType::Pointer outputPtr = this->GetOutputImage();

  if (!inputPtr || !outputPtr)
  {
    return;
  }

  //
  // FOR NOW WE'LL JUST SET THE INPUT REGION TO THE OUTPUT REGION
  //
  inputPtr->SetRequestedRegion(outputPtr->GetRequestedRegion());
}

template <typename TScalar, unsigned int TImageDimension>
void
Relabeler<TScalar, TImageDimension>::GenerateOutputRequestedRegion(DataObject * output)
{
  // Only the Image output need to be propagated through.
  // No choice but to use RTTI here.
  // All Image outputs set to the same RequestedRegion  other
  // outputs ignored.

  auto * imgData = dynamic_cast<ImageBase<ImageDimension> *>(output);
  if (imgData)
  {
    std::vector<ProcessObject::DataObjectPointer>::size_type idx = 0;
    for (idx = 0; idx < this->GetNumberOfIndexedOutputs(); ++idx)
    {
      if (this->GetOutput(idx) && this->GetOutput(idx) != output)
      {
        auto * op = dynamic_cast<ImageBase<ImageDimension> *>(this->GetOutput(idx));
        if (op)
        {
          this->GetOutput(idx)->SetRequestedRegion(output);
        }
      }
    }
  }
}

template <typename TScalar, unsigned int TImageDimension>
void
Relabeler<TScalar, TImageDimension>::GraftOutput(ImageType * graft)
{
  this->GraftNthOutput(0, graft);
}

template <typename TScalar, unsigned int TImageDimension>
void
Relabeler<TScalar, TImageDimension>::GraftNthOutput(unsigned int idx, ImageType * graft)
{
  using OutputImagePointer = typename ImageType::Pointer;

  if (idx < this->GetNumberOfIndexedOutputs())
  {
    const OutputImagePointer output = this->GetOutputImage();

    if (output && graft)
    {
      // grab a handle to the bulk data of the specified data object
      output->SetPixelContainer(graft->GetPixelContainer());

      // copy the region ivars of the specified data object
      output->SetRequestedRegion(graft->GetRequestedRegion());
      output->SetLargestPossibleRegion(graft->GetLargestPossibleRegion());
      output->SetBufferedRegion(graft->GetBufferedRegion());

      // copy the meta-information
      output->CopyInformation(graft);
    }
  }
}

template <typename TScalar, unsigned int TImageDimension>
void
Relabeler<TScalar, TImageDimension>::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
  os << indent << "FloodLevel: " << m_FloodLevel << std::endl;
}
} // namespace itk::watershed

#endif
