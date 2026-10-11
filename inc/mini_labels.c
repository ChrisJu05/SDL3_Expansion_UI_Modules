// Mini Labels module Code file
/*
Created by Christopher Jung 05.04.2026

This File Contains the Definitions for the Mini Labels expansion module.
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

  The Lable Struct is created using the MiLb_CreateLabel function, also a first specification of the Label parameters.
  (The fontpointer and pointers to the collors are added)

  After Sucsesfull Creation the Label can be Renderd Emidiatly using the MiLb_RenderLabel function in your code.
  (this will render the the lable to the current Render targed)

  If you wish to change the Text of the label use the MiLb_UpdateLabelText() function. this will automaticly
  update the label. (if other changes like font changes, these updates will be transferd to)

  If you applyed anny changes to the font or the lable size you can use the MiLb_UpdateLabel() function to apply the changes
  (Note: it is not recomended to update the lable every frame as this means constant realocation)

  At the End of the Lable Life the Lable must be destroyed to free the allocated memmory. you can do this with
  the MiLb_DestroyLabel() Function. Remember to close the font with TTF_CloseFont()

*/


// Header for working with the Module
#include "mini_labels.h"



// Function for Converting a argument list in to a string
// Returns a NULL terminated string uppon sucsess and NULL on Error
char* MiLb_FormatString(const char* fmt, va_list args){

  // Vlaidate input
  if(!fmt) return NULL;

  // Create a copy of the argument list
  va_list args_copy;
  va_copy(args_copy, args);

  // First, determine the required buffer size
  int size = vsnprintf(NULL, 0, fmt, args_copy);
  va_end(args_copy);

  if (size < 0) return NULL; // Error occurred

  // Allocate buffer (+1 for null terminator)
  char* buffer = malloc(size + 1);
  if (!buffer) return NULL; // Allocation failed

  // Format the string
  vsnprintf(buffer, size + 1, fmt, args);

  return buffer; // Caller must free this
}




// Function for Creating a String (like in a printf function)
// Note: resulting string is a new allocation and must be freed
char* MiLb_CreateString(const char *main_string, ...){

  if(!main_string) return NULL;

  // Initialize a va_list to hold the arguments
  va_list args;

  // Start processing the variable arguments
  va_start(args, main_string);

  // Create string
  char* result = MiLb_FormatString(main_string, args);

  // Clean up after the va_list
  va_end(args);

  // Return resulting string
  return result;
}


// Function for Creating a Text label
// Text functions like printf function and is copyed in to Label struct
// Text should be NULL Terminated (it will be copyed over to the )
// Set width to 0 for new line at new line characters

// Function for Creating a Text label
/*
ptr_Renderer: the render context used with the subject textures.
ptr_font: a pointer to the font to be used for the label.
x,y: location of the label rect.
w,h: the width and the hight of the label.
rendermethod: the method used for rendering the text (default recomendation: RENDERMETHOD_BLENDED)
fc,bc: pointers to the foreground and background collor
text: text string like in a printf function (a new string will be formated for the label structure).
*/
struct MiLb_Label *MiLb_CreateLabel(SDL_Renderer *ptr_Renderer, TTF_Font *ptr_font, float x, float y, float w, float h, int rendermethod, SDL_Color *fc, SDL_Color *bc, char* text, ...){

  // Validate input
  if(!ptr_font) return NULL;
  if(!fc || !bc) return NULL;

  // Create the Label structure
  struct MiLb_Label *ptr_label = (struct MiLb_Label*)calloc(1, sizeof(struct MiLb_Label));
  if(!ptr_label) return NULL;

  // fill in general stuf
  ptr_label->output_rect.x = x;
  ptr_label->output_rect.y = y;
  ptr_label->output_rect.w = w;
  ptr_label->output_rect.h = h;

  ptr_label->f_color = fc;
  ptr_label->b_color = bc;

  ptr_label->ptr_font = ptr_font;

  ptr_label->render_method = rendermethod;

  ptr_label->pt_size = TTF_GetFontSize(ptr_font);

