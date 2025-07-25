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
#ifndef itkScalarChanAndVeseDenseLevelSetImageFilter_h
#define itkScalarChanAndVeseDenseLevelSetImageFilter_h

#include "itkMultiphaseDenseFiniteDifferenceImageFilter.h"
#include "itkRegionOfInterestImageFilter.h"
#include "itkScalarChanAndVeseLevelSetFunction.h"

namespace itk
{
/** \class ScalarChanAndVeseDenseLevelSetImageFilter
 * \brief Dense implementation of the Chan and Vese multiphase level set image filter.
 *
 * This code was adapted from the paper \cite chan1999.
 *
 * \author Mosaliganti K., Smith B., Gelas A., Gouaillard A., Megason S.
 *
 *  This code was taken from the Insight Journal paper \cite Mosaliganti_2009_c
 *  that is based on the papers \cite Mosaliganti_2009_a and
 *  \cite  Mosaliganti_2009_b.
 *
 * \ingroup ITKReview
 */
template <typename TInputImage,
          typename TFeatureImage,
          typename TOutputImage,
          typename TFunction = ScalarChanAndVeseLevelSetFunction<TInputImage, TFeatureImage>,
          class TSharedData = typename TFunction::SharedDataType>
class ITK_TEMPLATE_EXPORT ScalarChanAndVeseDenseLevelSetImageFilter
  : public MultiphaseDenseFiniteDifferenceImageFilter<TInputImage, TFeatureImage, TOutputImage, TFunction>
{
public:
  ITK_DISALLOW_COPY_AND_MOVE(ScalarChanAndVeseDenseLevelSetImageFilter);

  using Self = ScalarChanAndVeseDenseLevelSetImageFilter;
  using Superclass = MultiphaseDenseFiniteDifferenceImageFilter<TInputImage, TFeatureImage, TOutputImage, TFunction>;
  using Pointer = SmartPointer<Self>;
  using ConstPointer = SmartPointer<const Self>;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** \see LightObject::GetNameOfClass() */
  itkOverrideGetNameOfClassMacro(ScalarChanAndVeseDenseLevelSetImageFilter);

  static constexpr unsigned int ImageDimension = TInputImage::ImageDimension;

  /** Inherited type alias from the superclass. */
  using typename Superclass::InputImageType;
  using typename Superclass::InputImagePointer;
  using typename Superclass::InputPointType;
  using typename Superclass::ValueType;
  using InputSpacingType = typename InputImageType::SpacingType;

  using FeatureImageType = TFeatureImage;
  using FeatureImagePointer = typename FeatureImageType::Pointer;
  using FeaturePixelType = typename FeatureImageType::PixelType;
  using FeatureIndexType = typename FeatureImageType::IndexType;
  using FeatureIndexValueType = typename FeatureIndexType::IndexValueType;
  using FeatureRegionType = typename FeatureImageType::RegionType;

  /** Output image type alias */
  using OutputImageType = TOutputImage;
  using IndexType = typename OutputImageType::IndexType;
  using OutputPixelType = typename OutputImageType::PixelType;

  using typename Superclass::TimeStepType;
  using typename Superclass::FiniteDifferenceFunctionType;

  using FunctionType = TFunction;
  using FunctionPointer = typename FunctionType::Pointer;

  using SharedDataType = TSharedData;
  using SharedDataPointer = typename SharedDataType::Pointer;

  using ROIFilterType = RegionOfInterestImageFilter<FeatureImageType, FeatureImageType>;
  using ROIFilterPointer = typename ROIFilterType::Pointer;

  itkConceptMacro(OutputHasNumericTraitsCheck, (Concept::HasNumericTraits<OutputPixelType>));

  /** Set/Get the feature image to be used for speed function of the level set
   *  equation.  Equivalent to calling Set/GetInput(1, ..) */
  virtual void
  SetFeatureImage(const FeatureImagePointer f)
  {
    this->SetInput(f);
  }

protected:
  ScalarChanAndVeseDenseLevelSetImageFilter()
    : m_SharedData(SharedDataType::New())
  {}

  ~ScalarChanAndVeseDenseLevelSetImageFilter() override = default;

  SharedDataPointer m_SharedData{};

  void
  Initialize() override;

  void
  InitializeIteration() override;
};
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#  include "itkScalarChanAndVeseDenseLevelSetImageFilter.hxx"
#endif

#endif
