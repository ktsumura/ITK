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

#include "itkFEMLinearSystemWrapperItpack.h"
#include <iostream>

/* Testing for linear system wrappers */
int
itkFEMLinearSystemWrapperItpackTest(int argc, char * argv[])
{
  /* declare wrapper */
  itk::fem::LinearSystemWrapperItpack it;

  /* system parameters */
  unsigned int N = 5;
  unsigned int nMatrices = 3;
  unsigned int nVectors = 2;
  unsigned int nSolutions = 2;

  /* Set up the system */
  it.SetSystemOrder(N);
  it.SetNumberOfMatrices(nMatrices);
  it.SetNumberOfVectors(nVectors);
  it.SetNumberOfSolutions(nSolutions);

  /* Set max non zeros in any matrix */
  it.SetMaximumNonZeroValuesInMatrix(12);
  /* Initialize memory */
  for (unsigned int i = 0; i < nMatrices; ++i)
  {
    it.InitializeMatrix(i);
  }
  for (unsigned int i = 0; i < nVectors; ++i)
  {
    it.InitializeVector(i);
  }
  for (unsigned int i = 0; i < nSolutions; ++i)
  {
    it.InitializeSolution(i);
  }

  /*     matrix 0
   * |11  0  0 14 15|
   * | 0 22  0  0  0|
   * | 0  0 33  0  0|
   * |14  0  0 44 45|
   * |15  0  0 45 55|
   */
  it.SetMatrixValue(0, 0, 11, 0);
  it.SetMatrixValue(0, 3, 14, 0);
  it.SetMatrixValue(0, 4, 15, 0);
  it.SetMatrixValue(1, 1, 22, 0);
  it.SetMatrixValue(2, 2, 33, 0);
  it.SetMatrixValue(3, 0, 14, 0);
  it.SetMatrixValue(3, 3, 44, 0);
  it.SetMatrixValue(3, 4, 45, 0);
  it.SetMatrixValue(4, 0, 15, 0);
  it.SetMatrixValue(4, 3, 45, 0);
  it.SetMatrixValue(4, 4, 55, 0);

  /* print matrix 0 */
  std::cout << "Matrix 0" << std::endl;
  for (unsigned int i = 0; i < N; ++i)
  {
    for (unsigned int j = 0; j < N; ++j)
    {
      std::cout << it.GetMatrixValue(i, j, 0) << ' ';
    }
    std::cout << std::endl;
  }
  std::cout << std::endl;

  /*     matrix 1
   * |11  0  0 14 15|
   * | 0 22  0  0  0|
   * | 0  0 33  0  0|
   * |14  0  0 44 45|
   * |15  0  0 45 55|
   */
  it.SetMatrixValue(0, 0, 11, 1);
  it.SetMatrixValue(0, 3, 14, 1);
  it.SetMatrixValue(0, 4, 15, 1);
  it.SetMatrixValue(1, 1, 22, 1);
  it.SetMatrixValue(2, 2, 33, 1);
  it.SetMatrixValue(3, 0, 14, 1);
  it.SetMatrixValue(3, 3, 44, 1);
  it.SetMatrixValue(3, 4, 45, 1);
  it.SetMatrixValue(4, 0, 15, 1);
  it.SetMatrixValue(4, 3, 45, 1);
  it.SetMatrixValue(4, 4, 55, 1);

  /* print Matrix 1 */
  std::cout << "Matrix 1" << std::endl;
  for (unsigned int i = 0; i < N; ++i)
  {
    for (unsigned int j = 0; j < N; ++j)
    {
      std::cout << it.GetMatrixValue(i, j, 1) << ' ';
    }
    std::cout << std::endl;
  }
  std::cout << std::endl;

  /* matrix 2 = matrix 0 * matrix 1 */
  it.MultiplyMatrixMatrix(2, 0, 1);

  /* print Matrix 2 */
  std::cout << "matrix 2 = matrix 0 and matrix 1" << std::endl;
  for (unsigned int i = 0; i < N; ++i)
  {
    for (unsigned int j = 0; j < N; ++j)
    {
      std::cout << it.GetMatrixValue(i, j, 2) << ' ';
    }
    std::cout << std::endl;
  }
  std::cout << std::endl;

  /* Vector 0 = [1 2 3 4 5] */
  it.SetVectorValue(0, 1, 0);
  it.SetVectorValue(1, 2, 0);
  it.SetVectorValue(2, 3, 0);
  it.SetVectorValue(3, 4, 0);
  it.SetVectorValue(4, 5, 0);

  /* print Vector 0 */
  std::cout << "Vector 0" << std::endl;
  for (unsigned int i = 0; i < N; ++i)
  {
    std::cout << it.GetVectorValue(i, 0) << ' ';
  }
  std::cout << std::endl << std::endl;

  /* vector 1 = matrix 0 * vector 0 */
  std::cout << "Vector 1 =  Matrix 0 * Vector 0" << std::endl;
  it.MultiplyMatrixVector(1, 0, 0);
  for (unsigned int i = 0; i < N; ++i)
  {
    std::cout << it.GetVectorValue(i, 1) << ' ';
  }
  std::cout << std::endl << std::endl;

  /* swap vectors */
  std::cout << "swap Vector 0 and Vector 1" << std::endl;
  std::cout << "Vector 0" << std::endl;
  it.SwapVectors(0, 1);
  for (unsigned int i = 0; i < N; ++i)
  {
    std::cout << it.GetVectorValue(i, 0) << ' ';
  }
  std::cout << std::endl << "Vector 1" << std::endl;
  for (unsigned int i = 0; i < 5; ++i)
  {
    std::cout << it.GetVectorValue(i, 1) << ' ';
  }
  std::cout << std::endl << std::endl;

  /* swap matrices */
  std::cout << "swap Matrix 0 and Matrix 2" << std::endl;
  it.SwapMatrices(0, 2);
  std::cout << "Matrix 0" << std::endl;
  for (unsigned int i = 0; i < N; ++i)
  {
    for (unsigned int j = 0; j < N; ++j)
    {
      std::cout << it.GetMatrixValue(i, j, 0) << ' ';
    }
    std::cout << std::endl;
  }
  std::cout << std::endl << "Matrix 2" << std::endl;
  for (unsigned int i = 0; i < N; ++i)
  {
    for (unsigned int j = 0; j < N; ++j)
    {
      std::cout << it.GetMatrixValue(i, j, 2) << ' ';
    }
    std::cout << std::endl;
  }
  std::cout << std::endl;

  /* solve system */
  std::cout << "Solve for x in: Matrix 0 * x = Vector 0" << std::endl;
  it.Solve();
  std::cout << "Solution 0" << std::endl;
  for (unsigned int i = 0; i < N; ++i)
  {
    std::cout << it.GetSolutionValue(i, 0) << ' ';
  }
  std::cout << std::endl << std::endl;

  /* set solution */
  std::cout << "Solution 1" << std::endl;
  it.SetSolutionValue(0, 1, 1);
  it.SetSolutionValue(1, 2, 1);
  it.SetSolutionValue(2, 3, 1);
  it.SetSolutionValue(3, 4, 1);
  it.SetSolutionValue(4, 5, 1);
  for (unsigned int i = 0; i < 5; ++i)
  {
    std::cout << it.GetSolutionValue(i, 1) << ' ';
  }
  std::cout << std::endl << std::endl;

  /* swap solutions */
  std::cout << "swap Solution 0 and Solution 1" << std::endl;
  it.SwapSolutions(0, 1);
  std::cout << "Solution 0" << std::endl;
  for (unsigned int i = 0; i < N; ++i)
  {
    std::cout << it.GetSolutionValue(i, 0) << ' ';
  }
  std::cout << std::endl << "Solution 1" << std::endl;
  for (unsigned int i = 0; i < N; ++i)
  {
    std::cout << it.GetSolutionValue(i, 1) << ' ';
  }
  std::cout << std::endl << std::endl;

  /* copy solution to vector */
  std::cout << "copy Solution 1 to Vector 0" << std::endl;
  it.CopySolution2Vector(1, 0);
  std::cout << "Vector 0" << std::endl;
  for (unsigned int i = 0; i < N; ++i)
  {
    std::cout << it.GetVectorValue(i, 0) << ' ';
  }
  std::cout << std::endl << std::endl;

  /* scale matrix */
  std::cout << "scale Matrix 2 by 2.0" << std::endl;
  it.ScaleMatrix(2.0, 2);
  std::cout << "Matrix 2" << std::endl;
  for (unsigned int i = 0; i < N; ++i)
  {
    for (unsigned int j = 0; j < N; ++j)
    {
      std::cout << it.GetMatrixValue(i, j, 2) << ' ';
    }
    std::cout << std::endl;
  }
  std::cout << std::endl;

  /* scale vector */
  std::cout << "scale Vector 0 by 3.0" << std::endl;
  it.ScaleVector(3.0, 0);
  std::cout << "Vector 0" << std::endl;
  for (unsigned int i = 0; i < 5; ++i)
  {
    std::cout << it.GetVectorValue(i, 0) << ' ';
  }
  std::cout << std::endl << std::endl;

  /* destroy matrix,vector,solution */
  it.DestroyMatrix(0);
  it.DestroyVector(1);
  it.DestroySolution(0);

  if (argc > 1)
  {
    int method = std::stoi(argv[1]);

    switch (method)
    {
      case 0:
        it.JacobianConjugateGradient();
        break;
      case 1:
        it.JacobianSemiIterative();
        break;
      case 2:
        it.SuccessiveOverrelaxation();
        break;
      case 3:
        it.SymmetricSuccessiveOverrelaxationConjugateGradient();
        break;
      case 4:
        it.SymmetricSuccessiveOverrelaxationSuccessiveOverrelaxation();
        break;
      case 5:
        it.ReducedSystemConjugateGradient();
        break;
      case 6:
        it.ReducedSystemSemiIteration();
        break;
    }
  }

  int    integerPass = 1;
  double doublePass = 1.0;

  std::cout << "Test itpack parameter setting..." << std::endl;

  it.SetMaximumNumberIterations(integerPass);
  [[maybe_unused]] auto returnval00 = it.GetMaximumNumberIterations();
  [[maybe_unused]] auto returnval01 = it.GetErrorReportingLevel();
  it.SetCommunicationSwitch(integerPass);
  [[maybe_unused]] auto returnval02 = it.GetCommunicationSwitch();
  [[maybe_unused]] auto returnval1 = it.GetOutputNumber();
  it.SetSymmetricMatrixFlag(integerPass);
  it.GetSymmetricMatrixFlag();
  it.SetAdaptiveSwitch(integerPass);
  [[maybe_unused]] auto returnval03 = it.GetAdaptiveSwitch();
  it.SetAdaptiveCaseSwitch(integerPass);
  [[maybe_unused]] auto returnval04 = it.GetAdaptiveCaseSwitch();
  it.SetWorkspaceUsed(integerPass);
  it.GetWorkspaceUsed();
  it.SetRedBlackOrderingSwitch(integerPass);
  it.GetRedBlackOrderingSwitch();
  it.SetRemoveSwitch(integerPass);
  it.GetRemoveSwitch();
  it.SetTimingSwitch(integerPass);
  it.GetTimingSwitch();
  it.SetErrorAnalysisSwitch(integerPass);
  [[maybe_unused]] auto returnval05 = it.GetErrorAnalysisSwitch();
  it.SetAccuracy(doublePass);
  [[maybe_unused]] auto returnval06 = it.GetAccuracy();
  it.SetLargestJacobiEigenvalueEstimate(doublePass);
  [[maybe_unused]] auto returnval07 = it.GetLargestJacobiEigenvalueEstimate();
  it.SetSmallestJacobiEigenvalueEstimate(doublePass);
  it.GetSmallestJacobiEigenvalueEstimate();
  it.SetDampingFactor(doublePass);
  [[maybe_unused]] auto returnval08 = it.GetDampingFactor();
  it.SetOverrelaxationParameter(doublePass);
  it.GetOverrelaxationParameter();
  it.SetEstimatedSpectralRadiusSSOR(doublePass);
  [[maybe_unused]] auto returnval09 = it.GetEstimatedSpectralRadiusSSOR();
  it.SetEstimatedSpectralRadiusLU(doublePass);
  [[maybe_unused]] auto returnval10 = it.GetEstimatedSpectralRadiusLU();
  it.SetTolerance(doublePass);
  it.GetTolerance();
  it.SetTimeToConvergence(doublePass);
  it.GetTimeToConvergence();
  it.SetTimeForCall(doublePass);
  it.GetTimeForCall();
  it.SetDigitsInError(doublePass);
  [[maybe_unused]] auto returnval11 = it.GetDigitsInError();
  it.SetDigitsInResidual(doublePass);
  [[maybe_unused]] auto returnval12 = it.GetDigitsInResidual();
  it.JacobianConjugateGradient();
  it.JacobianSemiIterative();
  it.SuccessiveOverrelaxation();
  it.SymmetricSuccessiveOverrelaxationConjugateGradient();
  it.SymmetricSuccessiveOverrelaxationSuccessiveOverrelaxation();
  it.ReducedSystemConjugateGradient();
  it.ReducedSystemSemiIteration();

  std::cout << "Done." << std::endl;

  return EXIT_SUCCESS;
}
