// Glass Header
/*
Created by Christopher Jung 04.04.2026

This Header File Contains the Declorations for the Glass expansion module.
With this module background blurs and transparent elements can be made easyly.

Requiures that the Application is set up so that the renderer renders to a
texture and then on to the screen, so that the texture can be read for creating
the blur effect.
Using the transparent overlay technique, an object of anny shape can be made to
have a blurd bacground texture.
When the blur effect is created there is also the option of adding a dye or
noise to the blur to improve the looks of the effect.

The Heder provides blur functions for simmple texture blurring operations as
well as dynamic blurring for realtime background bluring (of the window texture)
to create a glass like effect.

Dynamic Bluring, How it works:

First a Glass_BlurArea is defined that specefies the size and strength of the
blur area (for performance reasons, only a specefyed area is blured where Glass
UI elements are used), then each frame the blur area is updated by extracting a
section of the window texture, bluring it and the applying a dye of a defined
collor (and optionally noise).
Now a bluretexture is obtained for a section of the window to convert it to a
desiured shape a Transparednt overlay is used:
A texture where all pixels are set to invisible is created, the shpe is renderd
on to the texture in black with 255 alpha value (everything invisible eccept for
the shape). Finaly using the SDL_TextureOverlay_transparent function the
textures are overlayed to create a texture with the blurr in the shape of the
drawn shape on the base texture. For a more optimal effect the blur area should
be slightly larger than the shape.

*/

// Header Guard
#ifndef GLASS_HEADER_GUARD
#define GLASS_HEADER_GUARD

// Standerd Libuarys needed for the Project
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#define _USE_MATH_DEFINES
#include <time.h>

// Include SDL3 Libuarys
#include <SDL3/SDL.h>                   // the core SDL libuary
#include <SDL3/SDL_main.h>              // part of SDL

// Structure specefying a Blur Area
struct Glass_BlurArea
{

  // Rectangle defining blur area (NOT OWNED BY Glass_BlurArea)(should remain mostly constant in size)
  SDL_FRect *BlurRect;// NO DYNAMIC RESIZING!                                   // defines the location and size of the area of the window of witch a blured copy is created

  // Blure texture
  SDL_Texture *BlureTexture;                                                    // contains a blured copy of the section of the window specefyed by BlurRect

  // Blur parameters
  int BlurStrength;                                                             // The strength of the blur effect per blur cycle. (should not be changed from 2 or 4 (blurstrength per cycle) as strong blurstrengths can created distorted textures)
  int BlurCycles;                                                               // The number of blur cycles. determines how strong the blur effect is (Actual Blurstrength)(high blurvalues > 100 may be hardware intensive)
  Uint8 NoiseStrength;                                                          // The strength of the noise in the bluring process (0...255)
  Uint8 NoiseOpacety;                                                           // The Opacety of the noise when it is added to the process (0..255)
  Uint8 r,g,b,a;                                                                // overlay dye collor

  // Internal
  // Pregenerated noise texture
  SDL_Texture *NoiseTexture;                                                    // Randomly generated noise texture (for use in the bluring process)
};


// Function Declorations
SDL_Texture* Glass_CreateBluredTextureCopy(SDL_Renderer* renderer, SDL_Texture *origenal_texture, int blurstrength, int blurcycles);  // Function to Create a blured copy of a texture
bool Glass_BlureTexture(SDL_Renderer* renderer, SDL_Texture *origenal_texture, int blurstrength, int blurcycles); // Function to blur a texture directly (without creating a copy)
SDL_Texture* Glass_CopyTextureSection(SDL_Renderer* renderer, SDL_Texture* src_texture, SDL_FRect* section_rect); // Function for Creating a Copy of a section from a texture
bool Glass_CopyTextureSection_OnTo(SDL_Renderer* renderer, SDL_Texture* src_texture, SDL_FRect* section_rect, SDL_Texture* dest_texture, SDL_FRect* dest_rect); // Function for Copying a section of a texture on to a nother texture
SDL_Texture *Glass_CreateTransparenBaseTexture(SDL_Renderer* renderer, int w, int h); // Function for Creating a empty transparent base texture (for transparency overlay)
bool Glass_TransparentTextureOverlay(SDL_Renderer* renderer, SDL_Texture* src_texture, SDL_FRect* section_rect, SDL_Texture* dest_texture, SDL_FRect* base_FRect); // Function for Creating a transparent overlay effect (fill a shape in with a texture)
SDL_Texture* Glass_CreateWhiteNoiseTexture(SDL_Renderer* renderer, SDL_FRect* rect, Uint8 strength, Uint8 opacety); // Function for generating a Whitenoise texture
struct Glass_BlurArea* Glass_CreateBlurArea(SDL_Renderer* renderer, SDL_FRect *BlurRect, int BlurCycles, Uint8 NoiseStrength, Uint8 NoiseOpacety); // Function for creating a Blur Area with mostly default parameters (for UI glass aplications)
void Glass_DestroyBlurArea(struct Glass_BlurArea *ptr_BlurArea); // Function for safely destroying a Glass_BlurArea
bool SDL_UpdateBlurArea(SDL_Renderer* renderer, struct Glass_BlurArea* ptr_BlurArea); // Function for Updating a Glass_BlurArea for a new frame (should be called every draw cycle)



#endif
