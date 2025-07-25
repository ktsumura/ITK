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
#ifndef itkEllipseSpatialObject_h
#define itkEllipseSpatialObject_h

#include "itkSpatialObject.h"
#include "itkAffineTransform.h"
#include "itkFixedArray.h"

namespace itk
{
/**
 * \class EllipseSpatialObject
 *
 * \ingroup ITKSpatialObjects
 *
 * \sphinx
 * \sphinxexample{Core/SpatialObjects/Ellipse,Ellipse}
 * \endsphinx
 */

template <unsigned int TDimension = 3>
class ITK_TEMPLATE_EXPORT EllipseSpatialObject : public SpatialObject<TDimension>
{
public:
  ITK_DISALLOW_COPY_AND_MOVE(EllipseSpatialObject);

  using Self = EllipseSpatialObject;
  using ScalarType = double;
  using Pointer = SmartPointer<Self>;
  using ConstPointer = SmartPointer<const Self>;
  using Superclass = SpatialObject<TDimension>;
  using SuperclassPointer = SmartPointer<Superclass>;
  using typename Superclass::PointType;
  using typename Superclass::TransformType;
  using typename Superclass::BoundingBoxType;
  using PointContainerType = VectorContainer<PointType>;
  using PointContainerPointer = SmartPointer<PointContainerType>;

  using ArrayType = FixedArray<double, TDimension>;

  static constexpr unsigned int ObjectDimension = TDimension;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** \see LightObject::GetNameOfClass() */
  itkOverrideGetNameOfClassMacro(EllipseSpatialObject);

  /** Reset the spatial object to its initial condition, yet preserves
   *   Id, Parent, and Child information */
  void
  Clear() override;

  /** Set all radii to the same radius value
   *
   * Each radius is half the length of one axis of the ellipse.
   *
   * The ellipse is formed by setting the ObjectToParentTransform.
   */
  void
  SetRadiusInObjectSpace(double radius);

  /** Set radii via an array of radius values */
  itkSetMacro(RadiusInObjectSpace, ArrayType);

  /** Get radii via an array of radius values */
  itkGetConstReferenceMacro(RadiusInObjectSpace, ArrayType);

  /** Set center point in object space. */
  itkSetMacro(CenterInObjectSpace, PointType);

  /** Get center in object space */
  itkGetConstReferenceMacro(CenterInObjectSpace, PointType);

  /** Test whether a point is inside the object.
   *
   * For computational speed purposes, it is faster if the method does not  check the name of the class and the
   * current depth.
   */
  bool
  IsInsideInObjectSpace(const PointType & point) const override;

  /* Avoid hiding the overload that supports depth and name arguments */
  using Superclass::IsInsideInObjectSpace;

#if !defined(ITK_LEGACY_REMOVE)
  itkLegacyMacro(void SetRadius(double radius))
  {
    this->SetRadiusInObjectSpace(radius);
  }

  itkLegacyMacro(void SetRadius(ArrayType radii))
  {
    this->SetRadiusInObjectSpace(radii);
  }

  itkLegacyMacro(ArrayType GetRadius() const)
  {
    return this->GetRadiusInObjectSpace();
  }

  itkLegacyMacro(void SetRadiiInObjectSpace(ArrayType radii))
  {
    this->SetRadiusInObjectSpace(radii);
  }
#endif
protected:
  /** Get the boundaries of a specific object.  This function needs to
   *  be called every time one of the object's components is
   *  changed. */
  void
  ComputeMyBoundingBox() override;

  EllipseSpatialObject();
  ~EllipseSpatialObject() override = default;

  void
  PrintSelf(std::ostream & os, Indent indent) const override;

  [[nodiscard]] typename LightObject::Pointer
  InternalClone() const override;

private:
  /* object space */
  ArrayType m_RadiusInObjectSpace{};
  PointType m_CenterInObjectSpace{};
};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#  include "itkEllipseSpatialObject.hxx"
#endif

#endif // itkEllipseSpatialObject_h
