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
#ifndef itkObjectToObjectMetricBase_hxx
#define itkObjectToObjectMetricBase_hxx


namespace itk
{

//-------------------------------------------------------------------
template <typename TInternalComputationValueType>
ObjectToObjectMetricBaseTemplate<TInternalComputationValueType>::ObjectToObjectMetricBaseTemplate()
  : m_GradientSource(GradientSourceEnum::GRADIENT_SOURCE_MOVING)
  , m_Value(MeasureType{})
{
  // Don't call SetGradientSource, to avoid valgrind warning.
}

//-------------------------------------------------------------------
template <typename TInternalComputationValueType>
bool
ObjectToObjectMetricBaseTemplate<TInternalComputationValueType>::GetGradientSourceIncludesFixed() const
{
  return m_GradientSource == GradientSourceEnum::GRADIENT_SOURCE_FIXED ||
         m_GradientSource == GradientSourceEnum::GRADIENT_SOURCE_BOTH;
}

//-------------------------------------------------------------------
template <typename TInternalComputationValueType>
bool
ObjectToObjectMetricBaseTemplate<TInternalComputationValueType>::GetGradientSourceIncludesMoving() const
{
  return m_GradientSource == GradientSourceEnum::GRADIENT_SOURCE_MOVING ||
         m_GradientSource == GradientSourceEnum::GRADIENT_SOURCE_BOTH;
}

//-------------------------------------------------------------------
template <typename TInternalComputationValueType>
auto
ObjectToObjectMetricBaseTemplate<TInternalComputationValueType>::GetCurrentValue() const -> MeasureType
{
  return m_Value;
}

//-------------------------------------------------------------------
template <typename TInternalComputationValueType>
void
ObjectToObjectMetricBaseTemplate<TInternalComputationValueType>::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
  os << indent << "Value: " << m_Value << std::endl;
  os << indent << "GradientSourceEnum: " << m_GradientSource << std::endl;
}

} // namespace itk

#endif
