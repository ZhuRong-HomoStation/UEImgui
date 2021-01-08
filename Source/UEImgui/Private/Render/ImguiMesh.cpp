#include "ImguiMesh.h"

void FImguiMesh::SetMesh(ImDrawData* InDrawData)
{
	// get vertex and index num 
	int32 VtxNum = InDrawData->TotalVtxCount;
	int32 IdxNum = InDrawData->TotalIdxCount;

	// update buffer 
	_UpdateBufferSize(VtxNum, IdxNum);

	// copy data
	ImDrawVert* VtxBuf = (ImDrawVert*)RHILockVertexBuffer(VertexBufferRHI, 0, VtxNum * sizeof(ImDrawVert), EResourceLockMode::RLM_WriteOnly);
	ImDrawIdx* IdxBuf = (ImDrawIdx*)RHILockIndexBuffer(IndexBufferRHI, 0, sizeof(ImDrawIdx) * IdxNum, EResourceLockMode::RLM_WriteOnly);
	_AppendDrawData(InDrawData, VtxBuf, IdxBuf);
	RHIUnlockVertexBuffer(VertexBufferRHI);
	RHIUnlockIndexBuffer(IndexBufferRHI);
}

void FImguiMesh::SetMesh(const TArray<ImDrawData*>& InDrawData)
{
	// get vertex and index num 
	int32 VtxNum = 0;
	int32 IdxNum = 0;
	for (ImDrawData* DrawData : InDrawData)
	{
		VtxNum += DrawData->TotalVtxCount;
		IdxNum += DrawData->TotalIdxCount;
	}

	// update buffer
	_UpdateBufferSize(VtxNum, IdxNum);

	// copy dat
	ImDrawVert* VtxBuf = (ImDrawVert*)RHILockVertexBuffer(VertexBufferRHI, 0, VtxNum * sizeof(ImDrawVert), EResourceLockMode::RLM_WriteOnly);
	ImDrawIdx* IdxBuf = (ImDrawIdx*)RHILockIndexBuffer(IndexBufferRHI, 0, sizeof(ImDrawIdx) * IdxNum, EResourceLockMode::RLM_WriteOnly);

	int32 BaseVtx = 0;
	int32 BaseIdx = 0;
	for (ImDrawData* DrawData : InDrawData)
	{
		_AppendDrawData(DrawData, VtxBuf, IdxBuf, BaseVtx, BaseIdx);
		BaseVtx += DrawData->TotalVtxCount;
		BaseIdx += DrawData->TotalIdxCount;
	}
	
	RHIUnlockVertexBuffer(VertexBufferRHI);
	RHIUnlockIndexBuffer(IndexBufferRHI);
}

void FImguiMesh::DrawMesh(FRHICommandList& RHICmdList)
{
	check(IsInRenderingThread());

	RHICmdList.SetStreamSource(0, VertexBufferRHI, 0);
	for (DrawElement& Element : AllDrawElements)
	{
		RHICmdList.SetScissorRect(
			true,
			Element.ScissorRect.x,
			Element.ScissorRect.y,
			Element.ScissorRect.z,
			Element.ScissorRect.w);
		RHICmdList.DrawIndexedPrimitive(
			IndexBufferRHI,
			Element.VtxOffset,
			0,
			NumVertices,
			Element.IdxOffset,
			Element.NumIndices / 3,
			1);
	}
}

FVertexDeclarationRHIRef FImguiMesh::GetMeshDeclaration()
{
	static FVertexDeclarationRHIRef VertexDeclarationRHI;
	if (!VertexDeclarationRHI.IsValid())
	{
		FVertexDeclarationElementList Elements;
		uint16 Stride = sizeof(ImDrawVert);
		Elements.Add(FVertexElement(0,STRUCT_OFFSET(ImDrawVert,pos),VET_Float2,0,Stride));
		Elements.Add(FVertexElement(0,STRUCT_OFFSET(ImDrawVert,uv),VET_Float2,1,Stride));
		Elements.Add(FVertexElement(0,STRUCT_OFFSET(ImDrawVert,col),VET_UByte4N,2,Stride));
		VertexDeclarationRHI = PipelineStateCache::GetOrCreateVertexDeclaration(Elements);
	}
	return VertexDeclarationRHI;
}

void FImguiMesh::_UpdateBufferSize(int32 InVtxNum, int32 InIdxNum)
{
	FRHIResourceCreateInfo CreateInfo;
	if (InVtxNum > NumVertices)
	{
        VertexBufferRHI = RHICreateVertexBuffer(sizeof(ImDrawVert) * InVtxNum, BUF_Dynamic, CreateInfo);
		NumVertices = InVtxNum;
    }
	if (InIdxNum > NumTriangles * 3)
	{
		check(InIdxNum % 3 == 0);
		IndexBufferRHI = RHICreateIndexBuffer(sizeof(ImDrawIdx), sizeof(ImDrawIdx) * InIdxNum, BUF_Dynamic, CreateInfo);
		NumTriangles = InIdxNum / 3;
	}
}

void FImguiMesh::_AppendDrawData(ImDrawData* InDrawData, ImDrawVert* VtxBuf, ImDrawIdx* IdxBuf, int32 BaseVtx,
	int32 BaseIdx)
{
	// offset vtx and idx 
	VtxBuf += BaseVtx;
	IdxBuf += BaseIdx;
	
	uint32 GlobalVtxOffset = BaseVtx;
	uint32 GlobalIdxOffset = BaseIdx;
	for (int32 ListIdx = 0; ListIdx < InDrawData->CmdListsCount; ++ListIdx)
	{ 
		ImDrawList* DrawList = InDrawData->CmdLists[ListIdx];

		// copy vertices and indices
		FMemory::Memcpy(VtxBuf + GlobalVtxOffset, DrawList->VtxBuffer.Data, DrawList->VtxBuffer.Size * sizeof(ImDrawVert));
		FMemory::Memcpy(IdxBuf + GlobalIdxOffset, DrawList->IdxBuffer.Data, DrawList->IdxBuffer.Size * sizeof(ImDrawIdx));

		// copy draw commands
		for (int32 CmdIdx = 0; CmdIdx < DrawList->CmdBuffer.Size; ++CmdIdx)
		{
			ImDrawCmd& DrawCmd = DrawList->CmdBuffer[CmdIdx];

			AllDrawElements.AddUninitialized();
			auto& Element = AllDrawElements.Top();
			Element.TextureID = DrawCmd.TextureId;
			Element.VtxOffset = DrawCmd.VtxOffset + GlobalVtxOffset;
			Element.IdxOffset = DrawCmd.IdxOffset + GlobalIdxOffset;
			Element.NumIndices = DrawCmd.ElemCount;
			Element.ScissorRect = DrawCmd.ClipRect;
		}
		GlobalVtxOffset += DrawList->IdxBuffer.Size;
		GlobalIdxOffset += DrawList->VtxBuffer.Size;
	}
}
