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

#include "itkFEMElement2DC0LinearTriangularMembrane.h"

namespace itk::fem
{
// Overload the CreateAnother() method.
itk::LightObject::Pointer
Element2DC0LinearTriangularMembrane::CreateAnother() const
{
  itk::LightObject::Pointer smartPtr;
  Pointer                   copyPtr = Self::New();

  copyPtr->SetNode(0, this->GetNode(0));
  copyPtr->SetNode(1, this->GetNode(1));
  copyPtr->SetNode(2, this->GetNode(2));
  copyPtr->SetMaterial(this->GetMaterial());
  copyPtr->SetGlobalNumber(this->GetGlobalNumber());

  smartPtr = static_cast<Pointer>(copyPtr);

  return smartPtr;
}

Element2DC0LinearTriangularMembrane::Element2DC0LinearTriangularMembrane()
  : Superclass()
{}

Element2DC0LinearTriangularMembrane::Element2DC0LinearTriangularMembrane(NodeIDType             n1_,
                                                                         NodeIDType             n2_,
                                                                         NodeIDType             n3_,
                                                                         Material::ConstPointer m_)
  : Superclass()
{
  // Set the geometrical points
  this->SetNode(0, n1_);
  this->SetNode(1, n2_);
  this->SetNode(2, n3_);

  /*
   * Initialize the pointer to material object and check that
   * we were given the pointer to the right class.
   * If the material class was incorrect an exception is thrown.
   */
  m_mat = dynamic_cast<const MaterialLinearElasticity *>(m_.GetPointer());

  if (!m_mat)
  {
    throw FEMExceptionWrongClass(
      __FILE__, __LINE__, "Element2DC0LinearTriangularMembrane::Element2DC0LinearTriangularMembrane()");
  }
}

void
Element2DC0LinearTriangularMembrane::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
}

} // namespace itk::fem
