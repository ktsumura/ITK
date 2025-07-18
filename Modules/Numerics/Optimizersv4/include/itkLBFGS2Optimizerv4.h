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
#ifndef itkLBFGS2Optimizerv4_h
#define itkLBFGS2Optimizerv4_h

#include "itkGradientDescentOptimizerv4.h"
#include "ITKOptimizersv4Export.h"
#include <memory>

#include "itk_lbfgs.h"

namespace itk
{
/*** \class LBFGS2Optimizerv4Enums
 * \brief Scoped Enum classes for LBFGS2Optimizerv4Template class
 * \ingroup ITKOptimizersv4
 */
class LBFGS2Optimizerv4Enums
{
public:
  /*** \class LineSearchMethod
   * \ingroup ITKOptimizersv4
   * Line search method enum
   */
  enum class LineSearchMethod : uint8_t
  {
    /** The default algorithm (MoreThuente method). */
    LINESEARCH_DEFAULT = 0,

    /** MoreThuente method proposed by More and Thuente \cite more1994. */
    LINESEARCH_MORETHUENTE = 0,

    /**
     * Backtracking method with the Armijo condition.
     *  The backtracking method finds the step length such that it satisfies
     *  the sufficient decrease (Armijo) condition,
     *    - f(x + a * d) <= f(x) + lbfgs_parameter_t::ftol * a * g(x)^T d,
     *
     *  where x is the current point, d is the current search direction, and
     *  a is the step length.
     */
    LINESEARCH_BACKTRACKING_ARMIJO = 1,

    /** The backtracking method with the default (regular Wolfe) condition. */
    LINESEARCH_BACKTRACKING = 2,

    /**
     * Backtracking method with regular Wolfe condition.
     *  The backtracking method finds the step length such that it satisfies
     *  both the Armijo condition (LINESEARCH_BACKTRACKING_ARMIJO)
     *  and the curvature condition,
     *    - g(x + a * d)^T d >= lbfgs_parameter_t::wolfe * g(x)^T d,
     *
     *  where x is the current point, d is the current search direction, and
     *  a is the step length.
     */
    LINESEARCH_BACKTRACKING_WOLFE = 2,

