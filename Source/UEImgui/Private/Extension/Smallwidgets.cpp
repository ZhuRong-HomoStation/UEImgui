#include <fstream>

#include "imgui_internal.h"
#include "Extension/SmallWidgets.h"
#include "Extension/TextEditor.h"
#include "Interfaces/IPluginManager.h"

template<int steps>
    void bezier_table(ImVec2 P[4], ImVec2 results[steps + 1]) {
	static float C[(steps + 1) * 4], *K = 0;
	if (!K) {
		K = C;
		for (unsigned step = 0; step <= steps; ++step) {
			float t = (float) step / (float) steps;
			C[step * 4 + 0] = (1 - t)*(1 - t)*(1 - t);   // * P0
			C[step * 4 + 1] = 3 * (1 - t)*(1 - t) * t; // * P1
			C[step * 4 + 2] = 3 * (1 - t) * t*t;     // * P2
			C[step * 4 + 3] = t * t*t;               // * P3
		}
	}
	for (unsigned step = 0; step <= steps; ++step) {
		ImVec2 point = {
			K[step * 4 + 0] * P[0].x + K[step * 4 + 1] * P[1].x + K[step * 4 + 2] * P[2].x + K[step * 4 + 3] * P[3].x,
            K[step * 4 + 0] * P[0].y + K[step * 4 + 1] * P[1].y + K[step * 4 + 2] * P[2].y + K[step * 4 + 3] * P[3].y
        };
		results[step] = point;
	}
}

float BezierValue(float dt01, float P[4]) {
	enum { STEPS = 256 };
	ImVec2 Q[4] = { { 0, 0 }, { P[0], P[1] }, { P[2], P[3] }, { 1, 1 } };
	ImVec2 results[STEPS + 1];
	bezier_table<STEPS>(Q, results);
	return results[(int) ((dt01 < 0 ? 0 : dt01 > 1 ? 1 : dt01) * STEPS)].y;
}

