//------------------------------------------------------------------------------
// LICENSE
//   This software is dual-licensed to the public domain and under the following
//   license: you are granted a perpetual, irrevocable license to copy, modify,
//   publish, and distribute this file as you see fit.
//
// CREDITS
//   Written by Michal Cichon
//------------------------------------------------------------------------------
# ifndef __IMGUI_NODE_EDITOR_H__
# define __IMGUI_NODE_EDITOR_H__
# pragma once


//------------------------------------------------------------------------------
# include "imgui.h"
# include <cstdint> // std::uintXX_t
# include <utility> // std::move


//------------------------------------------------------------------------------
namespace ax {
namespace NodeEditor {


//------------------------------------------------------------------------------
struct NodeId;
struct LinkId;
struct PinId;


//------------------------------------------------------------------------------
enum class SaveReasonFlags: uint32_t
{
    None       = 0x00000000,
    Navigation = 0x00000001,
    Position   = 0x00000002,
    Size       = 0x00000004,
    Selection  = 0x00000008,
    User       = 0x00000010
};

inline SaveReasonFlags operator |(SaveReasonFlags lhs, SaveReasonFlags rhs) { return static_cast<SaveReasonFlags>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs)); }
inline SaveReasonFlags operator &(SaveReasonFlags lhs, SaveReasonFlags rhs) { return static_cast<SaveReasonFlags>(static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs)); }

using ConfigSaveSettings     = bool   (*)(const char* data, size_t size, SaveReasonFlags reason, void* userPointer);
using ConfigLoadSettings     = size_t (*)(char* data, void* userPointer);

using ConfigSaveNodeSettings = bool   (*)(NodeId nodeId, const char* data, size_t size, SaveReasonFlags reason, void* userPointer);
using ConfigLoadNodeSettings = size_t (*)(NodeId nodeId, char* data, void* userPointer);

using ConfigSession          = void   (*)(void* userPointer);

struct IMNODEEDITOR_API Config
{
    const char*             SettingsFile;
    ConfigSession           BeginSaveSession;
    ConfigSession           EndSaveSession;
    ConfigSaveSettings      SaveSettings;
    ConfigLoadSettings      LoadSettings;
    ConfigSaveNodeSettings  SaveNodeSettings;
    ConfigLoadNodeSettings  LoadNodeSettings;
    void*                   UserPointer;

    Config()
        : SettingsFile("NodeEditor.json")
        , BeginSaveSession(nullptr)
        , EndSaveSession(nullptr)
        , SaveSettings(nullptr)
        , LoadSettings(nullptr)
        , SaveNodeSettings(nullptr)
        , LoadNodeSettings(nullptr)
        , UserPointer(nullptr)
    {
    }
};


//------------------------------------------------------------------------------
enum class PinKind
{
    Input,
    Output
};


//------------------------------------------------------------------------------
enum StyleColor
{
    StyleColor_Bg,
    StyleColor_Grid,
    StyleColor_NodeBg,
    StyleColor_NodeBorder,
    StyleColor_HovNodeBorder,
    StyleColor_SelNodeBorder,
    StyleColor_NodeSelRect,
    StyleColor_NodeSelRectBorder,
    StyleColor_HovLinkBorder,
    StyleColor_SelLinkBorder,
    StyleColor_LinkSelRect,
    StyleColor_LinkSelRectBorder,
    StyleColor_PinRect,
    StyleColor_PinRectBorder,
    StyleColor_Flow,
    StyleColor_FlowMarker,
    StyleColor_GroupBg,
    StyleColor_GroupBorder,

    StyleColor_Count
};

enum StyleVar
{
    StyleVar_NodePadding,
    StyleVar_NodeRounding,
    StyleVar_NodeBorderWidth,
    StyleVar_HoveredNodeBorderWidth,
    StyleVar_SelectedNodeBorderWidth,
    StyleVar_PinRounding,
    StyleVar_PinBorderWidth,
    StyleVar_LinkStrength,
    StyleVar_SourceDirection,
    StyleVar_TargetDirection,
    StyleVar_ScrollDuration,
    StyleVar_FlowMarkerDistance,
    StyleVar_FlowSpeed,
    StyleVar_FlowDuration,
    StyleVar_PivotAlignment,
    StyleVar_PivotSize,
    StyleVar_PivotScale,
    StyleVar_PinCorners,
    StyleVar_PinRadius,
    StyleVar_PinArrowSize,
    StyleVar_PinArrowWidth,
    StyleVar_GroupRounding,
    StyleVar_GroupBorderWidth,

    StyleVar_Count
};

