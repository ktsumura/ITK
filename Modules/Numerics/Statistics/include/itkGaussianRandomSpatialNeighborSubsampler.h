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
#ifndef itkGaussianRandomSpatialNeighborSubsampler_h
#define itkGaussianRandomSpatialNeighborSubsampler_h

#include "itkUniformRandomSpatialNeighborSubsampler.h"

namespace itk::Statistics
{
/**
 * \class GaussianRandomSpatialNeighborSubsampler
 * \brief A subsampler that randomly selects points
 * according to a gaussian distribution
 * within the specified radius of the query point.
 *
 * This class derives from UniformRandomNeighborSubsampler and
 * overrides the GetIntegerVariate method to randomly select
 * points according to a gaussian distribution
 * within the Radius given by SetRadius(radius)
 * as long as that point is also within the RegionConstraint.
 * Use SetVariance(variance) to provide the variance of the
 * gaussian distribution.
 *
 * \sa SubsamplerBase, RegionConstrainedSubsampler
 * \sa SpatialNeighborSubsampler
 * \sa UniformRandomSpatialNeighborSubsampler
 * \ingroup ITKStatistics
 */

template <typename TSample, typename TRegion>
class ITK_TEMPLATE_EXPORT GaussianRandomSpatialNeighborSubsampler
  : public UniformRandomSpatialNeighborSubsampler<TSample, TRegion>
{
public:
  ITK_DISALLOW_COPY_AND_MOVE(GaussianRandomSpatialNeighborSubsampler);

  /** Standard class type aliases */
  using Self = GaussianRandomSpatialNeighborSubsampler<TSample, TRegion>;
  using Superclass = UniformRandomSpatialNeighborSubsampler<TSample, TRegion>;
  using Baseclass = typename Superclass::Baseclass;
  using Pointer = SmartPointer<Self>;
  using ConstPointer = SmartPointer<const Self>;

  /** \see LightObject::GetNameOfClass() */
  itkOverrideGetNameOfClassMacro(GaussianRandomSpatialNeighborSubsampler);

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** type alias alias for the source data container */
  using typename Superclass::SampleType;
  using typename Superclass::SampleConstPointer;
  using typename Superclass::MeasurementVectorType;
  using typename Superclass::InstanceIdentifier;

  using typename Superclass::SubsampleType;
  using typename Superclass::SubsamplePointer;
  using typename Superclass::SubsampleConstIterator;
  using typename Superclass::InstanceIdentifierHolder;

  using typename Superclass::SearchSizeType;
  using typename Superclass::RandomIntType;
  /** type alias related to image region */
  using typename Superclass::RadiusType;
  using typename Superclass::RegionType;
  using typename Superclass::IndexType;
  using typename Superclass::SizeType;
  using typename Superclass::ImageHelperType;

  using RealType = double;

  using typename Superclass::RandomGeneratorType;
  /** Default sampling variance */
  static constexpr int DefaultVariance = 900;

  /** Set the variance */
  itkSetMacro(Variance, RealType);

  /** Get the variance */
  itkGetConstMacro(Variance, RealType);

protected:
  /**
   * Clone the current subsampler.
   * This does a complete copy of the subsampler state
   * to the new subsampler
   */
  [[nodiscard]] typename LightObject::Pointer
  InternalClone() const override;

  GaussianRandomSpatialNeighborSubsampler();
  ~GaussianRandomSpatialNeighborSubsampler() override = default;

  void
  PrintSelf(std::ostream & os, Indent indent) const override;

  /** method to randomly generate an integer in the closed range
   * [0, upperBound]
   * using a gaussian selection method. */
  RandomIntType
  GetIntegerVariate(RandomIntType lowerBound, RandomIntType upperBound, RandomIntType mean) override;

  RealType m_Variance{};
}; // end of class GaussianRandomSpatialNeighborSubsampler

} // namespace itk::Statistics

#ifndef ITK_MANUAL_INSTANTIATION
#  include "itkGaussianRandomSpatialNeighborSubsampler.hxx"
#endif

#endif
