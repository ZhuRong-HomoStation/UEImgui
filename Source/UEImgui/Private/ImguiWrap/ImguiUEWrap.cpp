#include "ImguiWrap/ImguiUEWrap.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "Config/ImguiConfig.h"
#include "Engine/TextureRenderTarget2D.h"
#include "ImguiWrap/ImguiResourceManager.h"
#include "Widgets/SImguiWidget.h"

static void _HelpMarker(const char* desc)
{
	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

// detail
FName CurrentDetail;
TWeakPtr<SImguiWidgetRenderProxy> CurrentDetailWidget;

void ImGui::StyleColorUE(ImGuiStyle* dst)
{
	ImGuiStyle* style = dst ? dst : &ImGui::GetStyle();
    ImVec4* colors = style->Colors;
	
	// color settings 
	colors[ImGuiCol_Text]                   = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_TextDisabled]           = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
	colors[ImGuiCol_WindowBg]               = ImVec4(0.13f, 0.13f, 0.13f, 1.00f);
	colors[ImGuiCol_ChildBg]                = ImVec4(0.02f, 0.02f, 0.02f, 1.00f);
	colors[ImGuiCol_PopupBg]                = ImVec4(0.08f, 0.08f, 0.08f, 0.94f);
	colors[ImGuiCol_Border]                 = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
	colors[ImGuiCol_BorderShadow]           = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	colors[ImGuiCol_FrameBg]                = ImVec4(0.33f, 0.33f, 0.33f, 1.00f);
	colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.19f, 0.19f, 0.19f, 1.00f);
	colors[ImGuiCol_FrameBgActive]          = ImVec4(0.56f, 0.56f, 0.56f, 1.00f);
	colors[ImGuiCol_TitleBg]                = ImVec4(0.13f, 0.13f, 0.13f, 1.00f);
	colors[ImGuiCol_TitleBgActive]          = ImVec4(0.33f, 0.33f, 0.33f, 1.00f);
	colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.00f, 0.00f, 0.00f, 0.51f);
	colors[ImGuiCol_MenuBarBg]              = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);
	colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
	colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
	colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
	colors[ImGuiCol_CheckMark]              = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
	colors[ImGuiCol_SliderGrab]             = ImVec4(0.80f, 0.80f, 0.80f, 1.00f);
	colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.02f, 0.02f, 0.02f, 1.00f);
	colors[ImGuiCol_Button]                 = ImVec4(0.33f, 0.33f, 0.33f, 1.00f);
	colors[ImGuiCol_ButtonHovered]          = ImVec4(0.27f, 0.27f, 0.27f, 1.00f);
	colors[ImGuiCol_ButtonActive]           = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
	colors[ImGuiCol_Header]                 = ImVec4(0.28f, 0.28f, 0.28f, 1.00f);
	colors[ImGuiCol_HeaderHovered]          = ImVec4(0.50f, 0.50f, 0.50f, 0.80f);
	colors[ImGuiCol_HeaderActive]           = ImVec4(0.67f, 0.67f, 0.67f, 1.00f);
	colors[ImGuiCol_Separator]              = ImVec4(0.43f, 0.43f, 0.50f, 0.50f);
	colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.10f, 0.40f, 0.75f, 0.78f);
	colors[ImGuiCol_SeparatorActive]        = ImVec4(0.10f, 0.40f, 0.75f, 1.00f);
	colors[ImGuiCol_ResizeGrip]             = ImVec4(0.80f, 0.80f, 0.80f, 1.00f);
	colors[ImGuiCol_ResizeGripHovered]      = ImVec4(0.87f, 0.87f, 0.87f, 1.00f);
	colors[ImGuiCol_ResizeGripActive]       = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
	colors[ImGuiCol_Tab]                    = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
	colors[ImGuiCol_TabHovered]             = ImVec4(0.38f, 0.38f, 0.38f, 1.00f);
	colors[ImGuiCol_TabActive]              = ImVec4(0.47f, 0.47f, 0.47f, 1.00f);
	colors[ImGuiCol_TabUnfocused]           = ImVec4(0.07f, 0.10f, 0.15f, 0.97f);
	colors[ImGuiCol_TabUnfocusedActive]     = ImVec4(0.14f, 0.26f, 0.42f, 1.00f);
	colors[ImGuiCol_PlotLines]              = ImVec4(0.61f, 0.61f, 0.61f, 1.00f);
	colors[ImGuiCol_PlotLinesHovered]       = ImVec4(1.00f, 0.43f, 0.35f, 1.00f);
	colors[ImGuiCol_PlotHistogram]          = ImVec4(0.90f, 0.70f, 0.00f, 1.00f);
	colors[ImGuiCol_PlotHistogramHovered]   = ImVec4(1.00f, 0.60f, 0.00f, 1.00f);
	colors[ImGuiCol_TextSelectedBg]         = ImVec4(0.26f, 0.59f, 0.98f, 0.35f);
	colors[ImGuiCol_DragDropTarget]         = ImVec4(1.00f, 1.00f, 0.00f, 0.90f);
	colors[ImGuiCol_NavHighlight]           = ImVec4(0.26f, 0.59f, 0.98f, 1.00f);
	colors[ImGuiCol_NavWindowingHighlight]  = ImVec4(1.00f, 1.00f, 1.00f, 0.70f);
	colors[ImGuiCol_NavWindowingDimBg]      = ImVec4(0.80f, 0.80f, 0.80f, 0.20f);
	colors[ImGuiCol_ModalWindowDimBg]       = ImVec4(0.80f, 0.80f, 0.80f, 0.35f);

	// other settings
	style->ChildRounding = 0;
	style->FrameRounding = 0;
	style->PopupRounding = 0;
	style->ScrollbarRounding = 0;
	style->WindowRounding = 0;
}

