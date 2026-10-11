// Glass module Code file
/*
Created by Christopher Jung 04.04.2026

This File Contains the Definitions for the Glass expansion module.
It is intended to make the implementation of blured UI glass like UI objects
easy and efficent.
*/

#include "glass.h"


// Function to Create a blured copy of a texture
/*
renderer: the render context used with the subject textures.
origenal_texture: the texture to be blured.
blurstrength: the strength of the blur effect per blurcycle (recommended value: 2).
blurcycles: How manny bluring cycles should be executed determining the final blurstrength. (performing manny blur cycles may be process intensive)
RETURNS a pointer to a newly created blured copy of the origenal_texture, or NULL if an error occurs.
*/
SDL_Texture* Glass_CreateBluredTextureCopy(SDL_Renderer* renderer, SDL_Texture *origenal_texture, int blurstrength, int blurcycles){

  // Validate input
  if(!renderer || !origenal_texture) return NULL;

  // Error checking
  bool b_ret;

  // Save default render targed
  SDL_Texture* default_render_target = SDL_GetRenderTarget(renderer);

  // Create the rectangles needed for transfer
  SDL_FRect full_size_rect;
  full_size_rect.w = origenal_texture->w;
  full_size_rect.h = origenal_texture->h;

  SDL_FRect half_size_rect;
  half_size_rect.w = (float)(origenal_texture->w / 2);
  half_size_rect.h = (float)(origenal_texture->h / 2);

  // Create a copyed texture of the same size
  SDL_Texture *Texture_Blured = SDL_CreateTexture(renderer, origenal_texture->format, SDL_TEXTUREACCESS_TARGET, origenal_texture->w, origenal_texture->h);
  if(!Texture_Blured) return NULL;

  // Set texture sclae mode to linear (so that resizing blurs the texture)
  b_ret = SDL_SetTextureScaleMode(Texture_Blured, SDL_SCALEMODE_LINEAR);
  if(!b_ret){ SDL_DestroyTexture(Texture_Blured); return NULL;}

  // Copy origenal texture in to Blured Texture
  b_ret = SDL_SetRenderTarget(renderer, Texture_Blured);
  if(!b_ret){ SDL_DestroyTexture(Texture_Blured); return NULL;}

  b_ret = SDL_RenderTexture(renderer, origenal_texture, &full_size_rect, &full_size_rect);
  if(!b_ret){ SDL_DestroyTexture(Texture_Blured); return NULL;}


  // Create a texture half the size of the origenal texture
  SDL_Texture *texture_downscaled = SDL_CreateTexture(renderer, origenal_texture->format, SDL_TEXTUREACCESS_TARGET, origenal_texture->w / blurstrength, origenal_texture->h / blurstrength);
  if(!texture_downscaled){ SDL_DestroyTexture(Texture_Blured); return NULL;}

  // Set sclae mode of downscaled texture to linear
  b_ret = SDL_SetTextureScaleMode(texture_downscaled, SDL_SCALEMODE_LINEAR);
  if(!b_ret){ SDL_DestroyTexture(Texture_Blured); SDL_DestroyTexture(texture_downscaled); return NULL;}

  // For every Blur cycle (the texture gets blurryer)
  for(int i = 0; i < blurcycles; i++){

    // Set render targed to downscaled texture
    b_ret = SDL_SetRenderTarget(renderer, texture_downscaled);
    if(!b_ret){ SDL_DestroyTexture(Texture_Blured); SDL_DestroyTexture(texture_downscaled); return NULL;}

    // Render the Large texture in to the small texture, downscaling it
    b_ret = SDL_RenderTexture(renderer, Texture_Blured, &full_size_rect, &half_size_rect);
    if(!b_ret){ SDL_DestroyTexture(Texture_Blured); SDL_DestroyTexture(texture_downscaled); return NULL;}

    // Set reder tared to the Large textures
    b_ret = SDL_SetRenderTarget(renderer, Texture_Blured);
    if(!b_ret){ SDL_DestroyTexture(Texture_Blured); SDL_DestroyTexture(texture_downscaled); return NULL;}

    // Render the Small texture in to the Large texture, Upcaling it
    b_ret = SDL_RenderTexture(renderer, texture_downscaled, &half_size_rect, &full_size_rect);
    if(!b_ret){ SDL_DestroyTexture(Texture_Blured); SDL_DestroyTexture(texture_downscaled); return NULL;}
  }

  // Reset render targed to window
  SDL_SetRenderTarget(renderer, default_render_target);

  // Destroy downscaled texture (it is not needed)
  SDL_DestroyTexture(texture_downscaled);

  return Texture_Blured;
}







