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
#ifndef itkImplicitManifoldNormalVectorFilter_hxx
#define itkImplicitManifoldNormalVectorFilter_hxx

#include "itkNumericTraits.h"

namespace itk
{
template <typename TInputImage, typename TSparseOutputImage>
ImplicitManifoldNormalVectorFilter<TInputImage, TSparseOutputImage>::ImplicitManifoldNormalVectorFilter()
  : m_NormalFunction(nullptr)
  , m_MaxIteration(25)
  , m_IsoLevelLow(NodeValueType{})
  , m_IsoLevelHigh(NodeValueType{})
  , m_MinVectorNorm(static_cast<NodeValueType>(1.0e-6))
  , m_UnsharpMaskingWeight(NodeValueType{})
{
  this->SetPrecomputeFlag(true);
  // set defaults for parameters
  // compute constants used in computations
  for (unsigned int j = 0; j < ImageDimension; ++j)
  {
    m_Indicator[j] = 1 << j;
    m_ManifoldRadius[j] = 1;
  }
  m_NumVertex = 1 << ImageDimension;
  m_DimConst = static_cast<NodeValueType>(1.0 / m_NumVertex);
  m_DimConst2 = static_cast<NodeValueType>(4.0 * m_DimConst);
}

template <typename TInputImage, typename TSparseOutputImage>
void
ImplicitManifoldNormalVectorFilter<TInputImage, TSparseOutputImage>::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);
  os << indent << "IsoLevelLow: " << m_IsoLevelLow << std::endl;
  os << indent << "IsoLevelHigh: " << m_IsoLevelHigh << std::endl;
  os << indent << "MaxIteration: " << m_MaxIteration << std::endl;
  os << indent << "MinVectorNorm: " << m_MinVectorNorm << std::endl;
  os << indent << "UnsharpMaskingFlag: " << m_UnsharpMaskingFlag << std::endl;
  os << indent << "UnsharpMaskingWeight: " << m_UnsharpMaskingWeight << std::endl;
  os << indent << "DimConst: " << m_DimConst << std::endl;
  os << indent << "DimConst2: " << m_DimConst2 << std::endl;
  os << indent << "NumVertex: " << m_NumVertex << std::endl;
}

template <typename TInputImage, typename TSparseOutputImage>
void
ImplicitManifoldNormalVectorFilter<TInputImage, TSparseOutputImage>::SetNormalFunction(NormalFunctionType * nf)
{
  m_NormalFunction = nf;
  Superclass::SetSparseFunction(nf);
}

template <typename TInputImage, typename TSparseOutputImage>
void
ImplicitManifoldNormalVectorFilter<TInputImage, TSparseOutputImage>::Initialize()
{
  SetNormalBand();

  // call FDSparseImageFilter's Initialize to
  // prepare lists for multithreading
  Superclass::Initialize();
}

template <typename TInputImage, typename TSparseOutputImage>
void
ImplicitManifoldNormalVectorFilter<TInputImage, TSparseOutputImage>::SetNormalBand()
{
  const typename InputImageType::ConstPointer   ManifoldImage = this->GetInput();
  const typename SparseOutputImageType::Pointer output = this->GetOutput();

  InputImageIteratorType it(m_ManifoldRadius, ManifoldImage, ManifoldImage->GetRequestedRegion());

  IndexType     index;
  NodeValueType value;

  it.GoToBegin();
  while (!it.IsAtEnd())
  {
    value = it.GetCenterPixel();
    index = it.GetIndex();
    if ((value >= m_IsoLevelLow) && (value <= m_IsoLevelHigh))
    {
      this->InitializeNormalBandNode(output->AddNode(index), it);
    }
    else
    {
      output->SetPixel(index, nullptr);
    }
    ++it;
  }
}

template <typename TInputImage, typename TSparseOutputImage>
void
ImplicitManifoldNormalVectorFilter<TInputImage, TSparseOutputImage>::InitializeNormalBandNode(
  NormalBandNodeType *           node,
  const InputImageIteratorType & it)
{
  unsigned long    stride[ImageDimension];
  NormalVectorType normalvector;
  NodeValueType    derivative;

  for (unsigned int i = 0; i < ImageDimension; ++i)
  {
    stride[i] = it.GetStride(i);
  }
  const unsigned long center = it.Size() / 2;

  // Normal vector computation -- use positive quadrant of neighborhood
  for (unsigned int j = 0; j < ImageDimension; ++j) // derivative axis
  {
    normalvector[j] = NodeValueType{};
    for (unsigned int counter = 0; counter < m_NumVertex; ++counter)
    {
      unsigned long position = center;
      for (unsigned int k = 0; k < ImageDimension; ++k)
      {
        if (counter & m_Indicator[k])
        {
          position += stride[k];
        }
      }
      if (counter & m_Indicator[j])
      {
        normalvector[j] += it.GetPixel(position);
      }
      else
      {
        normalvector[j] -= it.GetPixel(position);
      }
    }
  } // end derivative axis
  node->m_Data = normalvector / (m_MinVectorNorm + normalvector.GetNorm());
  node->m_InputData = node->m_Data;

  // Manifold normal vector computation
  for (unsigned int i = 0; i < ImageDimension; ++i) // offset axis (flux position)
  {
    for (unsigned int j = 0; j < ImageDimension; ++j) // derivative axis
    {
      derivative = NodeValueType{};
      if (i != j)
      {
        for (unsigned int counter = 0; counter < m_NumVertex; ++counter)
        {
          if (!(counter & m_Indicator[i])) // is the offset axis bit off?
          {
            auto position = center;
            for (unsigned int k = 0; k < ImageDimension; ++k)
            {
              if (counter & m_Indicator[k])
              {
                position += stride[k];
              }
            }
            if (counter & m_Indicator[j]) // is the derivative axis bit on?
            {
              derivative += it.GetPixel(position);
            }
            else
            {
              derivative -= it.GetPixel(position);
            }
          } // if
        } // counter loop
        derivative *= m_DimConst2;
      } // if i!=j
      else
      {
        for (unsigned int counter = 0; counter < m_NumVertex; ++counter)
        {
          auto position = center;
          for (unsigned int k = 0; k < ImageDimension; ++k)
          {
            if ((k != i) && (counter & m_Indicator[k]))
            {
              position += stride[k];
            }
          }
          if (counter & m_Indicator[i])
          {
            derivative += it.GetPixel(position + stride[i]);
          }
          else
          {
            derivative -= it.GetPixel(position - stride[i]);
          }
        }
        derivative *= m_DimConst;
      }
      node->m_ManifoldNormal[i][j] = derivative;
    } // end derivative axis
    node->m_ManifoldNormal[i] /= (m_MinVectorNorm + node->m_ManifoldNormal[i].GetNorm());
  } // end offset axis (flux position)
}

template <typename TInputImage, typename TSparseOutputImage>
void
ImplicitManifoldNormalVectorFilter<TInputImage, TSparseOutputImage>::PostProcessOutput()
{
  if (m_UnsharpMaskingFlag)
  {
    const typename NodeListType::Pointer       nodelist = this->GetOutput()->GetNodeList();
    typename NodeListType::Iterator            it = nodelist->Begin();
    const typename NodeListType::ConstIterator last = nodelist->End();
    NormalVectorType                           nv;

    while (it != last)
    {
      nv = it->m_InputData * (1.0 + m_UnsharpMaskingWeight) - it->m_Data * m_UnsharpMaskingWeight;
      it->m_Data = nv / (m_MinVectorNorm + nv.GetNorm());
      ++it;
    }
  }
}
} // end namespace itk

#endif
