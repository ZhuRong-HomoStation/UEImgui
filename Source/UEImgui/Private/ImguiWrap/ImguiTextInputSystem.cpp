#include "ImguiWrap/ImguiTextInputSystem.h"
#include "imgui_internal.h"
#include "ImguiWrap/ImguiUEWrap.h"

namespace ImStb
{
static ImVec2 InputTextCalcTextSizeW(const ImWchar* text_begin, const ImWchar* text_end, const ImWchar** remaining, ImVec2* out_offset, bool stop_on_new_line, ImGuiContext* TargetContext = nullptr)
{
	ImGuiContext& g = TargetContext ? *TargetContext : *GImGui;
	ImFont* font = g.Font;
	const float line_height = g.FontSize;
	const float scale = line_height / font->FontSize;

	ImVec2 text_size = ImVec2(0, 0);
	float line_width = 0.0f;

	const ImWchar* s = text_begin;
	while (s < text_end)
	{
		unsigned int c = (unsigned int)(*s++);
		if (c == '\n')
		{
			text_size.x = ImMax(text_size.x, line_width);
			text_size.y += line_height;
			line_width = 0.0f;
			if (stop_on_new_line)
				break;
			continue;
		}
		if (c == '\r')
			continue;

		const float char_width = font->GetCharAdvance((ImWchar)c) * scale;
		line_width += char_width;
	}

	if (text_size.x < line_width)
		text_size.x = line_width;

	if (out_offset)
		*out_offset = ImVec2(line_width, text_size.y + line_height);  // offset allow for the possibility of sitting after a trailing \n

	if (line_width > 0 || text_size.y == 0.0f)                        // whereas size.y will ignore the trailing \n
		text_size.y += line_height;

	if (remaining)
		*remaining = s;

	return text_size;
}

static int     STB_TEXTEDIT_STRINGLEN(const STB_TEXTEDIT_STRING* obj)                             { return obj->CurLenW; }
static ImWchar STB_TEXTEDIT_GETCHAR(const STB_TEXTEDIT_STRING* obj, int idx)                      { return obj->TextW[idx]; }
static float   STB_TEXTEDIT_GETWIDTH(STB_TEXTEDIT_STRING* obj, int line_start_idx, int char_idx)  { ImWchar c = obj->TextW[line_start_idx + char_idx]; if (c == '\n') return STB_TEXTEDIT_GETWIDTH_NEWLINE; ImGuiContext& g = *GImGui; return g.Font->GetCharAdvance(c) * (g.FontSize / g.Font->FontSize); }
static int     STB_TEXTEDIT_KEYTOTEXT(int key)                                                    { return key >= 0x200000 ? 0 : key; }
static ImWchar STB_TEXTEDIT_NEWLINE = '\n';
static void    STB_TEXTEDIT_LAYOUTROW(StbTexteditRow* r, STB_TEXTEDIT_STRING* obj, int line_start_idx)
{
    const ImWchar* text = obj->TextW.Data;
    const ImWchar* text_remaining = NULL;
    const ImVec2 size = InputTextCalcTextSizeW(text + line_start_idx, text + obj->CurLenW, &text_remaining, NULL, true);
    r->x0 = 0.0f;
    r->x1 = size.x;
    r->baseline_y_delta = size.y;
    r->ymin = 0.0f;
    r->ymax = size.y;
    r->num_chars = (int)(text_remaining - (text + line_start_idx));
}

static bool is_separator(unsigned int c)                                        { return ImCharIsBlankW(c) || c==',' || c==';' || c=='(' || c==')' || c=='{' || c=='}' || c=='[' || c==']' || c=='|'; }
static int  is_word_boundary_from_right(STB_TEXTEDIT_STRING* obj, int idx)      { return idx > 0 ? (is_separator(obj->TextW[idx - 1]) && !is_separator(obj->TextW[idx]) ) : 1; }
static int  STB_TEXTEDIT_MOVEWORDLEFT_IMPL(STB_TEXTEDIT_STRING* obj, int idx)   { idx--; while (idx >= 0 && !is_word_boundary_from_right(obj, idx)) idx--; return idx < 0 ? 0 : idx; }
#ifdef __APPLE__    // FIXME: Move setting to IO structure
static int  is_word_boundary_from_left(STB_TEXTEDIT_STRING* obj, int idx)       { return idx > 0 ? (!is_separator(obj->TextW[idx - 1]) && is_separator(obj->TextW[idx]) ) : 1; }
static int  STB_TEXTEDIT_MOVEWORDRIGHT_IMPL(STB_TEXTEDIT_STRING* obj, int idx)  { idx++; int len = obj->CurLenW; while (idx < len && !is_word_boundary_from_left(obj, idx)) idx++; return idx > len ? len : idx; }
#else
static int  STB_TEXTEDIT_MOVEWORDRIGHT_IMPL(STB_TEXTEDIT_STRING* obj, int idx)  { idx++; int len = obj->CurLenW; while (idx < len && !is_word_boundary_from_right(obj, idx)) idx++; return idx > len ? len : idx; }
#endif
#define STB_TEXTEDIT_MOVEWORDLEFT   STB_TEXTEDIT_MOVEWORDLEFT_IMPL    // They need to be #define for stb_textedit.h
#define STB_TEXTEDIT_MOVEWORDRIGHT  STB_TEXTEDIT_MOVEWORDRIGHT_IMPL

static void STB_TEXTEDIT_DELETECHARS(STB_TEXTEDIT_STRING* obj, int pos, int n)
{
    ImWchar* dst = obj->TextW.Data + pos;

    // We maintain our buffer length in both UTF-8 and wchar formats
    obj->Edited = true;
    obj->CurLenA -= ImTextCountUtf8BytesFromStr(dst, dst + n);
    obj->CurLenW -= n;

    // Offset remaining text (FIXME-OPT: Use memmove)
    const ImWchar* src = obj->TextW.Data + pos + n;
    while (ImWchar c = *src++)
        *dst++ = c;
    *dst = '\0';
}

static bool STB_TEXTEDIT_INSERTCHARS(STB_TEXTEDIT_STRING* obj, int pos, const ImWchar* new_text, int new_text_len)
{
    const bool is_resizable = (obj->UserFlags & ImGuiInputTextFlags_CallbackResize) != 0;
    const int text_len = obj->CurLenW;
    IM_ASSERT(pos <= text_len);

    const int new_text_len_utf8 = ImTextCountUtf8BytesFromStr(new_text, new_text + new_text_len);
    if (!is_resizable && (new_text_len_utf8 + obj->CurLenA + 1 > obj->BufCapacityA))
        return false;

    // Grow internal buffer if needed
    if (new_text_len + text_len + 1 > obj->TextW.Size)
    {
        if (!is_resizable)
            return false;
        IM_ASSERT(text_len < obj->TextW.Size);
        obj->TextW.resize(text_len + ImClamp(new_text_len * 4, 32, ImMax(256, new_text_len)) + 1);
    }

    ImWchar* text = obj->TextW.Data;
    if (pos != text_len)
        memmove(text + pos + new_text_len, text + pos, (size_t)(text_len - pos) * sizeof(ImWchar));
    memcpy(text + pos, new_text, (size_t)new_text_len * sizeof(ImWchar));

    obj->Edited = true;
    obj->CurLenW += new_text_len;
    obj->CurLenA += new_text_len_utf8;
    obj->TextW[obj->CurLenW] = '\0';

    return true;
}

// We don't use an enum so we can build even with conflicting symbols (if another user of stb_textedit.h leak their STB_TEXTEDIT_K_* symbols)
#define STB_TEXTEDIT_K_LEFT         0x200000 // keyboard input to move cursor left
#define STB_TEXTEDIT_K_RIGHT        0x200001 // keyboard input to move cursor right
#define STB_TEXTEDIT_K_UP           0x200002 // keyboard input to move cursor up
#define STB_TEXTEDIT_K_DOWN         0x200003 // keyboard input to move cursor down
#define STB_TEXTEDIT_K_LINESTART    0x200004 // keyboard input to move cursor to start of line
#define STB_TEXTEDIT_K_LINEEND      0x200005 // keyboard input to move cursor to end of line
#define STB_TEXTEDIT_K_TEXTSTART    0x200006 // keyboard input to move cursor to start of text
#define STB_TEXTEDIT_K_TEXTEND      0x200007 // keyboard input to move cursor to end of text
#define STB_TEXTEDIT_K_DELETE       0x200008 // keyboard input to delete selection or character under cursor
#define STB_TEXTEDIT_K_BACKSPACE    0x200009 // keyboard input to delete selection or character left of cursor
#define STB_TEXTEDIT_K_UNDO         0x20000A // keyboard input to perform undo
#define STB_TEXTEDIT_K_REDO         0x20000B // keyboard input to perform redo
#define STB_TEXTEDIT_K_WORDLEFT     0x20000C // keyboard input to move cursor left one word
#define STB_TEXTEDIT_K_WORDRIGHT    0x20000D // keyboard input to move cursor right one word
#define STB_TEXTEDIT_K_PGUP         0x20000E // keyboard input to move cursor up a page
#define STB_TEXTEDIT_K_PGDOWN       0x20000F // keyboard input to move cursor down a page
#define STB_TEXTEDIT_K_SHIFT        0x400000

#define STB_TEXTEDIT_IMPLEMENTATION
#include "imstb_textedit.h"

// stb_textedit internally allows for a single undo record to do addition and deletion, but somehow, calling
// the stb_textedit_paste() function creates two separate records, so we perform it manually. (FIXME: Report to nothings/stb?)
static void stb_textedit_replace(STB_TEXTEDIT_STRING* str, STB_TexteditState* state, const STB_TEXTEDIT_CHARTYPE* text, int text_len)
{
    stb_text_makeundo_replace(str, state, 0, str->CurLenW, text_len);
    ImStb::STB_TEXTEDIT_DELETECHARS(str, 0, str->CurLenW);
    if (text_len <= 0)
        return;
    if (ImStb::STB_TEXTEDIT_INSERTCHARS(str, 0, text, text_len))
    {
        state->cursor = text_len;
        state->has_preferred_x = 0;
        return;
    }
    IM_ASSERT(0); // Failed to insert character, normally shouldn't happen because of how we currently use stb_textedit_replace()
}

} // namespace ImStb


