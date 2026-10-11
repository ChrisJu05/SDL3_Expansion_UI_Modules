// Window module Code file
/*
Created by Christopher Jung 04.04.2026

This File Contains the Definitions for the window expansion module.
It is intended to make the Creation, managment, and updating of appliction
window easyer and more compact in code.
*/

// Primarry Header File
#include "window.h"


// Function that renders the source texture on to the Destination texture in Anti Aliasing manner without affecting annything else.
// Note That only Even downscaling factors make sense.
// (does not clear ptr_src_texture befor rendering)
bool AliasingRender(SDL_Renderer* ptr_renderer, SDL_Texture* ptr_src_texture, SDL_Texture* ptr_dst_texture){

  // Validat the input
  if(!ptr_renderer || !ptr_src_texture || !ptr_dst_texture) return false;

  // Get the destination Texture from the renderer (so it can be reset after function completes)
  SDL_Texture* ptr_render_targed = SDL_GetRenderTarget(ptr_renderer);

  // Record Texture Propertys (so they can be reset after)
  SDL_ScaleMode src_ScaleMode;
  SDL_ScaleMode dst_ScaleMode;

  SDL_BlendMode scr_BlendMode;
  SDL_BlendMode dst_BlendMode;

  SDL_GetTextureScaleMode(ptr_src_texture, &src_ScaleMode);
  SDL_GetTextureScaleMode(ptr_dst_texture, &dst_ScaleMode);

  SDL_GetTextureBlendMode(ptr_src_texture, &scr_BlendMode);
  SDL_GetTextureBlendMode(ptr_dst_texture, &dst_BlendMode);

  // Set Scale nad Blend Mode Properys to Linear and Blend
  SDL_SetTextureScaleMode(ptr_src_texture, SDL_SCALEMODE_LINEAR);
  SDL_SetTextureScaleMode(ptr_dst_texture, SDL_SCALEMODE_LINEAR);

  SDL_SetTextureBlendMode(ptr_src_texture, SDL_BLENDMODE_BLEND);
  SDL_SetTextureBlendMode(ptr_dst_texture, SDL_BLENDMODE_BLEND);

  // Render Source on to destination
  if(!SDL_SetRenderTarget(ptr_renderer, ptr_dst_texture)) return false;
  if(!SDL_RenderTexture(ptr_renderer, ptr_src_texture, NULL, NULL)) return false;

  // Reset Texture Propertys
  SDL_SetTextureScaleMode(ptr_src_texture, src_ScaleMode);
  SDL_SetTextureScaleMode(ptr_dst_texture, dst_ScaleMode);

  SDL_SetTextureBlendMode(ptr_src_texture, scr_BlendMode);
  SDL_SetTextureBlendMode(ptr_dst_texture, dst_BlendMode);

  // Reset Render Targed
  return SDL_SetRenderTarget(ptr_renderer, ptr_render_targed);
}



// Function for Initializing Window Struct
// sets all pointers to NULL and sets default values to acceptable start values
void InitWindowStruct(struct AplicationWindow *ptr_Window){

  // Validate Input
  if(!ptr_Window) return;

  // Null all pointers
  ptr_Window->ptr_window = NULL;
  ptr_Window->ptr_renderer = NULL;
  ptr_Window->ptr_window_texture = NULL;

  ptr_Window->upscaling_factor = 1;
  ptr_Window->window_pixel_size.x = 0;
  ptr_Window->window_pixel_size.y = 0;
  ptr_Window->window_pixel_size.w = 400;
  ptr_Window->window_pixel_size.h = 200;

  ptr_Window->window_logical_size.x = 0;
  ptr_Window->window_logical_size.y = 0;
  ptr_Window->window_logical_size.w = 400;
  ptr_Window->window_logical_size.h = 200;

  ptr_Window->use_pointsize = false;

  ptr_Window->display_content_scale = 1.0f;
  ptr_Window->usr_scale = 1.0f;
  ptr_Window->ui_scale = 1.0f;

  ptr_Window->last_resize_time = 0;

  ptr_Window->background_color.r = 0;
  ptr_Window->background_color.g = 0;
  ptr_Window->background_color.b = 0;
  ptr_Window->background_color.a = 0;

  return;
}



