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

#ifndef itkFEMElement2DC0LinearQuadrilateralMembrane_h
#define itkFEMElement2DC0LinearQuadrilateralMembrane_h

#include "itkFEMElement2DC0LinearQuadrilateral.h"
#include "itkFEMElement2DMembrane.h"
#include "ITKFEMExport.h"

namespace itk::fem
{
/**
 * \class Element2DC0LinearQuadrilateralMembrane
 * \ingroup ITKFEM
 * \brief 4-noded finite element class in 2D space.
 *
 *The ordering of the nodes is counter clockwise. That is the nodes
 * should be defined in the following order:
 *
 *  3 (0,1)                  2 (1,1)
 *  *------------------------*
 *  |                        |
 *  |                        |
 *  |                        |
 *  |                        |
 *  |                        |
 *  |                        |
 *  *------------------------*
 *  0 (0,0)                  1 (0,1)
 *
 * The constitutive equation used is from membrane bending energy.
 * This class combines the geometry of the FE problem defined in
 * \link Element2DC0LinearQuadrilateral\endlink
 * and the physics of the problem defined in
 * \link Element2DMembrane\endlink
 *
 * \sa Element2DC0LinearQuadrilateralStrain
 *
 */
class ITKFEM_EXPORT Element2DC0LinearQuadrilateralMembrane : public Element2DMembrane<Element2DC0LinearQuadrilateral>
{
public:
  /** Standard class type aliases. */
  using Self = Element2DC0LinearQuadrilateralMembrane;
  using Superclass = Element2DMembrane<Element2DC0LinearQuadrilateral>;
  using Pointer = SmartPointer<Self>;
  using ConstPointer = SmartPointer<const Self>;

  /** Method for creation through the object factory. */
  itkSimpleNewMacro(Self);

  /** \see LightObject::GetNameOfClass() */
  itkOverrideGetNameOfClassMacro(Element2DC0LinearQuadrilateralMembrane);

  /** CreateAnother method will clone the existing instance of this type,
   * including its internal member variables. */
  itk::LightObject::Pointer
  CreateAnother() const override;

  /**
   * Default constructor only clears the internal storage
   */
  Element2DC0LinearQuadrilateralMembrane();

  /**
   * Construct an element by specifying pointers to
   * 4 points and a material.
   */
  Element2DC0LinearQuadrilateralMembrane(NodeIDType             n1_,
                                         NodeIDType             n2_,
                                         NodeIDType             n3_,
                                         NodeIDType             n4_,
                                         Material::ConstPointer p_);

protected:
  void
  PrintSelf(std::ostream & os, Indent indent) const override;

}; // class Element2DC0LinearQuadrilateralMembrane
} // namespace itk::fem

#endif // itkFEMElement2DC0LinearQuadrilateralMembrane_h