    /**
     * Backtracking method with strong Wolfe condition.
     *  The backtracking method finds the step length such that it satisfies
     *  both the Armijo condition (LINESEARCH_BACKTRACKING_ARMIJO)
     *  and the following condition,
     *    - |g(x + a * d)^T d| <= lbfgs_parameter_t::wolfe * |g(x)^T d|,
     *
     *  where x is the current point, d is the current search direction, and
     *  a is the step length.
     */
    LINESEARCH_BACKTRACKING_STRONG_WOLFE = 3,
  };
};
// Define how to print enumeration
extern ITKOptimizersv4_EXPORT std::ostream &
                              operator<<(std::ostream & out, LBFGS2Optimizerv4Enums::LineSearchMethod value);

/**
 * \class LBFGS2Optimizerv4Template
 * \brief Wrap of the [libLBFGS](https://www.chokkan.org/software/liblbfgs/) algorithm for use in ITKv4 registration
 * framework. LibLBFGS is a translation of LBFGS code by Nocedal [NETLIB
 * lbfgs](http://users.iems.northwestern.edu/~nocedal/lbfgs.html) and adds the orthantwise limited-memory Quasi-Newton
 * method \cite galen2007 for optimization with L1-norm on the parameters.
 *
 * LBFGS is a quasi-Newton method uses an approximate estimate of the inverse Hessian
 * \f$ (\nabla^2 f(x) )^-1 \f$ to scale the gradient step:
 * \f[
 * x_{n+1} = x_n - s (\nabla^2 f(x_n) )^-1 \nabla f(x)
 * \f]
 * with \f$ s \f$ the step size.
 *
 * The inverse Hessian is approximated from the gradients of previous iteration and
 * thus only the gradient of the objective function is required.
 *
 * The step size \f$ s \f$ is determined through line search which defaults to
 * the approach by More and Thuente  \cite more1994. This line search approach finds a step
 * size such that
 * \f[
 * \| \nabla f(x + s (\nabla^2 f(x_n) )^{-1} \nabla f(x) ) \|
 *   \le
 * \nu \| \nabla f(x) \|
 * \f]
 * The parameter \f$\nu\f$ is set through SetLineSearchAccuracy() (default 0.9)
 * and SetGradientLineSearchAccuracy()
 *
 * Instead of the More-Tunete method, backtracking with three different
 * conditions \cite dennis1983 are available and can be set through SetLineSearch():
 *  - LINESEARCH_BACKTRACKING_ARMIJO
 *  - LINESEARCH_BACKTRACKING_WOLFE
 *  - LINESEARCH_BACKTRACKING_STRONG_WOLFE
 *
 * The optimization stops when either the gradient satisfies the condition
 * \f[
 * \| \nabla f(x) \| \le \epsilon \max(1, \| X \|)
 * \f]
 * or a maximum number of function evaluations has been reached.
 * The tolerance \f$\epsilon\f$ is set through SetSolutionAccuracy()
 * (default 1e-5) and the maximum number of function evaluations is set
 * through SetMaximumIterations() (default 0 = no maximum).
 *
 *
 * For algorithmic details see
 * [libLBFGS](https://www.chokkan.org/software/liblbfgs/), [NETLIB
 * lbfgs](http://users.iems.northwestern.edu/~nocedal/lbfgs.html),
 * \cite galen2007, \cite nocedal1980, \cite liu1989,
 * \cite more1994, and \cite dennis1983.
 *
 * \ingroup ITKOptimizersv4
 */
template <typename TInternalComputationValueType>
class ITK_TEMPLATE_EXPORT LBFGS2Optimizerv4Template
  : public GradientDescentOptimizerv4Template<TInternalComputationValueType>
{
public:
  ITK_DISALLOW_COPY_AND_MOVE(LBFGS2Optimizerv4Template);

  using LineSearchMethodEnum = LBFGS2Optimizerv4Enums::LineSearchMethod;
#if !defined(ITK_LEGACY_REMOVE)
  /**Exposes enums values for backwards compatibility*/
  static constexpr LineSearchMethodEnum LINESEARCH_DEFAULT = LineSearchMethodEnum::LINESEARCH_DEFAULT;
  static constexpr LineSearchMethodEnum LINESEARCH_MORETHUENTE = LineSearchMethodEnum::LINESEARCH_MORETHUENTE;
  static constexpr LineSearchMethodEnum LINESEARCH_BACKTRACKING_ARMIJO =
    LineSearchMethodEnum::LINESEARCH_BACKTRACKING_ARMIJO;
  static constexpr LineSearchMethodEnum LINESEARCH_BACKTRACKING = LineSearchMethodEnum::LINESEARCH_BACKTRACKING;
  static constexpr LineSearchMethodEnum LINESEARCH_BACKTRACKING_WOLFE =
    LineSearchMethodEnum::LINESEARCH_BACKTRACKING_WOLFE;
  static constexpr LineSearchMethodEnum LINESEARCH_BACKTRACKING_STRONG_WOLFE =
    LineSearchMethodEnum::LINESEARCH_BACKTRACKING_STRONG_WOLFE;
#endif

  /**
   * TODO: currently only double is used in lbfgs need to figure
   * out how to make it a template parameter and set the required
   * define so lbfgs.h uses the correct version
   **/
  using PrecisionType = double;
  static_assert(std::is_same<TInternalComputationValueType, double>::value,
                "LBFGS2Optimizerv4Template only supports double precision");

  /** Standard "Self" type alias. */
  using Self = LBFGS2Optimizerv4Template;
  using Superclass = GradientDescentOptimizerv4Template<TInternalComputationValueType>;
  using Pointer = SmartPointer<Self>;
  using ConstPointer = SmartPointer<const Self>;

  using MetricType = typename Superclass::MetricType;
  using ParametersType = typename Superclass::ParametersType;
  using ScalesType = typename Superclass::ScalesType;

  /** Stop condition return string type */
  using typename Superclass::StopConditionReturnStringType;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** \see LightObject::GetNameOfClass() */
  itkOverrideGetNameOfClassMacro(LBFGS2Optimizerv4Template);

  /** Start optimization with an initial value. */
  void
  StartOptimization(bool doOnlyInitialization = false) override;

  /** Resume optimization.
   * This runs the optimization loop, and allows continuation
   * of stopped optimization */
  void
  ResumeOptimization() override;

  StopConditionReturnStringType
  GetStopConditionDescription() const override;

  /**
   * Set/Get the number of corrections to approximate the inverse hessian matrix.
   * The L-BFGS routine stores the computation results of previous \c m
   * iterations to approximate the inverse hessian matrix of the current
   * iteration. This parameter controls the size of the limited memories
   * (corrections). The default value is \c 6. Values less than \c 3 are
   * not recommended. Large values will result in excessive computing time.
   */
  /** @ITKStartGrouping */
  void
  SetHessianApproximationAccuracy(int m);
  [[nodiscard]] int
  GetHessianApproximationAccuracy() const;
  /** @ITKEndGrouping */
  /**
   * Set/Get epsilon for convergence test.
   * This parameter determines the accuracy with which the solution is to
   * be found. A minimization terminates when
   * \f$||g|| < \epsilon * max(1, ||x||)\f$,
   * where ||.|| denotes the Euclidean (L2) norm. The default value is
   * \c 1e-5.
   */
  /** @ITKStartGrouping */
  void
  SetSolutionAccuracy(PrecisionType epsilon);
  [[nodiscard]] PrecisionType
  GetSolutionAccuracy() const;
  /** @ITKEndGrouping */
  /**
   * Set/Get distance for delta-based convergence test.
   * This parameter determines the distance, in iterations, to compute
   * the rate of decrease of the objective function. If the value of this
   * parameter is zero, the library does not perform the delta-based
   * convergence test. The default value is \c 0.
   */
  /** @ITKStartGrouping */
  void
  SetDeltaConvergenceDistance(int nPast);
  [[nodiscard]] int
  GetDeltaConvergenceDistance() const;
  /** @ITKEndGrouping */
  /**
   * Delta for convergence test.
   * This parameter determines the minimum rate of decrease of the
   * objective function. The library stops iterations when the
   * following condition is met:
   * \f$(f' - f) / f < \delta\f$,
   * where f' is the objective value of past iterations ago, and f is
   * the objective value of the current iteration.
   * The default value is \c 0.
   */
  /** @ITKStartGrouping */
  void
  SetDeltaConvergenceTolerance(PrecisionType tol);
  [[nodiscard]] PrecisionType
  GetDeltaConvergenceTolerance() const;
  /** @ITKEndGrouping */
  /**
   * The maximum number of iterations.
   *  The lbfgs() function terminates an optimization process with
   *  \c LBFGSERR_MAXIMUMITERATION status code when the iteration count
   *  exceeds this parameter. Setting this parameter to zero continues an
   *  optimization process until a convergence or error. The default value
   *  is \c 0.
   */
  /** @ITKStartGrouping */
  void
  SetMaximumIterations(int maxIterations);
  [[nodiscard]] int
  GetMaximumIterations() const;
  /** @ITKEndGrouping */
  /** Aliased to Set/Get MaximumIterations to match base class interface.
   */
  /** @ITKStartGrouping */
  [[nodiscard]] SizeValueType
  GetNumberOfIterations() const override
  {
    return GetMaximumIterations();
  }
  void
  SetNumberOfIterations(const SizeValueType _arg) override
  {
    SetMaximumIterations(static_cast<int>(_arg));
  }
  /** @ITKEndGrouping */
  /**
   * The line search algorithm.
   * This parameter specifies a line search algorithm to be used by the
   * L-BFGS routine. See lbfgs.h for enumeration of line search type.
   * Defaults to More-Thuente's method.
   */
  /** @ITKStartGrouping */
  void
  SetLineSearch(const LineSearchMethodEnum & linesearch);
  [[nodiscard]] LineSearchMethodEnum
  GetLineSearch() const;
  /** @ITKEndGrouping */
  /**
   * The maximum number of trials for the line search.
   *  This parameter controls the number of function and gradients evaluations
   *  per iteration for the line search routine. The default value is \c 20.
   */
  /** @ITKStartGrouping */
  void
  SetMaximumLineSearchEvaluations(int n);
  [[nodiscard]] int
  GetMaximumLineSearchEvaluations() const;
  /** @ITKEndGrouping */
  /**
   * The minimum step of the line search routine.
   *  The default value is \c 1e-20. This value need not be modified unless
   *  the exponents are too large for the machine being used, or unless the
   *  problem is extremely badly scaled (in which case the exponents should
   *  be increased).
   */
  /** @ITKStartGrouping */
  void
  SetMinimumLineSearchStep(PrecisionType step);
  [[nodiscard]] PrecisionType
  GetMinimumLineSearchStep() const;
  /** @ITKEndGrouping */
  /**
   * The maximum step of the line search.
   *  The default value is \c 1e+20. This value need not be modified unless
   *  the exponents are too large for the machine being used, or unless the
   *  problem is extremely badly scaled (in which case the exponents should
   *  be increased).
   */
  /** @ITKStartGrouping */
  void
  SetMaximumLineSearchStep(PrecisionType step);
  [[nodiscard]] PrecisionType
  GetMaximumLineSearchStep() const;
  /** @ITKEndGrouping */
  /**
   * A parameter to control the accuracy of the line search routine.
   *  The default value is \c 1e-4. This parameter should be greater
   *  than zero and smaller than \c 0.5.
   */
  /** @ITKStartGrouping */
  void
  SetLineSearchAccuracy(PrecisionType ftol);
  [[nodiscard]] PrecisionType
  GetLineSearchAccuracy() const;
  /** @ITKEndGrouping */
  /**
   * A coefficient for the Wolfe condition.
   *  This parameter is valid only when the backtracking line-search
   *  algorithm is used with the Wolfe condition,
   *  LINESEARCH_BACKTRACKING_STRONG_WOLFE or
   *  LINESEARCH_BACKTRACKING_WOLFE .
   *  The default value is \c 0.9. This parameter should be greater
   *  than the \c ftol parameter and smaller than \c 1.0.
   */
  /** @ITKStartGrouping */
  void
  SetWolfeCoefficient(PrecisionType wc);
  [[nodiscard]] PrecisionType
  GetWolfeCoefficient() const;
  /** @ITKEndGrouping */
  /**
   * A parameter to control the gradient accuracy of the More-Thuente
   * line search routine.
   * The default value is \c 0.9. If the function and gradient
   * evaluations are inexpensive with respect to the cost of the
   * iteration (which is sometimes the case when solving very large
   * problems) it may be advantageous to set this parameter to a small
   * value. A typical small value is \c 0.1. This parameter should be
   * greater than the \c ftol parameter (\c 1e-4) and smaller than
   * \c 1.0.
   */
  /** @ITKStartGrouping */
  void
  SetLineSearchGradientAccuracy(PrecisionType gtol);
  [[nodiscard]] PrecisionType
  GetLineSearchGradientAccuracy() const;
  /** @ITKEndGrouping */
  /**
   * The machine precision for floating-point values.
   *  This parameter must be a positive value set by a client program to
   *  estimate the machine precision. The line search routine will terminate
   *  with the status code (\c LBFGSERR_ROUNDING_ERROR) if the relative width
   *  of the interval of uncertainty is less than this parameter.
   */
  /** @ITKStartGrouping */
  void
  SetMachinePrecisionTolerance(PrecisionType xtol);
  [[nodiscard]] PrecisionType
  GetMachinePrecisionTolerance() const;
  /** @ITKEndGrouping */
  /**
   * Coefficient for the L1 norm of variables.
   *  This parameter should be set to zero for standard minimization
   *  problems. Setting this parameter to a positive value activates
   *  Orthant-Wise Limited-memory Quasi-Newton (OWL-QN) method, which
   *  minimizes the objective function F(x) combined with the L1 norm |x|
   *  of the variables, \f$F(x) + C |x| \f$. This parameter is the coefficient
   *  for the |x|, i.e., C. As the L1 norm |x| is not differentiable at
   *  zero, the library modifies function and gradient evaluations from
   *  a client program suitably; a client program thus have only to return
   *  the function value F(x) and gradients G(x) as usual. The default value
   *  is zero.
   */
  /** @ITKStartGrouping */
  void
  SetOrthantwiseCoefficient(PrecisionType orthant_c);
  [[nodiscard]] PrecisionType
  GetOrthantwiseCoefficient() const;
  /** @ITKEndGrouping */
  /**
   * Start index for computing L1 norm of the variables.
   *  This parameter is valid only for OWL-QN method
   *  (i.e., \f$ orthantwise_c != 0 \f$). This parameter b (0 <= b < N)
   *  specifies the index number from which the library computes the
   *  L1 norm of the variables x,
   *  \f[
   *      |x| := |x_{b}| + |x_{b+1}| + ... + |x_{N}| .
   *  \f]
   *  In other words, variables \f$x_1, ..., x_{b-1}\f$ are not used for
   *  computing the L1 norm. Setting \c b, (0 < b < N), one can protect
   *  variables, \f$x_1, ..., x_{b-1}\f$ (e.g., a bias term of logistic
   *  regression) from being regularized. The default value is zero.
   */
  /** @ITKStartGrouping */
  void
  SetOrthantwiseStart(int start);
  [[nodiscard]] int
  GetOrthantwiseStart() const;
  /** @ITKEndGrouping */
  /**
   * End index for computing L1 norm of the variables.
   *  This parameter is valid only for OWL-QN method
   *  (i.e., \f$ orthantwise_c != 0 \f$). This parameter \c e, (0 < e <= N)
   *  specifies the index number at which the library stops computing the
   *  L1 norm of the variables x,
   */
  /** @ITKStartGrouping */
  void
  SetOrthantwiseEnd(int end);
  [[nodiscard]] int
  GetOrthantwiseEnd() const;
  /** @ITKEndGrouping */
  /** Get parameter norm of current iteration */
  itkGetConstMacro(CurrentParameterNorm, PrecisionType);

  /** Get gradient norm of current iteration */
  itkGetConstMacro(CurrentGradientNorm, PrecisionType);

  /** Get step size of current iteration */
  itkGetConstMacro(CurrentStepSize, PrecisionType);

  /** Get number of evaluations for current iteration */
  itkGetConstMacro(CurrentNumberOfEvaluations, PrecisionType);

  /** Option to use ScalesEstimator for estimating scales at
   * *each* iteration. The estimation overrides the scales
   * set by SetScales(). Default is true.
   */
  /** @ITKStartGrouping */
  itkSetMacro(EstimateScalesAtEachIteration, bool);
  itkGetConstReferenceMacro(EstimateScalesAtEachIteration, bool);
  itkBooleanMacro(EstimateScalesAtEachIteration);
  /** @ITKEndGrouping */
protected:
  LBFGS2Optimizerv4Template();
  ~LBFGS2Optimizerv4Template() override;
  void
  PrintSelf(std::ostream & os, Indent indent) const override;


  /** Progress callback from libLBFGS forwards it to the specific instance */
  static int
  UpdateProgressCallback(void *                instance,
                         const PrecisionType * x,
                         const PrecisionType * g,
                         const PrecisionType   fx,
                         const PrecisionType   xnorm,
                         const PrecisionType   gnorm,
                         const PrecisionType   step,
                         int                   n,
                         int                   k,
                         int                   ls);

  /** Update the progress as reported from libLBFSG and notify itkObject */
  int
  UpdateProgress(const PrecisionType * x,
                 const PrecisionType * g,
                 const PrecisionType   fx,
                 const PrecisionType   xnorm,
                 const PrecisionType   gnorm,
                 const PrecisionType   step,
                 int                   n,
                 int                   k,
                 int                   ls);

  /** Function evaluation callback from libLBFGS forward to instance */
  static PrecisionType
  EvaluateCostCallback(void *                instance,
                       const PrecisionType * x,
                       PrecisionType *       g,
                       const int             n,
                       const PrecisionType   step);

  PrecisionType
  EvaluateCost(const PrecisionType * x, PrecisionType * g, const int n, const PrecisionType step);

private:
  // Private Implementation (Pimpl), to hide liblbfgs data structures
  class PrivateImplementationHolder;
  lbfgs_parameter_t m_Parameters{};

  bool   m_EstimateScalesAtEachIteration{};
  double m_CurrentStepSize{};
  double m_CurrentParameterNorm{};
  double m_CurrentGradientNorm{};
  int    m_CurrentNumberOfEvaluations{};
  int    m_StatusCode{};

  /**
   * itkGradientDecentOptimizerv4Template specific non supported methods.
   */
  /** @ITKStartGrouping */
  void
  SetMinimumConvergenceValue(PrecisionType) override
  {
    itkWarningMacro("Not supported. Please use LBFGS specific convergence methods.");
  }
  void
  SetConvergenceWindowSize(SizeValueType) override
  {
    itkWarningMacro("Not supported. Please use LBFGS specific convergence methods.");
  }
  [[nodiscard]] const PrecisionType &
  GetConvergenceValue() const override
  {
    itkWarningMacro("Not supported. Please use LBFGS specific convergence methods.");
    static const PrecisionType value{};
    return value;
  }
  /** @ITKEndGrouping */
  void
  AdvanceOneStep() override
  {
    itkWarningMacro("LBFGS2Optimizerv4Template does not implement single step advance");
  }
};


/** This helps to meet backward compatibility */
using LBFGS2Optimizerv4 = LBFGS2Optimizerv4Template<double>;

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#  include "itkLBFGS2Optimizerv4.hxx"
#endif

#endif
