#include "Fuji.h"
#include "MFHeap.h"
#include "MFTexture_Internal.h"
#include "MFMaterial_Internal.h"
#include "Display_Internal.h"
#include "MFView_Internal.h"
#include "Renderer_PC.h"
#include "../../Source/Materials/Mat_Standard.h"

static MFMaterial *pSetMaterial;
extern uint32 renderSource;
extern uint32 currentRenderFlags;

extern IDirect3DDevice9 *pd3dDevice;

int MFMat_Standard_RegisterMaterial(void *pPlatformData)
{
	CALLSTACK;

	return 0;
}

void MFMat_Standard_UnregisterMaterial()
{
	CALLSTACK;

}

int MFMat_Standard_Begin(MFMaterial *pMaterial)
{
	CALLSTACK;

	MFMat_Standard_Data *pData = (MFMat_Standard_Data*)pMaterial->pInstanceData;

	if(pSetMaterial != pMaterial)
	{
		// set some render states
		if(pData->pTextures[pData->diffuseMapIndex])
		{
			RendererPC_SetTexture(0, pData->pTextures[pData->diffuseMapIndex]->pTexture);
			RendererPC_SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
			RendererPC_SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
			RendererPC_SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

			RendererPC_SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
			RendererPC_SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

			RendererPC_SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT2);
			pd3dDevice->SetTransform(D3DTS_TEXTURE0, (D3DMATRIX*)&pData->textureMatrix);
		}
		else
		{
			RendererPC_SetTexture(0, NULL);
		}

		switch(pData->materialType&MF_BlendMask)
		{
			case 0:
				RendererPC_SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
				RendererPC_SetRenderState(D3DRS_SRCBLEND, D3DBLEND_ONE);
				RendererPC_SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);
				break;
			case MF_AlphaBlend:
				RendererPC_SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
				RendererPC_SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
				RendererPC_SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
				break;
			case MF_Additive:
				RendererPC_SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
				RendererPC_SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
				RendererPC_SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
				break;
			case MF_Subtractive:
				RendererPC_SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
				RendererPC_SetRenderState(D3DRS_SRCBLEND, D3DBLEND_DESTCOLOR);
				RendererPC_SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ZERO);
				break;
		}
	}

	return 0;
}

void MFMat_Standard_CreateInstance(MFMaterial *pMaterial)
{
	CALLSTACK;

	pMaterial->pInstanceData = MFHeap_Alloc(sizeof(MFMat_Standard_Data));
	MFMat_Standard_Data *pData = (MFMat_Standard_Data*)pMaterial->pInstanceData;

	memset(pData, 0, sizeof(MFMat_Standard_Data));

	pData->ambient = MFVector::one;
	pData->diffuse = MFVector::one;

	pData->materialType = MF_AlphaBlend;
	pData->opaque = true;

	pData->textureMatrix = MFMatrix::identity;
	pData->uFrames = 1;
	pData->vFrames = 1;
}

void MFMat_Standard_DestroyInstance(MFMaterial *pMaterial)
{
	CALLSTACK;

	MFMat_Standard_Data *pData = (MFMat_Standard_Data*)pMaterial->pInstanceData;

	for(uint32 a=0; a<pData->textureCount; a++)
	{
		MFTexture_Destroy(pData->pTextures[a]);
	}

	MFHeap_Free(pMaterial->pInstanceData);
}
