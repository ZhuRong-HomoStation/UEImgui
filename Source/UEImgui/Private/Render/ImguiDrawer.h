#pragma once
#include "Rendering/RenderingCommon.h"

class FImguiDrawer : public ICustomSlateElement
{
public:

protected:
	// ~Begin ICustomSlateElement API 
	virtual void DrawRenderThread(FRHICommandListImmediate& RHICmdList, const void* RenderTarget) override;
	// ~End ICustomSlateElement API 
	
};