void FImguiTextInputSystem::Reset()
{
	CaretPosition = ECaretPosition::Beginning;
}

void FImguiTextInputSystem::BeginListen()
{
	NextWidget.Reset();
}

void FImguiTextInputSystem::EndListen()
{
	if (NextWidget.IsValid())
	{
		// update register 
		if (CurrentWidget != NextWidget && bHasRegistered)
		{
			FSlateApplication::Get().GetTextInputMethodSystem()->DeactivateContext(GetRef());
			FSlateApplication::Get().GetTextInputMethodSystem()->UnregisterContext(GetRef());
			bHasRegistered = false;
		}
		
		// set up window and widget 
		CurrentWidget = NextWidget;
		{
			SWidget* CurWidget = CurrentWidget.Pin().Get();
			while (CurWidget)
			{
				if (CurWidget->Advanced_IsWindow())
				{
					CurrentWindow = StaticCastSharedRef<SWindow>(CurWidget->AsShared());
					break;
				}
				CurWidget = CurWidget->GetParentWidget().Get();
			}
		}

		// register self
		if (!bHasRegistered)
		{
			Reset();
			FSlateApplication::Get().GetTextInputMethodSystem()->RegisterContext(GetRef());
			FSlateApplication::Get().GetTextInputMethodSystem()->ActivateContext(GetRef());
			bHasRegistered = true;
		}
	}
	else
	{
		// set up window and widget 
		CurrentWidget.Reset();
		CurrentWindow.Reset();
		if (bHasRegistered)
		{
			Reset();
			FSlateApplication::Get().GetTextInputMethodSystem()->DeactivateContext(GetRef());
			FSlateApplication::Get().GetTextInputMethodSystem()->UnregisterContext(GetRef());
			bHasRegistered = false;		
		}
	}
}

