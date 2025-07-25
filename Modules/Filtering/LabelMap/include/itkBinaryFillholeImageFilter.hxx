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
#ifndef itkBinaryFillholeImageFilter_hxx
#define itkBinaryFillholeImageFilter_hxx

#include "itkBinaryNotImageFilter.h"
#include "itkBinaryImageToShapeLabelMapFilter.h"
#include "itkShapeOpeningLabelMapFilter.h"
#include "itkLabelMapMaskImageFilter.h"
#include "itkProgressAccumulator.h"

namespace itk
{

template <typename TInputImage>
BinaryFillholeImageFilter<TInputImage>::BinaryFillholeImageFilter()
  : m_ForegroundValue(NumericTraits<InputImagePixelType>::max())

{}

template <typename TInputImage>
void
BinaryFillholeImageFilter<TInputImage>::GenerateInputRequestedRegion()
{
  // call the superclass' implementation of this method
  Superclass::GenerateInputRequestedRegion();

  // We need all the input.
  const InputImagePointer input = const_cast<InputImageType *>(this->GetInput());
  if (input)
  {
    input->SetRequestedRegion(input->GetLargestPossibleRegion());
  }
}


template <typename TInputImage>
void
BinaryFillholeImageFilter<TInputImage>::EnlargeOutputRequestedRegion(DataObject *)
{
  this->GetOutput()->SetRequestedRegion(this->GetOutput()->GetLargestPossibleRegion());
}


template <typename TInputImage>
void
BinaryFillholeImageFilter<TInputImage>::GenerateData()
{
  // let choose a background value. Background value should not be given by user
  // because closing is extensive so no background pixels will be added
  // it is just needed for internal erosion filter and constant padder
  InputImagePixelType backgroundValue{};
  if (m_ForegroundValue == backgroundValue)
  {
    // current background value is already used for foreground value
    // choose another one
    backgroundValue = NumericTraits<InputImagePixelType>::max();
  }

  // Create a process accumulator for tracking the progress of this minipipeline
  auto progress = ProgressAccumulator::New();
  progress->SetMiniPipelineFilter(this);

  // Allocate the output
  this->AllocateOutputs();

  using NotType = BinaryNotImageFilter<InputImageType>;
  auto notInput = NotType::New();
  notInput->SetInput(this->GetInput());
  notInput->SetForegroundValue(m_ForegroundValue);
  notInput->SetBackgroundValue(backgroundValue);
  notInput->SetNumberOfWorkUnits(this->GetNumberOfWorkUnits());
  notInput->SetReleaseDataFlag(true);
  progress->RegisterInternalFilter(notInput, .2f);

  using LabelizerType = typename itk::BinaryImageToShapeLabelMapFilter<InputImageType>;
  auto labelizer = LabelizerType::New();
  labelizer->SetInput(notInput->GetOutput());
  labelizer->SetInputForegroundValue(m_ForegroundValue);
  labelizer->SetOutputBackgroundValue(backgroundValue);
  labelizer->SetFullyConnected(m_FullyConnected);
  labelizer->SetNumberOfWorkUnits(this->GetNumberOfWorkUnits());
  progress->RegisterInternalFilter(labelizer, .5f);

  using LabelMapType = typename LabelizerType::OutputImageType;
  using OpeningType = typename itk::ShapeOpeningLabelMapFilter<LabelMapType>;
  auto opening = OpeningType::New();
  opening->SetInput(labelizer->GetOutput());
  opening->SetAttribute(LabelMapType::LabelObjectType::NUMBER_OF_PIXELS_ON_BORDER);
  opening->SetLambda(1);
  opening->SetNumberOfWorkUnits(this->GetNumberOfWorkUnits());
  progress->RegisterInternalFilter(opening, .1f);

  // invert the image during the binarization
  using BinarizerType = typename itk::LabelMapMaskImageFilter<LabelMapType, OutputImageType>;
  auto binarizer = BinarizerType::New();
  binarizer->SetInput(opening->GetOutput());
  binarizer->SetLabel(backgroundValue);
  binarizer->SetNegated(true);
  binarizer->SetBackgroundValue(m_ForegroundValue);
  binarizer->SetFeatureImage(this->GetInput());
  binarizer->SetNumberOfWorkUnits(this->GetNumberOfWorkUnits());
  progress->RegisterInternalFilter(binarizer, .2f);

  binarizer->GraftOutput(this->GetOutput());
  binarizer->Update();
  this->GraftOutput(binarizer->GetOutput());
}


template <typename TInputImage>
void
BinaryFillholeImageFilter<TInputImage>::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);

  os << indent
     << "ForegroundValue: " << static_cast<typename NumericTraits<InputImagePixelType>::PrintType>(m_ForegroundValue)
     << std::endl;
  itkPrintSelfBooleanMacro(FullyConnected);
}

} // end namespace itk
#endif
