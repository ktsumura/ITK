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
#ifndef itkAreaOpeningImageFilter_h
#define itkAreaOpeningImageFilter_h

#include "itkAttributeMorphologyBaseImageFilter.h"
#include <functional>

namespace itk
{
/**
 * \class AreaOpeningImageFilter
 * \brief Morphological opening by attributes
 *
 * An attribute opening removes blobs according to criteria
 * such as area. When applied to grayscale images they have the effect of
 * trimming peaks (regions brighter than their surroundings)
 * based on area while leaving the rest of the image
 * unchanged.
 *
 * This code was contributed in the Insight Journal paper
 *
 * "Grayscale morphological attribute operations"
 * by Beare R.
 * https://doi.org/10.54294/ifvjls
 *
 *
 * \author Richard Beare. Department of Medicine, Monash University, Melbourne, Australia.
 *
 * \ingroup ITKReview
 */
template <typename TInputImage,
          typename TOutputImage,
          typename TAttribute = typename TInputImage::SpacingType::ValueType>
class AreaOpeningImageFilter
  : public AttributeMorphologyBaseImageFilter<TInputImage,
                                              TOutputImage,
                                              TAttribute,
                                              std::greater<typename TInputImage::PixelType>>

{
public:
  ITK_DISALLOW_COPY_AND_MOVE(AreaOpeningImageFilter);

  using Self = AreaOpeningImageFilter;
  using Superclass = AttributeMorphologyBaseImageFilter<TInputImage,
                                                        TOutputImage,
                                                        TAttribute,
                                                        std::greater<typename TInputImage::PixelType>>;

  using Pointer = SmartPointer<Self>;
  using ConstPointer = SmartPointer<const Self>;

  /**
   * Extract some information from the image types.  Dimensionality
   * of the two images is assumed to be the same.
   */
  using OutputPixelType = typename TOutputImage::PixelType;
  using OutputInternalPixelType = typename TOutputImage::InternalPixelType;
  using InputPixelType = typename TInputImage::PixelType;
  using InputInternalPixelType = typename TInputImage::InternalPixelType;
  using IndexType = typename TInputImage::IndexType;
  using OffsetType = typename TInputImage::OffsetType;
  using SizeType = typename TInputImage::SizeType;
  using AttributeType = TAttribute;

  static constexpr unsigned int ImageDimension = TOutputImage::ImageDimension;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** \see LightObject::GetNameOfClass() */
  itkOverrideGetNameOfClassMacro(AreaOpeningImageFilter);

  /**
   * Set/Get whether the image spacing is used or not - defaults to true.
   */
  itkSetMacro(UseImageSpacing, bool);
  itkGetConstReferenceMacro(UseImageSpacing, bool);
  itkBooleanMacro(UseImageSpacing);

protected:
  AreaOpeningImageFilter()
    : m_UseImageSpacing(true)
  {}

  ~AreaOpeningImageFilter() override = default;

  void
  GenerateData() override
  {
    this->m_AttributeValuePerPixel = 1;
    if (m_UseImageSpacing)
    {
      const auto & spacing = this->GetInput()->GetSpacing();

      // compute pixel size
      double psize = 1.0;
      for (unsigned int i = 0; i < ImageDimension; ++i)
      {
        psize *= spacing[i];
      }
      this->m_AttributeValuePerPixel = static_cast<AttributeType>(psize);
      // std::cout << "m_AttributeValuePerPixel: " <<
      // this->m_AttributeValuePerPixel << std::endl;
      // and call superclass implementation of GenerateData()
    }
    Superclass::GenerateData();
  }

  void
  PrintSelf(std::ostream & os, Indent indent) const override
  {
    Superclass::PrintSelf(os, indent);
    itkPrintSelfBooleanMacro(UseImageSpacing);
  }

private:
  bool m_UseImageSpacing{};
};
} // namespace itk
#endif
