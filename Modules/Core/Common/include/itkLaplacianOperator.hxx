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
#ifndef itkLaplacianOperator_hxx
#define itkLaplacianOperator_hxx

namespace itk
{
template <typename TPixel, unsigned int VDimension, typename TAllocator>
void
LaplacianOperator<TPixel, VDimension, TAllocator>::SetDerivativeScalings(const double * s)
{
  for (unsigned int i = 0; i < VDimension; ++i)
  {
    m_DerivativeScalings[i] = s[i];
  }
}

// Create the operator
template <typename TPixel, unsigned int VDimension, typename TAllocator>
void
LaplacianOperator<TPixel, VDimension, TAllocator>::CreateOperator()
{
  const CoefficientVector coefficients = this->GenerateCoefficients();

  this->Fill(coefficients);
}

// This function fills the coefficients into the corresponding neighborhood.
template <typename TPixel, unsigned int VDimension, typename TAllocator>
void
LaplacianOperator<TPixel, VDimension, TAllocator>::Fill(const CoefficientVector & coeff)
{


  const std::slice temp_slice(0, coeff.size(), 1);

  // Copy the coefficients into the neighborhood
  {
    auto it = coeff.begin();
    for (typename Self::SliceIteratorType data(this, temp_slice); data < data.End(); ++data)
    {
      *data = *it;
      ++it;
    }
  }
}

template <typename TPixel, unsigned int VDimension, typename TAllocator>
auto
LaplacianOperator<TPixel, VDimension, TAllocator>::GenerateCoefficients() -> CoefficientVector
{
  // Here we set the radius to 1's, here the
  // operator is 3x3 for 2D, 3x3x3 for 3D.
  constexpr auto r = SizeType::Filled(1);
  this->SetRadius(r);

  // Create a vector of the correct size to hold the coefficients.
  const unsigned int w = this->Size();
  CoefficientVector  coeffP(w);

  // Set the coefficients
  double sum = 0.0;
  for (unsigned int i = 0; i < 2 * VDimension; i += 2)
  {
    const OffsetValueType stride = this->GetStride(i / 2);

    const double hsq = m_DerivativeScalings[i / 2] * m_DerivativeScalings[i / 2];
    coeffP[w / 2 - stride] = coeffP[w / 2 + stride] = hsq;
    sum += 2.0 * hsq;
  }
  coeffP[w / 2] = -sum;

  return coeffP;
}
} // namespace itk

#endif
