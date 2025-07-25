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
#ifndef itkArithmeticOpsFunctors_h
#define itkArithmeticOpsFunctors_h

#include "itkMath.h"

namespace itk::Functor
{

/**
 * \class Add2
 * \ingroup ITKImageIntensity
 */
template <typename TInput1, typename TInput2 = TInput1, typename TOutput = TInput1>
class ITK_TEMPLATE_EXPORT Add2
{
public:
  bool
  operator==(const Add2 &) const
  {
    return true;
  }

  ITK_UNEQUAL_OPERATOR_MEMBER_FUNCTION(Add2);

  inline TOutput
  operator()(const TInput1 & A, const TInput2 & B) const
  {
    return static_cast<TOutput>(A + B);
  }
};


/**
 * \class Add3
 * \brief
 * \ingroup ITKImageIntensity
 */
template <typename TInput1, typename TInput2, typename TInput3, typename TOutput>
class ITK_TEMPLATE_EXPORT Add3
{
public:
  bool
  operator==(const Add3 &) const
  {
    return true;
  }

  ITK_UNEQUAL_OPERATOR_MEMBER_FUNCTION(Add3);

  inline TOutput
  operator()(const TInput1 & A, const TInput2 & B, const TInput3 & C) const
  {
    return static_cast<TOutput>(A + B + C);
  }
};


/**
 * \class Sub2
 * \brief
 * \ingroup ITKImageIntensity
 */
template <typename TInput1, typename TInput2 = TInput1, typename TOutput = TInput1>
class ITK_TEMPLATE_EXPORT Sub2
{
public:
  bool
  operator==(const Sub2 &) const
  {
    return true;
  }

  ITK_UNEQUAL_OPERATOR_MEMBER_FUNCTION(Sub2);

  inline TOutput
  operator()(const TInput1 & A, const TInput2 & B) const
  {
    return static_cast<TOutput>(A - B);
  }
};


/**
 * \class Mult
 * \brief
 * \ingroup ITKImageIntensity
 */
template <typename TInput1, typename TInput2 = TInput1, typename TOutput = TInput1>
class ITK_TEMPLATE_EXPORT Mult
{
public:
  bool
  operator==(const Mult &) const
  {
    return true;
  }

  ITK_UNEQUAL_OPERATOR_MEMBER_FUNCTION(Mult);

  inline TOutput
  operator()(const TInput1 & A, const TInput2 & B) const
  {
    return static_cast<TOutput>(A * B);
  }
};


/**
 * \class Div
 * \brief
 * \ingroup ITKImageIntensity
 */
template <typename TInput1, typename TInput2, typename TOutput>
class ITK_TEMPLATE_EXPORT Div
{
public:
  bool
  operator==(const Div &) const
  {
    return true;
  }

  ITK_UNEQUAL_OPERATOR_MEMBER_FUNCTION(Div);

  inline TOutput
  operator()(const TInput1 & A, const TInput2 & B) const
  {
    if (itk::Math::NotAlmostEquals(B, TInput2{}))
    {
      return (TOutput)(A / B);
    }

    return NumericTraits<TOutput>::max(static_cast<TOutput>(A));
  }
};


/**
 * \class DivideOrZeroOut
 * \brief
 * \ingroup ITKImageIntensity
 */
template <typename TNumerator, typename TDenominator = TNumerator, typename TOutput = TNumerator>
class ITK_TEMPLATE_EXPORT DivideOrZeroOut
{
public:
  DivideOrZeroOut()
    : m_Threshold(1e-5 * NumericTraits<TDenominator>::OneValue())
    , m_Constant(TOutput{})
  {}

  ~DivideOrZeroOut() = default;

  bool
  operator==(const DivideOrZeroOut & itkNotUsed(other)) const
  {
    // Always return true for now.  Do a comparison to m_Threshold if it is
    // every made set-able.
    return true;
  }

  ITK_UNEQUAL_OPERATOR_MEMBER_FUNCTION(DivideOrZeroOut);

