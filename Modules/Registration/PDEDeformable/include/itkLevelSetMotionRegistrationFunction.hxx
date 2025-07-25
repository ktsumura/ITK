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
#ifndef itkLevelSetMotionRegistrationFunction_hxx
#define itkLevelSetMotionRegistrationFunction_hxx

#include "itkMacro.h"
#include "itkMath.h"

namespace itk
{

template <typename TFixedImage, typename TMovingImage, typename TDisplacementField>
LevelSetMotionRegistrationFunction<TFixedImage, TMovingImage, TDisplacementField>::LevelSetMotionRegistrationFunction()
{
  RadiusType r;

  for (unsigned int j = 0; j < ImageDimension; ++j)
  {
    r[j] = 0;
  }
  this->SetRadius(r);

  m_Alpha = 0.1;
  m_GradientMagnitudeThreshold = 1e-9;
  m_IntensityDifferenceThreshold = 0.001;
  m_GradientSmoothingStandardDeviations = 1.0;
  this->SetMovingImage(nullptr);
  this->SetFixedImage(nullptr);

  auto interp = DefaultInterpolatorType::New();

  m_MovingImageInterpolator = static_cast<InterpolatorType *>(interp.GetPointer());

  m_Metric = NumericTraits<double>::max();
  m_SumOfSquaredDifference = 0.0;
  m_NumberOfPixelsProcessed = 0L;
  m_RMSChange = NumericTraits<double>::max();
  m_SumOfSquaredChange = 0.0;

  m_MovingImageSmoothingFilter = MovingImageSmoothingFilterType::New();
  m_MovingImageSmoothingFilter->SetSigma(m_GradientSmoothingStandardDeviations);
  m_MovingImageSmoothingFilter->SetNormalizeAcrossScale(false);

  m_SmoothMovingImageInterpolator = static_cast<InterpolatorType *>(DefaultInterpolatorType::New().GetPointer());
}

template <typename TFixedImage, typename TMovingImage, typename TDisplacementField>
void
LevelSetMotionRegistrationFunction<TFixedImage, TMovingImage, TDisplacementField>::PrintSelf(std::ostream & os,
                                                                                             Indent indent) const
{
  Superclass::PrintSelf(os, indent);

  os << indent
     << "FixedImageSpacing: " << static_cast<typename NumericTraits<SpacingType>::PrintType>(m_FixedImageSpacing)
     << std::endl;
  os << indent << "FixedImageOrigin: " << static_cast<typename NumericTraits<PointType>::PrintType>(m_FixedImageOrigin)
     << std::endl;

  itkPrintSelfObjectMacro(MovingImageSmoothingFilter);

  itkPrintSelfObjectMacro(MovingImageInterpolator);
  itkPrintSelfObjectMacro(SmoothMovingImageInterpolator);

  os << indent << "Alpha: " << m_Alpha << std::endl;
  os << indent << "GradientMagnitudeThreshold: " << m_GradientMagnitudeThreshold << std::endl;
  os << indent << "IntensityDifferenceThreshold: " << m_IntensityDifferenceThreshold << std::endl;
  os << indent << "GradientSmoothingStandardDeviations: " << m_GradientSmoothingStandardDeviations << std::endl;

  os << indent << "Metric: " << m_Metric << std::endl;
  os << indent << "SumOfSquaredDifference: " << m_SumOfSquaredDifference << std::endl;
  os << indent << "NumberOfPixelsProcessed: "
     << static_cast<typename NumericTraits<SizeValueType>::PrintType>(m_NumberOfPixelsProcessed) << std::endl;
  os << indent << "RMSChange: " << m_RMSChange << std::endl;
  os << indent << "SumOfSquaredChange: " << m_SumOfSquaredChange << std::endl;

  itkPrintSelfBooleanMacro(UseImageSpacing);
}

template <typename TFixedImage, typename TMovingImage, typename TDisplacementField>
void
LevelSetMotionRegistrationFunction<TFixedImage, TMovingImage, TDisplacementField>::SetAlpha(double alpha)
{
  m_Alpha = alpha;
}

template <typename TFixedImage, typename TMovingImage, typename TDisplacementField>
double
LevelSetMotionRegistrationFunction<TFixedImage, TMovingImage, TDisplacementField>::GetAlpha() const
{
  return m_Alpha;
}

template <typename TFixedImage, typename TMovingImage, typename TDisplacementField>
void
LevelSetMotionRegistrationFunction<TFixedImage, TMovingImage, TDisplacementField>::SetIntensityDifferenceThreshold(
  double threshold)
{
  m_IntensityDifferenceThreshold = threshold;
}

template <typename TFixedImage, typename TMovingImage, typename TDisplacementField>
double
LevelSetMotionRegistrationFunction<TFixedImage, TMovingImage, TDisplacementField>::GetIntensityDifferenceThreshold()
  const
{
  return m_IntensityDifferenceThreshold;
}

template <typename TFixedImage, typename TMovingImage, typename TDisplacementField>
void
LevelSetMotionRegistrationFunction<TFixedImage, TMovingImage, TDisplacementField>::SetGradientMagnitudeThreshold(
  double threshold)
{
  m_GradientMagnitudeThreshold = threshold;
}

template <typename TFixedImage, typename TMovingImage, typename TDisplacementField>
double
LevelSetMotionRegistrationFunction<TFixedImage, TMovingImage, TDisplacementField>::GetGradientMagnitudeThreshold() const
{
  return m_GradientMagnitudeThreshold;
}

template <typename TFixedImage, typename TMovingImage, typename TDisplacementField>
void
LevelSetMotionRegistrationFunction<TFixedImage, TMovingImage, TDisplacementField>::
  SetGradientSmoothingStandardDeviations(double sigma)
{
  m_GradientSmoothingStandardDeviations = sigma;
}

template <typename TFixedImage, typename TMovingImage, typename TDisplacementField>
double
LevelSetMotionRegistrationFunction<TFixedImage, TMovingImage, TDisplacementField>::
  GetGradientSmoothingStandardDeviations() const
{
  return m_GradientSmoothingStandardDeviations;
}

template <typename TFixedImage, typename TMovingImage, typename TDisplacementField>
bool
LevelSetMotionRegistrationFunction<TFixedImage, TMovingImage, TDisplacementField>::GetUseImageSpacing() const
{
  return this->m_UseImageSpacing;
}

template <typename TFixedImage, typename TMovingImage, typename TDisplacementField>
void
LevelSetMotionRegistrationFunction<TFixedImage, TMovingImage, TDisplacementField>::SetUseImageSpacing(
  bool useImageSpacing)
{
  this->m_UseImageSpacing = useImageSpacing;
}

template <typename TFixedImage, typename TMovingImage, typename TDisplacementField>
void
LevelSetMotionRegistrationFunction<TFixedImage, TMovingImage, TDisplacementField>::InitializeIteration()
{
  if (!this->GetMovingImage() || !this->GetFixedImage() || !m_MovingImageInterpolator)
  {
    itkExceptionMacro("MovingImage, FixedImage and/or Interpolator not set");
  }

  // create a smoothed version of the moving image for the calculation
  // of gradients.  due to the pipeline structure, this will only be
  // calculated once. InitializeIteration() is called in a single
  // threaded execution model.
  m_MovingImageSmoothingFilter->SetInput(this->GetMovingImage());
  m_MovingImageSmoothingFilter->SetSigma(m_GradientSmoothingStandardDeviations);
  m_MovingImageSmoothingFilter->Update();

  m_SmoothMovingImageInterpolator->SetInputImage(m_MovingImageSmoothingFilter->GetOutput());

  // setup moving image interpolator
  m_MovingImageInterpolator->SetInputImage(this->GetMovingImage());

  // initialize metric computation variables
  m_SumOfSquaredDifference = 0.0;
  m_NumberOfPixelsProcessed = 0L;
  m_SumOfSquaredChange = 0.0;
}

template <typename TFixedImage, typename TMovingImage, typename TDisplacementField>
auto
LevelSetMotionRegistrationFunction<TFixedImage, TMovingImage, TDisplacementField>::ComputeUpdate(
  const NeighborhoodType & it,
  void *                   gd,
  const FloatOffsetType &  itkNotUsed(offset)) -> PixelType
{
  const IndexType index = it.GetIndex();

  // Get fixed image related information
  // Note: no need to check the index is within
  // fixed image buffer. This is done by the external filter.
  const auto fixedValue = static_cast<double>(this->GetFixedImage()->GetPixel(index));

  // Get moving image related information
  PointType mappedPoint;

  this->GetFixedImage()->TransformIndexToPhysicalPoint(index, mappedPoint);
  for (unsigned int j = 0; j < ImageDimension; ++j)
  {
    mappedPoint[j] += it.GetCenterPixel()[j];
  }
  PixelType update;
  double    movingValue = NAN;
  if (m_MovingImageInterpolator->IsInsideBuffer(mappedPoint))
  {
    movingValue = m_MovingImageInterpolator->Evaluate(mappedPoint);
  }
  else
  {
    update.Fill(0.0);
    return update;
  }

  // Calculate the gradient using minmod finite differences
  //
  //
  //

  // first calculate the forward and backward differences on the
  // smooth image. Do we need to structure the gradient calculation to
  // take into account the Jacobian of the deformation field? i.e. in
  // which coordinate frame do we ultimately want the gradient vector?

  MovingSpacingType mSpacing = this->GetMovingImage()->GetSpacing();

  if (!this->m_UseImageSpacing)
  {
    mSpacing.Fill(1.0);
  }

  PointType    mPoint(mappedPoint);
  const double centralValue = m_SmoothMovingImageInterpolator->Evaluate(mPoint);
  double       forwardDifferences[ImageDimension];
  double       backwardDifferences[ImageDimension];
  for (unsigned int j = 0; j < ImageDimension; ++j)
  {
    mPoint[j] += mSpacing[j];
    if (m_SmoothMovingImageInterpolator->IsInsideBuffer(mPoint))
    {
      forwardDifferences[j] = m_SmoothMovingImageInterpolator->Evaluate(mPoint) - centralValue;
      forwardDifferences[j] /= mSpacing[j];
    }
    else
    {
      forwardDifferences[j] = 0.0;
    }

    mPoint[j] -= (2.0 * mSpacing[j]);
    if (m_SmoothMovingImageInterpolator->IsInsideBuffer(mPoint))
    {
      backwardDifferences[j] = centralValue - m_SmoothMovingImageInterpolator->Evaluate(mPoint);
      backwardDifferences[j] /= mSpacing[j];
    }
    else
    {
      backwardDifferences[j] = 0.0;
    }
    // std::cout << "F(" << j << ") : " << forwardDifferences[j] << std::endl;
    // std::cout << "B(" << j << ") : " << backwardDifferences[j] << std::endl;
    mPoint[j] += mSpacing[j];
  }

  // minmod finite difference
  //
  // m(x,y) = sign(x) min(|x|, |y|)    if xy >  0
  //          0                        if xy <= 0
  //
  // gradient[j] = m(forwardDifferences[j], backwardDifferences[j])
  //
  CovariantVectorType gradient;
  double              gradientMagnitude = 0.0;
  for (unsigned int j = 0; j < ImageDimension; ++j)
  {
    if (forwardDifferences[j] * backwardDifferences[j] > 0.0)
    {
      const double bvalue = itk::Math::abs(backwardDifferences[j]);
      double       gvalue = itk::Math::abs(forwardDifferences[j]);
      if (gvalue > bvalue)
      {
        gvalue = bvalue;
      }
      gradient[j] = gvalue * itk::Math::sgn(forwardDifferences[j]);
    }
    else
    {
      gradient[j] = 0.0;
    }
    gradientMagnitude += itk::Math::sqr(gradient[j]);
  }
  gradientMagnitude = std::sqrt(gradientMagnitude);

  /**
   * Compute Update.
   */
  const double speedValue = fixedValue - movingValue;
  // update the metric
  auto * globalData = (GlobalDataStruct *)gd;
  if (globalData)
  {
    globalData->m_SumOfSquaredDifference += itk::Math::sqr(speedValue);
    globalData->m_NumberOfPixelsProcessed += 1;
  }

  if (itk::Math::abs(speedValue) < m_IntensityDifferenceThreshold || gradientMagnitude < m_GradientMagnitudeThreshold)
  {
    update.Fill(0.0);
    return update;
  }

  double L1norm = 0.0;
  for (unsigned int j = 0; j < ImageDimension; ++j)
  {
    update[j] = speedValue * gradient[j] / (gradientMagnitude + m_Alpha);
    if (globalData)
    {
      globalData->m_SumOfSquaredChange += itk::Math::sqr(update[j]);

      // build up the L1norm of the update, normalized by the pixel
      // spacing. we will use this to calculate a timestep which
      // converts the update (measured in intensity) to a vector
      // measured in physical units (mm).
      L1norm += (itk::Math::abs(update[j]) / mSpacing[j]);
    }
  }

  // Store the L1 norm of the update vector if it is the largest
  // update.  This is used in calculating the timestep.
  if (globalData && (L1norm > globalData->m_MaxL1Norm))
  {
    globalData->m_MaxL1Norm = L1norm;
  }
  return update;
}

template <typename TFixedImage, typename TMovingImage, typename TDisplacementField>
auto
LevelSetMotionRegistrationFunction<TFixedImage, TMovingImage, TDisplacementField>::ComputeGlobalTimeStep(
  void * GlobalData) const -> TimeStepType
{
  TimeStepType dt = 1.0;

  auto * d = (GlobalDataStruct *)GlobalData;

  if (d->m_MaxL1Norm > 0.0)
  {
    dt = 1.0 / d->m_MaxL1Norm;
  }

  return dt;
}

template <typename TFixedImage, typename TMovingImage, typename TDisplacementField>
void
LevelSetMotionRegistrationFunction<TFixedImage, TMovingImage, TDisplacementField>::ReleaseGlobalDataPointer(
  void * gd) const
{
  const std::unique_ptr<const GlobalDataStruct> globalData(static_cast<GlobalDataStruct *>(gd));

  const std::lock_guard<std::mutex> lockGuard(m_MetricCalculationMutex);
  m_SumOfSquaredDifference += globalData->m_SumOfSquaredDifference;
  m_NumberOfPixelsProcessed += globalData->m_NumberOfPixelsProcessed;
  m_SumOfSquaredChange += globalData->m_SumOfSquaredChange;
  if (m_NumberOfPixelsProcessed)
  {
    m_Metric = m_SumOfSquaredDifference / static_cast<double>(m_NumberOfPixelsProcessed);
    m_RMSChange = std::sqrt(m_SumOfSquaredChange / static_cast<double>(m_NumberOfPixelsProcessed));
  }
}
} // end namespace itk

#endif
