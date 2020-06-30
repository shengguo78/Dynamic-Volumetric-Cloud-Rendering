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
//=============================================================================
// Heightmap.h by Frank Luna (C) 2004 All Rights Reserved.
//=============================================================================

#ifndef HEIGHTMAP_H
#define HEIGHTMAP_H

#include "d3dUtil.h"
#include "Table.h"
#include <string>

class Heightmap
{
public:
	Heightmap();
	Heightmap(int m, int n);
	Heightmap(int m, int n, 
		LPCWSTR filename, float heightScale, float heightOffset);

	void recreate(int m, int n);

	void loadRAW(int m, int n,
		LPCWSTR filename, float heightScale, float heightOffset);

	int numRows()const;
	int numCols()const;

	// For non-const objects
	float& operator()(int i, int j);

	// For const objects
	const float& operator()(int i, int j)const;

private:
	bool  inBounds(int i, int j);
	float sampleHeight3x3(int i, int j);
	void  filter3x3();
private:
	LPCWSTR  mHeightMapFilename;
	Table<float> mHeightMap;
	float        mHeightScale;
	float        mHeightOffset;
};

#endif //HEIGHTMAP_H