bool FImguiTextInputSystem::IsComposing()
{
	return bIsComposing;
}

void FImguiTextInputSystem::BeginComposition()
{
	if (!bIsComposing)
	{
		bIsComposing = true;
	}
	// UE_LOG(LogTemp, Display, TEXT("BeginComposition"));
}

void FImguiTextInputSystem::UpdateCompositionRange(const int32 InBeginIndex, const uint32 InLength)
{
	// UE_LOG(LogTemp, Display, TEXT("CompositionRange----%d To %d"), InBeginIndex, InBeginIndex + InLength);
}

void FImguiTextInputSystem::EndComposition()
{
	bIsComposing = false;
	// UE_LOG(LogTemp, Display, TEXT("EndComposition"));
}

bool FImguiTextInputSystem::IsReadOnly()
{
	if (!ImguiInputState)
	{
		return false;
	}
	
	return ImguiInputState->UserFlags & ImGuiInputTextFlags_ReadOnly;
}

uint32 FImguiTextInputSystem::GetTextLength()
{
	if (!ImguiInputState)
	{
		return 0;
	}
	
	return ImguiInputState->TextW.Capacity;
}

void FImguiTextInputSystem::GetSelectionRange(uint32& OutBeginIndex, uint32& OutLength, ECaretPosition& OutCaretPosition)
{
	if (!ImguiInputState) return;

	if (ImguiInputState->Stb.select_start == ImguiInputState->Stb.select_end)
	{
		OutBeginIndex = ImguiInputState->Stb.cursor;
		OutLength = 0;
	}
	else
	{
		OutBeginIndex = ImguiInputState->Stb.select_start;
		int32 Len = ImguiInputState->Stb.select_end - ImguiInputState->Stb.select_start;
		if (Len < 0)
		{
			OutBeginIndex += Len;
			OutLength = -Len;
		}
		else
		{
			OutLength = Len;
		}
	}
}