  // only create Text if there is Text
  if(text){
    // Create the Text string
    va_list args; // initialize a va list
    va_start(args, text); // Start processing variable arguments

    // Create text string (NULL terminated)
    char* final_text = MiLb_FormatString(text, args);

    // Give final text to struct
    ptr_label->text = final_text;

    // Clean up after the va list
    va_end(args);

    // Switch case for the diffrent render methods
    switch (rendermethod){
      case 0:

        ptr_label->ptr_surface = TTF_RenderText_Solid(ptr_font, final_text, 0, *fc);
        if(!ptr_label->ptr_surface) {free(ptr_label); return NULL;}
        break;

      case 1:

        ptr_label->ptr_surface = TTF_RenderText_Solid_Wrapped(ptr_font, final_text, 0, *fc, (int)w);
        if(!ptr_label->ptr_surface) {free(ptr_label); return NULL;}
        break;

      case 2:

        ptr_label->ptr_surface = TTF_RenderText_Shaded(ptr_font, final_text, 0, *fc, *bc);
        if(!ptr_label->ptr_surface) {free(ptr_label); return NULL;}
        break;

      case 3:

        ptr_label->ptr_surface = TTF_RenderText_Shaded_Wrapped(ptr_font, final_text, 0, *fc, *bc, (int)w);
        if(!ptr_label->ptr_surface) {free(ptr_label); return NULL;}
        break;

      case 4:

        ptr_label->ptr_surface = TTF_RenderText_Blended(ptr_font, final_text, 0, *fc);
        if(!ptr_label->ptr_surface) {free(ptr_label); return NULL;}
        break;

      case 5:

        ptr_label->ptr_surface = TTF_RenderText_Blended_Wrapped(ptr_font, final_text, 0, *fc, (int)w);
        if(!ptr_label->ptr_surface) {free(ptr_label); return NULL;}
        break;

      case 6:

        ptr_label->ptr_surface = TTF_RenderText_LCD(ptr_font, final_text, 0, *fc, *bc);
        if(!ptr_label->ptr_surface) {free(ptr_label); return NULL;}
        break;

      case 7:

        ptr_label->ptr_surface = TTF_RenderText_LCD_Wrapped(ptr_font, final_text, 0, *fc, *bc, (int)w);
        if(!ptr_label->ptr_surface) {free(ptr_label); return NULL;}
        break;

      default:

        // case 0
        ptr_label->ptr_surface = TTF_RenderText_Solid(ptr_font, final_text, 0, *fc);
        if(!ptr_label->ptr_surface) {free(ptr_label); return NULL;}
        break;

    }// end of switch

    // Create Texture from surface
    ptr_label->ptr_texture = SDL_CreateTextureFromSurface(ptr_Renderer, ptr_label->ptr_surface);
    if(!ptr_label->ptr_surface){
      SDL_DestroySurface(ptr_label->ptr_surface);
      free(ptr_label);
      return NULL;
    }

    ptr_label->texture_rect.x = 0.0f;
    ptr_label->texture_rect.y = 0.0f;

    ptr_label->render_rect.x = x;
    ptr_label->render_rect.y = y;

    if(ptr_label->ptr_texture->w > ptr_label->output_rect.w){
      ptr_label->texture_rect.w = ptr_label->output_rect.w;
      ptr_label->render_rect.w = ptr_label->output_rect.w;
    }else{
      ptr_label->texture_rect.w = (float)(ptr_label->ptr_texture->w);
      ptr_label->render_rect.w = (float)(ptr_label->ptr_texture->w);
    }

    if(ptr_label->ptr_texture->h > ptr_label->output_rect.h){
      ptr_label->texture_rect.h = ptr_label->output_rect.h;
      ptr_label->render_rect.h = ptr_label->output_rect.h;
    }else{
      ptr_label->texture_rect.h = (float)(ptr_label->ptr_texture->h);
      ptr_label->render_rect.h = (float)(ptr_label->ptr_texture->h);
    }

    // Return the pointer to label struct
    return ptr_label;

  }else{
    // Create empty surface and empty texture to return
    ptr_label->ptr_surface = SDL_CreateSurface((int)w, (int)h, SDL_PIXELFORMAT_RGBA8888);
    if(!ptr_label->ptr_surface) {free(ptr_label); return NULL;}

    ptr_label->ptr_texture = SDL_CreateTextureFromSurface(ptr_Renderer, ptr_label->ptr_surface);
    if(!ptr_label->ptr_texture) {SDL_DestroySurface(ptr_label->ptr_surface); free(ptr_label); return NULL;}

    // Surface background must be background collor
    // Get current render targed
    SDL_Texture *ptr_render_targed = SDL_GetRenderTarget(ptr_Renderer);
    // Set Texture as render targed
    if(!SDL_SetRenderTarget(ptr_Renderer, ptr_label->ptr_texture)){
      SDL_DestroySurface(ptr_label->ptr_surface);
      SDL_DestroyTexture(ptr_label->ptr_texture);
      free(ptr_label);
      return NULL;
    }
    // Set draw colllor to background color
    if(!SDL_SetRenderDrawColor(ptr_Renderer, bc->r, bc->g, bc->b, bc->a)){
      SDL_SetRenderTarget(ptr_Renderer, ptr_render_targed);
      SDL_DestroySurface(ptr_label->ptr_surface);
      SDL_DestroyTexture(ptr_label->ptr_texture);
      free(ptr_label);
      return NULL;
    }
    // Fill in texture with background collor
    if(!SDL_RenderRect(ptr_Renderer, NULL)){
      SDL_SetRenderTarget(ptr_Renderer, ptr_render_targed);
      SDL_DestroySurface(ptr_label->ptr_surface);
      SDL_DestroyTexture(ptr_label->ptr_texture);
      free(ptr_label);
      return NULL;
    }
    // Reset Render targed
    if(!SDL_SetRenderTarget(ptr_Renderer, ptr_render_targed)){
      SDL_SetRenderTarget(ptr_Renderer, ptr_render_targed);
      SDL_DestroySurface(ptr_label->ptr_surface);
      SDL_DestroyTexture(ptr_label->ptr_texture);
      free(ptr_label);
      return NULL;
    }
    return ptr_label;
  }

