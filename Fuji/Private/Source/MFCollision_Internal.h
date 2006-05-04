#if !defined(_MFCOLLISION_INTERNAL_H)
#define _MFCOLLISION_INTERNAL_H

#include "MFCollision.h"
#include "MFBoundingVolume.h"
#include "MFPtrList.h"

struct MFCollisionTemplate
{
	MFBoundingVolume boundingVolume;
	uint32 type;

	void *pCollisionTemplateData;
	const char *pName;
};

struct MFCollisionItem
{
	MFMatrix worldPos;
	MFCollisionTemplate *pTemplate;
	uint16 refCount;
	uint16 flags;
};

struct MFCollisionSphere
{
	float radius;	// this could be jammed into the w of the position
};

struct MFCollisionBox
{
	MFVector radius;	// this could easilly be jammed into the 4th column of the matrix
};

struct MFCollisionMesh
{
	int numTris;
	MFCollisionTriangle *pTriangles;
};

struct MFCollisionField
{
	MFVector fieldMin;
	MFVector fieldMax;
	MFVector cellSize;

	int width, height, depth;

	MFPtrList<MFCollisionItem> itemList;

	MFCollisionItem ***pppItems;
};

MFCollisionItem* MFCollision_RayFieldTest(const MFVector& rayPos, const MFVector& rayDir, MFCollisionItem *pField, MFRayIntersectionResult *pResult);

#endif // _MFCOLLISION_INTERNAL_H