// Function to blur a texture directly (without creating a copy)
/*
renderer: the render context used with the subject textures.
origenal_texture: the texture to be blured.
blurstrength: the strength of the blur effect per blurcycle (recommended value: 2).
blurcycles: How manny bluring cycles should be executed determining the final blurstrength. (performing manny blur cycles may be process intensive)
RETURNS true on sucsesfull bluring and false on failure
Note: Make shore texture acces is targed so the texture can be renderd to.
Note: to increse execution speed error checking was reduced in comparison to Glass_CreateBluredTextureCopy function.
*/
bool Glass_BlureTexture(SDL_Renderer* renderer, SDL_Texture *origenal_texture, int blurstrength, int blurcycles){

  // Validate Input
  if(!renderer || !origenal_texture) return false;
  if(blurcycles <= 0) return true;

  // Save default render targed
  SDL_Texture* default_render_target = SDL_GetRenderTarget(renderer);

  // Save the textures current scale mode so it can be reset later
  SDL_ScaleMode saved_scale_mode;
  SDL_GetTextureScaleMode(origenal_texture, &saved_scale_mode);

  // Set sclae mode to linear
  SDL_SetTextureScaleMode(origenal_texture, SDL_SCALEMODE_LINEAR);

  // Create a texture half the size of the origenal
  SDL_Texture *texture_downscaled = SDL_CreateTexture(renderer, origenal_texture->format, SDL_TEXTUREACCESS_TARGET, origenal_texture->w / blurstrength, origenal_texture->h / blurstrength);
  if(!texture_downscaled) return false;

  // Set sclae mode of texture_downscaled to linear
  SDL_SetTextureScaleMode(texture_downscaled, SDL_SCALEMODE_LINEAR);

  // Set up Blendmodes for bluring
  SDL_BlendMode saved_blendmode;
  SDL_GetTextureBlendMode(origenal_texture, &saved_blendmode);
  SDL_SetTextureBlendMode(origenal_texture, SDL_BLENDMODE_NONE);
  SDL_SetTextureBlendMode(texture_downscaled, SDL_BLENDMODE_NONE);


  // Set render targed to downscaled texture
  SDL_SetRenderTarget(renderer, texture_downscaled);

  // Render the Large texture in to the small texture (downscaling it)
  SDL_RenderTexture(renderer, origenal_texture, NULL, NULL);

  // For every Blur cycle (the texture gets blurryer)
  for(int i = 0; i < blurcycles - 1; i++){

    // Set reder tared to the Large textures
    SDL_SetRenderTarget(renderer, origenal_texture);
    // Render the Small texture in to the Large texture, Upcaling it
    SDL_RenderTexture(renderer, texture_downscaled, NULL, NULL);

    // Set render targed to downscaled texture
    SDL_SetRenderTarget(renderer, texture_downscaled);
    // Render cthe Large texture in to the small texture, downscaling it
    SDL_RenderTexture(renderer, origenal_texture, NULL, NULL);

  }

  // Set reder tared to the Large textures
  SDL_SetRenderTarget(renderer, origenal_texture);
  // Render the Small texture in to the Large texture (Upcaling it)
  SDL_RenderTexture(renderer, texture_downscaled, NULL, NULL);

  // Reset render targed to default_render_target
  SDL_SetRenderTarget(renderer, default_render_target);

  // Destroy downscaled texture (it is not needed)
  SDL_DestroyTexture(texture_downscaled);

  // Reset Blend mode and scale mode
  SDL_SetTextureBlendMode(origenal_texture, saved_blendmode);
  SDL_SetTextureScaleMode(origenal_texture, saved_scale_mode);

  return true;
}




