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
#ifndef itkJointDomainImageToListSampleAdaptor_hxx
#define itkJointDomainImageToListSampleAdaptor_hxx

namespace itk::Statistics
{
template <typename TImage>
JointDomainImageToListSampleAdaptor<TImage>::JointDomainImageToListSampleAdaptor()
  : m_Image(nullptr)
  , m_UsePixelContainer(true)
{
  m_NormalizationFactors.Fill(1.0f);
}

/** returns the number of measurement vectors in this container*/
template <typename TImage>
auto
JointDomainImageToListSampleAdaptor<TImage>::Size() const -> InstanceIdentifier
{
  if (m_Image.IsNull())
  {
    itkExceptionMacro("Image has not been set yet");
  }

  return m_Image->GetPixelContainer()->Size();
}

template <typename TImage>
inline auto
JointDomainImageToListSampleAdaptor<TImage>::GetFrequency(InstanceIdentifier) const -> AbsoluteFrequencyType
{
  if (m_Image.IsNull())
  {
    itkExceptionMacro("Image has not been set yet");
  }

  return NumericTraits<AbsoluteFrequencyType>::OneValue();
}

template <typename TImage>
void
JointDomainImageToListSampleAdaptor<TImage>::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "Image: ";
  if (m_Image.IsNotNull())
  {
    os << m_Image << std::endl;
  }
  else
  {
    os << "not set." << std::endl;
  }
  os << indent << "UsePixelContainer: " << this->GetUsePixelContainer() << std::endl;
}

template <typename TImage>
void
JointDomainImageToListSampleAdaptor<TImage>::SetImage(const TImage * image)
{
  m_Image = image;
  m_PixelContainer = image->GetPixelContainer();
  this->Modified();
}

template <typename TImage>
const TImage *
JointDomainImageToListSampleAdaptor<TImage>::GetImage() const
{
  if (m_Image.IsNull())
  {
    itkExceptionMacro("Image has not been set yet");
  }

  return m_Image.GetPointer();
}

template <typename TImage>
auto
JointDomainImageToListSampleAdaptor<TImage>::GetTotalFrequency() const -> TotalAbsoluteFrequencyType
{
  if (m_Image.IsNull())
  {
    itkExceptionMacro("Image has not been set yet");
  }

  return this->Size();
}

template <typename TImage>
void
JointDomainImageToListSampleAdaptor<TImage>::SetNormalizationFactors(NormalizationFactorsType & factors)
{
  if (m_NormalizationFactors != factors)
  {
    m_NormalizationFactors = factors;
    this->Modified();
  }
}

template <typename TImage>
auto
JointDomainImageToListSampleAdaptor<TImage>::GetMeasurementVector(InstanceIdentifier id) const
  -> const MeasurementVectorType &
{
  m_TempIndex = this->GetImage()->ComputeIndex(id);

  this->GetImage()->TransformIndexToPhysicalPoint(m_TempIndex, m_TempPoint);

  for (unsigned int i = 0; i < TImage::ImageDimension; ++i)
  {
    m_TempVector[i] = m_TempPoint[i] / m_NormalizationFactors[i];
  }

  if (m_UsePixelContainer)
  {
    MeasurementVectorTraits::Assign(m_TempRangeVector, (*m_PixelContainer)[id]);
  }
  else
  {
    MeasurementVectorTraits::Assign(m_TempRangeVector, m_Image->GetPixel(m_Image->ComputeIndex(id)));
  }

  for (unsigned int i = TImage::ImageDimension; i < MeasurementVectorType::Length; ++i)
  {
    m_TempVector[i] = m_TempRangeVector[i - TImage::ImageDimension] / m_NormalizationFactors[i];
  }

  return m_TempVector;
}
} // namespace itk::Statistics

#endif
