module fuji.c.MFResource;

import fuji.c.MFString : toDStr;

nothrow:
@nogc:

enum MFResourceType
{
	All = -2,
	Unknown = -1,

	Texture = 0,
	Material,
	VertexDecl,
	VertexBuffer,
	IndexBuffer,
	BlendState,
	SamplerState,
	DepthStencilState,
	RasteriserState,
	Shader,
	Effect,
	Model,
	Animation,
	Sound,
	Font,

	User,

	Max = 32,

	UserCount = Max - User
};

struct MFResource
{
	uint hash;
	int type_refCount;
	const(char)* pName;

nothrow:
@nogc:
	@property MFResourceType type() const pure { return cast(MFResourceType)(type_refCount & 0xFF); }
	@property int refCount() const pure { return type_refCount >> 8; }
	@property const(char)[] name() const pure { return pName.toDStr; }

	int addRef() pure
	{
		type_refCount += 1 << 8;
		return type_refCount >> 8;
	}

	int release()
	{
		int rc = type_refCount >> 8;
		if(rc == 1)
			return MFResource_Release(&this);

		type_refCount -= 1 << 8;
		return rc - 1;
	}
};

struct MFResourceIterator;

extern (C) void MFResource_AddResource(MFResource* pResource, int type, uint hash, const(char)* pName = null);
extern (C) void MFResource_RemoveResource(MFResource* pResource);

extern (C) MFResource* MFResource_Find(uint hash);

extern (C) MFResourceType MFResource_GetType(const(MFResource)* pResource) pure;
extern (C) uint MFResource_GetHash(const(MFResource)* pResource) pure;
extern (C) int MFResource_GetRefCount(const(MFResource)* pResource) pure;
extern (C) const(char)* MFResource_GetName(const(MFResource)* pResource) pure;

int MFResource_AddRef(MFResource *pResource) pure
{
	pResource.type_refCount += 1 << 8;
	return pResource.type_refCount >> 8;
}
extern (C) int MFResource_Release(MFResource *pResource);

extern (C) int MFResource_GetNumResources(MFResourceType type = MFResourceType.All);

extern (C) MFResourceIterator* MFResource_EnumerateFirst(MFResourceType type = MFResourceType.All);
extern (C) MFResourceIterator* MFResource_EnumerateNext(MFResourceIterator* pIterator, MFResourceType type = MFResourceType.All);
extern (C) MFResource* MFResource_Get(MFResourceIterator* pIterator);

bool MFResource_IsType(MFResource* pResource, MFResourceType type) pure
{
	return MFResource_GetType(pResource) == type;
}