int ImGui::Bezier(const char* label, float P[5])
{
	// visuals
	enum { SMOOTHNESS = 64 }; // curve smoothness: the higher number of segments, the smoother curve
	enum { CURVE_WIDTH = 4 }; // main curved line width
	enum { LINE_WIDTH = 1 }; // handlers: small lines width
	enum { GRAB_RADIUS = 8 }; // handlers: circle radius
	enum { GRAB_BORDER = 2 }; // handlers: circle border width
	enum { AREA_CONSTRAINED = true }; // should grabbers be constrained to grid area?
	enum { AREA_WIDTH = 128 }; // area width in pixels. 0 for adaptive size (will use max avail width)

	// curve presets
	static struct
	{
		const char* name;
		float points[4];
	} presets[] = {
			{"Linear", 0.000f, 0.000f, 1.000f, 1.000f},

			{"In Sine", 0.470f, 0.000f, 0.745f, 0.715f},
			{"In Quad", 0.550f, 0.085f, 0.680f, 0.530f},
			{"In Cubic", 0.550f, 0.055f, 0.675f, 0.190f},
			{"In Quart", 0.895f, 0.030f, 0.685f, 0.220f},
			{"In Quint", 0.755f, 0.050f, 0.855f, 0.060f},
			{"In Expo", 0.950f, 0.050f, 0.795f, 0.035f},
			{"In Circ", 0.600f, 0.040f, 0.980f, 0.335f},
			{"In Back", 0.600f, -0.28f, 0.735f, 0.045f},

			{"Out Sine", 0.390f, 0.575f, 0.565f, 1.000f},
			{"Out Quad", 0.250f, 0.460f, 0.450f, 0.940f},
			{"Out Cubic", 0.215f, 0.610f, 0.355f, 1.000f},
			{"Out Quart", 0.165f, 0.840f, 0.440f, 1.000f},
			{"Out Quint", 0.230f, 1.000f, 0.320f, 1.000f},
			{"Out Expo", 0.190f, 1.000f, 0.220f, 1.000f},
			{"Out Circ", 0.075f, 0.820f, 0.165f, 1.000f},
			{"Out Back", 0.175f, 0.885f, 0.320f, 1.275f},

			{"InOut Sine", 0.445f, 0.050f, 0.550f, 0.950f},
			{"InOut Quad", 0.455f, 0.030f, 0.515f, 0.955f},
			{"InOut Cubic", 0.645f, 0.045f, 0.355f, 1.000f},
			{"InOut Quart", 0.770f, 0.000f, 0.175f, 1.000f},
			{"InOut Quint", 0.860f, 0.000f, 0.070f, 1.000f},
			{"InOut Expo", 1.000f, 0.000f, 0.000f, 1.000f},
			{"InOut Circ", 0.785f, 0.135f, 0.150f, 0.860f},
			{"InOut Back", 0.680f, -0.55f, 0.265f, 1.550f},

			// easeInElastic: not a bezier
			// easeOutElastic: not a bezier
			// easeInOutElastic: not a bezier
			// easeInBounce: not a bezier
			// easeOutBounce: not a bezier
			// easeInOutBounce: not a bezier
		};


	// preset selector

	bool reload = 0;
	ImGui::PushID(label);
	if (ImGui::ArrowButton("##lt", ImGuiDir_Left))
	{
		// ImGui::ArrowButton(ImGui::GetCurrentWindow()->GetID("##lt"), ImGuiDir_Left, ImVec2(0, 0), 0)
		if (--P[4] >= 0) reload = 1;
		else ++P[4];
	}
	ImGui::SameLine();

	if (ImGui::Button("Presets"))
	{
		ImGui::OpenPopup("!Presets");
	}
	if (ImGui::BeginPopup("!Presets"))
	{
		for (int i = 0; i < IM_ARRAYSIZE(presets); ++i)
		{
			if (i == 1 || i == 9 || i == 17) ImGui::Separator();
			if (ImGui::MenuItem(presets[i].name, NULL, P[4] == i))
			{
				P[4] = i;
				reload = 1;
			}
		}
		ImGui::EndPopup();
	}
	ImGui::SameLine();

	if (ImGui::ArrowButton("##rt", ImGuiDir_Right))
	{
		// ImGui::ArrowButton(ImGui::GetCurrentWindow()->GetID("##rt"), ImGuiDir_Right, ImVec2(0, 0), 0)
		if (++P[4] < IM_ARRAYSIZE(presets)) reload = 1;
		else --P[4];
	}
	ImGui::SameLine();
	ImGui::PopID();

	if (reload)
	{
		memcpy(P, presets[(int)P[4]].points, sizeof(float) * 4);
	}

	// bezier widget

	const ImGuiStyle& Style = GetStyle();
	const ImGuiIO& IO = GetIO();
	ImDrawList* DrawList = GetWindowDrawList();
	ImGuiWindow* Window = GetCurrentWindow();
	if (Window->SkipItems)
		return false;

	// header and spacing
	int changed = SliderFloat4(label, P, 0, 1, "%.3f", 1.0f);
	int hovered = IsItemActive() || IsItemHovered(); // IsItemDragged() ?
	Dummy(ImVec2(0, 3));

	// prepare canvas
	const float avail = GetContentRegionAvail().x;
	const float dim = AREA_WIDTH > 0 ? AREA_WIDTH : avail;
	ImVec2 Canvas(dim, dim);

	ImRect bb(Window->DC.CursorPos, Window->DC.CursorPos + Canvas);
	ItemSize(bb);
	if (!ItemAdd(bb, NULL))
		return changed;

	const ImGuiID id = Window->GetID(label);
	hovered |= 0 != ItemHoverable(ImRect(bb.Min, bb.Min + ImVec2(avail, dim)), id);

	RenderFrame(bb.Min, bb.Max, GetColorU32(ImGuiCol_FrameBg, 1), true, Style.FrameRounding);

	// background grid
	for (int i = 0; i <= Canvas.x; i += (Canvas.x / 4))
	{
		DrawList->AddLine(
			ImVec2(bb.Min.x + i, bb.Min.y),
			ImVec2(bb.Min.x + i, bb.Max.y),
			GetColorU32(ImGuiCol_TextDisabled));
	}
	for (int i = 0; i <= Canvas.y; i += (Canvas.y / 4))
	{
		DrawList->AddLine(
			ImVec2(bb.Min.x, bb.Min.y + i),
			ImVec2(bb.Max.x, bb.Min.y + i),
			GetColorU32(ImGuiCol_TextDisabled));
	}

	// eval curve
	ImVec2 Q[4] = {{0, 0}, {P[0], P[1]}, {P[2], P[3]}, {1, 1}};
	ImVec2 results[SMOOTHNESS + 1];
	bezier_table<SMOOTHNESS>(Q, results);

	// control points: 2 lines and 2 circles
	{
		// handle grabbers
		ImVec2 mouse = GetIO().MousePos, pos[2];
		float distance[2];

		for (int i = 0; i < 2; ++i)
		{
			pos[i] = ImVec2(P[i * 2 + 0], 1 - P[i * 2 + 1]) * (bb.Max - bb.Min) + bb.Min;
			distance[i] = (pos[i].x - mouse.x) * (pos[i].x - mouse.x) + (pos[i].y - mouse.y) * (pos[i].y - mouse.y);
		}

		int selected = distance[0] < distance[1] ? 0 : 1;
		if (distance[selected] < (4 * GRAB_RADIUS * 4 * GRAB_RADIUS))
		{
			SetTooltip("(%4.3f, %4.3f)", P[selected * 2 + 0], P[selected * 2 + 1]);

			if (/*hovered &&*/ (IsMouseClicked(0) || IsMouseDragging(0)))
			{
				float& px = (P[selected * 2 + 0] += GetIO().MouseDelta.x / Canvas.x);
				float& py = (P[selected * 2 + 1] -= GetIO().MouseDelta.y / Canvas.y);

				if (AREA_CONSTRAINED)
				{
					px = (px < 0 ? 0 : (px > 1 ? 1 : px));
					py = (py < 0 ? 0 : (py > 1 ? 1 : py));
				}

				changed = true;
			}
		}
	}

	// if (hovered || changed) DrawList->PushClipRectFullScreen();

	// draw curve
	{
		ImColor color(GetStyle().Colors[ImGuiCol_PlotLines]);
		for (int i = 0; i < SMOOTHNESS; ++i)
		{
			ImVec2 p = {results[i + 0].x, 1 - results[i + 0].y};
			ImVec2 q = {results[i + 1].x, 1 - results[i + 1].y};
			ImVec2 r(p.x * (bb.Max.x - bb.Min.x) + bb.Min.x, p.y * (bb.Max.y - bb.Min.y) + bb.Min.y);
			ImVec2 s(q.x * (bb.Max.x - bb.Min.x) + bb.Min.x, q.y * (bb.Max.y - bb.Min.y) + bb.Min.y);
			DrawList->AddLine(r, s, color, CURVE_WIDTH);
		}
	}

	// draw preview (cycles every 1s)
	static clock_t epoch = clock();
	ImVec4 white(GetStyle().Colors[ImGuiCol_Text]);
	for (int i = 0; i < 3; ++i)
	{
		double now = ((clock() - epoch) / (double)CLOCKS_PER_SEC);
		float delta = ((int)(now * 1000) % 1000) / 1000.f;
		delta += i / 3.f;
		if (delta > 1) delta -= 1;
		int idx = (int)(delta * SMOOTHNESS);
		float evalx = results[idx].x; // 
		float evaly = results[idx].y; // ImGui::BezierValue( delta, P );
		ImVec2 p0 = ImVec2(evalx, 1 - 0) * (bb.Max - bb.Min) + bb.Min;
		ImVec2 p1 = ImVec2(0, 1 - evaly) * (bb.Max - bb.Min) + bb.Min;
		ImVec2 p2 = ImVec2(evalx, 1 - evaly) * (bb.Max - bb.Min) + bb.Min;
		DrawList->AddCircleFilled(p0, GRAB_RADIUS / 2, ImColor(white));
		DrawList->AddCircleFilled(p1, GRAB_RADIUS / 2, ImColor(white));
		DrawList->AddCircleFilled(p2, GRAB_RADIUS / 2, ImColor(white));
	}

	// draw lines and grabbers
	float luma = IsItemActive() || IsItemHovered() ? 0.5f : 1.0f;
	ImVec4 pink(1.00f, 0.00f, 0.75f, luma), cyan(0.00f, 0.75f, 1.00f, luma);
	ImVec2 p1 = ImVec2(P[0], 1 - P[1]) * (bb.Max - bb.Min) + bb.Min;
	ImVec2 p2 = ImVec2(P[2], 1 - P[3]) * (bb.Max - bb.Min) + bb.Min;
	DrawList->AddLine(ImVec2(bb.Min.x, bb.Max.y), p1, ImColor(white), LINE_WIDTH);
	DrawList->AddLine(ImVec2(bb.Max.x, bb.Min.y), p2, ImColor(white), LINE_WIDTH);
	DrawList->AddCircleFilled(p1, GRAB_RADIUS, ImColor(white));
	DrawList->AddCircleFilled(p1, GRAB_RADIUS - GRAB_BORDER, ImColor(pink));
	DrawList->AddCircleFilled(p2, GRAB_RADIUS, ImColor(white));
	DrawList->AddCircleFilled(p2, GRAB_RADIUS - GRAB_BORDER, ImColor(cyan));

	// if (hovered || changed) DrawList->PopClipRect();

	return changed;
}

