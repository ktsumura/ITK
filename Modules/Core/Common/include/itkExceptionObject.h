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
#ifndef itkExceptionObject_h
#define itkExceptionObject_h

// NOTE: This itkExceptionObject.h file is included by itkMacro.h, and should never be included directly.
#ifndef allow_inclusion_of_itkExceptionObject_h
#  error "Do not include itkExceptionObject.h directly,  include itkMacro.h instead."
#endif

#include <memory> // For shared_ptr.
#include <string>

namespace itk
{
/** \class ExceptionObject
 * \brief Standard exception handling object.
 *
 * ExceptionObject provides standard methods for throwing
 * and managing exceptions in itk. Specific exceptions should be
 * derived from this class. Note that this class is derived from
 * std::exception, so an application can catch ITK exceptions as
 * std::exception if desired.
 *
 * ExceptionObject maintains two types of information: a location
 * and description (both of which are strings). The location is the
 * point in the code where the exception was thrown; the description
 * is an error message that describes the exception. The ExceptionObject
 * can be thrown explicitly in code, or more conveniently, the
 * itkExceptionMacro (found in Common/itkMacro.h) can be used.
 *
 * \ingroup ITKSystemObjects
 * \ingroup ITKCommon
 */
class ITKCommon_EXPORT ExceptionObject : public std::exception
{
public:
  static constexpr const char * const default_exception_message = "Generic ExceptionObject";
  using Superclass = std::exception;

  /** Explicitly-defaulted default-constructor. Creates an empty exception object. */
  ExceptionObject() noexcept = default;

  explicit ExceptionObject(std::string  file,
                           unsigned int lineNumber = 0,
                           std::string  description = "None",
                           std::string  location = {});

  /** Copy-constructor. */
  ExceptionObject(const ExceptionObject &) noexcept = default;

  /** Move-constructor. */
  ExceptionObject(ExceptionObject &&) noexcept = default;

  /** Copy-assignment operator. */
  ExceptionObject &
  operator=(const ExceptionObject &) noexcept = default;

  /** Move-assignment operator. */
  ExceptionObject &
  operator=(ExceptionObject &&) noexcept = default;

  /** Destructor.
   * \note It appears necessary to define the destructor "out-of-line" for external linkage. */
  ~ExceptionObject() override;

  /** Equivalence operator. */
  virtual bool
  operator==(const ExceptionObject & orig) const;

  /** \see LightObject::GetNameOfClass() */
  itkVirtualGetNameOfClassMacro(ExceptionObject);

  /** Print exception information.  This method can be overridden by
   * specific exception subtypes.  The default is to print out the
   * location where the exception was first thrown and any description
   * provided by the "thrower".   */
  virtual void
  Print(std::ostream & os) const;

  /** Methods to get and set the Location and Description fields. The Set
   * methods are overloaded to support both std::string and const char
   * array types. Get methods return const char arrays. */
  virtual void
  SetLocation(const std::string & s);

  virtual void
  SetDescription(const std::string & s);

  virtual void
  SetLocation(const char * s);

  virtual void
  SetDescription(const char * s);

  [[nodiscard]] virtual const char *
  GetLocation() const;

  [[nodiscard]] virtual const char *
  GetDescription() const;

  /** What file did the exception occur in? */
  [[nodiscard]] virtual const char *
  GetFile() const;

  /** What line did the exception occur in? */
  [[nodiscard]] virtual unsigned int
  GetLine() const;

  /** Provide std::exception::what() implementation. */
  [[nodiscard]] const char *
  what() const noexcept override;

private:
  class ExceptionData;

  std::shared_ptr<const ExceptionData> m_ExceptionData;
};

/** Generic inserter operator for ExceptionObject and its subclasses. */
inline std::ostream &
operator<<(std::ostream & os, const ExceptionObject & e)
{
  e.Print(os);
  return os;
}

/**
 * Specific exception types that are subclasses of ExceptionObject follow
 */

/** \class MemoryAllocationError
 * Exception thrown when image memory allocation fails.
 * \ingroup ITKSystemObjects
 * \ingroup ITKCommon
 */
class ITKCommon_EXPORT MemoryAllocationError : public ExceptionObject
{
public:
  // Inherit the constructors from its base class.
  using ExceptionObject::ExceptionObject;

  /** \see LightObject::GetNameOfClass() */
  itkOverrideGetNameOfClassMacro(MemoryAllocationError);
};

/** \class RangeError
 * Exception thrown when accessing memory out of range.
 * \ingroup ITKSystemObjects
 * \ingroup ITKCommon
 */
class ITKCommon_EXPORT RangeError : public ExceptionObject
{
public:
  // Inherit the constructors from its base class.
  using ExceptionObject::ExceptionObject;

  /** \see LightObject::GetNameOfClass() */
  itkOverrideGetNameOfClassMacro(RangeError);
};

/** \class InvalidArgumentError
 * Exception thrown when invalid argument is given to a method
 * or function.
 * \ingroup ITKSystemObjects
 * \ingroup ITKCommon
 */
class ITKCommon_EXPORT InvalidArgumentError : public ExceptionObject
{
public:
  // Inherit the constructors from its base class.
  using ExceptionObject::ExceptionObject;

  /** \see LightObject::GetNameOfClass() */
  itkOverrideGetNameOfClassMacro(InvalidArgumentError);
};

/** \class IncompatibleOperandsError
 * Exception thrown when two operands are incompatible.
 * \ingroup ITKSystemObjects
 * \ingroup ITKCommon
 */
class ITKCommon_EXPORT IncompatibleOperandsError : public ExceptionObject
{
public:
  // Inherit the constructors from its base class.
  using ExceptionObject::ExceptionObject;

  /** \see LightObject::GetNameOfClass() */
  itkOverrideGetNameOfClassMacro(IncompatibleOperandsError);
};

/** \class ProcessAborted
 * Exception thrown when a filter (actually a ProcessObject) has been aborted.
 * \ingroup ITKSystemObjects
 * \ingroup ITKCommon
 */
class ITKCommon_EXPORT ProcessAborted : public ExceptionObject
{
public:
  /** Default constructor.  Needed to ensure the exception object can be
   * copied. */
  ProcessAborted()
    : ExceptionObject()
  {
    this->SetDescription("Filter execution was aborted by an external request");
  }

  /** Constructor. Needed to ensure the exception object can be copied. */
  ProcessAborted(std::string file, unsigned int lineNumber)
    : ExceptionObject(std::move(file), lineNumber, "Filter execution was aborted by an external request")
  {}

  /** \see LightObject::GetNameOfClass() */
  itkOverrideGetNameOfClassMacro(ProcessAborted);
};

// Forward declaration in Macro.h, implementation here to avoid circular dependency
template <typename TTarget, typename TSource>
TTarget
itkDynamicCastInDebugMode(TSource x)
{
#ifndef NDEBUG
  if (x == nullptr)
  {
    return nullptr;
  }
  TTarget rval = dynamic_cast<TTarget>(x);
  if (rval == nullptr)
  {
    itkGenericExceptionMacro("Failed dynamic cast to " << typeid(TTarget).name()
                                                       << " object type = " << x->GetNameOfClass());
  }
  return rval;
#else
  return static_cast<TTarget>(x);
#endif
}

} // end namespace itk
#endif // itkExceptionObject_h
