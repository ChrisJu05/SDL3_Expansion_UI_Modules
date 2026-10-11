// Window Header
/*
Created by Christopher Jung 04.04.2026

This Header File Contains the Declorations for the window expansion module.
It is intended to make the Creation, managment, and updating of appliction
window easyer and more compact in code.
*/

// Header Guard
#ifndef WINDOW_HEADER_GUARD
#define WINDOW_HEADER_GUARD

// Standerd Libuary Inclusion
#include <stdlib.h>
#include <stdio.h>

// SDL3 Libuary Inclusion
#include <SDL3/SDL.h>                   // the core SDL libuary
#include <SDL3/SDL_main.h>              // part of SDL
#include <stdbool.h>

#include "glass.h"
#include "app_defaults.h"


// Application Window Structure: Holds all needed data for an application window
struct AplicationWindow
{
  // Core Window components
  SDL_Window *ptr_window;                                                       // Pointer to the SDL window structure
  SDL_Renderer *ptr_renderer;                                                   // Pointer to 2d Render Context of window
  SDL_Texture *ptr_window_texture;                                              // Pointer to window texture that is renderd to the window
  int upscaling_factor;                                                         // factor by how manny times larger the window texture is relative to the Window

  // Window propertys
  SDL_Rect window_pixel_size;                                                   // the windw size in pixels (window texture size may be diffrent from this size: texture size = window_pixel_size * scale)
  SDL_Rect window_logical_size;                                                 // The physical size of the window on screen
  bool use_pointsize;                                                           // if is true: objecs should be scaled by pointsize (for high dpi displays)
  float display_content_scale;                                                  // How the content should be scaled given by the screen the window is on. does not affect the window but is providet by it.
  float usr_scale;                                                              // User defined content scale multiplyer (for in case the user wants diffrently scaled UI). Does not affect the window and is given by the user 1.0f by default.
                                                                                // USE UpdateWindowUserScale() function to update this value as it effects UI_scale
  float ui_scale;                                                               // UI_scale = display_content_scale * usr_scaler . Is the final interface scaling factor (use for UI drawing).

  Uint64 last_resize_time;                                                      // The Last time the window was resized (for debounce timer)

  // Window default background color
  SDL_Color background_color;                                                   // may need to change the render clearing if the window is transparent
};


// Functions defined in window.c
// a create window function was intentionaly left out because a window may need difrent propertys depending on the application.
bool AliasingRender(SDL_Renderer* ptr_renderer, SDL_Texture* ptr_src_texture, SDL_Texture* ptr_dst_texture);
void InitWindowStruct(struct AplicationWindow *ptr_Window);
bool SetUpWindowTexture(struct AplicationWindow *ptr_Window, int upscaling_factor, float usr_scale, bool use_pointsize);
bool UpdateWindow(struct AplicationWindow *ptr_Window);
int HandlWindowResizing(struct AplicationWindow *ptr_Window);
void UpdateWindowUserScale(struct AplicationWindow *ptr_Window, float new_scale);
void DestroyAplicationWindow_stack(struct AplicationWindow *ptr_Window);
void DestroyAplicationWindow_heap(struct AplicationWindow *ptr_Window);

#endif
// END
