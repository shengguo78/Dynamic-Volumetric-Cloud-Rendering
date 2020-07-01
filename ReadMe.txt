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

/////////////////////////////////////////////////////////////////////////////

Contents
--------

	- Project Overview
	- Build Prerequisites
        - Build Environment
	- Additional Notes
	- Version History


Project Overview
----------------

LuckyCloud is a multi-threaded real-time 3D cloud simulation for games designed to take 
advantage of the latest Intel(R) multi-core microprocessors.  The algorithm is grid based 
and threaded with Intel(R) Threading Building Blocks.

Rendering is performed with billboard technology using DirectX 9.*

For detailed information on this sample, please visit:
https://software.intel.com/content/www/us/en/develop/articles/dynamic-volumetric-cloud-rendering-for-games-on-multi-core-platforms.html


Build Prerequisites
-------------------

Required:
	* Microsoft Visual Studio 2008 w/SP1*
	* Microsoft DirectX SDK November 2008*
	* Intel(R) Threading Building Blocks for Open Source
	
Build Environment
-----------------

	Prerequisite: Intel(R) Threading Building Blocks
	------------------------------------------------

		In order to compile the project, TBB must be downloaded and extracted to:
	
		<SolutionDir>\TBB\*.h
		
		Intel(R) Threading Building Blocks for Open Source provides the basis for the 
		multithreaded implementation of Kaboom's task manager system as well as the 
		parallel fluid solver.
	
		Download a release of TBB from: http://www.threadingbuildingblocks.org/

	Prerequisite: Intel(R) C++ Compiler
	------------------------------------------------
		If you want to enable the SSE optimization options, you should install the Intel(R) C++ Compiler to compile the SSE intrinsics codes.
		Download Intel® C++ Compiler from : http://software.intel.com/en-us/articles/intel-software-evaluation-center/#compilers

Code & User Interface
---------------------

In this sample code we places 16 predefined cloud cubes in the sky. It is easy to add more.
To compare the threading performance on the fly, we simply use granularity to control the 
number of threads. In real case, you can set the number of threads in TBB init function.

User Interface controls are provided to change the number of available threads and cloud 
parameters. Click "Parameters" button.
		

Version History
---------------

Version 1.0.0
	Initial Version.

Credits
-------
Programmers
	Sheng Guo		sheng.guo@intel.com
	lu cage			cage.lu@intel.com
	xiaochang Wu		xiaochang.wu@intel.com


Special Thanks

	Ron Fosner, Bruce Zhang; Crabtree Scott for their supports


/////////////////////////////////////////////////////////////////////////////

* Other names and brands may be claimed as the property of others.
