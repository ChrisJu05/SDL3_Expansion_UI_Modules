// Mini Labels Header
/*
Created by Christopher Jung 05.04.2026

This Header File Contains the Declorations for the Mini Labels expansion module.
It is intended to make the process of creating Labels for UI applicaqtions Using
SDL3 ttf easy and efficent.

Instructons for Use:

Lables are represented by the MiLb_Label struct.
The label struct contains the label texture and surface as well as the rendering rects.
The font (and the collors) is Referenced by the Label struct but is not part of the labels struct.
-> you must seperatly use TTF_OpenFont(...) to open a TTF font and add the font pointer to the struct at creation
    the same apllys to the collors

Label Lifecycle:

  Before Label creation the Collors (foreground and Background) are to be created and the font opended with TTF_OpenFont.

  The Lable Struct is created using the Create_Label function, also a first specification of the Label parameters.
  (The fontpointer and pointers to the collors are added)

  After Sucsesfull Creation the Label can be Renderd Emidiatly using the Render_Label function in your code.
  (this will render the the lable to the current Render targed)

  If you wish to change the Text of the label use the Update_Label_Text() function. this will automaticly
  update the label. (if other changes like font changes, these updates will be transferd to)

  If you applyed anny changes to the font or the lable size you can use the Update_Label() function to apply the changes
  (Note: it is not recomended to update the lable every frame as this means constant realocation)

  At the End of the Lable Life the Lable must be destroyed to free the allocated memmory. you can do this with
  the Destroy_Label() Function. Remember to close the font with TTF_CloseFont()

*/

// Header Guard
#ifndef MiLb_LabelS_HEADER_GUARD
#define MiLb_LabelS_HEADER_GUARD

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <SDL3_ttf/SDL_ttf.h>


// Label Render method enumeration
#define RENDERMETHOD_SOLID 0
#define RENDERMETHOD_SOLID_WRAPPED 1
#define RENDERMETHOD_SHADED 2
#define RENDERMETHOD_SHADED_WRAPPED 3
#define RENDERMETHOD_BLENDED 4
#define RENDERMETHOD_BLENDED_WRAPPED 5
#define RENDERMETHOD_LCD 6
#define RENDERMETHOD_LCD_WRAPPED 7


// Label Struct
struct MiLb_Label{

  // Lable Text
  char *text;

  // Label font
  TTF_Font *ptr_font;       // font styling is handled by the font
  float pt_size;

  // Size and Possition of the label
  // also specefys the maximum size of the labels visible area
  SDL_FRect output_rect;    // Specefys location and max size of label
  // Set width to 0 for new line at new line characters
  SDL_FRect texture_rect;   // the source rect for rendering
  SDL_FRect render_rect;    // the destination rect for rendering
  int render_method;

  // Label Collors
  SDL_Color *f_color;       // ForeGround Color
  SDL_Color *b_color;       // Background Color (you can set to transparent by making a = 0)

  // Label surface and Texture Pointer
  SDL_Surface *ptr_surface;
  SDL_Texture *ptr_texture;

};


// Function Declorations
char* MiLb_FormatString(const char* fmt, va_list args); // Function for Converting a argument list in to a string
char* MiLb_CreateString(const char *main_string, ...); // Function for Creating a String (like in a printf function)
struct MiLb_Label *MiLb_CreateLabel(SDL_Renderer *ptr_Renderer, TTF_Font *ptr_font, float x, float y, float w, float h, int rendermethod, SDL_Color *fc, SDL_Color *bc, char* text, ...); // Function for Creating a Text label
bool MiLb_RenderLabel(SDL_Renderer *ptr_Renderer, struct MiLb_Label* ptr_label); // Function for rendering the Label to the currrnet Render Targed
bool MiLb_UpdateLabel(SDL_Renderer *ptr_Renderer, struct MiLb_Label* ptr_label); // Function for Updating the lable if anny changes are applyed like diffrent text or size cahnges
bool MiLb_UpdateLabelText(SDL_Renderer *ptr_Renderer, struct MiLb_Label* ptr_label, char* New_Text, ...); // Function for Updating the Text of a Label.
void MiLb_DestroyLabel(struct MiLb_Label* ptr_label); // Function for safly Destroying a Label



#endif
