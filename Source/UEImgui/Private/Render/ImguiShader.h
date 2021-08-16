#pragma once
#include "CoreMinimal.h"
#include "GlobalShader.h"
#include "ShaderParameterUtils.h"
#include "ShaderParameterStruct.h"

class FImguiShaderVs : public FGlobalShader
{
	DECLARE_SHADER_TYPE(FImguiShaderVs, Global)
public:
	FImguiShaderVs()
	{ }

	FImguiShaderVs(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
        : FGlobalShader(Initializer)
	{
		InTransform.Bind(Initializer.ParameterMap, TEXT("InTransform"));
	}

	void SetParameters(FRHICommandList& RHICmdList, const FMatrix& TransformValue)
	{
		SetShaderValue(RHICmdList, RHICmdList.GetBoundVertexShader(), InTransform, TransformValue);
	}

	static bool ShouldCompilePermutation(
        const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
	}
	static void ModifyCompilationEnvironment(
        const FGlobalShaderPermutationParameters& Parameters,
        FShaderCompilerEnvironment& OutEnvironment)
	{
		FGlobalShader::ModifyCompilationEnvironment(Parameters,OutEnvironment);
	}
private:
	LAYOUT_FIELD(FShaderParameter, InTransform)
};

class FImguiShaderPs : public FGlobalShader
{
	DECLARE_SHADER_TYPE(FImguiShaderPs, Global)
public:
	FImguiShaderPs()
	{ }

	FImguiShaderPs(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
        : FGlobalShader(Initializer)
	{
		InTexture.Bind(Initializer.ParameterMap, TEXT("InTexture"));
		InTextureSampler.Bind(Initializer.ParameterMap, TEXT("InTextureSampler"));
		InHasTexture.Bind(Initializer.ParameterMap, TEXT("InHasTexture"));
	}
	
	static bool ShouldCompilePermutation(
        const FGlobalShaderPermutationParameters& Parameters)
	{
		return IsFeatureLevelSupported(Parameters.Platform, ERHIFeatureLevel::SM5);
	}
	static void ModifyCompilationEnvironment(
        const FGlobalShaderPermutationParameters& Parameters,
        FShaderCompilerEnvironment& OutEnvironment)
	{
		FGlobalShader::ModifyCompilationEnvironment(Parameters,OutEnvironment);
	}

	template<typename TShaderRHIParamRef>
    void SetParameters(FRHICommandListImmediate& RHICmdList, const TShaderRHIParamRef ShaderRHI, FRHITexture2D* SourceTexture)
	{
		SetTextureParameter(RHICmdList, ShaderRHI, InTexture, SourceTexture);
		RHICmdList.SetShaderSampler(ShaderRHI, InTextureSampler.GetBaseIndex(), TStaticSamplerState<SF_Bilinear, AM_Wrap, AM_Wrap, AM_Wrap>::GetRHI());
	}

	template<typename TShaderRHIParamRef>
    void SetHasTexture(FRHICommandListImmediate& RHICmdList, const TShaderRHIParamRef ShaderRHI, bool bHasTexture)
	{
		SetShaderValue(RHICmdList, ShaderRHI, InHasTexture, int(bHasTexture));
	}

private:
	LAYOUT_FIELD(FShaderResourceParameter, InTexture)
	LAYOUT_FIELD(FShaderResourceParameter, InTextureSampler)
	LAYOUT_FIELD(FShaderParameter, InHasTexture);
};