void ImGui::StyleColorConfig(ImGuiStyle* dst)
{
	ImGuiStyle* style = dst ? dst : &ImGui::GetStyle();
	ImVec4* colors = style->Colors;

	FMemory::Memcpy(colors ,UImguiConfig::Get()->ImguiColors.GetData(), ImGuiCol_COUNT * sizeof(ImVec4));
	FMemory::Memcpy(style, &UImguiConfig::Get()->ImguiStyle, sizeof(FImguiStyle));
}

void ImGui::SetCurrentDetail(FName InDetailName)
{
	CurrentDetail = InDetailName;
}

void ImGui::SetCurrentDetailWidget(TWeakPtr<SImguiWidgetRenderProxy> InDetailWidget)
{
	CurrentDetailWidget = InDetailWidget;
}

FName ImGui::GetCurrentDetail()
{
	return CurrentDetail;
}

void ImGui::BeginDetail()
{
	if (CurrentDetailWidget.IsValid())
	{
		FVector2D CurrentDetailSize = CurrentDetailWidget.Pin()->GetCachedGeometry().Size;
		ImGui::SetNextWindowSizeConstraints({CurrentDetailSize.X, -1}, {CurrentDetailSize.X, -1});
	}
	ImGui::Begin(TCHAR_TO_UTF8(*CurrentDetail.ToString()), nullptr, ImGuiWindowFlags_UEDetail);
}

void ImGui::EndDetail()
{
	ImGui::End();
}

void ImGui::UEText(const FString& InString)
{
	ImGui::Text(TCHAR_TO_UTF8(*InString));
}

bool ImGui::ShowUEStyleSelector(const char* Label)
{
	static int StyleIdx = 4;
	if (ImGui::Combo(Label, &StyleIdx, "Classic\0Dark\0Light\0Unreal\0Config\0"))
	{
		switch (StyleIdx)
		{
		case 0: ImGui::StyleColorsClassic(); break;
		case 1: ImGui::StyleColorsDark(); break;
		case 2: ImGui::StyleColorsLight(); break;
		case 3: ImGui::StyleColorUE(); break;
		case 4: ImGui::StyleColorConfig(); break;
		}
		return true;
	}
	return false;
}

