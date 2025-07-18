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
#ifndef itkQuadEdgeMeshDecimationCriteria_h
#define itkQuadEdgeMeshDecimationCriteria_h

#include "itkIntTypes.h"
#include "itkPriorityQueueContainer.h"

namespace itk
{
/**
 * \class QuadEdgeMeshDecimationCriterion
 * \brief
 * \ingroup ITKQuadEdgeMeshFiltering
 */
template <typename TMesh,
          typename TElement = IdentifierType,
          typename TMeasure = double,
          typename TPriorityQueueWrapper =
            MinPriorityQueueElementWrapper<typename TMesh::QEType *, std::pair<bool, TMeasure>>>
class QuadEdgeMeshDecimationCriterion : public Object
{
public:
  ITK_DISALLOW_COPY_AND_MOVE(QuadEdgeMeshDecimationCriterion);

  using Self = QuadEdgeMeshDecimationCriterion;
  using Pointer = SmartPointer<Self>;
  using ConstPointer = SmartPointer<const Self>;
  using Superclass = Object;

  /** \see LightObject::GetNameOfClass() */
  itkOverrideGetNameOfClassMacro(QuadEdgeMeshDecimationCriterion);

  using MeshType = TMesh;
  using ElementType = TElement;
  using MeasureType = TMeasure;
  using PriorityQueueWrapperType = TPriorityQueueWrapper;
  using PriorityType = typename PriorityQueueWrapperType::ElementPriorityType;

  void
  SetNumberOfElements(const SizeValueType & numberOfElements)
  {
    this->m_SizeCriterion = true;
    this->m_NumberOfElements = numberOfElements;
  }

  void
  SetMeasureBound(const MeasureType & bound)
  {
    this->m_SizeCriterion = false;
    this->m_MeasureBound = bound;
  }

  itkBooleanMacro(TopologicalChange);
  itkGetConstMacro(TopologicalChange, bool);
  itkSetMacro(TopologicalChange, bool);

  virtual bool
  is_satisfied(MeshType * iMesh, const ElementType & iElement, const MeasureType & iValue) const = 0;

protected:
  QuadEdgeMeshDecimationCriterion()
    : m_TopologicalChange(true)
    , m_SizeCriterion(true)
    , m_MeasureBound(MeasureType{})
  {}

  ~QuadEdgeMeshDecimationCriterion() override = default;
  void
  PrintSelf(std::ostream & os, Indent indent) const override
  {
    Superclass::PrintSelf(os, indent);
    itkPrintSelfBooleanMacro(TopologicalChange);
    itkPrintSelfBooleanMacro(SizeCriterion);
    os << indent << "NumberOfElements: " << m_NumberOfElements << std::endl;
    os << indent << "MeasureBound: " << m_MeasureBound << std::endl;
  }

  bool m_TopologicalChange{};
  bool m_SizeCriterion{};

  SizeValueType m_NumberOfElements{};

