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
#ifndef itkLineCell_h
#define itkLineCell_h

#include "itkVertexCell.h"
#include "itkMakeFilled.h"

#include <array>

namespace itk
{
/** \class LineCell
 *  \brief Represents a line segment for a Mesh.
 *
 * LineCell represents a line segment for a Mesh.
 *
 * Template parameters for LineCell:
 *
 * \ingroup MeshObjects
 * \ingroup ITKCommon
 */

template <typename TCellInterface>
class ITK_TEMPLATE_EXPORT LineCell : public TCellInterface
{
public:
  ITK_DISALLOW_COPY_AND_MOVE(LineCell);

  /** Standard class type aliases. */
  /** @ITKStartGrouping */
  itkCellCommonTypedefs(LineCell);
  itkCellInheritedTypedefs(TCellInterface);
  /** @ITKEndGrouping */
  /** \see LightObject::GetNameOfClass() */
  itkOverrideGetNameOfClassMacro(LineCell);

  /** The type of boundary for this lines's vertices. */
  using VertexType = VertexCell<TCellInterface>;
  using VertexAutoPointer = typename VertexType::SelfAutoPointer;

  /** Line-specific topology numbers. */
  static constexpr unsigned int NumberOfPoints = 2;
  static constexpr unsigned int NumberOfVertices = 2;
  static constexpr unsigned int CellDimension = 1;

  /** Implement the standard CellInterface. */
  /** @ITKStartGrouping */
  [[nodiscard]] CellGeometryEnum
  GetType() const override
  {
    return CellGeometryEnum::LINE_CELL;
  }
  void
  MakeCopy(CellAutoPointer &) const override;
  /** @ITKEndGrouping */
  [[nodiscard]] unsigned int
  GetDimension() const override;

  [[nodiscard]] unsigned int
  GetNumberOfPoints() const override;

  [[nodiscard]] CellFeatureCount
  GetNumberOfBoundaryFeatures(int dimension) const override;

  bool
  GetBoundaryFeature(int dimension, CellFeatureIdentifier, CellAutoPointer &) override;
  void
  SetPointIds(PointIdConstIterator first) override;

  void
  SetPointIds(PointIdConstIterator first, PointIdConstIterator last) override;

  void
  SetPointId(int localId, PointIdentifier) override;
  PointIdIterator
  PointIdsBegin() override;

  [[nodiscard]] PointIdConstIterator
  PointIdsBegin() const override;

  PointIdIterator
  PointIdsEnd() override;

  [[nodiscard]] PointIdConstIterator
  PointIdsEnd() const override;

  /** Line-specific interface. */
  [[nodiscard]] virtual CellFeatureCount
  GetNumberOfVertices() const;

  virtual bool
  GetVertex(CellFeatureIdentifier, VertexAutoPointer &);

  /** Visitor interface */
  itkCellVisitMacro(CellGeometryEnum::LINE_CELL);

  LineCell() = default;

  ~LineCell() override = default;

protected:
  /** Store number of points needed for a line segment. */
  std::array<PointIdentifier, NumberOfPoints> m_PointIds{ MakeFilled<std::array<PointIdentifier, NumberOfPoints>>(
    NumericTraits<PointIdentifier>::max()) };
};
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#  include "itkLineCell.hxx"
#endif

#endif
