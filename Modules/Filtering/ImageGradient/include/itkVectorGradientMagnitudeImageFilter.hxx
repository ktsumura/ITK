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
#ifndef itkVectorGradientMagnitudeImageFilter_hxx
#define itkVectorGradientMagnitudeImageFilter_hxx


#include "itkNeighborhoodAlgorithm.h"
#include "itkImageRegionIterator.h"
#include "itkZeroFluxNeumannBoundaryCondition.h"
#include "itkTotalProgressReporter.h"
#include "itkCastImageFilter.h"

#include "itkMath.h"
#include "itkPrintHelper.h"

namespace itk
{
template <typename TInputImage, typename TRealType, typename TOutputImage>
VectorGradientMagnitudeImageFilter<TInputImage, TRealType, TOutputImage>::VectorGradientMagnitudeImageFilter()
  : m_UsePrincipleComponents(true)
  , m_RequestedNumberOfWorkUnits(this->GetNumberOfWorkUnits())
{
  this->DynamicMultiThreadingOn();
  this->ThreaderUpdateProgressOff();
}

template <typename TInputImage, typename TRealType, typename TOutputImage>
void
VectorGradientMagnitudeImageFilter<TInputImage, TRealType, TOutputImage>::PrintSelf(std::ostream & os,
                                                                                    Indent         indent) const
{
  using namespace print_helper;

  Superclass::PrintSelf(os, indent);

  os << indent << "DerivativeWeights: " << m_DerivativeWeights << std::endl;
  os << indent << "ComponentWeights: " << m_ComponentWeights << std::endl;
  os << indent << "SqrtComponentWeights: " << m_SqrtComponentWeights << std::endl;
  itkPrintSelfBooleanMacro(UseImageSpacing);
  os << indent << "UsePrincipleComponents: " << m_UsePrincipleComponents << std::endl;
  os << indent << "RequestedNumberOfThreads: "
     << static_cast<typename NumericTraits<ThreadIdType>::PrintType>(m_RequestedNumberOfWorkUnits) << std::endl;

  itkPrintSelfObjectMacro(RealValuedInputImage);
}

template <typename TInputImage, typename TRealType, typename TOutputImage>
void
VectorGradientMagnitudeImageFilter<TInputImage, TRealType, TOutputImage>::SetUseImageSpacing(bool f)
{
  if (m_UseImageSpacing == f)
  {
    return;
  }

  // Only reset the weights if they were previously set to the image spacing,
  // otherwise, the user may have provided their own weightings.
  if (f == false && m_UseImageSpacing)
  {
    for (unsigned int i = 0; i < ImageDimension; ++i)
    {
      m_DerivativeWeights[i] = static_cast<TRealType>(1.0);
    }
  }

  m_UseImageSpacing = f;
}

template <typename TInputImage, typename TRealType, typename TOutputImage>
void
VectorGradientMagnitudeImageFilter<TInputImage, TRealType, TOutputImage>::GenerateInputRequestedRegion()
{
  // call the superclass' implementation of this method
  Superclass::GenerateInputRequestedRegion();

  // get pointers to the input and output
  const InputImagePointer  inputPtr = const_cast<InputImageType *>(this->GetInput());
  const OutputImagePointer outputPtr = this->GetOutput();

  if (!inputPtr || !outputPtr)
  {
    return;
  }

  // get a copy of the input requested region (should equal the output
  // requested region)
  typename TInputImage::RegionType inputRequestedRegion = inputPtr->GetRequestedRegion();

  constexpr auto r1 = MakeFilled<RadiusType>(1);
  // pad the input requested region by the operator radius
  inputRequestedRegion.PadByRadius(r1);

  // crop the input requested region at the input's largest possible region
  if (inputRequestedRegion.Crop(inputPtr->GetLargestPossibleRegion()))
  {
    inputPtr->SetRequestedRegion(inputRequestedRegion);
    return;
  }

  // Couldn't crop the region (requested region is outside the largest
  // possible region).  Throw an exception.

  // store what we tried to request (prior to trying to crop)
  inputPtr->SetRequestedRegion(inputRequestedRegion);

  // build an exception
  InvalidRequestedRegionError e(__FILE__, __LINE__);
  e.SetLocation(ITK_LOCATION);
  e.SetDescription("Requested region is (at least partially) outside the largest possible region.");
  e.SetDataObject(inputPtr);
  throw e;
}

template <typename TInputImage, typename TRealType, typename TOutputImage>
void
VectorGradientMagnitudeImageFilter<TInputImage, TRealType, TOutputImage>::BeforeThreadedGenerateData()
{
  Superclass::BeforeThreadedGenerateData();

  // Calculate the square-roots of the component weights.
  for (unsigned int i = 0; i < VectorDimension; ++i)
  {
    if (m_ComponentWeights[i] < 0)
    {
      itkExceptionMacro("Component weights must be positive numbers");
    }
    m_SqrtComponentWeights[i] = std::sqrt(m_ComponentWeights[i]);
  }

  // Set the weights on the derivatives.
  // Are we using image spacing in the calculations?  If so we must update now
  // in case our input image has changed.
  if (m_UseImageSpacing)
  {
    const auto & spacing = this->GetInput()->GetSpacing();

    for (unsigned int i = 0; i < ImageDimension; ++i)
    {
      if (static_cast<TRealType>(spacing[i]) == 0.0)
      {
        itkExceptionMacro("Image spacing in dimension " << i << " is zero.");
      }
      m_DerivativeWeights[i] = static_cast<TRealType>(1.0 / static_cast<TRealType>(spacing[i]));
    }
  }

  // If using the principle components method, then force this filter to use a
  // single thread because vnl eigensystem objects are not thread-safe.  3D
  // data is ok because we have a special solver.
  if (m_UsePrincipleComponents && ImageDimension != 3)
  {
    m_RequestedNumberOfWorkUnits = this->GetNumberOfWorkUnits();
    this->SetNumberOfWorkUnits(1);
  }
  else
  {
    this->SetNumberOfWorkUnits(m_RequestedNumberOfWorkUnits);
  }
  //
  // cast might not be necessary, but CastImageFilter is optimized for
  // the case where the InputImageType == OutputImageType
  auto caster = CastImageFilter<TInputImage, RealVectorImageType>::New();
  caster->SetInput(this->GetInput());
  caster->GetOutput()->SetRequestedRegion(this->GetInput()->GetRequestedRegion());
  caster->Update();
  m_RealValuedInputImage = caster->GetOutput();
}

template <typename TInputImage, typename TRealType, typename TOutputImage>
void
VectorGradientMagnitudeImageFilter<TInputImage, TRealType, TOutputImage>::DynamicThreadedGenerateData(
  const OutputImageRegionType & outputRegionForThread)
{
  ZeroFluxNeumannBoundaryCondition<RealVectorImageType> nbc;
  ConstNeighborhoodIteratorType                         bit;
  ImageRegionIterator<TOutputImage>                     it;

  // Find the data-set boundary "faces"
  NeighborhoodAlgorithm::ImageBoundaryFacesCalculator<RealVectorImageType> bC;
  auto                                                                     r1 = MakeFilled<RadiusType>(1);
  const typename NeighborhoodAlgorithm::ImageBoundaryFacesCalculator<RealVectorImageType>::FaceListType faceList =
    bC(m_RealValuedInputImage.GetPointer(), outputRegionForThread, r1);

  TotalProgressReporter progress(this, this->GetOutput()->GetRequestedRegion().GetNumberOfPixels());

  // Process each of the data set faces.  The iterator is reinitialized on each
  // face so that it can determine whether or not to check for boundary
  // conditions.
  for (const auto & face : faceList)
  {
    bit = ConstNeighborhoodIteratorType(r1, m_RealValuedInputImage.GetPointer(), face);
    it = ImageRegionIterator<TOutputImage>(this->GetOutput(), face);
    bit.OverrideBoundaryCondition(&nbc);
    bit.GoToBegin();

    if (m_UsePrincipleComponents)
    {
      if constexpr (ImageDimension == 3)
      { // Use the specialized eigensolve which can be threaded
        while (!bit.IsAtEnd())
        {
          it.Set(this->EvaluateAtNeighborhood3D(bit));
          ++bit;
          ++it;
          progress.CompletedPixel();
        }
      }
      else
      {
        while (!bit.IsAtEnd())
        {
          it.Set(this->EvaluateAtNeighborhood(bit));
          ++bit;
          ++it;
          progress.CompletedPixel();
        }
      }
    }
    else
    {
      while (!bit.IsAtEnd())
      {
        it.Set(this->NonPCEvaluateAtNeighborhood(bit));
        ++bit;
        ++it;
        progress.CompletedPixel();
      }
    }
  }
}

template <typename TInputImage, typename TRealType, typename TOutputImage>
int
VectorGradientMagnitudeImageFilter<TInputImage, TRealType, TOutputImage>::CubicSolver(const double * c, double * s)
{
  // IMPORTANT
  // This code is specialized for particular case of positive symmetric
  // matrix.   It also assumes that x^3 coefficient is 1. c contains the
  // coefficients of the polynomial: x^3 + c[2]x^2 + c[1]x^1 + c[0].  The roots
  // s are not necessarily sorted, and int is the number of distinct roots
  // found in s.

  const double     dpi = itk::Math::pi;
  constexpr double epsilon = 1.0e-11;

  // Substitution of  x = y - c[2]/3 eliminate the quadric term  x^3 +px + q = 0
  const double sq_c2 = c[2] * c[2];
  const double p = 1.0 / 3 * (-1.0 / 3.0 * sq_c2 + c[1]);
  const double q = 1.0 / 2 * (2.0 / 27.0 * c[2] * sq_c2 - 1.0 / 3.0 * c[2] * c[1] + c[0]);

  // Cardano's formula
  const double cb_p = p * p * p;
  const double D = q * q + cb_p;

  int num = 0;
  if (D < -epsilon) // D < 0, three real solutions, by far the common case.
  {
    const double phi = 1.0 / 3.0 * std::acos(-q / std::sqrt(-cb_p));
    const double t = 2.0 * std::sqrt(-p);

    s[0] = t * std::cos(phi);
    s[1] = -t * std::cos(phi + dpi / 3);
    s[2] = -t * std::cos(phi - dpi / 3);
    num = 3;
  }
  else if (D < epsilon) // D == 0
  {
    if (q > -epsilon && q < epsilon)
    {
      s[0] = 0.0;
      num = 1;
    }
    else
    {
      const double u = itk::Math::cbrt(-q);
      s[0] = 2 * u;
      s[1] = -u;
      num = 2;
    }
  }
  else // Only one real solution. This case misses a double root on rare
       // occasions with very large char eqn coefficients.
  {
    const double sqrt_D = std::sqrt(D);
    const double u = itk::Math::cbrt(sqrt_D - q);
    const double v = -itk::Math::cbrt(sqrt_D + q);

    s[0] = u + v;
    num = 1;
  }

  // Resubstitute
  const double sub = 1.0 / 3.0 * c[2];

  for (int i = 0; i < num; ++i)
  {
    s[i] -= sub;
  }

  return num;
}
} // end namespace itk

#endif
