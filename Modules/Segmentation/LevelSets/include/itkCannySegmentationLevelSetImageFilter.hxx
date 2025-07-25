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
#ifndef itkCannySegmentationLevelSetImageFilter_hxx
#define itkCannySegmentationLevelSetImageFilter_hxx


namespace itk
{
template <typename TInputImage, typename TFeatureImage, typename TOutputPixelType>
CannySegmentationLevelSetImageFilter<TInputImage, TFeatureImage, TOutputPixelType>::
  CannySegmentationLevelSetImageFilter()
  : m_CannyFunction(CannyFunctionType::New())
{
  this->SetSegmentationFunction(m_CannyFunction);
}
} // end namespace itk

#endif
