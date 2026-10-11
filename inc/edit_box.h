/*
  Copyright (C) 1997-2025 Sam Lantinga <slouken@libsdl.org>

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any damages
  arising from the use of this software.

  Permission is granted to anyone to use this software for any purpose,
  including commercial applications, and to alter it and redistribute it
  freely.

  Note:
  to use eddit boxes you need to create a textengine in the programm settup:
  // General Text Engine (for all text)
  TTF_TextEngine *ptr_ttf_engine = TTF_CreateRendererTextEngine(Prim_Window.ptr_renderer);
*/

/*

  Modefyed by Chris ju

*/

/*
Core Components of this Module:
void EditBox_Destroy(EditBox *edit);
EditBox *EditBox_Create(SDL_Window *window, SDL_Renderer *renderer, TTF_TextEngine *engine, TTF_Font *font, const SDL_FRect *rect);
void EditBox_Draw(EditBox *edit);
bool EditBox_HandleEvent(EditBox *edit, SDL_Event *event);
*/

// Header Guard
#ifndef EDIT_BOX_HEADER_GUARD
#define EDIT_BOX_HEADER_GUARD

#include <stdlib.h>
#include <stdio.h>

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

/* Define this if you want to test the surface text engine */
#define TEST_SURFACE_ENGINE

#define CURSOR_BLINK_INTERVAL_MS    500


typedef struct EditBox {
    SDL_Window *window;
    SDL_Renderer *renderer;
    TTF_Font *font;
    TTF_Text *text;
    SDL_FRect rect;
    bool has_focus;

    /* Cursor support */
    int cursor;
    int cursor_length;
    bool cursor_visible;
    Uint64 last_cursor_change;
    SDL_FRect cursor_rect;

    /* Highlight support */
    bool highlighting;
    int highlight1;
    int highlight2;

    /* IME composition */
    int composition_start;
    int composition_length;
    int composition_cursor;
    int composition_cursor_length;

    /* IME candidates */
    TTF_Text *candidates;
    int selected_candidate_start;
    int selected_candidate_length;

#ifdef TEST_SURFACE_ENGINE
    SDL_Surface *window_surface;
#endif
} EditBox;


// Function Declorations (typicaly externaly not needed declorations are commented out)
//static void DrawText(EditBox *edit, TTF_Text *text, float x, float y);
//static bool GetHighlightExtents(EditBox *edit, int *marker, int *length);
//static void ResetComposition(EditBox *edit);
//static int UTF8ByteLength(const char *text, int num_codepoints);
//static void CancelComposition(EditBox *edit);
//static void SetCursorPosition(EditBox *edit, int position);
//bool EditBox_DeleteHighlight(EditBox *edit);
//static void HandleComposition(EditBox *edit, const SDL_TextEditingEvent *event);
//static void DrawComposition(EditBox *edit);
//static void DrawCompositionCursor(EditBox *edit);
//static void ClearCandidates(EditBox *edit);
//static void SaveCandidates(EditBox *edit, const SDL_Event *event);
//static void DrawCandidates(EditBox *edit);
//static void UpdateTextInputArea(EditBox *edit);
//static void DrawCursor(EditBox *edit);
//static int GetCursorTextIndex(int x, const TTF_SubString *substring);
//static void MoveCursorIndex(EditBox *edit, int direction);
//void EditBox_SetFocus(EditBox *edit, bool focus);
//static bool HandleMouseDown(EditBox *edit, float x, float y);
//static bool HandleMouseMotion(EditBox *edit, float x, float y);
//static bool HandleMouseUp(EditBox *edit, float x, float y);
 void EditBox_Destroy(EditBox *edit);
 EditBox *EditBox_Create(SDL_Window *window, SDL_Renderer *renderer, TTF_TextEngine *engine, TTF_Font *font, const SDL_FRect *rect);
 void EditBox_Draw(EditBox *edit);
//void EditBox_MoveCursorLeft(EditBox *edit);
//void EditBox_MoveCursorRight(EditBox *edit);
//void EditBox_MoveCursorUp(EditBox *edit);
//void EditBox_MoveCursorDown(EditBox *edit);
//void EditBox_MoveCursorBeginningOfLine(EditBox *edit);
//void EditBox_MoveCursorEndOfLine(EditBox *edit);
//void EditBox_MoveCursorBeginning(EditBox *edit);
//void EditBox_MoveCursorEnd(EditBox *edit);
//void EditBox_Backspace(EditBox *edit);
//void EditBox_BackspaceToBeginning(EditBox *edit);
//void EditBox_DeleteToEnd(EditBox *edit);
//void EditBox_Delete(EditBox *edit);
//void EditBox_SelectAll(EditBox *edit);
//void EditBox_Copy(EditBox *edit);
//void EditBox_Cut(EditBox *edit);
//void EditBox_Insert(EditBox *edit, const char *text);
//void EditBox_Paste(EditBox *edit);
 bool EditBox_HandleEvent(EditBox *edit, SDL_Event *event);



#endif
