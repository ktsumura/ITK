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
#ifndef itkChainCodeToFourierSeriesPathFilter_hxx
#define itkChainCodeToFourierSeriesPathFilter_hxx

#include <cmath>

namespace itk
{

template <typename TInputChainCodePath, typename TOutputFourierSeriesPath>
ChainCodeToFourierSeriesPathFilter<TInputChainCodePath, TOutputFourierSeriesPath>::ChainCodeToFourierSeriesPathFilter()
  : m_NumberOfHarmonics(8)
{
  this->SetNumberOfRequiredInputs(1);
}

template <typename TInputChainCodePath, typename TOutputFourierSeriesPath>
void
ChainCodeToFourierSeriesPathFilter<TInputChainCodePath, TOutputFourierSeriesPath>::GenerateData()
{

  unsigned int numHarmonics = m_NumberOfHarmonics; // private copy
  const int    dimension = OffsetType::GetOffsetDimension();

  const typename Superclass::InputPathConstPointer inputPtr = this->GetInput();
  const typename Superclass::OutputPathPointer     outputPtr = this->GetOutput(0);

  // outputPtr->SetRequestedRegion( inputPtr->GetRequestedRegion() );
  // outputPtr->SetBufferedRegion( inputPtr->GetBufferedRegion() );
  // outputPtr->SetLargestPossibleRegion( inputPtr->GetLargestPossibleRegion() );
  // outputPtr->Allocate();  // Allocate() is an Image function

  const auto numSteps = inputPtr->NumberOfSteps();
  outputPtr->Clear();

  const double nPI = 4.0 * std::atan(1.0);

  // Adjust our private copy of numHarmonics if necessary
  if (numHarmonics <= 1)
  {
    numHarmonics = 2;
  }
  else if (numHarmonics * 2 > numSteps)
  {
    numHarmonics = numSteps / 2;
  }

  for (unsigned int n = 0; n < numHarmonics; ++n)
  {
    auto       index = inputPtr->GetStart();
    VectorType cosCoefficient;
    cosCoefficient.Fill(0.0);
    VectorType sinCoefficient;
    sinCoefficient.Fill(0.0);

    for (InputPathInputType step = 0; step < numSteps; ++step)
    {
      index += inputPtr->Evaluate(step);
      const auto theta = 2 * n * nPI * (static_cast<double>(step + 1)) / numSteps;

      // turn the current index into a vector
      VectorType indexVector;
      for (int d = 0; d < dimension; ++d)
      {
        indexVector[d] = index[d];
      }
      cosCoefficient += indexVector * (std::cos(theta) / numSteps);
      sinCoefficient += indexVector * (std::sin(theta) / numSteps);
    }

    outputPtr->AddHarmonic(cosCoefficient, sinCoefficient);
  }
}

template <typename TInputChainCodePath, typename TOutputFourierSeriesPath>
void
ChainCodeToFourierSeriesPathFilter<TInputChainCodePath, TOutputFourierSeriesPath>::PrintSelf(std::ostream & os,
                                                                                             Indent indent) const
{
  Superclass::PrintSelf(os, indent);

  os << indent << "NumberOfHarmonics: " << m_NumberOfHarmonics << std::endl;
}
} // end namespace itk

#endif
