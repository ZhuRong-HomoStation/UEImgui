#pragma once
#include "imgui.h"

namespace ImGui
{
	// ImGui Bezier widget. @r-lyeh, public domain
	// v1.03: improve grabbing, confine grabbers to area option, adaptive size, presets, preview.
	// v1.02: add BezierValue(); comments; usage
	// v1.01: out-of-bounds coord snapping; custom border width; spacing; cosmetics
	// v1.00: initial version
	//
	// [ref] http://robnapier.net/faster-bezier
	// [ref] http://easings.net/es#easeInSine
	//
	// Usage:
	// {  static float v[5] = { 0.390f, 0.575f, 0.565f, 1.000f }; 
	//    ImGui::Bezier( "easeOutSine", v );       // draw
	//    float y = ImGui::BezierValue( 0.5f, v ); // x delta in [0..1] range
	// }
	//
	// Source: https://github.com/ocornut/imgui/issues/786
	UEIMGUI_API int Bezier(const char* label, float P[5]);

	// Source: https://github.com/ocornut/imgui/issues/2718 
	UEIMGUI_API bool SelectableInput(const char* str_id, bool selected, ImGuiSelectableFlags flags, char* buf, size_t buf_size);

	// Source: https://github.com/ocornut/imgui/issues/2718 
	UEIMGUI_API bool SpinScaler(const char* label, ImGuiDataType data_type, void* data_ptr, const void* step, const void* step_fast, const char* format, ImGuiInputTextFlags flags);
	UEIMGUI_API bool SpinInt(const char* label, int* v, int step = 1, int step_fast = 100, ImGuiInputTextFlags flags = 0);
	UEIMGUI_API bool SpinFloat(const char* label, float* v, float step = 1.0f, float step_fast = 100.0f, const char* format = "%.3f", ImGuiInputTextFlags flags = 0);
	UEIMGUI_API bool SpinDouble(const char* label, double* v, double step = 1.0, double step_fast = 100.0, const char* format = "%.6f", ImGuiInputTextFlags flags = 0);

	// Source: https://github.com/ocornut/imgui/issues/1901 
	UEIMGUI_API bool BufferingBar(const char* label, float value, const ImVec2& size_arg, const ImU32& bg_col, const ImU32& fg_col);
	UEIMGUI_API bool Spinner(const char* label, float radius, int thickness, const ImU32& color);
	UEIMGUI_API void LoadingIndicatorCircle(const char* label, const float indicator_radius, const ImVec4& main_color, const ImVec4& backdrop_color, const int circle_count, const float speed);

	// Source: https://github.com/ocornut/imgui/issues/1537
	UEIMGUI_API void ToggleButton(const char* str_id, bool* v);

	// Source: https://github.com/ocornut/imgui/issues/1496 
	UEIMGUI_API void BeginGroupPanel(const char* name, const ImVec2& size = ImVec2(0, 0));
	UEIMGUI_API void EndGroupPanel();

	// Source: https://github.com/ocornut/imgui/issues/319 
	UEIMGUI_API bool Splitter(bool split_vertically, float thickness, float* size1, float* size2, float min_size1, float min_size2, float splitter_long_axis_size = -1.0f);

	// =========================Demo=========================
	UEIMGUI_API void DrawSmallWidgetDemo();
	UEIMGUI_API void DrawTextEditorDemo(bool* bIsOpen = nullptr);
}