bool ImGui::SelectableInput(const char* str_id, bool selected, ImGuiSelectableFlags flags, char* buf, size_t buf_size)
{
	ImGuiContext& g = *GImGui;
	ImGuiWindow* window = g.CurrentWindow;
	ImVec2 pos_before = window->DC.CursorPos;

	PushID(str_id);
	PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(g.Style.ItemSpacing.x, g.Style.FramePadding.y * 2.0f));
	bool ret = Selectable("##Selectable", selected,
						  flags | ImGuiSelectableFlags_AllowDoubleClick | ImGuiSelectableFlags_AllowItemOverlap);
	PopStyleVar();

	ImGuiID id = window->GetID("##Input");
	bool temp_input_is_active = TempInputIsActive(id);
	bool temp_input_start = ret ? IsMouseDoubleClicked(0) : false;

	if (temp_input_start)
		SetActiveID(id, window);

	if (temp_input_is_active || temp_input_start)
	{
		ImVec2 pos_after = window->DC.CursorPos;
		window->DC.CursorPos = pos_before;
		ret = TempInputText(window->DC.LastItemRect, id, "##Input", buf, (int)buf_size, ImGuiInputTextFlags_None);
		window->DC.CursorPos = pos_after;
	}
	else
	{
		window->DrawList->AddText(pos_before, GetColorU32(ImGuiCol_Text), buf);
	}

	PopID();
	return ret;
}