// Function for Creating a Copy of a section from a texture
/*
renderer: the render context used with the subject textures.
src_texture: the source texture from witch to copy a section.
section_rect: a Rectangle defining the section to be copyed (my be NULL for whole texture).
RETURNS a pointer to a newly created texture (containing a copy of the source texture section) or NULL on failure.
*/
SDL_Texture* Glass_CopyTextureSection(SDL_Renderer* renderer, SDL_Texture* src_texture, SDL_FRect* section_rect){

  // Validate input
  if(!renderer || !src_texture) return NULL;

  // Save default render targed
  SDL_Texture* default_render_target = SDL_GetRenderTarget(renderer);

  // Create a Texture the size of the section
  SDL_Texture *dest_texture;

  // Two cases: copy whole texture or just a section
  if(section_rect == NULL){

    // Create Texture of requested size
    dest_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, src_texture->w, src_texture->h);
    if(!dest_texture) return NULL;

    // Set render targed
    SDL_SetRenderTarget(renderer, dest_texture);

    // Render to destination
    SDL_RenderTexture(renderer, src_texture, section_rect, NULL);
    //SDL_RenderTexture(renderer, src_texture, section_rect, section_rect);

  }else{

    // Create offset destination rect
    SDL_FRect render_FRect;
    render_FRect.w = section_rect->w;
    render_FRect.h = section_rect->h;
    render_FRect.x = 0;
    render_FRect.y = 0;

    // Create Texture of requested size
    dest_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, render_FRect.w, render_FRect.h);
    if(!dest_texture) return NULL;

    // Set render targed
    SDL_SetRenderTarget(renderer, dest_texture);

    // Render to destination
    SDL_RenderTexture(renderer, src_texture, section_rect, &render_FRect);
    //SDL_RenderTexture(renderer, src_texture, section_rect, section_rect);
  }

  // ReSet render targed
  SDL_SetRenderTarget(renderer, default_render_target);

  // Return Texture
  return dest_texture;
}





// Function for Copying a section of a texture on to a nother texture
/*
renderer: the render context used with the subject textures.
src_texture: the source texture to copy from.
section_rect: a pointer to a SDL_FRect defining the section (NULL for whole texture).
dest_texture: the destination texture to copy the section on to.
dest_rect: a pointer to a SDL_FRect defining the section on the destination texture where the copyed section is to be added (NULL for whole texture).
RETURNS true on sucsess and false on failure.
*/
bool Glass_CopyTextureSection_OnTo(SDL_Renderer* renderer, SDL_Texture* src_texture, SDL_FRect* section_rect, SDL_Texture* dest_texture, SDL_FRect* dest_rect){

  // Validate Input
  if(!renderer || !src_texture || !dest_texture) return false;

  // Save default render targed
  SDL_Texture* default_render_target = SDL_GetRenderTarget(renderer);
  if(!default_render_target) return false;

  // Save default blend mode
  SDL_BlendMode def_blendmode;
  SDL_GetTextureBlendMode(src_texture, &def_blendmode);

  // Set Blendmode to none
  SDL_SetTextureBlendMode(src_texture, SDL_BLENDMODE_NONE);

  // Render on to destination
  SDL_SetRenderTarget(renderer, dest_texture);
  SDL_RenderTexture(renderer, src_texture, section_rect, dest_rect);

  // Reset Blend mode and Render targed
  SDL_SetTextureBlendMode(src_texture, def_blendmode);
  SDL_SetRenderTarget(renderer, default_render_target);

  return true;
}



// Function for Creating a empty transparent base texture (for transparency overlay)
/*
renderer: the render context used with the subject textures.
w: the width of the new texture.
h: the hight of the new texture.
RETURNS a pointer to a newly created texture where all pixels are set to transparent or NULL on failure.
*/
SDL_Texture *Glass_CreateTransparenBaseTexture(SDL_Renderer* renderer, int w, int h){

  // Validate function input
  if(!renderer || w <= 0 || h <= 0) return NULL;

  // Save default render targed
  SDL_Texture* default_render_target = SDL_GetRenderTarget(renderer);

  // Create a new texture with the requested size
  SDL_Texture *Created_Texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h);
  if(!Created_Texture) return NULL;

  // Fill with all 0 opacety and collor
  SDL_SetRenderTarget(renderer, Created_Texture);
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
  SDL_RenderFillRect(renderer, NULL);

  // ReSet render targed
  SDL_SetRenderTarget(renderer, default_render_target);

  // Return created texture
  return Created_Texture;
}







// Function for Creating a transparent overlay effect (fill a shape in with a texture)
/*
input to the function is a texture in RGBA foramt (the opacety is importatn) and
the transparent background texture (optionaly preblured if a blur is desiured).
The function will overlay the background on the visible pixels with this
function complex features like TTF writing in blured collor is possible.

additive blending: dstRGB = (srcRGB * srcA) + dstRGB, dstA = dstA

renderer: the render context used with the subject textures.
src_texture: (typicaly the collor dictating texture) the source texture to be renderd on to the (semi)transparent texture.
dest_texture: (opacety dictating texture) the texture where the collor is layerd on to.
RETURNS true on succses and false on fail.
*/
bool Glass_TransparentTextureOverlay(SDL_Renderer* renderer, SDL_Texture* src_texture, SDL_FRect* section_rect, SDL_Texture* dest_texture, SDL_FRect* base_FRect){

  // Validate Input
  if(!renderer || !src_texture || !dest_texture) return false;

  // Save default render targed
  SDL_Texture* default_render_target = SDL_GetRenderTarget(renderer);
  //if(!default_render_target) return false;

  // Save default blend mode
  SDL_BlendMode def_blendmode;
  SDL_GetTextureBlendMode(src_texture, &def_blendmode);

  // set source textures blendmode to additive
  SDL_SetTextureBlendMode(src_texture, SDL_BLENDMODE_ADD);

  // Render on to destination
  SDL_SetRenderTarget(renderer, dest_texture);
  SDL_RenderTexture(renderer, src_texture, section_rect, base_FRect);

  // Reset Blend mode
  SDL_SetTextureBlendMode(src_texture, def_blendmode);

  // ReSet render targed
  SDL_SetRenderTarget(renderer, default_render_target);

  return true;
}