  return ptr_label;
}


// Function for rendering the Label to the currrnet Render Targed
// RETURNS true on sucsess and false on failure.
bool MiLb_RenderLabel(SDL_Renderer *ptr_Renderer, struct MiLb_Label* ptr_label){

  if(!ptr_label || !ptr_Renderer) return false;

  return SDL_RenderTexture(ptr_Renderer, ptr_label->ptr_texture, &(ptr_label->texture_rect), &(ptr_label->render_rect));

}


// Function for Updating the lable if anny changes are applyed like diffrent text or size cahnges
// RETURNS true on succsess and false on failure.
bool MiLb_UpdateLabel(SDL_Renderer *ptr_Renderer, struct MiLb_Label* ptr_label){

  // Validate input
  if(!ptr_label || !ptr_Renderer) return false;

  // First destroy the old texture and surface
  SDL_DestroySurface(ptr_label->ptr_surface);
  SDL_DestroyTexture(ptr_label->ptr_texture);

  // Recreate Surface and Textue
  if(ptr_label->text){

    // Switch case for the diffrent render methods
    switch (ptr_label->render_method){
      case 0:

        ptr_label->ptr_surface = TTF_RenderText_Solid(ptr_label->ptr_font, ptr_label->text, 0, *(ptr_label->f_color));
        if(!ptr_label->ptr_surface) return false;
        break;

      case 1:

        ptr_label->ptr_surface = TTF_RenderText_Solid_Wrapped(ptr_label->ptr_font, ptr_label->text, 0, *(ptr_label->f_color), (int)(ptr_label->output_rect.w));
        if(!ptr_label->ptr_surface) return false;
        break;

      case 2:

        ptr_label->ptr_surface = TTF_RenderText_Shaded(ptr_label->ptr_font, ptr_label->text, 0, *(ptr_label->f_color), *(ptr_label->b_color));
        if(!ptr_label->ptr_surface) return false;
        break;

      case 3:

        ptr_label->ptr_surface = TTF_RenderText_Shaded_Wrapped(ptr_label->ptr_font, ptr_label->text, 0, *(ptr_label->f_color), *(ptr_label->b_color), (int)(ptr_label->output_rect.w));
        if(!ptr_label->ptr_surface) return false;
        break;

      case 4:

        ptr_label->ptr_surface = TTF_RenderText_Blended(ptr_label->ptr_font, ptr_label->text, 0, *(ptr_label->f_color));
        if(!ptr_label->ptr_surface) return false;
        break;

      case 5:

        ptr_label->ptr_surface = TTF_RenderText_Blended_Wrapped(ptr_label->ptr_font, ptr_label->text, 0, *(ptr_label->f_color), (int)(ptr_label->output_rect.w));
        if(!ptr_label->ptr_surface) return false;
        break;

      case 6:

        ptr_label->ptr_surface = TTF_RenderText_LCD(ptr_label->ptr_font, ptr_label->text, 0, *(ptr_label->f_color), *(ptr_label->b_color));
        if(!ptr_label->ptr_surface) return false;
        break;

      case 7:

        ptr_label->ptr_surface = TTF_RenderText_LCD_Wrapped(ptr_label->ptr_font, ptr_label->text, 0, *(ptr_label->f_color), *(ptr_label->b_color), (int)(ptr_label->output_rect.w));
        if(!ptr_label->ptr_surface) return false;
        break;

      default:

        // case 0
        ptr_label->ptr_surface = TTF_RenderText_Solid(ptr_label->ptr_font, ptr_label->text, 0, *(ptr_label->f_color));
        if(!ptr_label->ptr_surface) return false;
        break;

    }// end of switch


    // Create Texture from surface
    ptr_label->ptr_texture = SDL_CreateTextureFromSurface(ptr_Renderer, ptr_label->ptr_surface);
    if(!ptr_label->ptr_surface){
      SDL_DestroySurface(ptr_label->ptr_surface);
      return false;
    }

    if(ptr_label->ptr_texture->w > ptr_label->output_rect.w){
      ptr_label->texture_rect.w = ptr_label->output_rect.w;
      ptr_label->render_rect.w = ptr_label->output_rect.w;
    }else{
      ptr_label->texture_rect.w = (float)(ptr_label->ptr_texture->w);
      ptr_label->render_rect.w = (float)(ptr_label->ptr_texture->w);
    }

    if(ptr_label->ptr_texture->h > ptr_label->output_rect.h){
      ptr_label->texture_rect.h = ptr_label->output_rect.h;
      ptr_label->render_rect.h = ptr_label->output_rect.h;
    }else{
      ptr_label->texture_rect.h = (float)(ptr_label->ptr_texture->h);
      ptr_label->render_rect.h = (float)(ptr_label->ptr_texture->h);
    }

    // Sucsesfully updated the Label
    return true;

  }else{

    // Create empty surface and empty texture to return
    ptr_label->ptr_surface = SDL_CreateSurface((int)(ptr_label->output_rect.w), (int)(ptr_label->output_rect.h), SDL_PIXELFORMAT_RGBA8888);
    if(!ptr_label->ptr_surface) return false;

    ptr_label->ptr_texture = SDL_CreateTextureFromSurface(ptr_Renderer, ptr_label->ptr_surface);
    if(!ptr_label->ptr_texture) return false;

    // Surface background must be background collor
    // Get current render targed
    SDL_Texture *ptr_render_targed = SDL_GetRenderTarget(ptr_Renderer);
    // Set Texture as render targed
    if(!SDL_SetRenderTarget(ptr_Renderer, ptr_label->ptr_texture)){
      return false;
    }
    // Set draw colllor to background color
    if(!SDL_SetRenderDrawColor(ptr_Renderer, ptr_label->b_color->r, ptr_label->b_color->g, ptr_label->b_color->b, ptr_label->b_color->a)){
      SDL_SetRenderTarget(ptr_Renderer, ptr_render_targed);
      return false;
    }
    // Fill in texture with background collor
    if(!SDL_RenderRect(ptr_Renderer, NULL)){
      SDL_SetRenderTarget(ptr_Renderer, ptr_render_targed);
      return false;
    }
    // Reset Render targed
    if(!SDL_SetRenderTarget(ptr_Renderer, ptr_render_targed)){
      return false;
    }

    // Sucsesfully updated Label
    return true;
  }

  return false;
}