  MeasureType m_MeasureBound{};
};

/**
 * \class NumberOfPointsCriterion
 * \brief
 * \ingroup ITKQuadEdgeMeshFiltering
 */
template <typename TMesh,
          typename TElement = IdentifierType,
          typename TMeasure = double,
          typename TPriorityQueueWrapper =
            MinPriorityQueueElementWrapper<typename TMesh::QEType *, std::pair<bool, TMeasure>>>
class ITK_TEMPLATE_EXPORT NumberOfPointsCriterion
  : public QuadEdgeMeshDecimationCriterion<TMesh, TElement, TMeasure, TPriorityQueueWrapper>
{
public:
  ITK_DISALLOW_COPY_AND_MOVE(NumberOfPointsCriterion);

  using Self = NumberOfPointsCriterion;
  using Pointer = SmartPointer<Self>;
  using ConstPointer = SmartPointer<const Self>;
  using Superclass = QuadEdgeMeshDecimationCriterion<TMesh, TElement, TMeasure, TPriorityQueueWrapper>;

  /** \see LightObject::GetNameOfClass() */
  itkOverrideGetNameOfClassMacro(NumberOfPointsCriterion);

  /** New macro for creation of through a Smart Pointer   */
  itkNewMacro(Self);

  using typename Superclass::MeshType;
  using typename Superclass::ElementType;
  using typename Superclass::MeasureType;
  using typename Superclass::PriorityQueueWrapperType;
  using typename Superclass::PriorityType;

  inline bool
  is_satisfied(MeshType *          iMesh,
               const ElementType & itkNotUsed(iElement),
               const MeasureType & itkNotUsed(iValue)) const override
  {
    return (iMesh->GetNumberOfPoints() <= this->m_NumberOfElements);
  }

protected:
  NumberOfPointsCriterion() = default;
  ~NumberOfPointsCriterion() override = default;
};

/**
 * \class NumberOfFacesCriterion
 * \brief
 * \ingroup ITKQuadEdgeMeshFiltering
 */
template <typename TMesh,
          typename TElement = IdentifierType,
          typename TMeasure = double,
          typename TPriorityQueueWrapper =
            MinPriorityQueueElementWrapper<typename TMesh::QEType *, std::pair<bool, TMeasure>>>
class ITK_TEMPLATE_EXPORT NumberOfFacesCriterion
  : public QuadEdgeMeshDecimationCriterion<TMesh, TElement, TMeasure, TPriorityQueueWrapper>
{
public:
  ITK_DISALLOW_COPY_AND_MOVE(NumberOfFacesCriterion);

  using Self = NumberOfFacesCriterion;
  using Pointer = SmartPointer<Self>;
  using ConstPointer = SmartPointer<const Self>;
  using Superclass = QuadEdgeMeshDecimationCriterion<TMesh, TElement, TMeasure, TPriorityQueueWrapper>;

  /** \see LightObject::GetNameOfClass() */
  itkOverrideGetNameOfClassMacro(NumberOfFacesCriterion);

  /** New macro for creation of through a Smart Pointer   */
  itkNewMacro(Self);

  using typename Superclass::MeshType;
  using CellsContainerConstIterator = typename MeshType::CellsContainerConstIterator;
  using typename Superclass::ElementType;
  using typename Superclass::MeasureType;
  using typename Superclass::PriorityQueueWrapperType;
  using typename Superclass::PriorityType;

  bool
  is_satisfied(MeshType *          iMesh,
               const ElementType & itkNotUsed(iElement),
               const MeasureType & itkNotUsed(iValue)) const override
  {
    return (iMesh->GetNumberOfFaces() <= this->m_NumberOfElements);
  }

protected:
  NumberOfFacesCriterion() = default;
  ~NumberOfFacesCriterion() override = default;
};

/**
 * \class MaxMeasureBoundCriterion
 * \brief
 * \ingroup ITKQuadEdgeMeshFiltering
 */
template <typename TMesh,
          typename TElement = IdentifierType,
          typename TMeasure = double,
          typename TPriorityQueueWrapper =
            MinPriorityQueueElementWrapper<typename TMesh::QEType *, std::pair<bool, TMeasure>>>
class MaxMeasureBoundCriterion
  : public QuadEdgeMeshDecimationCriterion<TMesh, TElement, TMeasure, TPriorityQueueWrapper>
{
public:
  ITK_DISALLOW_COPY_AND_MOVE(MaxMeasureBoundCriterion);

  using Self = MaxMeasureBoundCriterion;
  using Pointer = SmartPointer<Self>;
  using ConstPointer = SmartPointer<const Self>;
  using Superclass = QuadEdgeMeshDecimationCriterion<TMesh, TElement, TMeasure, TPriorityQueueWrapper>;

  /** \see LightObject::GetNameOfClass() */
  itkOverrideGetNameOfClassMacro(MaxMeasureBoundCriterion);

  /** New macro for creation of through a Smart Pointer   */
  itkNewMacro(Self);

  using typename Superclass::MeshType;
  using CellsContainerConstIterator = typename MeshType::CellsContainerConstIterator;
  using typename Superclass::ElementType;
  using typename Superclass::MeasureType;
  using typename Superclass::PriorityQueueWrapperType;
  using typename Superclass::PriorityType;

  bool
  is_satisfied(MeshType *          itkNotUsed(iMesh),
               const ElementType & itkNotUsed(iElement),
               const MeasureType & iValue) const override
  {
    return (iValue <= this->m_MeasureBound);
  }

protected:
  MaxMeasureBoundCriterion()
    : Superclass()
  {}
  ~MaxMeasureBoundCriterion() override = default;
};

/**
 * \class MinMeasureBoundCriterion
 * \brief
 * \ingroup ITKQuadEdgeMeshFiltering
 */
template <typename TMesh,
          typename TElement = IdentifierType,
          typename TMeasure = double,
          typename TPriorityQueueWrapper =
            MaxPriorityQueueElementWrapper<typename TMesh::QEType *, std::pair<bool, TMeasure>>>
class MinMeasureBoundCriterion
  : public QuadEdgeMeshDecimationCriterion<TMesh, TElement, TMeasure, TPriorityQueueWrapper>
{
public:
  ITK_DISALLOW_COPY_AND_MOVE(MinMeasureBoundCriterion);

  using Self = MinMeasureBoundCriterion;
  using Pointer = SmartPointer<Self>;
  using ConstPointer = SmartPointer<const Self>;
  using Superclass = QuadEdgeMeshDecimationCriterion<TMesh, TElement, TMeasure, TPriorityQueueWrapper>;

  /** \see LightObject::GetNameOfClass() */
  itkOverrideGetNameOfClassMacro(MinMeasureBoundCriterion);

  /** New macro for creation of through a Smart Pointer   */
  itkNewMacro(Self);

  using typename Superclass::MeshType;
  using CellsContainerConstIterator = typename MeshType::CellsContainerConstIterator;
  using typename Superclass::ElementType;
  using typename Superclass::MeasureType;
  using typename Superclass::PriorityQueueWrapperType;
  using typename Superclass::PriorityType;

  inline bool
  is_satisfied(MeshType *, const ElementType &, const MeasureType & iValue) const override
  {
    return (iValue >= this->m_MeasureBound);
  }

protected:
  MinMeasureBoundCriterion() = default;
  ~MinMeasureBoundCriterion() override = default;
};
} // namespace itk

#endif