// Function for setting up window Texture
// return true on sucsess and false on fail
// Is called after the window and the renderer are created to set up the rest
// Note only Evven Values make sens for upscaling values
bool SetUpWindowTexture(struct AplicationWindow *ptr_Window, int upscaling_factor, float usr_scale, bool use_pointsize){

  // Validate Input
  if(!ptr_Window) return false;
  if(ptr_Window->ptr_window_texture) return false;

  // return value holder
  bool b_ret;

  // Fill in Window Parameters
  // Logical window size
  b_ret = SDL_GetWindowSize(ptr_Window->ptr_window, &(ptr_Window->window_logical_size.w), &(ptr_Window->window_logical_size.h));
  if(!b_ret) return false;

  // Pixel window size
  b_ret = SDL_GetWindowSizeInPixels(ptr_Window->ptr_window, &(ptr_Window->window_pixel_size.w), &(ptr_Window->window_pixel_size.h));
  if(!b_ret) return false;

  ptr_Window->display_content_scale = SDL_GetDisplayContentScale(SDL_GetDisplayForWindow(ptr_Window->ptr_window));
  if(ptr_Window->display_content_scale == 0) ptr_Window->display_content_scale = 1.0f;

  ptr_Window->upscaling_factor = upscaling_factor;
  ptr_Window->usr_scale = usr_scale;
  ptr_Window->use_pointsize = use_pointsize;

  ptr_Window->ui_scale = ptr_Window->display_content_scale * ptr_Window->usr_scale;

  // make shore the cordinates are 0:
  ptr_Window->window_pixel_size.x = 0;
  ptr_Window->window_pixel_size.y = 0;
  ptr_Window->window_logical_size.x = 0;
  ptr_Window->window_logical_size.y = 0;
  
  // Create a Texture for the Window
  ptr_Window->ptr_window_texture = SDL_CreateTexture(ptr_Window->ptr_renderer, DEFAULT_PIXELFORMAT, SDL_TEXTUREACCESS_TARGET, ptr_Window->window_pixel_size.w * ptr_Window->upscaling_factor, ptr_Window->window_pixel_size.h * ptr_Window->upscaling_factor);
  if(!ptr_Window->ptr_window_texture) return false;

  // Set Texture scale Mode so that downsampling results in anti alisaing
  b_ret = SDL_SetTextureScaleMode(ptr_Window->ptr_window_texture, SDL_SCALEMODE_LINEAR);
  if(!b_ret) return false;

  // Set the Window Texture as the Renderer Targed
  b_ret = SDL_SetRenderTarget(ptr_Window->ptr_renderer, ptr_Window->ptr_window_texture);
  if(!b_ret) return false;

  return true;
}


// Function for Updating the window frame (typ. called after drawing is complete)
// Clears the window (but not the window texture) and renders the window texture to the screen (also resets the renderer draw collor to all 0)
bool UpdateWindow(struct AplicationWindow *ptr_Window){

  // Validate Input
  if(!ptr_Window) return false;

  // bool return value
  bool b_ret;

  // Clear the window
  b_ret = SDL_SetRenderTarget(ptr_Window->ptr_renderer, NULL);
  if(!b_ret) return false;

  b_ret = SDL_SetRenderDrawColor(ptr_Window->ptr_renderer, 0, 0, 0, 0);
  if(!b_ret) return false;

  // Render the window texture to the window
  b_ret = SDL_RenderTexture(ptr_Window->ptr_renderer, ptr_Window->ptr_window_texture, NULL, NULL);
  if(!b_ret) return false;

  // Update the window
  b_ret = SDL_RenderPresent(ptr_Window->ptr_renderer);
  if(!b_ret) return false;

  b_ret = SDL_RenderClear(ptr_Window->ptr_renderer);
  if(!b_ret) return false;

  // Reset the render Targed
  b_ret = SDL_SetRenderTarget(ptr_Window->ptr_renderer, ptr_Window->ptr_window_texture);
  if(!b_ret) return false;

  return true;
}