// Function for Updating the Text of a Label.
/*
ptr_Renderer: the render context used with the subject textures.
ptr_label: pointer to the label to be updated.
New_Text: new text string to be formated to the new text (like in printf function)
Note: the New_Text string is copyed in to the MiLb_Label structure (it does not become part of it).
*/
bool MiLb_UpdateLabelText(SDL_Renderer *ptr_Renderer, struct MiLb_Label* ptr_label, char* New_Text, ...){

  // Delete old Text
  free(ptr_label->text);

  if(New_Text){
    // Create new text
    va_list args; // Initialize va_list to hold variable arguments
    va_start(args, New_Text); // Start Processing Variable Arguments

    // Create new text
    char *complete_text = MiLb_FormatString(New_Text, args);

    // Clean up after VA list
    va_end(args);

    // Set new string as Lable text
    ptr_label->text = complete_text;
  }

  // Update Label
  return MiLb_UpdateLabel(ptr_Renderer, ptr_label);
}



// Function for safly Destroying a Label
void MiLb_DestroyLabel(struct MiLb_Label* ptr_label){

  // Start by destroying the Surface and Textures
  SDL_DestroySurface(ptr_label->ptr_surface);
  SDL_DestroyTexture(ptr_label->ptr_texture);

  // Free the allocated string
  free(ptr_label->text);

  // Free The structure
  free(ptr_label);
}


// End