struct IMNODEEDITOR_API Style
{
    ImVec4  NodePadding;
    float   NodeRounding;
    float   NodeBorderWidth;
    float   HoveredNodeBorderWidth;
    float   SelectedNodeBorderWidth;
    float   PinRounding;
    float   PinBorderWidth;
    float   LinkStrength;
    ImVec2  SourceDirection;
    ImVec2  TargetDirection;
    float   ScrollDuration;
    float   FlowMarkerDistance;
    float   FlowSpeed;
    float   FlowDuration;
    ImVec2  PivotAlignment;
    ImVec2  PivotSize;
    ImVec2  PivotScale;
    float   PinCorners;
    float   PinRadius;
    float   PinArrowSize;
    float   PinArrowWidth;
    float   GroupRounding;
    float   GroupBorderWidth;
    ImVec4  Colors[StyleColor_Count];

    Style()
    {
        NodePadding             = ImVec4(8, 8, 8, 8);
        NodeRounding            = 12.0f;
        NodeBorderWidth         = 1.5f;
        HoveredNodeBorderWidth  = 3.5f;
        SelectedNodeBorderWidth = 3.5f;
        PinRounding             = 4.0f;
        PinBorderWidth          = 0.0f;
        LinkStrength            = 100.0f;
        SourceDirection         = ImVec2(1.0f, 0.0f);
        TargetDirection         = ImVec2(-1.0f, 0.0f);
        ScrollDuration          = 0.35f;
        FlowMarkerDistance      = 30.0f;
        FlowSpeed               = 150.0f;
        FlowDuration            = 2.0f;
        PivotAlignment          = ImVec2(0.5f, 0.5f);
        PivotSize               = ImVec2(0.0f, 0.0f);
        PivotScale              = ImVec2(1, 1);
        PinCorners              = ImDrawCornerFlags_All;
        PinRadius               = 0.0f;
        PinArrowSize            = 0.0f;
        PinArrowWidth           = 0.0f;
        GroupRounding           = 6.0f;
        GroupBorderWidth        = 1.0f;

        Colors[StyleColor_Bg]                 = ImColor( 60,  60,  70, 200);
        Colors[StyleColor_Grid]               = ImColor(120, 120, 120,  40);
        Colors[StyleColor_NodeBg]             = ImColor( 32,  32,  32, 200);
        Colors[StyleColor_NodeBorder]         = ImColor(255, 255, 255,  96);
        Colors[StyleColor_HovNodeBorder]      = ImColor( 50, 176, 255, 255);
        Colors[StyleColor_SelNodeBorder]      = ImColor(255, 176,  50, 255);
        Colors[StyleColor_NodeSelRect]        = ImColor(  5, 130, 255,  64);
        Colors[StyleColor_NodeSelRectBorder]  = ImColor(  5, 130, 255, 128);
        Colors[StyleColor_HovLinkBorder]      = ImColor( 50, 176, 255, 255);
        Colors[StyleColor_SelLinkBorder]      = ImColor(255, 176,  50, 255);
        Colors[StyleColor_LinkSelRect]        = ImColor(  5, 130, 255,  64);
        Colors[StyleColor_LinkSelRectBorder]  = ImColor(  5, 130, 255, 128);
        Colors[StyleColor_PinRect]            = ImColor( 60, 180, 255, 100);
        Colors[StyleColor_PinRectBorder]      = ImColor( 60, 180, 255, 128);
        Colors[StyleColor_Flow]               = ImColor(255, 128,  64, 255);
        Colors[StyleColor_FlowMarker]         = ImColor(255, 128,  64, 255);
        Colors[StyleColor_GroupBg]            = ImColor(  0,   0,   0, 160);
        Colors[StyleColor_GroupBorder]        = ImColor(255, 255, 255,  32);
    }
};


//------------------------------------------------------------------------------
struct EditorContext;


//------------------------------------------------------------------------------
IMNODEEDITOR_API void SetCurrentEditor(EditorContext* ctx);
IMNODEEDITOR_API EditorContext* GetCurrentEditor();
IMNODEEDITOR_API EditorContext* CreateEditor(const Config* config = nullptr);
IMNODEEDITOR_API void DestroyEditor(EditorContext* ctx);

IMNODEEDITOR_API Style& GetStyle();
IMNODEEDITOR_API const char* GetStyleColorName(StyleColor colorIndex);

IMNODEEDITOR_API void PushStyleColor(StyleColor colorIndex, const ImVec4& color);
IMNODEEDITOR_API void PopStyleColor(int count = 1);

