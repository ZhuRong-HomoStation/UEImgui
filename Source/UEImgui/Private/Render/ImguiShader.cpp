#include "ImguiShader.h"

IMPLEMENT_SHADER_TYPE(, FImguiShaderVs, TEXT("/Plugin/UEImgui/Private/ImguiShader.usf"), TEXT("ImguiShader_VS"), SF_Vertex);
IMPLEMENT_SHADER_TYPE(, FImguiShaderPs, TEXT("/Plugin/UEImgui/Private/ImguiShader.usf"), TEXT("ImguiShader_PS"), SF_Pixel);
IMPLEMENT_SHADER_TYPE(, FImguiShaderPsNoTex, TEXT("/Plugin/UEImgui/Private/ImguiShader.usf"), TEXT("ImguiShader_PSNoTex"), SF_Pixel);