void ImGui::ShowUEStyleEditor()
{
	UImguiConfig* Config = UImguiConfig::Get();
	ImGuiStyle& CurStyle = ImGui::GetStyle();
	
	// Style selector
	ImGui::PushItemWidth(ImGui::GetWindowWidth() * 0.50f);
	static int StyleIdx = 4;
	if (ImGui::Combo("Colors##Style", &StyleIdx, "Classic\0Dark\0Light\0Unreal\0Config\0"))
	{
		switch (StyleIdx)
		{
		case 0: ImGui::StyleColorsClassic(); break;
		case 1: ImGui::StyleColorsDark(); break;
		case 2: ImGui::StyleColorsLight(); break;
		case 3: ImGui::StyleColorUE(); break;
		case 4: ImGui::StyleColorConfig(); break;
		}
	}
	
	// Save buttons
	if (ImGui::Button("Save to config"))
	{
		Config->SetStyle(&ImGui::GetStyle());
		Config->SaveConfig();
	}
	ImGui::SameLine();
	if (ImGui::Button("Revert"))
	{
		switch (StyleIdx)
		{
		case 0: ImGui::StyleColorsClassic(); break;
		case 1: ImGui::StyleColorsDark(); break;
		case 2: ImGui::StyleColorsLight(); break;
		case 3: ImGui::StyleColorUE(); break;
		case 4: ImGui::StyleColorConfig(); break;
		}
	}
	ImGui::SameLine();
	_HelpMarker("please save every thing after modify!!!!");
	
	ImGui::Separator();

	// TabBar
	if (ImGui::BeginTabBar("##tabs", ImGuiTabBarFlags_None))
	{
		if (ImGui::BeginTabItem("Sizes"))
        {
            ImGui::Text("Main");
            ImGui::SliderFloat2("WindowPadding", (float*)&CurStyle.WindowPadding, 0.0f, 20.0f, "%.0f");
            ImGui::SliderFloat2("FramePadding", (float*)&CurStyle.FramePadding, 0.0f, 20.0f, "%.0f");
            ImGui::SliderFloat2("ItemSpacing", (float*)&CurStyle.ItemSpacing, 0.0f, 20.0f, "%.0f");
            ImGui::SliderFloat2("ItemInnerSpacing", (float*)&CurStyle.ItemInnerSpacing, 0.0f, 20.0f, "%.0f");
            ImGui::SliderFloat2("TouchExtraPadding", (float*)&CurStyle.TouchExtraPadding, 0.0f, 10.0f, "%.0f");
            ImGui::SliderFloat("IndentSpacing", &CurStyle.IndentSpacing, 0.0f, 30.0f, "%.0f");
            ImGui::SliderFloat("ScrollbarSize", &CurStyle.ScrollbarSize, 1.0f, 20.0f, "%.0f");
            ImGui::SliderFloat("GrabMinSize", &CurStyle.GrabMinSize, 1.0f, 20.0f, "%.0f");
            ImGui::Text("Borders");
            ImGui::SliderFloat("WindowBorderSize", &CurStyle.WindowBorderSize, 0.0f, 1.0f, "%.0f");
            ImGui::SliderFloat("ChildBorderSize", &CurStyle.ChildBorderSize, 0.0f, 1.0f, "%.0f");
            ImGui::SliderFloat("PopupBorderSize", &CurStyle.PopupBorderSize, 0.0f, 1.0f, "%.0f");
            ImGui::SliderFloat("FrameBorderSize", &CurStyle.FrameBorderSize, 0.0f, 1.0f, "%.0f");
            ImGui::SliderFloat("TabBorderSize", &CurStyle.TabBorderSize, 0.0f, 1.0f, "%.0f");
            ImGui::Text("Rounding");
            ImGui::SliderFloat("WindowRounding", &CurStyle.WindowRounding, 0.0f, 12.0f, "%.0f");
            ImGui::SliderFloat("ChildRounding", &CurStyle.ChildRounding, 0.0f, 12.0f, "%.0f");
            ImGui::SliderFloat("FrameRounding", &CurStyle.FrameRounding, 0.0f, 12.0f, "%.0f");
            ImGui::SliderFloat("PopupRounding", &CurStyle.PopupRounding, 0.0f, 12.0f, "%.0f");
            ImGui::SliderFloat("ScrollbarRounding", &CurStyle.ScrollbarRounding, 0.0f, 12.0f, "%.0f");
            ImGui::SliderFloat("GrabRounding", &CurStyle.GrabRounding, 0.0f, 12.0f, "%.0f");
            ImGui::SliderFloat("LogSliderDeadzone", &CurStyle.LogSliderDeadzone, 0.0f, 12.0f, "%.0f");
            ImGui::SliderFloat("TabRounding", &CurStyle.TabRounding, 0.0f, 12.0f, "%.0f");
            ImGui::Text("Alignment");
            ImGui::SliderFloat2("WindowTitleAlign", (float*)&CurStyle.WindowTitleAlign, 0.0f, 1.0f, "%.2f");
            int window_menu_button_position = CurStyle.WindowMenuButtonPosition + 1;
            if (ImGui::Combo("WindowMenuButtonPosition", (int*)&window_menu_button_position, "None\0Left\0Right\0"))
                CurStyle.WindowMenuButtonPosition = window_menu_button_position - 1;
            ImGui::Combo("ColorButtonPosition", (int*)&CurStyle.ColorButtonPosition, "Left\0Right\0");
            ImGui::SliderFloat2("ButtonTextAlign", (float*)&CurStyle.ButtonTextAlign, 0.0f, 1.0f, "%.2f");
            ImGui::SameLine(); _HelpMarker("Alignment applies when a button is larger than its text content.");
            ImGui::SliderFloat2("SelectableTextAlign", (float*)&CurStyle.SelectableTextAlign, 0.0f, 1.0f, "%.2f");
            ImGui::SameLine(); _HelpMarker("Alignment applies when a selectable is larger than its text content.");
            ImGui::Text("Safe Area Padding");
            ImGui::SameLine(); _HelpMarker("Adjust if you cannot see the edges of your screen (e.g. on a TV where scaling has not been configured).");
            ImGui::SliderFloat2("DisplaySafeAreaPadding", (float*)&CurStyle.DisplaySafeAreaPadding, 0.0f, 30.0f, "%.0f");
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Colors"))
        {
            static ImGuiTextFilter filter;
            filter.Draw("Filter colors", ImGui::GetFontSize() * 16);

            static ImGuiColorEditFlags alpha_flags = 0;
            if (ImGui::RadioButton("Opaque", alpha_flags == ImGuiColorEditFlags_None))             { alpha_flags = ImGuiColorEditFlags_None; } ImGui::SameLine();
            if (ImGui::RadioButton("Alpha",  alpha_flags == ImGuiColorEditFlags_AlphaPreview))     { alpha_flags = ImGuiColorEditFlags_AlphaPreview; } ImGui::SameLine();
            if (ImGui::RadioButton("Both",   alpha_flags == ImGuiColorEditFlags_AlphaPreviewHalf)) { alpha_flags = ImGuiColorEditFlags_AlphaPreviewHalf; } ImGui::SameLine();
            _HelpMarker(
                "In the color list:\n"
                "Left-click on color square to open color picker,\n"
                "Right-click to open edit options menu.");

            ImGui::BeginChild("##colors", ImVec2(0, 0), true, ImGuiWindowFlags_AlwaysVerticalScrollbar | ImGuiWindowFlags_AlwaysHorizontalScrollbar | ImGuiWindowFlags_NavFlattened);
            ImGui::PushItemWidth(-160);
            for (int i = 0; i < ImGuiCol_COUNT; i++)
            {
                const char* name = ImGui::GetStyleColorName(i);
                if (!filter.PassFilter(name))
                    continue;
                ImGui::PushID(i);
                ImGui::ColorEdit4("##color", (float*)&CurStyle.Colors[i], ImGuiColorEditFlags_AlphaBar | alpha_flags);
                if (memcmp(&CurStyle.Colors[i], &Config->ImguiColors[i], sizeof(ImVec4)) != 0)
                {
                    // Tips: in a real user application, you may want to merge and use an icon font into the main font,
                    // so instead of "Save"/"Revert" you'd use icons!
                    // Read the FAQ and docs/FONTS.md about using icon fonts. It's really easy and super convenient!
                    ImGui::SameLine(0.0f, CurStyle.ItemInnerSpacing.x); if (ImGui::Button("Save")) { Config->ImguiColors[i] = ImGui::UEType(CurStyle.Colors[i]); }
                    ImGui::SameLine(0.0f, CurStyle.ItemInnerSpacing.x); if (ImGui::Button("Revert")) { CurStyle.Colors[i] = ImGui::ImType(Config->ImguiColors[i]); }
                }
                ImGui::SameLine(0.0f, CurStyle.ItemInnerSpacing.x);
                ImGui::TextUnformatted(name);
                ImGui::PopID();
            }
            ImGui::PopItemWidth();
            ImGui::EndChild();

            ImGui::EndTabItem();
        }

        // if (ImGui::BeginTabItem("Fonts"))
        // {
        //     ImGuiIO& io = ImGui::GetIO();
        //     ImFontAtlas* atlas = io.Fonts;
        //     _HelpMarker("Read FAQ and docs/FONTS.md for details on font loading.");
        //     ImGui::PushItemWidth(120);
        //     for (int i = 0; i < atlas->Fonts.Size; i++)
        //     {
        //         ImFont* font = atlas->Fonts[i];
        //         ImGui::PushID(font);
        //         NodeFont(font);
        //         ImGui::PopID();
        //     }
        //     if (ImGui::TreeNode("Atlas texture", "Atlas texture (%dx%d pixels)", atlas->TexWidth, atlas->TexHeight))
        //     {
        //         ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
        //         ImVec4 border_col = ImVec4(1.0f, 1.0f, 1.0f, 0.5f);
        //         ImGui::Image(atlas->TexID, ImVec2((float)atlas->TexWidth, (float)atlas->TexHeight), ImVec2(0, 0), ImVec2(1, 1), tint_col, border_col);
        //         ImGui::TreePop();
        //     }
        //
        //     // Post-baking font scaling. Note that this is NOT the nice way of scaling fonts, read below.
        //     // (we enforce hard clamping manually as by default DragFloat/SliderFloat allows CTRL+Click text to get out of bounds).
        //     const float MIN_SCALE = 0.3f;
        //     const float MAX_SCALE = 2.0f;
        //     _HelpMarker(
        //         "Those are old settings provided for convenience.\n"
        //         "However, the _correct_ way of scaling your UI is currently to reload your font at the designed size, "
        //         "rebuild the font atlas, and call style.ScaleAllSizes() on a reference ImGuiStyle structure.\n"
        //         "Using those settings here will give you poor quality results.");
        //     static float window_scale = 1.0f;
        //     if (ImGui::DragFloat("window scale", &window_scale, 0.005f, MIN_SCALE, MAX_SCALE, "%.2f", ImGuiSliderFlags_AlwaysClamp)) // Scale only this window
        //         ImGui::SetWindowFontScale(window_scale);
        //     ImGui::DragFloat("global scale", &io.FontGlobalScale, 0.005f, MIN_SCALE, MAX_SCALE, "%.2f", ImGuiSliderFlags_AlwaysClamp); // Scale everything
        //     ImGui::PopItemWidth();
        //
        //     ImGui::EndTabItem();
        // }

        // if (ImGui::BeginTabItem("Rendering"))
        // {
        //     ImGui::Checkbox("Anti-aliased lines", &style.AntiAliasedLines);
        //     ImGui::SameLine();
        //     HelpMarker("When disabling anti-aliasing lines, you'll probably want to disable borders in your style as well.");
        //
        //     ImGui::Checkbox("Anti-aliased lines use texture", &style.AntiAliasedLinesUseTex);
        //     ImGui::SameLine();
        //     HelpMarker("Faster lines using texture data. Require backend to render with bilinear filtering (not point/nearest filtering).");
        //
        //     ImGui::Checkbox("Anti-aliased fill", &style.AntiAliasedFill);
        //     ImGui::PushItemWidth(100);
        //     ImGui::DragFloat("Curve Tessellation Tolerance", &style.CurveTessellationTol, 0.02f, 0.10f, 10.0f, "%.2f");
        //     if (style.CurveTessellationTol < 0.10f) style.CurveTessellationTol = 0.10f;
        //
        //     // When editing the "Circle Segment Max Error" value, draw a preview of its effect on auto-tessellated circles.
        //     ImGui::DragFloat("Circle Segment Max Error", &style.CircleSegmentMaxError, 0.01f, 0.10f, 10.0f, "%.2f");
        //     if (ImGui::IsItemActive())
        //     {
        //         ImGui::SetNextWindowPos(ImGui::GetCursorScreenPos());
        //         ImGui::BeginTooltip();
        //         ImVec2 p = ImGui::GetCursorScreenPos();
        //         ImDrawList* draw_list = ImGui::GetWindowDrawList();
        //         float RAD_MIN = 10.0f, RAD_MAX = 80.0f;
        //         float off_x = 10.0f;
        //         for (int n = 0; n < 7; n++)
        //         {
        //             const float rad = RAD_MIN + (RAD_MAX - RAD_MIN) * (float)n / (7.0f - 1.0f);
        //             draw_list->AddCircle(ImVec2(p.x + off_x + rad, p.y + RAD_MAX), rad, ImGui::GetColorU32(ImGuiCol_Text), 0);
        //             off_x += 10.0f + rad * 2.0f;
        //         }
        //         ImGui::Dummy(ImVec2(off_x, RAD_MAX * 2.0f));
        //         ImGui::EndTooltip();
        //     }
        //     ImGui::SameLine();
        //     HelpMarker("When drawing circle primitives with \"num_segments == 0\" tesselation will be calculated automatically.");
        //
        //     ImGui::DragFloat("Global Alpha", &style.Alpha, 0.005f, 0.20f, 1.0f, "%.2f"); // Not exposing zero here so user doesn't "lose" the UI (zero alpha clips all widgets). But application code could have a toggle to switch between zero and non-zero.
        //     ImGui::PopItemWidth();
        //
        //     ImGui::EndTabItem();
        // }
        
		ImGui::EndTabBar();
	}

	ImGui::PopItemWidth();
}