void FImguiTextInputSystem::SetSelectionRange(const uint32 InBeginIndex, const uint32 InLength, const ECaretPosition InCaretPosition)
{
	if (!ImguiInputState) return;

	ImguiInputState->Stb.select_start = InBeginIndex;
	ImguiInputState->Stb.select_end = InBeginIndex + InLength;
}

void FImguiTextInputSystem::GetTextInRange(const uint32 InBeginIndex, const uint32 InLength, FString& OutString)
{
	if (!ImguiInputState) return;
	const uint32 CurLen = ImguiInputState->CurLenW - InBeginIndex;
	OutString.Reset();
	OutString.Append(ImguiInputState->TextW.Data, InLength);
}

void FImguiTextInputSystem::SetTextInRange(const uint32 InBeginIndex, const uint32 InLength, const FString& InString)
{
	if (!ImguiInputState) return;
	if (InBeginIndex == 0 && InLength == 0) return;
	const uint32 CurLen = ImguiInputState->CurLenW - InBeginIndex;
	ImguiInputState->Stb.cursor = InBeginIndex;
	if (InLength)
	{
		ImguiInputState->Stb.select_start = InBeginIndex;
		ImguiInputState->Stb.select_end = InBeginIndex + InLength;
	}
	ImStb::stb_textedit_paste(ImguiInputState, &ImguiInputState->Stb, (const ImWchar*)*InString, InString.Len());
}

int32 FImguiTextInputSystem::GetCharacterIndexFromPoint(const FVector2D& InPoint)
{
	FVector2D RelativePos = InPoint - TextBeginPos;
	return ImStb::stb_text_locate_coord(ImguiInputState, RelativePos.X, RelativePos.Y);
}

bool FImguiTextInputSystem::GetTextBounds(
	const uint32 InBeginIndex,
	const uint32 InLength,
	FVector2D& OutPosition,
	FVector2D& OutSize)
{
	if (!TargetContext)
	{
		return true;
	}
	ImVec2 Size = ImStb::InputTextCalcTextSizeW(
		ImguiInputState->TextW.Data + InBeginIndex,
		ImguiInputState->TextW.Data + InBeginIndex + InLength,
		nullptr,
		nullptr,
		false,
		TargetContext);
	ImVec2 Pos = ImStb::InputTextCalcTextSizeW(
        ImguiInputState->TextW.Data,
        ImguiInputState->TextW.Data + InBeginIndex,
        nullptr,
        nullptr,
        false,
        TargetContext);
 	OutSize.X = Size.x;
	OutSize.Y = Size.y;
	OutPosition.X = Pos.x + TextBeginPos.X;
	OutPosition.Y = Pos.y + TextBeginPos.Y;
	return false;
}

void FImguiTextInputSystem::GetScreenBounds(FVector2D& OutPosition, FVector2D& OutSize)
{
	OutPosition.X = InputAreaBound.X;
	OutPosition.Y = InputAreaBound.Y;
	OutSize.X = InputAreaBound.Z;
	OutSize.Y = InputAreaBound.W;
}

TSharedPtr<FGenericWindow> FImguiTextInputSystem::GetWindow()
{
	return CurrentWindow.IsValid() ? CurrentWindow.Pin()->GetNativeWindow() : nullptr;
}