IMNODEEDITOR_API void PushStyleVar(StyleVar varIndex, float value);
IMNODEEDITOR_API void PushStyleVar(StyleVar varIndex, const ImVec2& value);
IMNODEEDITOR_API void PushStyleVar(StyleVar varIndex, const ImVec4& value);
IMNODEEDITOR_API void PopStyleVar(int count = 1);

IMNODEEDITOR_API void Begin(const char* id, const ImVec2& size = ImVec2(0, 0));
IMNODEEDITOR_API void End();

IMNODEEDITOR_API void BeginNode(NodeId id);
IMNODEEDITOR_API void BeginPin(PinId id, PinKind kind);
IMNODEEDITOR_API void PinRect(const ImVec2& a, const ImVec2& b);
IMNODEEDITOR_API void PinPivotRect(const ImVec2& a, const ImVec2& b);
IMNODEEDITOR_API void PinPivotSize(const ImVec2& size);
IMNODEEDITOR_API void PinPivotScale(const ImVec2& scale);
IMNODEEDITOR_API void PinPivotAlignment(const ImVec2& alignment);
IMNODEEDITOR_API void EndPin();
IMNODEEDITOR_API void Group(const ImVec2& size);
IMNODEEDITOR_API void EndNode();

IMNODEEDITOR_API bool BeginGroupHint(NodeId nodeId);
IMNODEEDITOR_API ImVec2 GetGroupMin();
IMNODEEDITOR_API ImVec2 GetGroupMax();
IMNODEEDITOR_API ImDrawList* GetHintForegroundDrawList();
IMNODEEDITOR_API ImDrawList* GetHintBackgroundDrawList();
IMNODEEDITOR_API void EndGroupHint();

// TODO: Add a way to manage node background channels
IMNODEEDITOR_API ImDrawList* GetNodeBackgroundDrawList(NodeId nodeId);

IMNODEEDITOR_API bool Link(LinkId id, PinId startPinId, PinId endPinId, const ImVec4& color = ImVec4(1, 1, 1, 1), float thickness = 1.0f);

IMNODEEDITOR_API void Flow(LinkId linkId);

IMNODEEDITOR_API bool BeginCreate(const ImVec4& color = ImVec4(1, 1, 1, 1), float thickness = 1.0f);
IMNODEEDITOR_API bool QueryNewLink(PinId* startId, PinId* endId);
IMNODEEDITOR_API bool QueryNewLink(PinId* startId, PinId* endId, const ImVec4& color, float thickness = 1.0f);
IMNODEEDITOR_API bool QueryNewNode(PinId* pinId);
IMNODEEDITOR_API bool QueryNewNode(PinId* pinId, const ImVec4& color, float thickness = 1.0f);
IMNODEEDITOR_API bool AcceptNewItem();
IMNODEEDITOR_API bool AcceptNewItem(const ImVec4& color, float thickness = 1.0f);
IMNODEEDITOR_API void RejectNewItem();
IMNODEEDITOR_API void RejectNewItem(const ImVec4& color, float thickness = 1.0f);
IMNODEEDITOR_API void EndCreate();

IMNODEEDITOR_API bool BeginDelete();
IMNODEEDITOR_API bool QueryDeletedLink(LinkId* linkId, PinId* startId = nullptr, PinId* endId = nullptr);
IMNODEEDITOR_API bool QueryDeletedNode(NodeId* nodeId);
IMNODEEDITOR_API bool AcceptDeletedItem();
IMNODEEDITOR_API void RejectDeletedItem();
IMNODEEDITOR_API void EndDelete();

IMNODEEDITOR_API void SetNodePosition(NodeId nodeId, const ImVec2& editorPosition);
IMNODEEDITOR_API ImVec2 GetNodePosition(NodeId nodeId);
IMNODEEDITOR_API ImVec2 GetNodeSize(NodeId nodeId);
IMNODEEDITOR_API void CenterNodeOnScreen(NodeId nodeId);

IMNODEEDITOR_API void RestoreNodeState(NodeId nodeId);

IMNODEEDITOR_API void Suspend();
IMNODEEDITOR_API void Resume();
IMNODEEDITOR_API bool IsSuspended();

IMNODEEDITOR_API bool IsActive();

IMNODEEDITOR_API bool HasSelectionChanged();
IMNODEEDITOR_API int  GetSelectedObjectCount();
IMNODEEDITOR_API int  GetSelectedNodes(NodeId* nodes, int size);
IMNODEEDITOR_API int  GetSelectedLinks(LinkId* links, int size);
IMNODEEDITOR_API void ClearSelection();
IMNODEEDITOR_API void SelectNode(NodeId nodeId, bool append = false);
IMNODEEDITOR_API void SelectLink(LinkId linkId, bool append = false);
IMNODEEDITOR_API void DeselectNode(NodeId nodeId);
IMNODEEDITOR_API void DeselectLink(LinkId linkId);