  inline TOutput
  operator()(const TNumerator & n, const TDenominator & d) const
  {
    if (d < m_Threshold)
    {
      return m_Constant;
    }
    return static_cast<TOutput>(n) / static_cast<TOutput>(d);
  }
  TDenominator m_Threshold;
  TOutput      m_Constant;
};


/**
 * \class Modulus
 *
 * \ingroup ITKImageIntensity
 */
template <typename TInput1, typename TInput2, typename TOutput>
class ITK_TEMPLATE_EXPORT Modulus
{
public:
  bool
  operator==(const Modulus &) const
  {
    return true;
  }

  ITK_UNEQUAL_OPERATOR_MEMBER_FUNCTION(Modulus);

  inline TOutput
  operator()(const TInput1 & A, const TInput2 & B) const
  {
    if (B != TInput2{})
    {
      return static_cast<TOutput>(A % B);
    }

    return NumericTraits<TOutput>::max(static_cast<TOutput>(A));
  }
};

#if !defined(ITK_FUTURE_LEGACY_REMOVE)

/**
 * \class ModulusTransform
 *
 * \deprecated The two template parameter ModulusTransform functor
 * is deprecated. Please use the version with 3 template parameters.
 *
 * \ingroup ITKImageIntensity
 */
template <typename TInput, typename TOutput>
class ITK_TEMPLATE_EXPORT ModulusTransform
{
public:
  ModulusTransform() { m_Dividend = 5; }
  ~ModulusTransform() = default;
  void
  SetDividend(TOutput dividend)
  {
    m_Dividend = dividend;
  }

  bool
  operator==(const ModulusTransform & other) const
  {
    return m_Dividend == other.m_Dividend;
  }

  ITK_UNEQUAL_OPERATOR_MEMBER_FUNCTION(ModulusTransform);

  inline TOutput
  operator()(const TInput & x) const
  {
    auto result = static_cast<TOutput>(x % m_Dividend);

    return result;
  }

private:
  TInput m_Dividend;
};

#endif

/**
 * \class DivFloor
 * \brief Cast arguments to double, performs division then takes the floor.
 *
 * This division operator is intended to implement Pythons PEP 238
 * "Floor Division"-style operation.
 *
 * \ingroup ITKImageIntensity
 */
template <class TInput1, class TInput2, class TOutput>
class DivFloor
{
public:
  bool
  operator==(const DivFloor &) const
  {
    return true;
  }

  ITK_UNEQUAL_OPERATOR_MEMBER_FUNCTION(DivFloor);

  inline TOutput
  operator()(const TInput1 & A, const TInput2 & B) const
  {
    const double temp = std::floor(static_cast<double>(A) / static_cast<double>(B));
    if (std::is_integral_v<TOutput> && Math::isinf(temp))
    {
      if (temp > 0)
      {
        return NumericTraits<TOutput>::max(A);
      }

      return NumericTraits<TOutput>::NonpositiveMin(A);
    }
    return static_cast<TOutput>(temp);
  }
};

/**
 * \class DivReal
 * \brief Promotes arguments to real type and performs division.
 *
 * This division operator is intended to implement Pythons PEP 238
 * "True Division". The output type is intended to be a real
 * type. Division by zero is expected to produce an "inf" floating
 * point value.
 *
 * \ingroup ITKImageIntensity
 */
template <class TInput1, class TInput2, class TOutput>
class DivReal
{
public:
  // Use default copy, assigned and destructor
  bool
  operator==(const DivReal &) const
  {
    return true;
  }

  ITK_UNEQUAL_OPERATOR_MEMBER_FUNCTION(DivReal);

  inline TOutput
  operator()(const TInput1 & A, const TInput2 & B) const
  {
    return static_cast<TOutput>(static_cast<typename NumericTraits<TInput1>::RealType>(A) /
                                static_cast<typename NumericTraits<TInput2>::RealType>(B));
  }
};
/**
 * \class UnaryMinus
 * \brief Apply the unary minus operator
 *
 * Assumed that the output type is signed.
 * \ingroup ITKImageIntensity
 */
template <class TInput1, class TOutput = TInput1>
class UnaryMinus
{
public:
  bool
  operator==(const UnaryMinus &) const
  {
    return true;
  }

  ITK_UNEQUAL_OPERATOR_MEMBER_FUNCTION(UnaryMinus);

  inline TOutput
  operator()(const TInput1 & A) const
  {
    return (TOutput)(-A);
  }
};
} // namespace itk::Functor


#endif
