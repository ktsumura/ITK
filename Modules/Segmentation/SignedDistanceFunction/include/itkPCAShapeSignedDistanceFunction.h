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
#ifndef itkPCAShapeSignedDistanceFunction_h
#define itkPCAShapeSignedDistanceFunction_h

#include "itkShapeSignedDistanceFunction.h"
#include "itkImage.h"
#include "itkInterpolateImageFunction.h"
#include "itkExtrapolateImageFunction.h"
#include "itkTransform.h"

namespace itk
{
/**
 * \class PCAShapeSignedDistanceFunction
 * \brief Compute the signed distance from a N-dimensional PCA Shape.
 *
 * This class computes the signed distance from a N-dimensional shape defined
 * by:
 * (1) a mean signed distance image \f$ M(x) \f$,
 * (2) the first \f$ q \f$ principal components images
 * \f$ P_i(x) \f$ and
 * (3) a transform \f$ T(x) \f$ to define the pose
 * (i.e. position or orientation of the shape).
 *
 * A particular instance of the shape is defined by a set of parameters \f$ p \f$.
 * The first \f$ q \f$ parameters defines the weights applied to each principal components
 * and the remaining parameters is used to define the transform. The user
 * should refer to the documentation of the particular Transform class being used.
 * The first set of parameters are called the ShapeParameters and the remaining
 * parameters the PoseParameters.
 *
 * The method Evaluate( point x ) returns the approximate signed to the
 * shape at point x such that:
 *
 * \f[ s = M(T(x)) + \sum_i^{q} p[i] * \sigma[i] * P_i(T(x)) \f]
 *
 * Where \f$\sigma[i]\f$ are the square root of the eigenvalues. These are defined using
 * method SetPrincipalComponentStandardDeviations().
 *
 * This class is templated over the coordinate representation type
 * (e.g. float or double) and the space dimension.
 *
 * \sa ShapeSignedDistanceFunction
 * \sa Transform
 * \ingroup ImageFunctions
 *
 *
 * \ingroup ITKSignedDistanceFunction
 */
template <typename TCoordinate, unsigned int VSpaceDimension, typename TImage = Image<double, VSpaceDimension>>
class ITK_TEMPLATE_EXPORT PCAShapeSignedDistanceFunction
  : public ShapeSignedDistanceFunction<TCoordinate, VSpaceDimension>
{
public:
  ITK_DISALLOW_COPY_AND_MOVE(PCAShapeSignedDistanceFunction);

  /** Standard class type aliases. */
  using Self = PCAShapeSignedDistanceFunction;
  using Superclass = ShapeSignedDistanceFunction<TCoordinate, VSpaceDimension>;

  using Pointer = SmartPointer<Self>;
  using ConstPointer = SmartPointer<const Self>;

  /** \see LightObject::GetNameOfClass() */
  itkOverrideGetNameOfClassMacro(PCAShapeSignedDistanceFunction);

  /** New macro for creation of through the object factory. */
  itkNewMacro(Self);

  /** Dimension underlying input image. */
  static constexpr unsigned int SpaceDimension = Superclass::SpaceDimension;

  /** CoordRep type alias support */
  using typename Superclass::CoordinateType;

  /** InputType type alias support */
  using typename Superclass::InputType;

  /** OutputType type alias support */
  using typename Superclass::OutputType;

  /** Point type alias support */
  using typename Superclass::PointType;

  /** Parameters type alias support */
  using typename Superclass::ParametersType;

  /** Image type alias support */
  using ImageType = TImage;
  using ImagePointer = typename ImageType::Pointer;
  using ImagePointerVector = std::vector<ImagePointer>;

  /** Transform type alias support */
  using TransformType = Transform<CoordinateType, Self::SpaceDimension, Self::SpaceDimension>;

  /** Interpolator type alias support */
  using InterpolatorType = InterpolateImageFunction<ImageType, CoordinateType>;
  using InterpolatorPointer = typename InterpolatorType::Pointer;
  using InterpolatorPointerVector = std::vector<InterpolatorPointer>;

  /** extrapolator type alias support */
  using ExtrapolatorType = ExtrapolateImageFunction<ImageType, CoordinateType>;
  using ExtrapolatorPointer = typename ExtrapolatorType::Pointer;
  using ExtrapolatorPointerVector = std::vector<ExtrapolatorPointer>;

  /** function type alias support */
  using FunctionType = ImageFunction<ImageType, double, CoordinateType>;
  using FunctionPointer = typename FunctionType::Pointer;
  using FunctionPointerVector = std::vector<FunctionPointer>;

  /** Set/Get the number of principal components
   * SetNumberOfPrincipalComponents must be called before SetParameters */
  void
  SetNumberOfPrincipalComponents(unsigned int n);

  itkGetConstMacro(NumberOfPrincipalComponents, unsigned int);

  /** Set/Get the mean image. */
  /** @ITKStartGrouping */
  itkSetObjectMacro(MeanImage, ImageType);
  itkGetModifiableObjectMacro(MeanImage, ImageType);
  /** @ITKEndGrouping */

  /** Set/Get the principal component images. */
  void
  SetPrincipalComponentImages(ImagePointerVector v)
  {
    m_PrincipalComponentImages = v;
  }
  //  ImagePointerVector & GetPrincipalComponentImages()
  //    { return m_PrincipalComponentImages; }

  /** Set/Get the principal component standard deviations. These values corresponds
   * to the square root of the eigenvalues of the principal components. */
  /** @ITKStartGrouping */
  itkSetMacro(PrincipalComponentStandardDeviations, ParametersType);
  itkGetConstMacro(PrincipalComponentStandardDeviations, ParametersType);
  /** @ITKEndGrouping */

  /** Set/Get transform. */
  /** @ITKStartGrouping */
  itkSetObjectMacro(Transform, TransformType);
  itkGetModifiableObjectMacro(Transform, TransformType);
  /** @ITKEndGrouping */

  /** A PCAShape is defined by a set of shape and pose parameters. */
  void
  SetParameters(const ParametersType &) override;

  [[nodiscard]] unsigned int
  GetNumberOfShapeParameters() const override
  {
    return m_NumberOfPrincipalComponents;
  }
  [[nodiscard]] unsigned int
  GetNumberOfPoseParameters() const override
  {
    return m_Transform ? m_Transform->GetNumberOfParameters() : 0;
  }

  /** Evaluate the signed distance from a shape at a given position. */
  OutputType
  Evaluate(const PointType & point) const override;

  /** Initialize must be called before the first call of
   Evaluate() to allow the class to validate any inputs. */
  void
  Initialize() override;

protected:
  PCAShapeSignedDistanceFunction();
  ~PCAShapeSignedDistanceFunction() override = default;

  void
  PrintSelf(std::ostream & os, Indent indent) const override;

private:
  /** intrinsic data members */
  unsigned int m_NumberOfPrincipalComponents{};
  unsigned int m_NumberOfTransformParameters{};

  ImagePointer       m_MeanImage{};
  ImagePointerVector m_PrincipalComponentImages{};
  ParametersType     m_PrincipalComponentStandardDeviations{};

  /** transform and interpolator/extrapolator for image interpolation */
  typename TransformType::Pointer m_Transform{};

  InterpolatorPointerVector m_Interpolators{};
  ExtrapolatorPointerVector m_Extrapolators{};

  /** shape and pose parameters */
  ParametersType m_WeightOfPrincipalComponents{};
  ParametersType m_TransformParameters{};
};
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#  include "itkPCAShapeSignedDistanceFunction.hxx"
#endif

#endif