bool ImGui::SpinScaler(const char* label, ImGuiDataType data_type, void* data_ptr, const void* step, const void* step_fast, const char* format, ImGuiInputTextFlags flags)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	ImGuiStyle& style = g.Style;

	if (format == NULL)
		format = DataTypeGetInfo(data_type)->PrintFmt;

	char buf[64];
	DataTypeFormatString(buf, IM_ARRAYSIZE(buf), data_type, data_ptr, format);

	bool value_changed = false;
	if ((flags & (ImGuiInputTextFlags_CharsHexadecimal | ImGuiInputTextFlags_CharsScientific)) == 0)
		flags |= ImGuiInputTextFlags_CharsDecimal;
	flags |= ImGuiInputTextFlags_AutoSelectAll;
	flags |= ImGuiInputTextFlags_NoMarkEdited;  // We call MarkItemEdited() ourselve by comparing the actual data rather than the string.

	if (step != NULL)
	{
		const float button_size = GetFrameHeight();

		BeginGroup(); // The only purpose of the group here is to allow the caller to query item data e.g. IsItemActive()
		PushID(label);
		SetNextItemWidth(ImMax(1.0f, CalcItemWidth() - (button_size + style.ItemInnerSpacing.x) * 2));
		if (InputText("", buf, IM_ARRAYSIZE(buf), flags)) // PushId(label) + "" gives us the expected ID from outside point of view
			value_changed = DataTypeApplyOpFromText(buf, g.InputTextState.InitialTextA.Data, data_type, data_ptr, format);

		// Step buttons
		const ImVec2 backup_frame_padding = style.FramePadding;
		style.FramePadding.x = style.FramePadding.y;
		ImGuiButtonFlags button_flags = ImGuiButtonFlags_Repeat | ImGuiButtonFlags_DontClosePopups;
		if (flags & ImGuiInputTextFlags_ReadOnly)
			button_flags |= ImGuiButtonFlags_Disabled;
		SameLine(0, style.ItemInnerSpacing.x);

// start diffs
		float frame_height = GetFrameHeight();
		float arrow_size = std::floor(frame_height * .45f);
		float arrow_spacing = frame_height - 2.0f * arrow_size;

		BeginGroup();
		PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{g.Style.ItemSpacing.x, arrow_spacing});

		// save/change font size to draw arrow buttons correctly
		float org_font_size = GetDrawListSharedData()->FontSize;
		GetDrawListSharedData()->FontSize = arrow_size;

		if (ArrowButtonEx("+", ImGuiDir_Up, ImVec2(arrow_size, arrow_size), button_flags))
		{
			DataTypeApplyOp(data_type, '+', data_ptr, data_ptr, g.IO.KeyCtrl && step_fast ? step_fast : step);
			value_changed = true;
		}

		if (ArrowButtonEx("-", ImGuiDir_Down, ImVec2(arrow_size, arrow_size), button_flags))
		{
			DataTypeApplyOp(data_type, '-', data_ptr, data_ptr, g.IO.KeyCtrl && step_fast ? step_fast : step);
			value_changed = true;
		}

		// restore font size
		GetDrawListSharedData()->FontSize = org_font_size;

		PopStyleVar(1);
		EndGroup();
// end diffs

		const char* label_end = FindRenderedTextEnd(label);
		if (label != label_end)
		{
			SameLine(0, style.ItemInnerSpacing.x);
			TextEx(label, label_end);
		}
		style.FramePadding = backup_frame_padding;

		PopID();
		EndGroup();
	}
	else
	{
		if (InputText(label, buf, IM_ARRAYSIZE(buf), flags))
			value_changed = DataTypeApplyOpFromText(buf, g.InputTextState.InitialTextA.Data, data_type, data_ptr, format);
	}
	if (value_changed)
		MarkItemEdited(window->DC.LastItemId);

	return value_changed;
}

bool ImGui::SpinInt(const char* label, int* v, int step, int step_fast, ImGuiInputTextFlags flags)
{
	// Hexadecimal input provided as a convenience but the flag name is awkward. Typically you'd use InputText() to parse your own data, if you want to handle prefixes.
	const char* format = (flags & ImGuiInputTextFlags_CharsHexadecimal) ? "%08X" : "%d";
	return SpinScaler(label, ImGuiDataType_S32, (void*)v, (void*)(step>0 ? &step : NULL), (void*)(step_fast>0 ? &step_fast : NULL), format, flags);
}

bool ImGui::SpinFloat(const char* label, float* v, float step, float step_fast, const char* format, ImGuiInputTextFlags flags)
{
	flags |= ImGuiInputTextFlags_CharsScientific;
	return SpinScaler(label, ImGuiDataType_Float, (void*)v, (void*)(step>0.0f ? &step : NULL), (void*)(step_fast>0.0f ? &step_fast : NULL), format, flags);
}

bool ImGui::SpinDouble(const char* label, double* v, double step, double step_fast, const char* format, ImGuiInputTextFlags flags)
{
	flags |= ImGuiInputTextFlags_CharsScientific;
	return SpinScaler(label, ImGuiDataType_Double, (void*)v, (void*)(step>0.0 ? &step : NULL), (void*)(step_fast>0.0 ? &step_fast : NULL), format, flags);
}

