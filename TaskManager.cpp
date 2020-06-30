//-------------------------------------------------------------------------------------
//
// Copyright 2009 Intel Corporation
// All Rights Reserved
//
// Permission is granted to use, copy, distribute and prepare derivative works of this
// software for any purpose and without fee, provided, that the above copyright notice
// and this statement appear in all copies.  Intel makes no representations about the
// suitability of this software for any purpose.  THIS SOFTWARE IS PROVIDED "AS IS."
// INTEL SPECIFICALLY DISCLAIMS ALL WARRANTIES, EXPRESS OR IMPLIED, AND ALL LIABILITY,
// INCLUDING CONSEQUENTIAL AND OTHER INDIRECT DAMAGES, FOR THE USE OF THIS SOFTWARE,
// INCLUDING LIABILITY FOR INFRINGEMENT OF ANY PROPRIETARY RIGHTS, AND INCLUDING THE
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  Intel does not
// assume any responsibility for any errors which may appear in this software nor any
// responsibility to update it.
//
//--------------------------------------------------------------------------------------
// DXUT was adapted from the Microsoft DirectX SDK(November 2008)
// Copyright (c) Microsoft Corporation. All rights reserved.
//
// The skybox is free downloaded from :
//   http://en.pudn.com/downloads119/sourcecode/others/detail508412_en.html
//-------------------------------------------------------------------------------------

#pragma warning(disable:4996) // For deprecated mbstowcs calls in TBB
#include "TaskManager.h"

#include "tbb/task_scheduler_init.h"
#include "tbb/task.h"

TaskManager *TaskManager::sm_pTaskManager = NULL;

TaskManager *TaskManager::getTaskManager()
{
    if(sm_pTaskManager == NULL)
    {
        sm_pTaskManager = new TaskManager();
    }
    
    return sm_pTaskManager;
}

TaskManager::JobResult::JobResult(tbb::task *pTask):
    m_pTask(pTask),
    m_bDone(true),
    m_tMutex()
{}

TaskManager::JobResult::~JobResult()
{
}

void TaskManager::JobResult::waitUntilDone()
{
    if(m_pTask != NULL)
    {
        //return, for the first waitUntilDone() call when there is no job submitted yet
        if (m_pTask->ref_count() == 0) return;
        if(m_pTask != NULL)
        {
            m_pTask->wait_for_all();
        }
    }
}

bool TaskManager::JobResult::isJobDone()
{
     if((m_bDone) && (m_pTask != NULL))
     {
         //The child tasks are finished, but we need to call
         //wait_for_all to let the root task's ref count to be zero for next using
         if (m_pTask->ref_count() > 0)
             m_pTask->wait_for_all();
     }
    return m_bDone;
}

tbb::task *TaskManager::JobTask::execute()
{
    m_tFunc(m_pParam);
    m_pResult->markAsDone(); //child tasks are finished
    
    return NULL;
}

TaskManager::TaskManager():
    m_pTaskScheduler(NULL)
{}

void TaskManager::init(int NumThread)
{
	m_pTaskScheduler = new tbb::task_scheduler_init(NumThread);

    //create a empty task as the root task, it will exist until shutdown.
    m_pRootTask = new(tbb::task::allocate_root()) tbb::empty_task();
    m_pResult = new JobResult(m_pRootTask);
}

unsigned int TaskManager::getThreadCount()
{
    // Return the number of new threads created by TBB specifically, don't include the primary thread in this count
    return (tbb::task_scheduler_init::default_num_threads() - 1);
}

void TaskManager::submitJob(JobFunction tFunc, void *pParam)
{
    JobTask *pJobTask = new(m_pRootTask->allocate_child()) JobTask(tFunc, pParam, m_pResult);

    m_pResult->markAsDoing();
    m_pRootTask->set_ref_count(2);
    m_pRootTask->spawn(*pJobTask);
    
    return;
}

void TaskManager::shutdown()
{
    if (m_pRootTask != NULL)
    {
        if (m_pRootTask->ref_count() != 0)
            m_pRootTask->wait_for_all();
        m_pRootTask->destroy(*m_pRootTask);
        m_pRootTask = NULL;
    }
    if(m_pResult)
    {
        delete m_pResult;
        m_pResult = NULL;
    }
    if(m_pTaskScheduler)
    {
        delete m_pTaskScheduler;
        m_pTaskScheduler = NULL;
    }
    delete sm_pTaskManager;
    sm_pTaskManager = NULL;
}