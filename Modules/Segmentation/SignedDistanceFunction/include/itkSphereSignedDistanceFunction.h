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
#ifndef itkSphereSignedDistanceFunction_h
#define itkSphereSignedDistanceFunction_h

#include "itkShapeSignedDistanceFunction.h"
#include "itkVector.h"

namespace itk
{
/**
 * \class SphereSignedDistanceFunction
 * \brief Compute the signed distance from a N-dimensional sphere.
 *
 * A instance of sphere is defined by a set parameters. The first parameter
 * is the radius and the next SpaceDimension parameters represent the center.
 * The first parameter forms the set of ShapeParameters and the remaining
 * parameters the set of PoseParameters.
 *
 * This class is templated over the coordinate representation type
 * (e.g. float or double) and the space dimension.
 *
 * \sa ShapeSignedDistanceFunction
 * \ingroup ImageFunctions
 *
 *
 * \ingroup ITKSignedDistanceFunction
 */
template <typename TCoordinate, unsigned int VSpaceDimension>
class ITK_TEMPLATE_EXPORT SphereSignedDistanceFunction
  : public ShapeSignedDistanceFunction<TCoordinate, VSpaceDimension>
{
public:
  ITK_DISALLOW_COPY_AND_MOVE(SphereSignedDistanceFunction);

  /** Standard class type aliases. */
  using Self = SphereSignedDistanceFunction;
  using Superclass = ShapeSignedDistanceFunction<TCoordinate, VSpaceDimension>;
  using Pointer = SmartPointer<Self>;
  using ConstPointer = SmartPointer<const Self>;

  /** \see LightObject::GetNameOfClass() */
  itkOverrideGetNameOfClassMacro(SphereSignedDistanceFunction);

  /** New macro for creation of through the object factory. */
  itkNewMacro(Self);

  /** OutputType type alias support */
  using typename Superclass::OutputType;

  /** InputType type alias support */
  using typename Superclass::InputType;

  /** Dimension underlying input image. */
  static constexpr unsigned int SpaceDimension = Superclass::SpaceDimension;

  /** CoordRep type alias support */
  using typename Superclass::CoordinateType;

  /** Point type alias support */
  using typename Superclass::PointType;

  /** Type of the shape parameters. */
  using typename Superclass::ParametersType;

  /** A sphere is defined by a set of shape parameters. The first parameter
   * is the radius and the next SpaceDimension parameters represent the center. */
  void
  SetParameters(const ParametersType &) override;

  [[nodiscard]] unsigned int
  GetNumberOfShapeParameters() const override
  {
    return 1;
  }
  [[nodiscard]] unsigned int
  GetNumberOfPoseParameters() const override
  {
    return SpaceDimension;
  }

  /** Evaluate the signed distance from a shape at a given position. */
  OutputType
  Evaluate(const PointType & point) const override;

protected:
  SphereSignedDistanceFunction();
  ~SphereSignedDistanceFunction() override = default;

  void
  PrintSelf(std::ostream & os, Indent indent) const override;

private:
  using VectorType = Vector<CoordinateType, Self::SpaceDimension>;

  VectorType m_Translation{};
  double     m_Radius{};
};
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#  include "itkSphereSignedDistanceFunction.hxx"
#endif

#endif