bool ImGui::BufferingBar(const char* label, float value, const ImVec2& size_arg, const ImU32& bg_col,
						 const ImU32& fg_col)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);

	ImVec2 pos = window->DC.CursorPos;
	ImVec2 size = size_arg;
	size.x -= style.FramePadding.x * 2;

	const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
	ItemSize(bb, style.FramePadding.y);
	if (!ItemAdd(bb, id))
		return false;

	// Render
	const float circleStart = size.x * 0.7f;
	const float circleEnd = size.x;
	const float circleWidth = circleEnd - circleStart;

	window->DrawList->AddRectFilled(bb.Min, ImVec2(pos.x + circleStart, bb.Max.y), bg_col);
	window->DrawList->AddRectFilled(bb.Min, ImVec2(pos.x + circleStart * value, bb.Max.y), fg_col);

	const float t = g.Time;
	const float r = size.y / 2;
	const float speed = 1.5f;

	const float a = speed * 0;
	const float b = speed * 0.333f;
	const float c = speed * 0.666f;

	const float o1 = (circleWidth + r) * (t + a - speed * (int)((t + a) / speed)) / speed;
	const float o2 = (circleWidth + r) * (t + b - speed * (int)((t + b) / speed)) / speed;
	const float o3 = (circleWidth + r) * (t + c - speed * (int)((t + c) / speed)) / speed;

	window->DrawList->AddCircleFilled(ImVec2(pos.x + circleEnd - o1, bb.Min.y + r), r, bg_col);
	window->DrawList->AddCircleFilled(ImVec2(pos.x + circleEnd - o2, bb.Min.y + r), r, bg_col);
	window->DrawList->AddCircleFilled(ImVec2(pos.x + circleEnd - o3, bb.Min.y + r), r, bg_col);
	return false;
}

bool ImGui::Spinner(const char* label, float radius, int thickness, const ImU32& color)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
		return false;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);

	ImVec2 pos = window->DC.CursorPos;
	ImVec2 size((radius) * 2, (radius + style.FramePadding.y) * 2);

	const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
	ItemSize(bb, style.FramePadding.y);
	if (!ItemAdd(bb, id))
		return false;

	// Render
	window->DrawList->PathClear();

	int num_segments = 30;
	int start = abs(ImSin(g.Time*1.8f) * (num_segments - 5));

	const float a_min = IM_PI * 2.0f * ((float)start) / (float)num_segments;
	const float a_max = IM_PI * 2.0f * ((float)num_segments - 3) / (float)num_segments;

	const ImVec2 centre = ImVec2(pos.x + radius, pos.y + radius + style.FramePadding.y);

	for (int i = 0; i < num_segments; i++)
	{
		const float a = a_min + ((float)i / (float)num_segments) * (a_max - a_min);
		window->DrawList->PathLineTo(ImVec2(centre.x + ImCos(a+g.Time*8) * radius,
											centre.y + ImSin(a+g.Time*8) * radius));
	}

	window->DrawList->PathStroke(color, false, thickness);
	return false;
}

void ImGui::LoadingIndicatorCircle(const char* label, const float indicator_radius, const ImVec4& main_color,
								   const ImVec4& backdrop_color, const int circle_count, const float speed)
{
	ImGuiWindow* window = GetCurrentWindow();
	if (window->SkipItems)
	{
		return;
	}

	ImGuiContext& g = *GImGui;
	const ImGuiID id = window->GetID(label);

	const ImVec2 pos = window->DC.CursorPos;
	const float circle_radius = indicator_radius / 10.0f;
	const ImRect bb(pos, ImVec2(pos.x + indicator_radius * 2.0f,
								pos.y + indicator_radius * 2.0f));
	ItemSize(bb, g.Style.FramePadding.y);
	if (!ItemAdd(bb, id))
	{
		return;
	}
	const float t = g.Time;
	const auto degree_offset = 2.0f * IM_PI / circle_count;
	for (int i = 0; i < circle_count; ++i)
	{
		const auto x = indicator_radius * std::sin(degree_offset * i);
		const auto y = indicator_radius * std::cos(degree_offset * i);
		const auto growth = std::max(0.0f, std::sin(t * speed - i * degree_offset));
		ImVec4 color;
		color.x = main_color.x * growth + backdrop_color.x * (1.0f - growth);
		color.y = main_color.y * growth + backdrop_color.y * (1.0f - growth);
		color.z = main_color.z * growth + backdrop_color.z * (1.0f - growth);
		color.w = 1.0f;
		window->DrawList->AddCircleFilled(ImVec2(pos.x + indicator_radius + x,
												 pos.y + indicator_radius - y),
										  circle_radius + growth * circle_radius,
										  GetColorU32(color));
	}
}

void ImGui::ToggleButton(const char* str_id, bool* v)
{
	ImVec2 p = ImGui::GetCursorScreenPos();
	ImDrawList* draw_list = ImGui::GetWindowDrawList();

	float height = ImGui::GetFrameHeight();
	float width = height * 1.55f;
	float radius = height * 0.50f;

	ImGui::InvisibleButton(str_id, ImVec2(width, height));
	if (ImGui::IsItemClicked())
		*v = !*v;

	float t = *v ? 1.0f : 0.0f;

	ImGuiContext& g = *GImGui;
	float ANIM_SPEED = 0.08f;
	if (g.LastActiveId == g.CurrentWindow->GetID(str_id)) // && g.LastActiveIdTimer < ANIM_SPEED)
	{
		float t_anim = ImSaturate(g.LastActiveIdTimer / ANIM_SPEED);
		t = *v ? (t_anim) : (1.0f - t_anim);
	}

	ImU32 col_bg;
	if (ImGui::IsItemHovered())
		col_bg = ImGui::GetColorU32(ImLerp(ImVec4(0.78f, 0.78f, 0.78f, 1.0f), ImVec4(0.64f, 0.83f, 0.34f, 1.0f), t));
	else
		col_bg = ImGui::GetColorU32(ImLerp(ImVec4(0.85f, 0.85f, 0.85f, 1.0f), ImVec4(0.56f, 0.83f, 0.26f, 1.0f), t));

	draw_list->AddRectFilled(p, ImVec2(p.x + width, p.y + height), col_bg, height * 0.5f);
	draw_list->AddCircleFilled(ImVec2(p.x + radius + t * (width - radius * 2.0f), p.y + radius), radius - 1.5f,
							   ImGui::GetColorU32(g.Style.Colors[ImGuiCol_Button]));
}

