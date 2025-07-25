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
#ifndef itkPointSetToPointSetRegistrationMethod_hxx
#define itkPointSetToPointSetRegistrationMethod_hxx


namespace itk
{

template <typename TFixedPointSet, typename TMovingPointSet>
PointSetToPointSetRegistrationMethod<TFixedPointSet, TMovingPointSet>::PointSetToPointSetRegistrationMethod()
  : m_InitialTransformParameters(ParametersType(FixedPointSetType::PointDimension))
  , m_LastTransformParameters(ParametersType(FixedPointSetType::PointDimension))
{
  this->SetNumberOfRequiredOutputs(1);

  m_InitialTransformParameters.Fill(0);
  m_LastTransformParameters.Fill(0);

  const TransformOutputPointer transformDecorator =
    itkDynamicCastInDebugMode<TransformOutputType *>(this->MakeOutput(0).GetPointer());

  this->ProcessObject::SetNthOutput(0, transformDecorator.GetPointer());
}

template <typename TFixedPointSet, typename TMovingPointSet>
void
PointSetToPointSetRegistrationMethod<TFixedPointSet, TMovingPointSet>::SetInitialTransformParameters(
  const ParametersType & param)
{
  m_InitialTransformParameters = param;
  this->Modified();
}

template <typename TFixedPointSet, typename TMovingPointSet>
void
PointSetToPointSetRegistrationMethod<TFixedPointSet, TMovingPointSet>::Initialize()
{
  if (!m_FixedPointSet)
  {
    itkExceptionMacro("FixedPointSet is not present");
  }

  if (!m_MovingPointSet)
  {
    itkExceptionMacro("MovingPointSet is not present");
  }

  if (!m_Metric)
  {
    itkExceptionMacro("Metric is not present");
  }

  if (!m_Optimizer)
  {
    itkExceptionMacro("Optimizer is not present");
  }

  if (!m_Transform)
  {
    itkExceptionMacro("Transform is not present");
  }

  // Set up the metric
  m_Metric->SetMovingPointSet(m_MovingPointSet);
  m_Metric->SetFixedPointSet(m_FixedPointSet);
  m_Metric->SetTransform(m_Transform);

  m_Metric->Initialize();

  // Set up the optimizer
  m_Optimizer->SetCostFunction(m_Metric);

  // Validate initial transform parameters
  if (m_InitialTransformParameters.Size() != m_Transform->GetNumberOfParameters())
  {
    itkExceptionMacro("Size mismatch between initial parameter and transform");
  }

  m_Optimizer->SetInitialPosition(m_InitialTransformParameters);

  // Connect the transform to the Decorator
  auto * transformOutput = static_cast<TransformOutputType *>(this->ProcessObject::GetOutput(0));

  transformOutput->Set(m_Transform);
}

template <typename TFixedPointSet, typename TMovingPointSet>
void
PointSetToPointSetRegistrationMethod<TFixedPointSet, TMovingPointSet>::GenerateData()
{
  // Initialize the interconnects between components
  try
  {
    this->Initialize();
  }
  catch (const ExceptionObject &)
  {
    m_LastTransformParameters = ParametersType(1);
    m_LastTransformParameters.Fill(0.0f);

    // Pass the  exception to the caller
    throw;
  }

  // Do the optimization
  try
  {
    m_Optimizer->StartOptimization();
  }
  catch (const ExceptionObject &)
  {
    // An error has occurred in the optimization.
    // Update the parameters
    m_LastTransformParameters = m_Optimizer->GetCurrentPosition();

    // Pass the exception to the caller
    throw;
  }

  // Get the results
  m_LastTransformParameters = m_Optimizer->GetCurrentPosition();

  m_Transform->SetParameters(m_LastTransformParameters);
}

template <typename TFixedPointSet, typename TMovingPointSet>
auto
PointSetToPointSetRegistrationMethod<TFixedPointSet, TMovingPointSet>::GetOutput() const -> const TransformOutputType *
{
  return static_cast<const TransformOutputType *>(this->ProcessObject::GetOutput(0));
}

template <typename TFixedPointSet, typename TMovingPointSet>
DataObject::Pointer
PointSetToPointSetRegistrationMethod<TFixedPointSet, TMovingPointSet>::MakeOutput(DataObjectPointerArraySizeType output)
{
  if (output > 0)
  {
    itkExceptionMacro("MakeOutput request for an output number larger than the expected number of outputs.");
  }
  return TransformOutputType::New().GetPointer();
}

template <typename TFixedPointSet, typename TMovingPointSet>
ModifiedTimeType
PointSetToPointSetRegistrationMethod<TFixedPointSet, TMovingPointSet>::GetMTime() const
{
  ModifiedTimeType mtime = Superclass::GetMTime();

  // Some of the following should be removed once ivars are put in the
  // input and output lists

  if (m_Transform)
  {
    ModifiedTimeType m = m_Transform->GetMTime();
    mtime = (m > mtime ? m : mtime);
  }

  if (m_Metric)
  {
    ModifiedTimeType m = m_Metric->GetMTime();
    mtime = (m > mtime ? m : mtime);
  }

  if (m_Optimizer)
  {
    ModifiedTimeType m = m_Optimizer->GetMTime();
    mtime = (m > mtime ? m : mtime);
  }

  if (m_FixedPointSet)
  {
    ModifiedTimeType m = m_FixedPointSet->GetMTime();
    mtime = (m > mtime ? m : mtime);
  }

  if (m_MovingPointSet)
  {
    ModifiedTimeType m = m_MovingPointSet->GetMTime();
    mtime = (m > mtime ? m : mtime);
  }

  return mtime;
}

template <typename TFixedPointSet, typename TMovingPointSet>
void
PointSetToPointSetRegistrationMethod<TFixedPointSet, TMovingPointSet>::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);

  itkPrintSelfObjectMacro(Metric);
  itkPrintSelfObjectMacro(Optimizer);
  itkPrintSelfObjectMacro(Transform);
  itkPrintSelfObjectMacro(FixedPointSet);
  itkPrintSelfObjectMacro(MovingPointSet);

  os << indent << "Initial Transform Parameters: " << m_InitialTransformParameters << std::endl;
  os << indent << "Last    Transform Parameters: " << m_LastTransformParameters << std::endl;
}
} // end namespace itk
#endif
