#pragma once
#include "imgui.h"

class FImguiMesh
{
	struct DrawElement
	{
		ImTextureID		TextureID;
		uint32			VtxOffset;
		uint32			IdxOffset;
		uint32			NumIndices;
		ImVec4			ScissorRect;
	};
public:
	void SetMesh(ImDrawData* InDrawData);
	void SetMesh(const TArray<ImDrawData*>& InDrawData);
	void DrawMesh(FRHICommandList& RHICmdList);

	static FVertexDeclarationRHIRef GetMeshDeclaration();
private:
	void _UpdateBufferSize(int32 InVtxNum, int32 InIdxNum);
	void _AppendDrawData(ImDrawData* InDrawData, ImDrawVert* VtxBuf, ImDrawIdx* IdxBuf, int32 BaseVtx = 0, int32 BaseIdx = 0);
public:
	FIndexBufferRHIRef			IndexBufferRHI;
	FVertexBufferRHIRef			VertexBufferRHI;
	int32						NumVertices;
	int32						NumTriangles;
	TArray<DrawElement>			AllDrawElements;
};
