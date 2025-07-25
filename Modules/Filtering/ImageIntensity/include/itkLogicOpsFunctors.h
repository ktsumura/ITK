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
#ifndef itkLogicOpsFunctors_h
#define itkLogicOpsFunctors_h

#include "itkNumericTraits.h"
#include "itkMath.h"

namespace itk::Functor
{

/**
 * \class LogicOpBase
 * \brief Base class for some logic functors. Provides the Foreground
 * and background setting methods.
 *
 * The derived classes can be used as follows:
 *
 *  using myFilterType = itk::BinaryFunctorImageFilter<
 *    myImageType1,
 *    myImageType2,
 *    myImageType3,
 *    itk::Functor::Equal2<myImageType1::PixelType,
 *                          myImageType2::PixelType,
 *                          myImageType3::PixelType> >;
 *
 *  using myFilterTypePointer = myFilterType::Pointer;
 *
 *  myFilterTypePointer filter = myFilterType::New();
 *
 *  filter->SetInput1( inputImageA );
 *  filter->SetInput2( inputImageB );
 *
 *
 *  filter->SetConstant1(3.0);
 *  filter->SetInput2(inputImageB);
 *
 * \ingroup ITKImageIntensity
 */

template <typename TInput1, typename TInput2 = TInput1, typename TOutput = TInput1>
class ITK_TEMPLATE_EXPORT LogicOpBase
{
public:
  using Self = LogicOpBase;
  LogicOpBase()
    : m_ForegroundValue(itk::NumericTraits<TOutput>::OneValue())
    , m_BackgroundValue(TOutput{})
  {}

  ~LogicOpBase() = default;

  bool
  operator==(const Self &) const
  {
    return true;
  }

  ITK_UNEQUAL_OPERATOR_MEMBER_FUNCTION(Self);

  void
  SetForegroundValue(const TOutput & FG)
  {
    m_ForegroundValue = FG;
  }
  void
  SetBackgroundValue(const TOutput & BG)
  {
    m_BackgroundValue = BG;
  }

  [[nodiscard]] TOutput
  GetForegroundValue() const
  {
    return (m_ForegroundValue);
  }
  [[nodiscard]] TOutput
  GetBackgroundValue() const
  {
    return (m_BackgroundValue);
  }

protected:
  TOutput m_ForegroundValue;
  TOutput m_BackgroundValue;
};

/**
 * \class Equal
 * \brief Functor for == operation on images and constants.
 *
 * Operations by c++ casting defaults. Foreground and background
 * values are set by methods. Defaults are 1, 0.
 *
 * \ingroup ITKImageIntensity
 */

template <typename TInput1, typename TInput2 = TInput1, typename TOutput = TInput1>
class ITK_TEMPLATE_EXPORT Equal : public LogicOpBase<TInput1, TInput2, TOutput>
{
public:
  using Self = Equal;

  bool
  operator==(const Self &) const
  {
    return true;
  }

  ITK_UNEQUAL_OPERATOR_MEMBER_FUNCTION(Self);

  inline TOutput
  operator()(const TInput1 & A, const TInput2 & B) const
  {
    if (Math::ExactlyEquals(A, static_cast<TInput1>(B)))
    {
      return this->m_ForegroundValue;
    }
    return this->m_BackgroundValue;
  }
};
/**
 * \class NotEqual
 * \brief Functor for != operation on images and constants.
 *
 * Operations by c++ casting defaults. Foreground and background
 * values are set by methods. Defaults are 1, 0.
 *
 * \ingroup ITKImageIntensity
 */

template <typename TInput1, typename TInput2 = TInput1, typename TOutput = TInput1>
class ITK_TEMPLATE_EXPORT NotEqual : public LogicOpBase<TInput1, TInput2, TOutput>
{
public:
  using Self = NotEqual;

  bool
  operator==(const Self &) const
  {
    return true;
  }

  ITK_UNEQUAL_OPERATOR_MEMBER_FUNCTION(Self);

  inline TOutput
  operator()(const TInput1 & A, const TInput2 & B) const
  {
    if (Math::NotExactlyEquals(A, B))
    {
      return this->m_ForegroundValue;
    }
    return this->m_BackgroundValue;
  }
};

/**
 * \class GreaterEqual
 * \brief Functor for >= operation on images and constants.
 *
 * Operations by c++ casting defaults. Foreground and background
 * values are set by methods. Defaults are 1, 0.
 *
 * \ingroup ITKImageIntensity
 */

template <typename TInput1, typename TInput2 = TInput1, typename TOutput = TInput1>
class ITK_TEMPLATE_EXPORT GreaterEqual : public LogicOpBase<TInput1, TInput2, TOutput>
{
public:
  using Self = GreaterEqual;

