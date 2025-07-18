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
#ifndef itkNeighborhoodOperator_h
#define itkNeighborhoodOperator_h

#include "itkNeighborhood.h"
#include "itkNumericTraits.h"
#include <vector>

namespace itk
{
/** \class NeighborhoodOperator
 * \brief Virtual class that defines a common interface to all
 *        neighborhood operator subtypes.
 *
 * A NeighborhoodOperator is a set of pixel values that can be applied to a
 * Neighborhood to perform a user-defined operation (i.e. convolution kernel,
 * morphological structuring element).  A NeighborhoodOperator is itself a
 * specialized Neighborhood, with functionality to generate its coefficients
 * according to user-defined parameters.  Because the operator is a subclass
 * of Neighborhood, it is a valid operand in any of the operations
 * defined on the Neighborhood object (convolution, inner product, etc.).
 *
 * NeighborhoodOperator is a pure virtual object that must be
 * subclassed to be used.  A user's subclass must implement two methods:
 *
 * (1) GenerateCoefficients -- the algorithm that computes the scalar
 *   coefficients of the operator.
 *
 * (2) Fill -- the algorithm that places the scalar coefficients into
 *   the memory buffer of the operator (arranges them spatially in the
 *   neighborhood).
 *
 * NeighborhoodOperator supports the concept of a "directional operator."
 * A directional operator is defined in this context to be an operator
 * that is applied along a single dimension.  Examples of this type of
 * operator are directional derivatives and the individual, directional
 * components of separable processes such as Gaussian smoothing.
 *
 * How a NeighborhoodOperator is applied to data is up to the user who
 * defines it.  One possible use of an operator would be to take its
 * inner product with a neighborhood of values to produce
 * a scalar result.  This process effects convolution when applied to
 * successive neighborhoods across a region of interest in an image.
 *
 * \note NeighborhoodOperator does not have any user-declared "special member function",
 * following the C++ Rule of Zero: the compiler will generate them if necessary.
 *
 * \ingroup Operators
 * \ingroup ITKCommon
 *
 * \sphinx
 * \sphinxexample{Core/Common/DemonstrateAllOperators,Demonstrate All Operators}
 * \endsphinx
 */
template <typename TPixel, unsigned int VDimension, typename TAllocator = NeighborhoodAllocator<TPixel>>
class ITK_TEMPLATE_EXPORT NeighborhoodOperator : public Neighborhood<TPixel, VDimension, TAllocator>
{
public:
  /** Standard class type aliases. */
  using Self = NeighborhoodOperator;
  using Superclass = Neighborhood<TPixel, VDimension, TAllocator>;

  itkOverrideGetNameOfClassMacro(NeighborhoodOperator);

  /** Size object type alias support */
  using typename Superclass::SizeType;

  /** External support for pixel type */
  using PixelType = TPixel;

  /** Slice iterator type alias support */
  using SliceIteratorType = SliceIterator<TPixel, Self>;

  using PixelRealType = typename NumericTraits<TPixel>::RealType;

  /** Sets the dimensional direction of a directional operator. */
  void
  SetDirection(const unsigned int direction)
  {
    if (direction >= VDimension)
    {
      itkExceptionMacro(<< " Can not set direction " << direction << " greater than dimensionality of neighborhood "
                        << VDimension);
    }
    m_Direction = direction;
  }

  /** Returns the direction (dimension number) of a directional operator. */
  [[nodiscard]] unsigned int
  GetDirection() const
  {
    return m_Direction;
  }

  /** Creates the operator with length only in the specified direction.
   * The radius of the operator will be 0 except along the axis on which
   * the operator will work.
   * \sa CreateToRadius \sa FillCenteredDirectional \sa SetDirection() \sa GetDirection() */
  virtual void
  CreateDirectional();

  /** Creates the operator with a specified radius.  The spatial location of
   * the coefficients within the operator is defined by the subclass
   * implementation of the Fill method.
   * \sa CreateDirectional \sa Fill */
  virtual void
  CreateToRadius(const SizeType &);

  /** Creates the operator with a specified radius ("square", same length
   * on each side). The spatial location of the coefficients within the
   * operator is defined by the subclass implementation of the Fill method.
   * \sa CreateDirectional \sa Fill */
  virtual void
  CreateToRadius(const SizeValueType);

  /** Reverses the direction of all axes of the operator by reversing the order
   * of the coefficients. */
  virtual void
  FlipAxes();

  void
  PrintSelf(std::ostream & os, Indent indent) const override
  {
    Superclass::PrintSelf(os, indent);

    os << indent << "Direction: " << m_Direction << std::endl;
  }

  /** Multiplies all of the coefficients of the kernel by a single scalar value.
   */
  void ScaleCoefficients(PixelRealType);

protected:
  /** Type alias support for coefficient vector type. Necessary
   * to fix bug in the microsoft VC++ compiler. */
  using CoefficientVector = typename std::vector<PixelRealType>;

  /** A subclass-specific algorithm that computes the coefficients
   * of the operator. */
  virtual CoefficientVector
  GenerateCoefficients() = 0;

  /** A subclass-specific algorithm that positions the coefficients
   * spatially in the operator. */
  virtual void
  Fill(const CoefficientVector &) = 0;

  /** A pre-defined Fill function that can be called by a subclass
   * Fill function to center coefficients along the axis specified
   * by the SetDirection method.  Useful for creating directional
   * operators, or centering coefficients in an N-dimensional
   * neighborhood. */
  virtual void
  FillCenteredDirectional(const CoefficientVector &);

  /** Initializes all the coefficients in the neighborhood to zero values */
  void
  InitializeToZero()
  {
    for (unsigned int i = 0; i < this->Size(); ++i)
    {
      this->operator[](i) = PixelType{};
    }
  }

private:
  /** Direction (dimension number) of the derivative. */
  unsigned int m_Direction{ 0 };
};
} // namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#  include "itkNeighborhoodOperator.hxx"
#endif

/*
#ifndef ITK_MANUAL_INSTANTIATION
#include "itkNeighborhoodOperator.hxx"
#endif
*/
#endif