static ImVector<ImRect> s_GroupPanelLabelStack;

void ImGui::BeginGroupPanel(const char* name, const ImVec2& size)
{
	ImGui::BeginGroup();

	auto cursorPos = ImGui::GetCursorScreenPos();
	auto itemSpacing = ImGui::GetStyle().ItemSpacing;
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));

	auto frameHeight = ImGui::GetFrameHeight();
	ImGui::BeginGroup();

	ImVec2 effectiveSize = size;
	if (size.x < 0.0f)
		effectiveSize.x = ImGui::GetContentRegionAvail().x;
	else
		effectiveSize.x = size.x;
	ImGui::Dummy(ImVec2(effectiveSize.x, 0.0f));

	ImGui::Dummy(ImVec2(frameHeight * 0.5f, 0.0f));
	ImGui::SameLine(0.0f, 0.0f);
	ImGui::BeginGroup();
	ImGui::Dummy(ImVec2(frameHeight * 0.5f, 0.0f));
	ImGui::SameLine(0.0f, 0.0f);
	ImGui::TextUnformatted(name);
	auto labelMin = ImGui::GetItemRectMin();
	auto labelMax = ImGui::GetItemRectMax();
	ImGui::SameLine(0.0f, 0.0f);
	ImGui::Dummy(ImVec2(0.0, frameHeight + itemSpacing.y));
	ImGui::BeginGroup();

	//ImGui::GetWindowDrawList()->AddRect(labelMin, labelMax, IM_COL32(255, 0, 255, 255));

	ImGui::PopStyleVar(2);

#if IMGUI_VERSION_NUM >= 17301
	ImGui::GetCurrentWindow()->ContentRegionRect.Max.x -= frameHeight * 0.5f;
	ImGui::GetCurrentWindow()->WorkRect.Max.x -= frameHeight * 0.5f;
	ImGui::GetCurrentWindow()->InnerRect.Max.x -= frameHeight * 0.5f;
#else
    ImGui::GetCurrentWindow()->ContentsRegionRect.Max.x -= frameHeight * 0.5f;
#endif
	ImGui::GetCurrentWindow()->Size.x -= frameHeight;

	auto itemWidth = ImGui::CalcItemWidth();
	ImGui::PushItemWidth(ImMax(0.0f, itemWidth - frameHeight));

	s_GroupPanelLabelStack.push_back(ImRect(labelMin, labelMax));
}

void ImGui::EndGroupPanel()
{
	ImGui::PopItemWidth();

	auto itemSpacing = ImGui::GetStyle().ItemSpacing;

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0.0f, 0.0f));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.0f, 0.0f));

	auto frameHeight = ImGui::GetFrameHeight();

	ImGui::EndGroup();

	//ImGui::GetWindowDrawList()->AddRectFilled(ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), IM_COL32(0, 255, 0, 64), 4.0f);

	ImGui::EndGroup();

	ImGui::SameLine(0.0f, 0.0f);
	ImGui::Dummy(ImVec2(frameHeight * 0.5f, 0.0f));
	ImGui::Dummy(ImVec2(0.0, frameHeight - frameHeight * 0.5f - itemSpacing.y));

	ImGui::EndGroup();

	auto itemMin = ImGui::GetItemRectMin();
	auto itemMax = ImGui::GetItemRectMax();
	//ImGui::GetWindowDrawList()->AddRectFilled(itemMin, itemMax, IM_COL32(255, 0, 0, 64), 4.0f);

	auto labelRect = s_GroupPanelLabelStack.back();
	s_GroupPanelLabelStack.pop_back();

	ImVec2 halfFrame = ImVec2(frameHeight * 0.25f, frameHeight) * 0.5f;
	ImRect frameRect = ImRect(itemMin + halfFrame, itemMax - ImVec2(halfFrame.x, 0.0f));
	labelRect.Min.x -= itemSpacing.x;
	labelRect.Max.x += itemSpacing.x;
	for (int i = 0; i < 4; ++i)
	{
		switch (i)
		{
			// left half-plane
		case 0: ImGui::PushClipRect(ImVec2(-FLT_MAX, -FLT_MAX), ImVec2(labelRect.Min.x, FLT_MAX), true);
			break;
			// right half-plane
		case 1: ImGui::PushClipRect(ImVec2(labelRect.Max.x, -FLT_MAX), ImVec2(FLT_MAX, FLT_MAX), true);
			break;
			// top
		case 2: ImGui::PushClipRect(ImVec2(labelRect.Min.x, -FLT_MAX), ImVec2(labelRect.Max.x, labelRect.Min.y), true);
			break;
			// bottom
		case 3: ImGui::PushClipRect(ImVec2(labelRect.Min.x, labelRect.Max.y), ImVec2(labelRect.Max.x, FLT_MAX), true);
			break;
		}

		ImGui::GetWindowDrawList()->AddRect(
			frameRect.Min, frameRect.Max,
			ImColor(ImGui::GetStyleColorVec4(ImGuiCol_Border)),
			halfFrame.x);

		ImGui::PopClipRect();
	}

	ImGui::PopStyleVar(2);