// Function for generating a Whitenoise texture
/*
renderer: the render context used with the subject textures.
rect: a rectangle specefying the size of the texture.
strength: the strength of the noise.
opacety: specefys the opacety of the noise texture.
Note: creating a noise terxture is CPU intensive and should not (and does not need to be) generated at real time
RETURNS a pointer to a newly generated noise texture or NULL on failure.
*/
SDL_Texture* Glass_CreateWhiteNoiseTexture(SDL_Renderer* renderer, SDL_FRect* rect, Uint8 strength, Uint8 opacety){

  // Validate input
  if(!renderer || !rect || rect->w <= 0 || rect->h <= 0) return NULL;

  // reference values
  int width = (int)rect->w;
  int hight = (int)rect->h;
  Uint8 value;

  // Allocate pixel buffer (RGBA8888)
  Uint32 *pixels = (Uint32*)malloc(width * hight * sizeof(Uint32));
  if(!pixels) return NULL;

  //Random seed
  srand((unsigned int)time(NULL));

  // Allocate fomat details
  const SDL_PixelFormatDetails *RGBA8888_format_details = SDL_GetPixelFormatDetails(SDL_PIXELFORMAT_RGBA8888);
  // Comes from shared cash therfore does not need to be freed

  // Fill with random noise
  for(int y = 0; y < hight; y++){
    for(int x = 0; x < width; x++){

      // Generate random 8 bit value
      value = (Uint8)(rand() % (strength + 1)); // noise strength

      // fill in (white noise style RGB all get same value, opacety is filled in)
      pixels[(y * width) + x] = SDL_MapRGBA(RGBA8888_format_details, NULL, value, value, value, opacety);

    }
  }

  // Create SDL3 surface
  SDL_Surface *surface = SDL_CreateSurfaceFrom(width, hight, SDL_PIXELFORMAT_RGBA8888, pixels, width * sizeof(Uint32));
  if(!surface){
    free(pixels);
    return NULL;
  }

  // Create Texture from surface
  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
  if(!texture){ SDL_DestroySurface(surface); return NULL;}

  // Clean up
  SDL_DestroySurface(surface);
  free(pixels);

  // Return the generated Noise texture
  return texture;
}







// Function for creating a Blur Area with mostly default parameters (for UI glass aplications)
/*
with a Glass_BlurArea realtime bluring can be done easyly.
renderer: the render context used with the subject textures.
BlurRect: a rectangle specefying the blur area.
BlurCycles: the number of blur cycles to be done, determining the blur strength.
NoiseStrength: the strength of the noise on the blur area.
NoiseOpacety: the opacety of the noise when it is layerd on to the blur area.
RETURNS a Glass_BlurArea struct pointer or NULL on failure.
*/
struct Glass_BlurArea* Glass_CreateBlurArea(SDL_Renderer* renderer, SDL_FRect *BlurRect, int BlurCycles, Uint8 NoiseStrength, Uint8 NoiseOpacety){

  // Validate Input
  if(!renderer) return NULL;

  // start by allocating structure
  struct Glass_BlurArea *ptr_BlurArea = (struct Glass_BlurArea*)malloc(sizeof(struct Glass_BlurArea));
  if(!ptr_BlurArea) return NULL;

  // fill in values
  ptr_BlurArea->BlurRect = BlurRect;
  ptr_BlurArea->BlurStrength = 2;
  ptr_BlurArea->BlurCycles = BlurCycles;
  ptr_BlurArea->NoiseStrength = NoiseStrength;
  ptr_BlurArea->NoiseOpacety = NoiseOpacety;
  ptr_BlurArea->r = 0;
  ptr_BlurArea->g = 0;
  ptr_BlurArea->b = 0;
  ptr_BlurArea->a = 0;
  ptr_BlurArea->NoiseTexture = NULL; // Initialize pointer
  ptr_BlurArea->BlureTexture = NULL; // Initialize pointer


