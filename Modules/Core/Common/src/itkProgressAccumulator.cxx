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
#include "itkProgressAccumulator.h"

namespace itk
{
ProgressAccumulator::ProgressAccumulator()
  : m_MiniPipelineFilter(nullptr)
// Initialize the progress values
{
  // Create a member command
  m_CallbackCommand = CommandType::New();
  m_CallbackCommand->SetCallbackFunction(this, &Self::ReportProgress);
}

ProgressAccumulator::~ProgressAccumulator() { UnregisterAllFilters(); }

void
ProgressAccumulator::RegisterInternalFilter(GenericFilterType * filter, float weight)
{
  // Observe the filter
  const unsigned long progressTag = filter->AddObserver(ProgressEvent(), m_CallbackCommand);
  const unsigned long startTag = filter->AddObserver(StartEvent(), m_CallbackCommand);

  // Create a record for the filter
  struct FilterRecord record;

  record.Filter = filter;
  record.Weight = weight;
  record.ProgressObserverTag = progressTag;
  record.StartObserverTag = startTag;

  // Add the record to the list
  m_FilterRecord.push_back(record);
}

void
ProgressAccumulator::UnregisterAllFilters()
{
  // The filters should no longer be observing us

  for (auto & it : m_FilterRecord)
  {
    it.Filter->RemoveObserver(it.ProgressObserverTag);
    it.Filter->RemoveObserver(it.StartObserverTag);
  }

  // Clear the filter array
  m_FilterRecord.clear();

  // Reset the accumulated progress
  m_AccumulatedProgress = 0.0f;
  m_BaseAccumulatedProgress = 0.0f;
}

#if !defined(ITK_LEGACY_REMOVE)
void
ProgressAccumulator::ResetProgress()
{
  // Reset the accumulated progress
  m_AccumulatedProgress = 0.0f;
  m_BaseAccumulatedProgress = 0.0f;

  // Reset each of the individual progress meters
  for (auto it = m_FilterRecord.begin(); it != m_FilterRecord.end(); ++it)
  {
    it->Filter->UpdateProgress(0.0f);
  }
}
#endif

#if !defined(ITK_LEGACY_REMOVE)
void
ProgressAccumulator::ResetFilterProgressAndKeepAccumulatedProgress()
{
  // Do nothing.  After all, this method is deprecated.
}
#endif

void
ProgressAccumulator::ReportProgress(Object * who, const EventObject & event)
{
  const ProgressEvent pe;
  const StartEvent    se;

  if (typeid(event) == typeid(pe))
  {
    // Start the progress from the progress accumulated so far.
    m_AccumulatedProgress = m_BaseAccumulatedProgress;

    // Add up the new progress from different filters.
    for (auto & it : m_FilterRecord)
    {
      const float progress = it.Filter->GetProgress();
      if (progress != it.AccumulatedProgress)
      {
        m_AccumulatedProgress += progress * it.Weight;
        it.AccumulatedProgress = 0.0; // this filter is now active
      }
      // else skip this filter, as it has finished
    }

    // Update the progress of the client mini-pipeline filter
    m_MiniPipelineFilter->UpdateProgress(m_AccumulatedProgress);

    // check for abort
    if (m_MiniPipelineFilter->GetAbortGenerateData())
    {
      // Abort the filter that is reporting progress
      for (auto & fit : m_FilterRecord)
      {
        if (who == fit.Filter)
        {
          fit.Filter->AbortGenerateDataOn();
        }
      }
    }
  }
  else if (typeid(event) == typeid(se))
  {
    // When a filter is restarted, we can capture the progress it has made so far and add it
    // to the accumulated value.
    // This is especially helpful when streaming is used since the filter is restarted multiple
    // times for different regions of the input.
    // By capturing the start event, it is no longer necessary for filters that use the ProgressAccumulator
    // to explicitly call ResetFilterProgressAndKeepAccumulatedProgress().

    for (auto & it : m_FilterRecord)
    {
      if (who == it.Filter)
      {
        // On a start event, we need to capture the accumulated progress for this filter
        // and then reset this filter's progress.
        // It is not necessary to call UpdateProgress(0.0f) explicitly on the filter because this is done
        // automatically when the filter is restarted.
        it.AccumulatedProgress = it.Filter->GetProgress();
        m_BaseAccumulatedProgress += it.AccumulatedProgress * it.Weight;
      }
    }
  }
}

void
ProgressAccumulator::PrintSelf(std::ostream & os, Indent indent) const
{
  Superclass::PrintSelf(os, indent);

  if (m_MiniPipelineFilter)
  {
    os << indent << m_MiniPipelineFilter << std::endl;
  }
  os << indent << m_AccumulatedProgress << std::endl;
  os << indent << m_BaseAccumulatedProgress << std::endl;
}
} // End namespace itk