#if IMGUI_VERSION_NUM >= 17301
	ImGui::GetCurrentWindow()->ContentRegionRect.Max.x += frameHeight * 0.5f;
	ImGui::GetCurrentWindow()->WorkRect.Max.x += frameHeight * 0.5f;
	ImGui::GetCurrentWindow()->InnerRect.Max.x += frameHeight * 0.5f;
#else
    ImGui::GetCurrentWindow()->ContentsRegionRect.Max.x += frameHeight * 0.5f;
#endif
	ImGui::GetCurrentWindow()->Size.x += frameHeight;

	ImGui::Dummy(ImVec2(0.0f, 0.0f));

	ImGui::EndGroup();
}

bool ImGui::Splitter(bool split_vertically, float thickness, float* size1, float* size2, float min_size1,
					 float min_size2, float splitter_long_axis_size)
{
	using namespace ImGui;
	ImGuiContext& g = *GImGui;
	ImGuiWindow* window = g.CurrentWindow;
	ImGuiID id = window->GetID("##Splitter");
	ImRect bb;
	bb.Min = window->DC.CursorPos + (split_vertically ? ImVec2(*size1, 0.0f) : ImVec2(0.0f, *size1));
	bb.Max = bb.Min + CalcItemSize(split_vertically
									   ? ImVec2(thickness, splitter_long_axis_size)
									   : ImVec2(splitter_long_axis_size, thickness), 0.0f, 0.0f);
	return SplitterBehavior(bb, id, split_vertically ? ImGuiAxis_X : ImGuiAxis_Y, size1, size2, min_size1, min_size2,
							0.0f);
}

static constexpr float HANDLE_RADIUS = 4;

void ImGui::DrawSmallWidgetDemo()
{
	// ======================================bezier====================================== 
	ImGui::Text("Bezier");
	ImGui::Separator();
	static float v[5] = {0.950f, 0.050f, 0.795f, 0.035f};
	ImGui::Bezier("##easeInExpo", v);
	ImGui::NewLine();
	
	// ======================================selectable input====================================== 
	ImGui::Text("Selectable Input");
	ImGui::Separator();
	static char buf1[256] = "Hello1";
	static char buf2[256] = "Hello2";
	ImGui::SelectableInput("obj1", false, ImGuiSelectableFlags_None, buf1, IM_ARRAYSIZE(buf1));
	ImGui::SelectableInput("obj2", false, ImGuiSelectableFlags_None, buf2, IM_ARRAYSIZE(buf2));
	ImGui::NewLine();
	
	// ======================================spin input====================================== 
	ImGui::Text("Spin Input");
	ImGui::Separator();
	static int IntVar = 0;
	static float FloatVar = 0.0f;
	static double DoubleVar = 0.0;
	ImGui::SpinInt("Spin Int", &IntVar);
	ImGui::SpinFloat("Spin Float", &FloatVar);
	ImGui::SpinDouble("Spin Double", &DoubleVar);
	ImGui::NewLine();

	// ======================================loading====================================== 
	ImGui::Text("Loading");
	ImGui::Separator();
	static float Percent = 0.f;
	Percent = FMath::Frac(Percent + FSlateApplication::Get().GetDeltaTime());
	static const ImU32 col = ImGui::GetColorU32(ImGuiCol_ButtonHovered);
	static const ImU32 bg = ImGui::GetColorU32(ImGuiCol_Button);
	ImGui::BufferingBar("Buffering Bar", Percent, ImVec2(400, 6), bg, col);
	ImGui::Spinner("Spinner", 15, 6, col);
	static const ImVec4 MainCol(0.8f, 0.8f, 0.8f, 1.0f);
	static const ImVec4 BackDrop(0.5f, 0.5f, 0.5f, 1.0f);
	ImGui::Indent(5);
	ImGui::LoadingIndicatorCircle("Loading Indicator Circle", 40, MainCol, BackDrop, 10, 5);
	ImGui::NewLine();

	// ======================================toggle button======================================
	ImGui::Text("toggle button");
	ImGui::Separator();
	static bool ToggleState = false;
	ImGui::ToggleButton("Toggle Button", &ToggleState);
	ImGui::SameLine();
	ImGui::Text("ToggleState: %s", ToggleState ? "Open" : "Close");
	ImGui::NewLine();

	// ======================================group panel======================================
	ImGui::Text("group panel");
	ImGui::Separator();

	ImGui::BeginGroupPanel("Group1", ImVec2(ImGui::GetContentRegionAvail().x, 0));
	ImGui::Text("Here is some Text balabala...............");

	if (ImGui::CollapsingHeader("Other Groups", ImGuiTreeNodeFlags_SpanAvailWidth))
	{
		ImGui::BeginGroupPanel("Group1_1");
		ImGui::Text("Group1_1");
		ImGui::Text("Group1_1");
		ImGui::Text("Group1_1");
		ImGui::Text("Group1_1");
		ImGui::Text("Group1_1");
		ImGui::EndGroupPanel();

		ImGui::SameLine();

		ImGui::BeginGroupPanel("Group1_2");
		ImGui::Text("Group1_2");
		ImGui::Text("Group1_2");
		ImGui::Text("Group1_2");
		ImGui::Text("Group1_2");
		ImGui::Text("Group1_2");
		ImGui::Text("Group1_2");
		ImGui::Text("Group1_2");
		ImGui::EndGroupPanel();		
	}
	ImGui::EndGroupPanel();
	
	ImGui::NewLine();

	// ======================================splitter======================================  
	ImGui::Text("Splitter");
	ImGui::Separator();

	float h = 200;
	static float sz1 = 300;
	static float sz2 = 300;
	Splitter(true, 8.0f, &sz1, &sz2, 8, 8, h);
	ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(1,0,0,1));
	BeginChild("1", ImVec2(sz1, h), true);
	EndChild();
	ImGui::PopStyleColor();
	SameLine();
	ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0,1,0,1));
	BeginChild("2", ImVec2(sz2, h), true);
	EndChild();
	ImGui::PopStyleColor();
	
	ImGui::NewLine();
}

