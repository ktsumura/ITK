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
#ifndef itkWeightedCovarianceSampleFilter_h
#define itkWeightedCovarianceSampleFilter_h

#include "itkFunctionBase.h"
#include "itkCovarianceSampleFilter.h"
#include "itkDataObjectDecorator.h"

namespace itk::Statistics
{
/**
 * \class WeightedCovarianceSampleFilter
 * \brief Calculates the covariance matrix of the target sample data.
 *  where each measurement vector has an associated weight value
 *
 * Weight values can be specified in two ways: using a weighting function
 * or an array containing weight values. If none of these two is specified,
 * the covariance matrix is generated with equal weights.
 *
 * \sa CovarianceSampleFilter
 *
 * \ingroup ITKStatistics
 */

template <typename TSample>
class ITK_TEMPLATE_EXPORT WeightedCovarianceSampleFilter : public CovarianceSampleFilter<TSample>
{
public:
  ITK_DISALLOW_COPY_AND_MOVE(WeightedCovarianceSampleFilter);

  /** Standard class type aliases. */
  using Self = WeightedCovarianceSampleFilter;
  using Superclass = CovarianceSampleFilter<TSample>;
  using Pointer = SmartPointer<Self>;
  using ConstPointer = SmartPointer<const Self>;

  /** \see LightObject::GetNameOfClass() */
  /** @ITKStartGrouping */
  itkOverrideGetNameOfClassMacro(WeightedCovarianceSampleFilter);
  itkNewMacro(Self);
  /** @ITKEndGrouping */
  /** Types derived from the base class */
  using typename Superclass::SampleType;
  using typename Superclass::MeasurementVectorType;
  using typename Superclass::MeasurementVectorSizeType;
  using typename Superclass::MeasurementType;

  /** Types derived from the base class */
  using typename Superclass::MeasurementVectorRealType;
  using typename Superclass::MeasurementRealType;


  /** Type of weight values */
  using WeightValueType = double;


  /** Array type for weights */
  using WeightArrayType = Array<WeightValueType>;

  /** Type of DataObjects to use for the weight array type */
  using InputWeightArrayObjectType = SimpleDataObjectDecorator<WeightArrayType>;

  /** Method to set the input value of the weight array */
  itkSetGetDecoratedInputMacro(Weights, WeightArrayType);


  /** Weight calculation function type */
  using WeightingFunctionType = FunctionBase<MeasurementVectorType, WeightValueType>;

  /** Type of DataObjects to use for Weight function */
  using InputWeightingFunctionObjectType = DataObjectDecorator<WeightingFunctionType>;

  /** Method to set/get the weighting function */
  itkSetGetDecoratedObjectInputMacro(WeightingFunction, WeightingFunctionType);


  /** Types derived from the base class */
  using typename Superclass::MatrixType;
  using typename Superclass::MatrixDecoratedType;

  /** Types derived from the base class */
  using typename Superclass::MeasurementVectorDecoratedType;
  using typename Superclass::OutputType;

protected:
  WeightedCovarianceSampleFilter();
  ~WeightedCovarianceSampleFilter() override = default;

  void
  GenerateData() override;

  /** Compute covariance matrix with weights computed from a function */
  void
  ComputeCovarianceMatrixWithWeightingFunction();

  /** Compute covariance matrix with weights specified in an array */
  void
  ComputeCovarianceMatrixWithWeights();
}; // end of class
} // namespace itk::Statistics

#ifndef ITK_MANUAL_INSTANTIATION
#  include "itkWeightedCovarianceSampleFilter.hxx"
#endif

#endif
