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
#ifndef itkDataObjectConstIterator_h
#define itkDataObjectConstIterator_h

#include "itkProcessObject.h"

namespace itk
{
/** \class DataObjectConstIterator
 * \brief A forward iterator over the DataObject of a ProcessObject
 * \author Gaetan Lehmann. Biologie du Developpement et de la Reproduction, INRA de Jouy-en-Josas, France.
 * \ingroup ITKCommon
 */
class DataObjectConstIterator
{
public:
  using DataObjectIdentifierType = DataObject::DataObjectIdentifierType;

  DataObjectConstIterator() = default;

  DataObjectConstIterator(const DataObjectConstIterator & iter) = default;

  DataObjectConstIterator &
  operator=(const DataObjectConstIterator & iter)
  {
    if (this != &iter)
    {
      m_Iterator = iter.m_Iterator;
      m_Begin = iter.m_Begin;
      m_End = iter.m_End;
    }
    return *this;
  }

  [[nodiscard]] const DataObject *
  GetDataObject() const
  {
    return m_Iterator->second;
  }

  [[nodiscard]] const DataObjectIdentifierType &
  GetName() const
  {
    return m_Iterator->first;
  }

  DataObjectConstIterator
  operator++(int)
  {
    const DataObjectConstIterator tmp = *this;
    ++(*this);
    return tmp;
  }

  DataObjectConstIterator &
  operator++()
  {
    ++m_Iterator;
    return *this;
  }

  bool
  operator==(const DataObjectConstIterator & iter) const
  {
    return m_Iterator == iter.m_Iterator && m_Begin == iter.m_Begin && m_End == iter.m_End;
  }

  ITK_UNEQUAL_OPERATOR_MEMBER_FUNCTION(DataObjectConstIterator);

  void
  GoToBegin()
  {
    m_Iterator = m_Begin;
  }

  [[nodiscard]] bool
  IsAtEnd() const
  {
    return m_Iterator == m_End;
  }

protected:
  using InternalIteratorType = ProcessObject::DataObjectPointerMap::const_iterator;
  InternalIteratorType m_Iterator;
  InternalIteratorType m_Begin;
  InternalIteratorType m_End;
};
} // namespace itk
#endif