IMNODEEDITOR_API bool DeleteNode(NodeId nodeId);
IMNODEEDITOR_API bool DeleteLink(LinkId linkId);

IMNODEEDITOR_API void NavigateToContent(float duration = -1);
IMNODEEDITOR_API void NavigateToSelection(bool zoomIn = false, float duration = -1);

IMNODEEDITOR_API bool ShowNodeContextMenu(NodeId* nodeId);
IMNODEEDITOR_API bool ShowPinContextMenu(PinId* pinId);
IMNODEEDITOR_API bool ShowLinkContextMenu(LinkId* linkId);
IMNODEEDITOR_API bool ShowBackgroundContextMenu();

IMNODEEDITOR_API void EnableShortcuts(bool enable);
IMNODEEDITOR_API bool AreShortcutsEnabled();

IMNODEEDITOR_API bool BeginShortcut();
IMNODEEDITOR_API bool AcceptCut();
IMNODEEDITOR_API bool AcceptCopy();
IMNODEEDITOR_API bool AcceptPaste();
IMNODEEDITOR_API bool AcceptDuplicate();
IMNODEEDITOR_API bool AcceptCreateNode();
IMNODEEDITOR_API int  GetActionContextSize();
IMNODEEDITOR_API int  GetActionContextNodes(NodeId* nodes, int size);
IMNODEEDITOR_API int  GetActionContextLinks(LinkId* links, int size);
IMNODEEDITOR_API void EndShortcut();

IMNODEEDITOR_API float GetCurrentZoom();

IMNODEEDITOR_API NodeId GetDoubleClickedNode();
IMNODEEDITOR_API PinId GetDoubleClickedPin();
IMNODEEDITOR_API LinkId GetDoubleClickedLink();
IMNODEEDITOR_API bool IsBackgroundClicked();
IMNODEEDITOR_API bool IsBackgroundDoubleClicked();

IMNODEEDITOR_API bool PinHadAnyLinks(PinId pinId);

IMNODEEDITOR_API ImVec2 GetScreenSize();
IMNODEEDITOR_API ImVec2 ScreenToCanvas(const ImVec2& pos);
IMNODEEDITOR_API ImVec2 CanvasToScreen(const ImVec2& pos);










//------------------------------------------------------------------------------
namespace Details {

template <typename T, typename Tag>
struct SafeType
{
    SafeType(T t)
        : m_Value(std::move(t))
    {
    }

    SafeType(const SafeType&) = default;

    template <typename T2, typename Tag2>
    SafeType(
        const SafeType
        <
            typename std::enable_if<!std::is_same<T, T2>::value, T2>::type,
            typename std::enable_if<!std::is_same<Tag, Tag2>::value, Tag2>::type
        >&) = delete;

    SafeType& operator=(const SafeType&) = default;

    explicit operator T() const { return Get(); }

    T Get() const { return m_Value; }

private:
    T m_Value;
};


template <typename Tag>
struct SafePointerType
    : SafeType<uintptr_t, Tag>
{
    static const Tag Invalid;

    using SafeType<uintptr_t, Tag>::SafeType;

    SafePointerType()
        : SafePointerType(Invalid)
    {
    }

    template <typename T = void> explicit SafePointerType(T* ptr): SafePointerType(reinterpret_cast<uintptr_t>(ptr)) {}
    template <typename T = void> T* AsPointer() const { return reinterpret_cast<T*>(this->Get()); }

    explicit operator bool() const { return *this != Invalid; }
};

template <typename Tag>
const Tag SafePointerType<Tag>::Invalid = { 0 };

template <typename Tag>
inline bool operator==(const SafePointerType<Tag>& lhs, const SafePointerType<Tag>& rhs)
{
    return lhs.Get() == rhs.Get();
}

template <typename Tag>
inline bool operator!=(const SafePointerType<Tag>& lhs, const SafePointerType<Tag>& rhs)
{
    return lhs.Get() != rhs.Get();
}

} // namespace Details

struct NodeId final: Details::SafePointerType<NodeId>
{
    using SafePointerType::SafePointerType;
};

struct LinkId final: Details::SafePointerType<LinkId>
{
    using SafePointerType::SafePointerType;
};

struct PinId final: Details::SafePointerType<PinId>
{
    using SafePointerType::SafePointerType;
};


//------------------------------------------------------------------------------
} // namespace Editor
} // namespace ax


//------------------------------------------------------------------------------
# endif // __IMGUI_NODE_EDITOR_H__