  bool
  operator==(const Self &) const
  {
    return true;
  }

  ITK_UNEQUAL_OPERATOR_MEMBER_FUNCTION(Self);

  inline TOutput
  operator()(const TInput1 & A, const TInput2 & B) const
  {
    if (A >= B)
    {
      return this->m_ForegroundValue;
    }
    return this->m_BackgroundValue;
  }
};


/**
 * \class Greater
 * \brief Functor for > operation on images and constants.
 *
 * Operations by c++ casting defaults. Foreground and background
 * values are set by methods. Defaults are 1, 0.
 *
 * \ingroup ITKImageIntensity
 */
template <typename TInput1, typename TInput2 = TInput1, typename TOutput = TInput1>
class ITK_TEMPLATE_EXPORT Greater : public LogicOpBase<TInput1, TInput2, TOutput>
{
public:
  using Self = Greater;

  bool
  operator==(const Self &) const
  {
    return true;
  }

  ITK_UNEQUAL_OPERATOR_MEMBER_FUNCTION(Self);

  inline TOutput
  operator()(const TInput1 & A, const TInput2 & B) const
  {
    if (A > B)
    {
      return this->m_ForegroundValue;
    }
    return this->m_BackgroundValue;
  }
};


/**
 * \class LessEqual
 * \brief Functor for <= operation on images and constants.
 *
 * Operations by c++ casting defaults. Foreground and background
 * values are set by methods. Defaults are 1, 0.
 *
 * \ingroup ITKImageIntensity
 */
template <typename TInput1, typename TInput2 = TInput1, typename TOutput = TInput1>
class ITK_TEMPLATE_EXPORT LessEqual : public LogicOpBase<TInput1, TInput2, TOutput>
{
public:
  using Self = LessEqual;

  bool
  operator==(const Self &) const
  {
    return true;
  }

  ITK_UNEQUAL_OPERATOR_MEMBER_FUNCTION(Self);

  inline TOutput
  operator()(const TInput1 & A, const TInput2 & B) const
  {
    if (A <= B)
    {
      return this->m_ForegroundValue;
    }
    return this->m_BackgroundValue;
  }
};


/**
 * \class Less
 * \brief Functor for < operation on images and constants.
 *
 * Operations by c++ casting defaults. Foreground and background
 * values are set by methods. Defaults are 1, 0.
 *
 * \ingroup ITKImageIntensity
 */
template <typename TInput1, typename TInput2 = TInput1, typename TOutput = TInput1>
class ITK_TEMPLATE_EXPORT Less : public LogicOpBase<TInput1, TInput2, TOutput>
{
public:
  using Self = Less;

  bool
  operator==(const Self &) const
  {
    return true;
  }

  ITK_UNEQUAL_OPERATOR_MEMBER_FUNCTION(Self);

  inline TOutput
  operator()(const TInput1 & A, const TInput2 & B) const
  {
    if (A < B)
    {
      return this->m_ForegroundValue;
    }
    return this->m_BackgroundValue;
  }
};


/**
 * \class NOT
 * \brief Unary logical NOT functor
 * \ingroup ITKImageIntensity
 */
template <typename TInput, typename TOutput = TInput>
class ITK_TEMPLATE_EXPORT NOT : public LogicOpBase<TInput, TInput, TOutput>
{
public:
  bool
  operator==(const NOT &) const
  {
    return true;
  }

  ITK_UNEQUAL_OPERATOR_MEMBER_FUNCTION(NOT);

  inline TOutput
  operator()(const TInput & A) const
  {
    if (!A)
    {
      return this->m_ForegroundValue;
    }
    return this->m_BackgroundValue;
  }
};

/**
 * \class TernaryOperator
 * \brief Return argument 2 if argument 1 is false, and argument 3 otherwise.
 * \ingroup ITKImageIntensity
 */
template <typename TInput1, typename TInput2, typename TInput3, typename TOutput>
class ITK_TEMPLATE_EXPORT TernaryOperator
{
public:
  bool
  operator==(const TernaryOperator &) const
  {
    return true;
  }

  ITK_UNEQUAL_OPERATOR_MEMBER_FUNCTION(TernaryOperator);

  inline TOutput
  operator()(const TInput1 & A, const TInput2 & B, const TInput3 & C) const
  {
    if (A)
    {
      return static_cast<TOutput>(B);
    }

    return static_cast<TOutput>(C);
  }
};

} // namespace itk::Functor


#endif