// Function for Handeling window resizing
int HandlWindowResizing(struct AplicationWindow *ptr_Window){

  // as the window is resized the Window texture must be resized with it
  // to avoid regenerating a new texture every frame a debounce of 200ms is set
  if((SDL_GetTicks() - ptr_Window->last_resize_time) < 200) return 0;

  // bool return value
  bool b_ret;

  // Logical window size
  b_ret = SDL_GetWindowSize(ptr_Window->ptr_window, &(ptr_Window->window_logical_size.w), &(ptr_Window->window_logical_size.h));
  if(!b_ret) return 16;

  // Pixel window size
  b_ret = SDL_GetWindowSizeInPixels(ptr_Window->ptr_window, &(ptr_Window->window_pixel_size.w), &(ptr_Window->window_pixel_size.h));
  if(!b_ret) return 17;

  // Resize Window Texture if window size has changed
  if(ptr_Window->window_pixel_size.w != (ptr_Window->ptr_window_texture)->w || ptr_Window->window_pixel_size.h != (ptr_Window->ptr_window_texture)->h){

    // set renderer targed to Window so that ptr_Window->ptr_window_texture can be destroyed
    b_ret = SDL_SetRenderTarget(ptr_Window->ptr_renderer, NULL);
    if(!b_ret) return 10;

    // Destroy the window Texture (returns void)
    SDL_DestroyTexture(ptr_Window->ptr_window_texture);

    // Create a new window texture with the right size
    ptr_Window->ptr_window_texture = SDL_CreateTexture(ptr_Window->ptr_renderer, DEFAULT_PIXELFORMAT, SDL_TEXTUREACCESS_TARGET, ptr_Window->window_pixel_size.w * ptr_Window->upscaling_factor, ptr_Window->window_pixel_size.h * ptr_Window->upscaling_factor);
    if(!ptr_Window->ptr_window_texture) return 9;

    // Set the texture scaling mode to act anti alizing like
    b_ret = SDL_SetTextureScaleMode(ptr_Window->ptr_window_texture, SDL_SCALEMODE_LINEAR); // SDL_SCALEMODE_LINEAR
    if(!b_ret) return 10;

    // set the new texture as the Render Targed
    b_ret = SDL_SetRenderTarget(ptr_Window->ptr_renderer, ptr_Window->ptr_window_texture);
    if(!b_ret) return 10;

    // set time stamp for debounce timer
    ptr_Window->last_resize_time = SDL_GetTicks();
  }

  // Resizing Complet => Return sucssfull
  return 0;
}





// Function for updating the user scale
void UpdateWindowUserScale(struct AplicationWindow *ptr_Window, float new_scale){

  // Verify Input
  if(!ptr_Window || new_scale <= 0) return;

  // Update Scale values
  ptr_Window->usr_scale = new_scale;
  ptr_Window->ui_scale = ptr_Window->usr_scale * ptr_Window->display_content_scale;
}


// Function for safely Destroying a Application window (including its renderer)
// for a AplicationWindow struct allocated in heap memmory
void DestroyAplicationWindow_heap(struct AplicationWindow *ptr_Window){

  // Destroy the window Texture
  SDL_DestroyTexture(ptr_Window->ptr_window_texture);

  // Destroy the Renderer
  SDL_DestroyRenderer(ptr_Window->ptr_renderer);

  // Destroy the window
  SDL_DestroyWindow(ptr_Window->ptr_window);

  // free the AplicationWindow structure
  free(ptr_Window);
}


// Function for safely Destroying a Application window (including its renderer)
// for a AplicationWindow struct allocated in stack memmory or globaly defined
void DestroyAplicationWindow_stack(struct AplicationWindow *ptr_Window){

  // Destroy the window Texture
  SDL_DestroyTexture(ptr_Window->ptr_window_texture);

  // Destroy the Renderer
  SDL_DestroyRenderer(ptr_Window->ptr_renderer);

  // Destroy the window
  SDL_DestroyWindow(ptr_Window->ptr_window);
}




// END
