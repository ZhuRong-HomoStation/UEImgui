#pragma once
#include "Rendering/RenderingCommon.h"
#include "imgui.h"

class FImguiDrawer : public ICustomSlateElement
{
public:
	struct DrawElement
	{
		ImTextureID		TextureID;
		uint32			VtxOffset;
		uint32			IdxOffset;
		uint32			NumIndices;
		FSlateRect		ScissorRect;
	};
public:
	void SetDrawData(ImDrawData* InDrawData);
	void SetDrawData(const TArray<ImDrawData*>& InDrawData);
	void SetDrawData(const TArray<ImDrawList*>& InDrawData);

	void SetSlateTransform(FVector2D InVertexOffset, float InScale, const FMatrix& InOrthoMatrix)
	{
		Scale = InScale;
		VertexOffset = InVertexOffset;
		OrthoMatrix = InOrthoMatrix;
	}

	void SetClipRect(FSlateRect InRect) { ClippingRect = InRect; }

	static TSharedPtr<FImguiDrawer, ESPMode::ThreadSafe> AllocDrawer();
protected:
	// ~Begin ICustomSlateElement API 
	virtual void DrawRenderThread(FRHICommandListImmediate& RHICmdList, const void* RenderTarget) override;
	// ~End ICustomSlateElement API 
private:
	FVertexDeclarationRHIRef _GetMeshDeclaration();
	void _UpdateBufferSize(int32 InVtxNum, int32 InIdxNum);
	void _AppendDrawData(ImDrawData* InDrawData, ImDrawVert* InVtxBuf, ImDrawIdx* InIdxBuf, int32 BaseVtx = 0, int32 BaseIdx = 0);
	void _AppendDrawList(ImDrawList* InDrawList, ImDrawVert* InVtxBuf, ImDrawIdx* InIdxBuf, int32 BaseVtx = 0, int32 BaseIdx = 0);
	FRHITexture2D* _GetTextureFromID(ImTextureID InID);
public:
	bool				bIsFree = false;
protected :
	FIndexBufferRHIRef	IndexBufferRHI;
	FVertexBufferRHIRef	VertexBufferRHI;
	int32				NumVertices = 0;
	int32				NumTriangles = 0;
	ImDrawVert*			VtxBuf = nullptr;
    ImDrawIdx*			IdxBuf = nullptr;
	
	FVector2D			VertexOffset = FVector2D::ZeroVector;
	float				Scale = 1.f;
	FMatrix				OrthoMatrix = FMatrix::Identity;
	FSlateRect			ClippingRect;
	TArray<DrawElement>	AllDrawElements;
private:
	static TArray<TSharedPtr<FImguiDrawer, ESPMode::ThreadSafe>> GlobalPool;
};
