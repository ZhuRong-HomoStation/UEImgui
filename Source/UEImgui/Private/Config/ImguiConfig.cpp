#include "Config/ImguiConfig.h"
#include "imgui.h"

UImguiConfig::UImguiConfig()
{
	// setup category 
	CategoryName = TEXT("Plugins");
	
	// color settings
	ImguiColors.SetNumUninitialized(ImGuiCol_COUNT);
	ImguiColors[ImGuiCol_Text]                  = FLinearColor(1.00f, 1.00f, 1.00f, 1.00f);
	ImguiColors[ImGuiCol_TextDisabled]          = FLinearColor(0.50f, 0.50f, 0.50f, 1.00f);
	ImguiColors[ImGuiCol_WindowBg]              = FLinearColor(0.13f, 0.13f, 0.13f, 1.00f);
	ImguiColors[ImGuiCol_ChildBg]               = FLinearColor(0.02f, 0.02f, 0.02f, 1.00f);
	ImguiColors[ImGuiCol_PopupBg]               = FLinearColor(0.08f, 0.08f, 0.08f, 0.94f);
	ImguiColors[ImGuiCol_Border]                = FLinearColor(0.43f, 0.43f, 0.50f, 0.50f);
	ImguiColors[ImGuiCol_BorderShadow]          = FLinearColor(0.00f, 0.00f, 0.00f, 0.00f);
	ImguiColors[ImGuiCol_FrameBg]               = FLinearColor(0.33f, 0.33f, 0.33f, 1.00f);
	ImguiColors[ImGuiCol_FrameBgHovered]        = FLinearColor(0.19f, 0.19f, 0.19f, 1.00f);
	ImguiColors[ImGuiCol_FrameBgActive]         = FLinearColor(0.56f, 0.56f, 0.56f, 1.00f);
	ImguiColors[ImGuiCol_TitleBg]               = FLinearColor(0.13f, 0.13f, 0.13f, 1.00f);
	ImguiColors[ImGuiCol_TitleBgActive]         = FLinearColor(0.33f, 0.33f, 0.33f, 1.00f);
	ImguiColors[ImGuiCol_TitleBgCollapsed]      = FLinearColor(0.00f, 0.00f, 0.00f, 0.51f);
	ImguiColors[ImGuiCol_MenuBarBg]             = FLinearColor(0.14f, 0.14f, 0.14f, 1.00f);
	ImguiColors[ImGuiCol_ScrollbarBg]           = FLinearColor(0.02f, 0.02f, 0.02f, 0.53f);
	ImguiColors[ImGuiCol_ScrollbarGrab]         = FLinearColor(0.31f, 0.31f, 0.31f, 1.00f);
	ImguiColors[ImGuiCol_ScrollbarGrabHovered]  = FLinearColor(0.41f, 0.41f, 0.41f, 1.00f);
	ImguiColors[ImGuiCol_ScrollbarGrabActive]   = FLinearColor(0.51f, 0.51f, 0.51f, 1.00f);
	ImguiColors[ImGuiCol_CheckMark]             = FLinearColor(0.04f, 0.04f, 0.04f, 1.00f);
	ImguiColors[ImGuiCol_SliderGrab]            = FLinearColor(0.80f, 0.80f, 0.80f, 1.00f);
	ImguiColors[ImGuiCol_SliderGrabActive]      = FLinearColor(0.02f, 0.02f, 0.02f, 1.00f);
	ImguiColors[ImGuiCol_Button]                = FLinearColor(0.33f, 0.33f, 0.33f, 1.00f);
	ImguiColors[ImGuiCol_ButtonHovered]         = FLinearColor(0.27f, 0.27f, 0.27f, 1.00f);
	ImguiColors[ImGuiCol_ButtonActive]          = FLinearColor(0.15f, 0.15f, 0.15f, 1.00f);
	ImguiColors[ImGuiCol_Header]                = FLinearColor(0.28f, 0.28f, 0.28f, 1.00f);
	ImguiColors[ImGuiCol_HeaderHovered]         = FLinearColor(0.50f, 0.50f, 0.50f, 0.80f);
	ImguiColors[ImGuiCol_HeaderActive]          = FLinearColor(0.67f, 0.67f, 0.67f, 1.00f);
	ImguiColors[ImGuiCol_Separator]             = FLinearColor(0.43f, 0.43f, 0.50f, 0.50f);
	ImguiColors[ImGuiCol_SeparatorHovered]      = FLinearColor(0.10f, 0.40f, 0.75f, 0.78f);
	ImguiColors[ImGuiCol_SeparatorActive]       = FLinearColor(0.10f, 0.40f, 0.75f, 1.00f);
	ImguiColors[ImGuiCol_ResizeGrip]            = FLinearColor(0.80f, 0.80f, 0.80f, 1.00f);
	ImguiColors[ImGuiCol_ResizeGripHovered]     = FLinearColor(0.87f, 0.87f, 0.87f, 1.00f);
	ImguiColors[ImGuiCol_ResizeGripActive]      = FLinearColor(1.00f, 1.00f, 1.00f, 1.00f);
	ImguiColors[ImGuiCol_Tab]                   = FLinearColor(0.31f, 0.31f, 0.31f, 1.00f);
	ImguiColors[ImGuiCol_TabHovered]            = FLinearColor(0.38f, 0.38f, 0.38f, 1.00f);
	ImguiColors[ImGuiCol_TabActive]             = FLinearColor(0.47f, 0.47f, 0.47f, 1.00f);
	ImguiColors[ImGuiCol_TabUnfocused]          = FLinearColor(0.07f, 0.10f, 0.15f, 0.97f);
	ImguiColors[ImGuiCol_TabUnfocusedActive]    = FLinearColor(0.14f, 0.26f, 0.42f, 1.00f);
	ImguiColors[ImGuiCol_PlotLines]             = FLinearColor(0.61f, 0.61f, 0.61f, 1.00f);
	ImguiColors[ImGuiCol_PlotLinesHovered]      = FLinearColor(1.00f, 0.43f, 0.35f, 1.00f);
	ImguiColors[ImGuiCol_PlotHistogram]         = FLinearColor(0.90f, 0.70f, 0.00f, 1.00f);
	ImguiColors[ImGuiCol_PlotHistogramHovered]  = FLinearColor(1.00f, 0.60f, 0.00f, 1.00f);
	ImguiColors[ImGuiCol_TextSelectedBg]        = FLinearColor(0.26f, 0.59f, 0.98f, 0.35f);
	ImguiColors[ImGuiCol_DragDropTarget]        = FLinearColor(1.00f, 1.00f, 0.00f, 0.90f);
	ImguiColors[ImGuiCol_NavHighlight]          = FLinearColor(0.26f, 0.59f, 0.98f, 1.00f);
	ImguiColors[ImGuiCol_NavWindowingHighlight] = FLinearColor(1.00f, 1.00f, 1.00f, 0.70f);
	ImguiColors[ImGuiCol_NavWindowingDimBg]     = FLinearColor(0.80f, 0.80f, 0.80f, 0.20f);
	ImguiColors[ImGuiCol_ModalWindowDimBg]      = FLinearColor(0.80f, 0.80f, 0.80f, 0.35f);
	ImguiColors[ImGuiCol_DockingPreview]		= FLinearColor(1.00f, 1.00f, 1.00f, 0.50f);
	ImguiColors[ImGuiCol_TabUnfocused]			= FLinearColor(0.50f, 0.50f, 0.50f, 1.00f);

	// set up imgui style
	ImguiStyle.Alpha                   = 1.0f;							// Global alpha applies to everything in ImGui
    ImguiStyle.WindowPadding           = FVector2D(8,8);			// Padding within a window
    ImguiStyle.WindowRounding          = 0.0f;							// Radius of window corners rounding. Set to 0.0f to have rectangular windows. Large values tend to lead to variety of artifacts and are not recommended.
    ImguiStyle.WindowBorderSize        = 1.0f;							// Thickness of border around windows. Generally set to 0.0f or 1.0f. Other values not well tested.
    ImguiStyle.WindowMinSize           = FVector2D(32,32);		// Minimum window size
    ImguiStyle.WindowTitleAlign        = FVector2D(0.0f,0.5f);	// Alignment for title bar text
    ImguiStyle.WindowMenuButtonPosition= ImGuiDir_Left;					// Position of the collapsing/docking button in the title bar (left/right). Defaults to ImGuiDir_Left.
    ImguiStyle.ChildRounding           = 0.0f;          				// Radius of child window corners rounding. Set to 0.0f to have rectangular child windows
    ImguiStyle.ChildBorderSize         = 1.0f;          				// Thickness of border around child windows. Generally set to 0.0f or 1.0f. Other values not well tested.
    ImguiStyle.PopupRounding           = 0.0f;          				// Radius of popup window corners rounding. Set to 0.0f to have rectangular child windows
    ImguiStyle.PopupBorderSize         = 1.0f;          				// Thickness of border around popup or tooltip windows. Generally set to 0.0f or 1.0f. Other values not well tested.
    ImguiStyle.FramePadding            = FVector2D(4,3);			// Padding within a framed rectangle (used by most widgets)
    ImguiStyle.FrameRounding           = 0.0f;							// Radius of frame corners rounding. Set to 0.0f to have rectangular frames (used by most widgets).
    ImguiStyle.FrameBorderSize         = 0.0f;							// Thickness of border around frames. Generally set to 0.0f or 1.0f. Other values not well tested.
    ImguiStyle.ItemSpacing             = FVector2D(8,4); 		// Horizontal and vertical spacing between widgets/lines
    ImguiStyle.ItemInnerSpacing        = FVector2D(4,4); 		// Horizontal and vertical spacing between within elements of a composed widget (e.g. a slider and its label)
    ImguiStyle.CellPadding             = FVector2D(4,2); 		// Padding within a table cell
    ImguiStyle.TouchExtraPadding       = FVector2D(0,0); 		// Expand reactive bounding box for touch-based system where touch position is not accurate enough. Unfortunately we don't sort widgets so priority on overlap will always be given to the first widget. So don't grow this too much!
    ImguiStyle.IndentSpacing           = 21.0f;         				// Horizontal spacing when e.g. entering a tree node. Generally == (FontSize + FramePadding.x*2).
    ImguiStyle.ColumnsMinSpacing       = 6.0f;          				// Minimum horizontal spacing between two columns. Preferably > (FramePadding.x + 1).
    ImguiStyle.ScrollbarSize           = 14.0f;         				// Width of the vertical scrollbar, Height of the horizontal scrollbar
    ImguiStyle.ScrollbarRounding       = 9.0f;          				// Radius of grab corners rounding for scrollbar
    ImguiStyle.GrabMinSize             = 10.0f;         				// Minimum width/height of a grab box for slider/scrollbar
    ImguiStyle.GrabRounding            = 0.0f;          				// Radius of grabs corners rounding. Set to 0.0f to have rectangular slider grabs.
    ImguiStyle.LogSliderDeadzone       = 4.0f;          				// The size in pixels of the dead-zone around zero on logarithmic sliders that cross zero.
    ImguiStyle.TabRounding             = 4.0f;          				// Radius of upper corners of a tab. Set to 0.0f to have rectangular tabs.
    ImguiStyle.TabBorderSize           = 0.0f;          				// Thickness of border around tabs.
    ImguiStyle.TabMinWidthForCloseButton = 0.0f;        				// Minimum width for close button to appears on an unselected tab when hovered. Set to 0.0f to always show when hovering, set to FLT_MAX to never show close button unless selected.
    ImguiStyle.ColorButtonPosition     = ImGuiDir_Right;				// Side of the color button in the ColorEdit4 widget (left/right). Defaults to ImGuiDir_Right.
    ImguiStyle.ButtonTextAlign         = FVector2D(0.5f,0.5f);	// Alignment of button text when button is larger than text.
    ImguiStyle.SelectableTextAlign     = FVector2D(0.0f,0.0f);	// Alignment of selectable text. Defaults to (0.0f, 0.0f) (top-left aligned). It's generally important to keep this left-aligned if you want to lay multiple items on a same line.
    ImguiStyle.DisplayWindowPadding    = FVector2D(19,19);    	// Window position are clamped to be visible within the display area or monitors by at least this amount. Only applies to regular windows.
    ImguiStyle.DisplaySafeAreaPadding  = FVector2D(3,3);      	// If you cannot see the edge of your screen (e.g. on a TV) increase the safe area padding. Covers popups/tooltips as well regular windows.
    ImguiStyle.MouseCursorScale        = 1.0f;     						// Scale software rendered mouse cursor (when io.MouseDrawCursor is enabled). May be removed later.
    ImguiStyle.AntiAliasedLines        = true;     						// Enable anti-aliased lines/borders. Disable if you are really tight on CPU/GPU.
    ImguiStyle.AntiAliasedLinesUseTex  = true;     						// Enable anti-aliased lines/borders using textures where possible. Require backend to render with bilinear filtering.
    ImguiStyle.AntiAliasedFill         = true;     						// Enable anti-aliased filled shapes (rounded rectangles, circles, etc.).
    ImguiStyle.CurveTessellationTol    = 1.25f;    						// Tessellation tolerance when using PathBezierCurveTo() without a specific number of segments. Decrease for highly tessellated curves (higher quality, more polygons), increase to reduce quality.
    ImguiStyle.CircleSegmentMaxError   = 1.60f;    						// Maximum error (in pixels) allowed when using AddCircle()/AddCircleFilled() or drawing rounded corner rectangles with no explicit segment count specified. Decrease for higher quality but more geometry.

}

void UImguiConfig::SetStyle(ImGuiStyle* InStyle)
{
	FMemory::Memcpy(UImguiConfig::Get()->ImguiColors.GetData(), InStyle->Colors, ImGuiCol_COUNT * sizeof(ImVec4));
	FMemory::Memcpy(&UImguiConfig::Get()->ImguiStyle,  InStyle,sizeof(FImguiStyle));
}