  // Pre generate Noise Texture
  ptr_BlurArea->NoiseTexture = Glass_CreateWhiteNoiseTexture(renderer, BlurRect, NoiseStrength, NoiseOpacety);
  if(!ptr_BlurArea->NoiseTexture){ free(ptr_BlurArea); return NULL;}
  SDL_SetTextureBlendMode(ptr_BlurArea->NoiseTexture, SDL_BLENDMODE_BLEND);
  //SDL_SetTextureScaleMode(ptr_BlurArea->NoiseTexture, SDL_ScaleMode scaleMode); // default scale mode is linear

  // Create Blur Texture holding blured background
  ptr_BlurArea->BlureTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, (int)(BlurRect->w), (int)(BlurRect->h));
  if(!ptr_BlurArea->BlureTexture){ SDL_DestroyTexture(ptr_BlurArea->NoiseTexture); free(ptr_BlurArea); return NULL;}
  //SDL_SetTextureBlendMode(ptr_BlurArea->BlureTexture, SDL_BLENDMODE_ADD);

  // Return the created Blur area
  return ptr_BlurArea;
}





// Function for safely destroying a Glass_BlurArea
// Note: BlurRect is not freed by this function as it is considered external
void Glass_DestroyBlurArea(struct Glass_BlurArea *ptr_BlurArea){
  if(!ptr_BlurArea) return;
  SDL_DestroyTexture(ptr_BlurArea->NoiseTexture);
  SDL_DestroyTexture(ptr_BlurArea->BlureTexture);
  free(ptr_BlurArea);
}




// Function for Updating a Glass_BlurArea for a new frame (should be called every draw cycle)
/*
must be called after the to be background to be blured is rendered
requiures that the render targed is the blur background (MUST BE A TEXTURE)

renderer: the render context used with the subject textures.
ptr_BlurArea: the Glass_BlurArea to be updated.
RETURNS true on sucsess and false on failure.
*/
bool SDL_UpdateBlurArea(SDL_Renderer* renderer, struct Glass_BlurArea* ptr_BlurArea){

  // Validate Input
  if(!renderer || !ptr_BlurArea) return false;

  // if BlurRect has changed size the BlureTexture and NoiseTexture must be resized (this process is performance intensive)
  if((int)(ptr_BlurArea->BlurRect->w) != (ptr_BlurArea->BlureTexture->w) || (int)(ptr_BlurArea->BlurRect->h) != (ptr_BlurArea->BlureTexture->h)){

    // Destroy the current BlureTexture
    SDL_DestroyTexture(ptr_BlurArea->BlureTexture);

    // Create a new Blur texture with new size
    ptr_BlurArea->BlureTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, (int)(ptr_BlurArea->BlurRect->w), (int)(ptr_BlurArea->BlurRect->h));
    if(!ptr_BlurArea->BlureTexture) return false;

    // Destroy current noise texture
    SDL_DestroyTexture(ptr_BlurArea->NoiseTexture);

    // Create new Noise Texture
    ptr_BlurArea->NoiseTexture = Glass_CreateWhiteNoiseTexture(renderer, ptr_BlurArea->BlurRect, ptr_BlurArea->NoiseStrength, ptr_BlurArea->NoiseOpacety);
    if(!ptr_BlurArea->NoiseTexture) return false;
  }

  // Save default render targed
  SDL_Texture* default_render_target = SDL_GetRenderTarget(renderer);
  if(!default_render_target) return false; // this is neccecary as NULL is the window and the system cant copy sections from the window.

  // Copy section from screen on to the BlureTexture
  Glass_CopyTextureSection_OnTo(renderer, default_render_target, ptr_BlurArea->BlurRect, ptr_BlurArea->BlureTexture, NULL);

  // Render Noise Texture on to BlureTexture
  SDL_SetRenderTarget(renderer, ptr_BlurArea->BlureTexture);
  SDL_RenderTexture(renderer, ptr_BlurArea->NoiseTexture, NULL, NULL);

  // Blur the Blurtexture
  Glass_BlureTexture(renderer, ptr_BlurArea->BlureTexture, ptr_BlurArea->BlurStrength, ptr_BlurArea->BlurCycles);

  // Dye overlay
  SDL_SetRenderDrawColor(renderer, ptr_BlurArea->r, ptr_BlurArea->g, ptr_BlurArea->b, ptr_BlurArea->a);
  SDL_RenderFillRect(renderer, NULL);

  // Reset render targed
  SDL_SetRenderTarget(renderer, default_render_target);

  // Return sucsess
  return true;
}
