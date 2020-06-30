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

#pragma once

#include "tbb/task_scheduler_init.h"
#include "tbb/task.h"
#include "tbb/spin_mutex.h"

// TBB task manager class adopted from Brad Werth's presentation at 
// Austin GDC 2008: "PetMe: Pamper Your Pets With CPU Power"
// Brad Werth, Sr. Software Engineer, Intel - Visual Computing Software Division.
// bradley.j.werth@intel.com
//	http://software.intel.com/en-us/videos/gdc-session-pamper-your-pets-with-cpu-power/
class TaskManager
{
public:
    static TaskManager *getTaskManager();

    typedef void (*JobFunction)(void *);
    
    class JobResult
    {
    public:
        JobResult(tbb::task *pTask);
        ~JobResult();
        
        bool isJobDone();
        void markAsDone() { m_bDone = true; }
        void markAsDoing() { m_bDone = false; }
        void waitUntilDone();
        
    private:
        bool m_bDone;
        tbb::task *m_pTask;
        tbb::spin_mutex m_tMutex;
    };

    class JobTask: public tbb::task
    {
    public:
        JobTask(JobFunction tFunc, void *pParam, JobResult *pResult):
			m_tFunc(tFunc),
			m_pParam(pParam),
			m_pResult(pResult)
		{}
        tbb::task *execute();
        
    private:
        JobFunction m_tFunc;
        void *m_pParam;
        JobResult *m_pResult;
    };
    
    TaskManager();

    tbb::task *m_pRootTask;
    JobResult *m_pResult;

	void init(int NumThread);
    void shutdown(void);
    
    unsigned int getThreadCount();
    JobResult *getJobResult() {return m_pResult;};
    void submitJob(JobFunction tFunc, void *pParam);
	
private:
    static TaskManager *sm_pTaskManager;
    tbb::task_scheduler_init *m_pTaskScheduler;
};