void ImGui::DrawTextEditorDemo(bool* bIsOpen)
{
	if (bIsOpen && !(*bIsOpen)) return;
	// TEXT EDITOR SAMPLE
	static TextEditor editor;
	static std::string fileToEdit = TCHAR_TO_UTF8(*FPaths::Combine(IPluginManager::Get().FindPlugin(TEXT("UEImgui"))->GetBaseDir(),TEXT("Shaders/Private/ImguiShader.usf")));
	
	static bool bIsInit = false;
	if (!bIsInit)
	{
		auto lang = TextEditor::LanguageDefinition::HLSL();
		editor.SetLanguageDefinition(lang);

		// error markers
		TextEditor::ErrorMarkers markers;
		markers.insert(std::make_pair<int, std::string>(6, "Example error here:\nInclude file not found: \"TextEditor.h\""));
		markers.insert(std::make_pair<int, std::string>(41, "Another example error"));
		editor.SetErrorMarkers(markers);

		std::ifstream t(fileToEdit);
		if (t.good())
		{
			std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
			editor.SetText(str);
		}
		bIsInit = true;
	}


	auto cpos = editor.GetCursorPosition();
	ImGui::SetWindowSize(ImVec2(800, 600), ImGuiCond_FirstUseEver);
	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Save"))
			{
				auto textToSave = editor.GetText();
				/// save text....
			}
			if (ImGui::MenuItem("Quit", "Alt-F4"))
			{
				if (bIsOpen != nullptr)
					bIsOpen = false;
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Edit"))
		{
			bool ro = editor.IsReadOnly();
			if (ImGui::MenuItem("Read-only mode", nullptr, &ro))
				editor.SetReadOnly(ro);
			ImGui::Separator();

			if (ImGui::MenuItem("Undo", "ALT-Backspace", nullptr, !ro && editor.CanUndo()))
				editor.Undo();
			if (ImGui::MenuItem("Redo", "Ctrl-Y", nullptr, !ro && editor.CanRedo()))
				editor.Redo();

			ImGui::Separator();

			if (ImGui::MenuItem("Copy", "Ctrl-C", nullptr, editor.HasSelection()))
				editor.Copy();
			if (ImGui::MenuItem("Cut", "Ctrl-X", nullptr, !ro && editor.HasSelection()))
				editor.Cut();
			if (ImGui::MenuItem("Delete", "Del", nullptr, !ro && editor.HasSelection()))
				editor.Delete();
			if (ImGui::MenuItem("Paste", "Ctrl-V", nullptr, !ro && ImGui::GetClipboardText() != nullptr))
				editor.Paste();

			ImGui::Separator();

			if (ImGui::MenuItem("Select all", nullptr, nullptr))
				editor.SetSelection(TextEditor::Coordinates(), TextEditor::Coordinates(editor.GetTotalLines(), 0));

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("View"))
		{
			if (ImGui::MenuItem("Dark palette"))
				editor.SetPalette(TextEditor::GetDarkPalette());
			if (ImGui::MenuItem("Light palette"))
				editor.SetPalette(TextEditor::GetLightPalette());
			if (ImGui::MenuItem("Retro blue palette"))
				editor.SetPalette(TextEditor::GetRetroBluePalette());
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}

	ImGui::Text("%6d/%-6d %6d lines  | %s | %s | %s | %s", cpos.mLine + 1, cpos.mColumn + 1, editor.GetTotalLines(),
		editor.IsOverwrite() ? "Ovr" : "Ins",
		editor.CanUndo() ? "*" : " ",
		editor.GetLanguageDefinition().mName.c_str(), fileToEdit.c_str());

	editor.Render("TextEditor");
}
