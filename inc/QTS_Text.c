// Qualety Text Core by Christopher Jung
// for Creating Dynamic text elements with SDL3 and SDL3 TTF

#include "QualityTextSystem.h"







// Function for creating QTS_Text object in memmory
// Returns a QTS_Text Object pointer on sucsess and NULL on fail
// font is a pointer to a font that the text is suposed to use (can be null but should be asinged before rerndering)
// string is a UTF8 string of characters that will be copyed in to the structure (is read for pslen or untill NULL character occurs)
// pslen is the size of the string
// ptr_color_f/ptr_color_b the foreground/background collor to be used when rendering (can be NULL => all clolr values 0)
// render_func is an optional custom rendering function that can be used to render the text (Leave NULL if not using this feature)
// ptr_format_data is an optional pointer for format data passed to the render_func (Leave NULL if not using this feature)
struct QTS_Text* QTS_CreateText(SDL_Renderer *ptr_renderer, TTF_Font *ptr_font, char *string, size_t pslen, int render_method, SDL_Color *ptr_color_f, SDL_Color *ptr_color_b, SDL_Texture* (*render_func)(struct QTS_Text*, void*), void* ptr_format_data) {

  // Allocate QTS_Text object in memmory
  struct QTS_Text* ptr_Text = (struct QTS_Text*)calloc(1, sizeof(struct QTS_Text));
  if(!ptr_Text) return NULL;

  // Initialize basic values
  ptr_Text->ptr_font = ptr_font;
  if(pslen) ptr_Text->length = pslen;
  ptr_Text->ptr_color_f = ptr_color_f;
  ptr_Text->ptr_color_b = ptr_color_b;
  ptr_Text->render_method = render_method;
  ptr_Text->render_func = render_func;
  ptr_Text->ptr_format_data = ptr_format_data;
  ptr_Text->pre_kernel = true;

  // if a font is given get general size parameters
  if(ptr_Text->ptr_font) {

    // Get ascent and decent
    ptr_Text->ascent = TTF_GetFontAscent(ptr_Text->ptr_font);
    ptr_Text->descent = TTF_GetFontDescent(ptr_Text->ptr_font);

    // Calculate overall hight
    //ptr_Text->hight = abs(ptr_Text->ascent) + abs(ptr_Text->descent);
    ptr_Text->hight = TTF_GetFontHeight(ptr_Text->ptr_font);

  }

  // Only do this if there is data
  if(string) {

    // if string length is given else NULL Terminated string
    if(ptr_Text->length > 0){
      // Allocate and copy string
      ptr_Text->string = (char*)calloc(ptr_Text->length, sizeof(char));
      memcpy(ptr_Text->string, string, ptr_Text->length);

    } else { // NULL terminated string

      int i = 0;
      while(string[i]) i++; // go through string untill NULL (end) is found
      ptr_Text->length = i + 1; // add one for the NULL termination
      ptr_Text->string = (char*)calloc(ptr_Text->length, sizeof(char));
      memcpy(ptr_Text->string, string, ptr_Text->length);

    }

    // General variables for UTF8 stepping
    const char* ptr_string = ptr_Text->string; // will be adjusted by SDL_StepUTF8 function
    size_t adj_length = ptr_Text->length; // will be adjusted by SDL_StepUTF8 function

    // Count how manny UTF8 characters are in string
    // while there is length left and NULL is not detected as the next symbol
    // get first codepoint
    if(adj_length > 0 && ptr_string[0]){
      ptr_Text->first_codepoint = SDL_StepUTF8(&ptr_string, &adj_length);
      ptr_Text->codepoints++;
      ptr_Text->last_codepoint = ptr_Text->first_codepoint;
    }
    // Get remaining codepoints and store last codepoint
    while(adj_length > 0 && ptr_string[0]){
      ptr_Text->last_codepoint = SDL_StepUTF8(&ptr_string, &adj_length);
      ptr_Text->codepoints++;
    }

    // allocate spacing array
    ptr_Text->spacing = (int*)calloc(ptr_Text->codepoints, sizeof(int));
    if(!(ptr_Text->spacing)) { // ERROR Handeling
      free(ptr_Text->string);
      free(ptr_Text);
      return NULL;
    }

    if(ptr_Text->ptr_font){

      // Fill in spacing
      int spacing_holding = 0;
      int kerneling_holding = 0;
      int generic_data; // direct not needed stuff here
      ptr_string = ptr_Text->string;
      adj_length = ptr_Text->length;
      Uint32 code_point = 0;
      Uint32 prev_code_point = SDL_StepUTF8(&ptr_string, &adj_length);

      TTF_GetGlyphMetrics(ptr_Text->ptr_font, prev_code_point, &generic_data, &generic_data, &generic_data, &generic_data, &spacing_holding);
      ptr_Text->spacing[0] = spacing_holding;

      for(Uint32 i = 1; i < ptr_Text->codepoints; i++){

        // Identify the curtrent codepoint
        code_point = SDL_StepUTF8(&ptr_string, &adj_length);

        // Get the glyph spacing
        TTF_GetGlyphMetrics(ptr_Text->ptr_font, code_point, &generic_data, &generic_data, &generic_data, &generic_data, &spacing_holding);
        QTS_GetGlyphKerning(ptr_Text->ptr_font, prev_code_point, code_point, &kerneling_holding);

        ptr_Text->spacing[i] = spacing_holding + kerneling_holding + ptr_Text->spacing[i - 1];

        prev_code_point = code_point;

      } // end of spacing loop

      // Redner Text
      switch (ptr_Text->render_method) {

        // QTS_RENDERMETHOD_CUSTOM
        case -1:
          if(ptr_Text->render_func){
            // if there is a Custom rendermethod given use it
            ptr_Text->ptr_texture = ptr_Text->render_func(ptr_Text, ptr_Text->ptr_format_data);
          }
          break;

        // Q_RENDERMETHOD_SOLID / RENDERMETHOD_SOLID
        case 0:
          if(ptr_Text->ptr_color_f){
            ptr_Text->ptr_surface = TTF_RenderText_Solid(ptr_Text->ptr_font, ptr_Text->string, ptr_Text->length - 1, *(ptr_Text->ptr_color_f));
          }else{
            return NULL;
          }
          break;

        // RENDERMETHOD_SOLID_WRAPPED
        case 1:
          if(ptr_Text->ptr_color_f){
            ptr_Text->ptr_surface = TTF_RenderText_Solid_Wrapped(ptr_Text->ptr_font, ptr_Text->string, ptr_Text->length - 1, *(ptr_Text->ptr_color_f), 0);
          }else{
            return NULL;
          }
          break;

        // Q_RENDERMETHOD_SHADED / RENDERMETHOD_SHADED
        case 2:
          if(ptr_Text->ptr_color_f && ptr_Text->ptr_color_b){
            ptr_Text->ptr_surface = TTF_RenderText_Shaded(ptr_Text->ptr_font, ptr_Text->string, ptr_Text->length - 1, *(ptr_Text->ptr_color_f), *(ptr_Text->ptr_color_b));
          }else{
            return NULL;
          }
          break;

        // RENDERMETHOD_SHADED_WRAPPED
        case 3:
          if(ptr_Text->ptr_color_f && ptr_Text->ptr_color_b){
            ptr_Text->ptr_surface = TTF_RenderText_Shaded_Wrapped(ptr_Text->ptr_font, ptr_Text->string, ptr_Text->length - 1, *(ptr_Text->ptr_color_f), *(ptr_Text->ptr_color_b), 0);
          }else{
            return NULL;
          }
          break;

        // Q_RENDERMETHOD_BLENDED / RENDERMETHOD_BLENDED
        case 4:
          if(ptr_Text->ptr_color_f){
            ptr_Text->ptr_surface = TTF_RenderText_Blended(ptr_Text->ptr_font, ptr_Text->string, ptr_Text->length - 1, *(ptr_Text->ptr_color_f));
          }else{
            return NULL;
          }
          break;

        // RENDERMETHOD_BLENDED_WRAPPED
        case 5:
          if(ptr_Text->ptr_color_f){
            ptr_Text->ptr_surface = TTF_RenderText_Blended_Wrapped(ptr_Text->ptr_font, ptr_Text->string, ptr_Text->length - 1, *(ptr_Text->ptr_color_f), 0);
          }else{
            return NULL;
          }
          break;

        // Q_RENDERMETHOD_LCD / RENDERMETHOD_LCD
        case 6:
          if(ptr_Text->ptr_color_f && ptr_Text->ptr_color_b){
            ptr_Text->ptr_surface = TTF_RenderText_LCD(ptr_Text->ptr_font, ptr_Text->string, ptr_Text->length - 1, *(ptr_Text->ptr_color_f), *(ptr_Text->ptr_color_b));
          }else{
            return NULL;
          }
          break;

        // RENDERMETHOD_LCD_WRAPPED
        case 7:
          if(ptr_Text->ptr_color_f && ptr_Text->ptr_color_b){
            ptr_Text->ptr_surface = TTF_RenderText_LCD_Wrapped(ptr_Text->ptr_font, ptr_Text->string, ptr_Text->length - 1, *(ptr_Text->ptr_color_f), *(ptr_Text->ptr_color_b), 0);
          }else{
            return NULL;
          }
          break;

        // Default is RENDERMETHOD_BLENDED
        default:
          if(ptr_Text->ptr_color_f){
            ptr_Text->ptr_surface = TTF_RenderText_Blended(ptr_Text->ptr_font, ptr_Text->string, ptr_Text->length - 1, *(ptr_Text->ptr_color_f));
          }else{
            return NULL;
          }
          break;

      }// End of render_method switch

      // Error Check
      if(!(ptr_Text->ptr_surface) && render_method != -1){
        free(ptr_Text->spacing);
        free(ptr_Text->string);
        free(ptr_Text);
        return NULL;
      }

      // Convert surface to Texture
      if(render_method != -1) ptr_Text->ptr_texture = SDL_CreateTextureFromSurface(ptr_renderer, ptr_Text->ptr_surface);

      // Error Check
      if(!(ptr_Text->ptr_texture)){
        SDL_DestroySurface(ptr_Text->ptr_surface);
        free(ptr_Text->spacing);
        free(ptr_Text->string);
        free(ptr_Text);
        return NULL;
      }

    }// end of if font

  }// end of if string

  // Return the Text structure
  return ptr_Text;
} // End of QTS_Create_Text






// Function for Destroying (Freeing memory) a QTS_Text object
// Also frees the text string
// will not free the collors or destroy the font
void QTS_DestroyTextOBJ(struct QTS_Text* ptr_Text)
{

  if(!ptr_Text) return;

  // Free internal array
  free(ptr_Text->string);
  free(ptr_Text->spacing);
  if(ptr_Text->ptr_surface != NULL) SDL_DestroySurface(ptr_Text->ptr_surface); // mainly to preserve potential SDL error mesage
  if(ptr_Text->ptr_texture != NULL) SDL_DestroyTexture(ptr_Text->ptr_texture); // mainly to preserve potential SDL error mesage
  free(ptr_Text);

  return;
}


// Function for destroying a lose chain of text
void QTS_DestroyTextChain(struct QTS_Text* ptr_Text){

  // check input
  if(!ptr_Text) return;

  // Create two holding pointers
  struct QTS_Text* ptr_Text_holding = ptr_Text;
  struct QTS_Text* ptr_Text_free;

  // Destroy chain of text elements
  while(ptr_Text_holding){

    // Assing pointers
    ptr_Text_free = ptr_Text_holding;
    ptr_Text_holding = ptr_Text_holding->ptr_NextText;

    // Destroy text elements
    QTS_DestroyTextOBJ(ptr_Text_free);
  }

  return;
}






// Simple function for Rendering a QTS_Text to the current rendering targed at given cordinates
bool QTS_RenderText(SDL_Renderer *ptr_renderer, struct QTS_Text* ptr_Text, float x, float y)
{

  SDL_FRect DestRect;
  DestRect.x = x;
  DestRect.y = y;
  DestRect.w = ptr_Text->ptr_texture->w;
  DestRect.h = ptr_Text->ptr_texture->h;

  return SDL_RenderTexture(ptr_renderer, ptr_Text->ptr_texture, NULL, &DestRect);

}





// Function for Updating a QTS_Text Object after applying changes
// Updates values and rerenders the Text
bool QTS_UpdateText(SDL_Renderer *ptr_renderer, struct QTS_Text* ptr_Text)
{

  if(!ptr_renderer || !ptr_Text) return false;

  // if font is assinged get general size parameters
  if(ptr_Text->ptr_font){

    // Get ascent and decent
    ptr_Text->ascent = TTF_GetFontAscent(ptr_Text->ptr_font);
    ptr_Text->descent = TTF_GetFontDescent(ptr_Text->ptr_font);

    // Calculate overall hight
    //ptr_Text->hight = abs(ptr_Text->ascent) + abs(ptr_Text->descent);
    ptr_Text->hight = TTF_GetFontHeight(ptr_Text->ptr_font);

  }

  // Only do this if there is data
  if(ptr_Text->string){

    // Determine Length of String if is not given (implys NULL terminated string)
    if(ptr_Text->length == 0){
      int i = 0;
      while(ptr_Text->string[i]) i++; // go through string untill NULL (end) is found
      ptr_Text->length = i + 1; // add one for the NULL termination
    }

    // General variables for UTF8 stepping
    const char* ptr_string = ptr_Text->string; // will be adjusted by SDL_StepUTF8 function
    size_t adj_length = ptr_Text->length; // will be adjusted by SDL_StepUTF8 function

    // Count how manny UTF8 characters are in string
    // while there is length left and NULL is not detected as the next symbol
    ptr_Text->codepoints = 0; // Reset codepoint count
    // Get first codepoint
    if(adj_length > 0 && ptr_string[0]){
      ptr_Text->first_codepoint = SDL_StepUTF8(&ptr_string, &adj_length);
      ptr_Text->codepoints++;
      ptr_Text->last_codepoint = ptr_Text->first_codepoint;
    }
    while(adj_length > 0 && ptr_string[0]){
      ptr_Text->last_codepoint = SDL_StepUTF8(&ptr_string, &adj_length);
      ptr_Text->codepoints++;
    }

    ptr_Text->pre_kernel_offset = QTS_GetPreKernelOffset(ptr_Text);

    // Realocate Spacings array
    free(ptr_Text->spacing);
    ptr_Text->spacing = (int*)calloc(ptr_Text->codepoints, sizeof(int));
    if(!ptr_Text->spacing) return false;

    // Only do this part if there is a font
    if(ptr_Text->ptr_font){

      // Fill in spacing
      int spacing_holding = 0;
      int kerneling_holding = 0;
      int generic_data; // direct not needed stuff here
      ptr_string = ptr_Text->string;
      adj_length = ptr_Text->length;
      Uint32 code_point = 0;
      Uint32 prev_code_point = SDL_StepUTF8(&ptr_string, &adj_length);

      TTF_GetGlyphMetrics(ptr_Text->ptr_font, prev_code_point, &generic_data, &generic_data, &generic_data, &generic_data, &spacing_holding);
      ptr_Text->spacing[0] = spacing_holding;

      // Iterate through every codepoint
      for(Uint32 i = 1; i < ptr_Text->codepoints; i++){

        // Identify the curtrent codepoint
        code_point = SDL_StepUTF8(&ptr_string, &adj_length);

        // Get the glyph spacing
        TTF_GetGlyphMetrics(ptr_Text->ptr_font, code_point, &generic_data, &generic_data, &generic_data, &generic_data, &spacing_holding);
        QTS_GetGlyphKerning(ptr_Text->ptr_font, prev_code_point, code_point, &kerneling_holding);

        ptr_Text->spacing[i] = spacing_holding + kerneling_holding + ptr_Text->spacing[i - 1];

        prev_code_point = code_point;

      } // end of spacing loop

      // if there is old surface, destroy it to make room for new surface
      if(ptr_Text->ptr_surface) SDL_DestroySurface(ptr_Text->ptr_surface);

      // Redner new Text
      switch (ptr_Text->render_method) {

        // QTS_RENDERMETHOD_CUSTOM
        case -1:
          if(ptr_Text->render_func){

            // First destroy old texture
            SDL_DestroyTexture(ptr_Text->ptr_texture);

            // if there is a Custom rendermethod given use it
            ptr_Text->ptr_texture = ptr_Text->render_func(ptr_Text, ptr_Text->ptr_format_data);
          }
          break;

        // Q_RENDERMETHOD_SOLID / RENDERMETHOD_SOLID
        case 0:
          if(ptr_Text->ptr_color_f){
            ptr_Text->ptr_surface = TTF_RenderText_Solid(ptr_Text->ptr_font, ptr_Text->string, ptr_Text->length - 1, *(ptr_Text->ptr_color_f));
          }else{
            return false;
          }
          break;

        // RENDERMETHOD_SOLID_WRAPPED
        case 1:
          if(ptr_Text->ptr_color_f){
            ptr_Text->ptr_surface = TTF_RenderText_Solid_Wrapped(ptr_Text->ptr_font, ptr_Text->string, ptr_Text->length - 1, *(ptr_Text->ptr_color_f), 0);
          }else{
            return false;
          }
          break;

        // Q_RENDERMETHOD_SHADED / RENDERMETHOD_SHADED
        case 2:
          if(ptr_Text->ptr_color_f && ptr_Text->ptr_color_b){
            ptr_Text->ptr_surface = TTF_RenderText_Shaded(ptr_Text->ptr_font, ptr_Text->string, ptr_Text->length - 1, *(ptr_Text->ptr_color_f), *(ptr_Text->ptr_color_b));
          }else{
            return false;
          }
          break;

        // RENDERMETHOD_SHADED_WRAPPED
        case 3:
          if(ptr_Text->ptr_color_f && ptr_Text->ptr_color_b){
            ptr_Text->ptr_surface = TTF_RenderText_Shaded_Wrapped(ptr_Text->ptr_font, ptr_Text->string, ptr_Text->length - 1, *(ptr_Text->ptr_color_f), *(ptr_Text->ptr_color_b), 0);
          }else{
            return false;
          }
          break;

        // Q_RENDERMETHOD_BLENDED / RENDERMETHOD_BLENDED
        case 4:
          if(ptr_Text->ptr_color_f){
            ptr_Text->ptr_surface = TTF_RenderText_Blended(ptr_Text->ptr_font, ptr_Text->string, ptr_Text->length - 1, *(ptr_Text->ptr_color_f));
          }else{
            return false;
          }
          break;

        // RENDERMETHOD_BLENDED_WRAPPED
        case 5:
          if(ptr_Text->ptr_color_f){
            ptr_Text->ptr_surface = TTF_RenderText_Blended_Wrapped(ptr_Text->ptr_font, ptr_Text->string, ptr_Text->length - 1, *(ptr_Text->ptr_color_f), 0);
          }else{
            return false;
          }
          break;

        // Q_RENDERMETHOD_LCD / RENDERMETHOD_LCD
        case 6:
          if(ptr_Text->ptr_color_f && ptr_Text->ptr_color_b){
            ptr_Text->ptr_surface = TTF_RenderText_LCD(ptr_Text->ptr_font, ptr_Text->string, ptr_Text->length - 1, *(ptr_Text->ptr_color_f), *(ptr_Text->ptr_color_b));
          }else{
            return false;
          }
          break;

        // RENDERMETHOD_LCD_WRAPPED
        case 7:
          if(ptr_Text->ptr_color_f && ptr_Text->ptr_color_b){
            ptr_Text->ptr_surface = TTF_RenderText_LCD_Wrapped(ptr_Text->ptr_font, ptr_Text->string, ptr_Text->length - 1, *(ptr_Text->ptr_color_f), *(ptr_Text->ptr_color_b), 0);
          }else{
            return false;
          }
          break;

        // Default is RENDERMETHOD_BLENDED
        default:
          if(ptr_Text->ptr_color_f){
            ptr_Text->ptr_surface = TTF_RenderText_Blended(ptr_Text->ptr_font, ptr_Text->string, ptr_Text->length - 1, *(ptr_Text->ptr_color_f));
          }else{
            return false;
          }
          break;

      }// End of render_method switch

      // Error Check
      if(!(ptr_Text->ptr_surface) && (ptr_Text->render_method != -1)){
        //free(ptr_Text->spacing);
        //free(ptr_Text->string);
        //free(ptr_Text);
        return false;
      }

      // if there is old Texture destroy it to make room for new texture
      if(ptr_Text->ptr_texture && (ptr_Text->render_method != -1)) SDL_DestroyTexture(ptr_Text->ptr_texture);

      // Convert surface to Texture
      if(ptr_Text->render_method != -1) ptr_Text->ptr_texture = SDL_CreateTextureFromSurface(ptr_renderer, ptr_Text->ptr_surface);

      // Error Check
      if(!(ptr_Text->ptr_texture)){
        //SDL_DestroySurface(ptr_Text->ptr_surface);
        //free(ptr_Text->spacing);
        //free(ptr_Text->string);
        //free(ptr_Text);
        return false;
      }

    }// End of if font

  }// End of if string

  return true;
}


// Function alternative to TTF_GetGlyphKerning
// Because TTF_GetGlyphKerning does not work for some reason this function is used instead.
// the functions are simulare in regards to input and output.
bool QTS_GetGlyphKerning(TTF_Font* ptr_Font, Uint32 ch1, Uint32 ch2, int* ptr_kerneling){

  // Veryfy Input
  if(!ptr_Font || !ptr_kerneling){
    SDL_SetError("QTS_GetGlyphKerning: NULL argument");
    return false;
  }

  // Help variables
  int w1 = 0, w2 = 0, w_combined = 0;

  // Measure Each glyph individualy
  if(!TTF_GetGlyphMetrics(ptr_Font, ch1, NULL, NULL, NULL, NULL, &w1)) {
    return false;
  }
  if(!TTF_GetGlyphMetrics(ptr_Font, ch2, NULL, NULL, NULL, NULL, &w2)) {
    return false;
  }

  // create a buffer with the two characters
  char buf[9] = {0};
  int pos = 0;

  // Encode ch1 as UTF-8
  if (ch1 < 0x80) {
      buf[pos++] = (char)ch1;
  } else if (ch1 < 0x800) {
      buf[pos++] = (char)(0xC0 | (ch1 >> 6));
      buf[pos++] = (char)(0x80 | (ch1 & 0x3F));
  } else if (ch1 < 0x10000) {
      buf[pos++] = (char)(0xE0 | (ch1 >> 12));
      buf[pos++] = (char)(0x80 | ((ch1 >> 6) & 0x3F));
      buf[pos++] = (char)(0x80 | (ch1 & 0x3F));
  } else {
      buf[pos++] = (char)(0xF0 | (ch1 >> 18));
      buf[pos++] = (char)(0x80 | ((ch1 >> 12) & 0x3F));
      buf[pos++] = (char)(0x80 | ((ch1 >> 6)  & 0x3F));
      buf[pos++] = (char)(0x80 | (ch1 & 0x3F));
  }

  // Encode ch2 as UTF-8
  if (ch2 < 0x80) {
      buf[pos++] = (char)ch2;
  } else if (ch2 < 0x800) {
      buf[pos++] = (char)(0xC0 | (ch2 >> 6));
      buf[pos++] = (char)(0x80 | (ch2 & 0x3F));
  } else if (ch2 < 0x10000) {
      buf[pos++] = (char)(0xE0 | (ch2 >> 12));
      buf[pos++] = (char)(0x80 | ((ch2 >> 6) & 0x3F));
      buf[pos++] = (char)(0x80 | (ch2 & 0x3F));
  } else {
      buf[pos++] = (char)(0xF0 | (ch2 >> 18));
      buf[pos++] = (char)(0x80 | ((ch2 >> 12) & 0x3F));
      buf[pos++] = (char)(0x80 | ((ch2 >> 6)  & 0x3F));
      buf[pos++] = (char)(0x80 | (ch2 & 0x3F));
  }

  // Get size of full string
  if(!TTF_GetStringSize(ptr_Font, buf, pos, &w_combined, NULL)){
    return false;
  }

  // Add up lengths
  *ptr_kerneling = w_combined - (w1 + w2);

  return true;
}



// Function for getting the kerenling offset of the Text object in Line
int QTS_GetPreKernelOffset(struct QTS_Text* ptr_Text){
  if(!ptr_Text) return 0;
  if(!ptr_Text->ptr_PrevText) return 0;
  bool b_ret;
  int kernel_holding = 0;

  // SDL TTF version of function (use when was fixed in newer version)
  //if(ptr_Text->ptr_PrevText) b_ret = TTF_GetGlyphKerning(ptr_Text->ptr_PrevText->ptr_font, ptr_Text->ptr_PrevText->last_codepoint, ptr_Text->first_codepoint, &kernel_holding);

  // QTS Version of function
  if(ptr_Text->ptr_PrevText) b_ret = QTS_GetGlyphKerning(ptr_Text->ptr_PrevText->ptr_font, ptr_Text->ptr_PrevText->last_codepoint, ptr_Text->first_codepoint, &kernel_holding);

  if(!b_ret) return 0;
  return kernel_holding;
}





// Simple function for updateing the Text of a QTS_Text object (runn QTS_UpdateText function to update render)
// ptr_Text is the pointer to the QTS_Text objet to update
// new_string the new string for the QTS_Text Object
// strlen the size in bytes of the new string, 0 for NULL Terminated
// copy if true then copy the string in to the structure, else (false) then use the string providet (QTS_Text object will take ownership of the string)
// return true on sucsess and false on fail
bool QTS_UpdateTextString(struct QTS_Text* ptr_Text, char* new_string, size_t strlen, bool copy)
{

  //printf("Parameters TextOBJ: %p, new_string: %s, string length: %ld, copy %d \n", ptr_Text, new_string, strlen, copy);


  // Verify input
  if(!ptr_Text) return false;

  // if set string to NULL (is a valid case)
  if(!new_string) {

    // First free old string
    free(ptr_Text->string);
    ptr_Text->length = 0;
    // Then reset pointer to NULL
    ptr_Text->string = NULL;

    return true;
  }

  // Pass the new size to Text object
  ptr_Text->length = strlen;

  // Diffrent behavior depending on copy or not
  if(copy) {

    if(strlen > 0) {

      // allocate new string
      char* StringCopy = (char*)calloc(strlen + 1, sizeof(char)); // +1 for a null termination

      // Copy data in to new string
      //char* StringCopy = (char*)malloc(strlen * sizeof(char));
      memcpy(StringCopy, new_string, strlen);

      // Give string to text element
      ptr_Text->length = strlen;
      ptr_Text->string = StringCopy;

    } else { // NULL Terminated

      // Find the length of the string
      size_t new_len = 0;
      for(int i = 0; new_string[i]; i++) new_len++;

      // allocate new string
      char* StringCopy = (char*)calloc(new_len + 1, sizeof(char));  // plus NULL Termination

      // Copy data in to new string
      //char* StringCopy = (char*)malloc((new_len + 1) * sizeof(char));  // plus NULL Termination
      memcpy(StringCopy, new_string, new_len);

      // Give string to text element
      ptr_Text->length = new_len + 1;
      ptr_Text->string = StringCopy;

    }

  } else {

    // First free old string
    free(ptr_Text->string);
    // Then Replace with new string
    ptr_Text->string = new_string;

  }
  return true;
}



// Function for splitting a QTS_Text object after a number of code points.
// The QTS_Text object is split in to two after the SplitCodepoint character
// the object passed to the function is shortend and the split off part is returned,
// the spit off par is integrated as the following part in the line to the origenal
// Returns QTS_Text pointer on sucsess and NULL on fail
struct QTS_Text* QTS_SplitText(SDL_Renderer *ptr_renderer, struct QTS_Text* ptr_Text, int SplitCodepoint)
{
  // Validate input
  if(!ptr_Text || SplitCodepoint <= 0) return NULL;

  // Get the size of the new string of the first part

  // General variables for UTF8 stepping
  const char* ptr_string = ptr_Text->string; // will be adjusted by SDL_StepUTF8 function
  size_t adj_length = ptr_Text->length; // will be adjusted by SDL_StepUTF8 function

  for(int i = 0; i < SplitCodepoint; i++) SDL_StepUTF8(&ptr_string, &adj_length);
  size_t length_A = ptr_Text->length - adj_length;// Length of the first part in bytes
  size_t length_B = adj_length;

  // Create two new strings, each one half
  char* string_A = (char*)calloc(length_A + 1, sizeof(char)); // +1 for NULL Termination
  memcpy(string_A, ptr_Text->string, length_A); // copy contents up to split

  char* string_B = (char*)calloc(length_B + 1, sizeof(char)); // +1 for NULL Termination
  memcpy(string_B, ptr_string, length_B); // copy contents from split byte to end to string_B

  // Return value for error handeling
  bool lb_ret;

  // Update the first Text obj
  lb_ret = QTS_UpdateTextString(ptr_Text, string_A, 0 /*length_A*/, false);
  if(!lb_ret){
    free(string_A);
    free(string_B);
    return NULL;
  }
  lb_ret = QTS_UpdateText(ptr_renderer, ptr_Text);
  if(!lb_ret){
    free(string_A);
    free(string_B);
    return NULL;
  }

  // Create New Text object following the old one (part B)
  struct QTS_Text* ptr_Text_B = QTS_CreateText(ptr_renderer, ptr_Text->ptr_font, NULL, 0, ptr_Text->render_method, ptr_Text->ptr_color_f, ptr_Text->ptr_color_b, ptr_Text->render_func, ptr_Text->ptr_format_data);
  if(!ptr_Text_B){
    free(string_A);
    free(string_B);
    return NULL;
  }
  lb_ret = QTS_UpdateTextString(ptr_Text_B, string_B, 0 /*length_B*/, false);
  if(!lb_ret){
    free(string_A);
    free(string_B);
    QTS_DestroyTextOBJ(ptr_Text_B);
    return NULL;
  }
  lb_ret = QTS_UpdateText(ptr_renderer, ptr_Text_B);
  if(!lb_ret){
    free(string_A);
    free(string_B);
    QTS_DestroyTextOBJ(ptr_Text_B);
    return NULL;
  }

  // Text element B follows text ement A
  struct QTS_Text* ptr_Text_A = ptr_Text;


  if(ptr_Text_A->ptr_NextText) ptr_Text_A->ptr_NextText->ptr_PrevText = ptr_Text_B;
  if(ptr_Text_B) ptr_Text_B->ptr_NextText = ptr_Text_A->ptr_NextText;
  ptr_Text_B->ptr_PrevText = ptr_Text_A;
  ptr_Text_A->ptr_NextText = ptr_Text_B;


  /*
  // Bind new text obj as next in Line
  struct QTS_Text* ptr_TextHolding;
  ptr_TextHolding = ptr_Text->ptr_NextText;

  ptr_Text->ptr_NextText = ptr_Text_B;
  ptr_Text_B->ptr_NextText = ptr_TextHolding;
  ptr_Text_B->ptr_PrevText = ptr_Text;
*/
  return ptr_Text_B;
}



// Function for Comparing Text Formats
// Return true if they have the same format and false if they have diffren formats or Error
bool QTS_CompareTextFormats(struct QTS_Text* ptr_Text_A, struct QTS_Text* ptr_Text_B){

  // Validate Input
  if(!ptr_Text_A || !ptr_Text_B) return false;

  // compare formats one by one
  if(ptr_Text_A->hight != ptr_Text_B->hight) return false;
  if(ptr_Text_A->ascent != ptr_Text_B->ascent) return false;
  if(ptr_Text_A->descent != ptr_Text_B->descent) return false;
  if(ptr_Text_A->ptr_font != ptr_Text_B->ptr_font) return false;
  if(ptr_Text_A->render_method != ptr_Text_B->render_method) return false;
  if(ptr_Text_A->render_func != ptr_Text_B->render_func) return false;
  if(ptr_Text_A->ptr_color_f != ptr_Text_B->ptr_color_f) return false;
  if(ptr_Text_A->ptr_color_b != ptr_Text_B->ptr_color_b) return false;

  return true;
}




// Function for merging a QTS_Text object with its follower if requested by force
// Return true if was merged and false if was not merged or Error
// if force_merge is false ptr_Text is only merged with its follower if they share the same format (same size, font, collor, ...)
// if force_merge is true ptr_Text is extended by its followers text and its follower is destroyed safly
bool QTS_MergeText(SDL_Renderer *ptr_renderer, struct QTS_Text* ptr_Text, bool force_merge){

  // Validate Input
  if(!ptr_renderer || !ptr_Text) return false;
  if(ptr_Text->ptr_NextText == NULL) return false;

  // Holding variables
  struct QTS_Text* ptr_text_holding;
  size_t new_size;
  char* new_string = NULL;


  // Compare Formats
  if(force_merge){

    // Merge the two strings
    new_string = QTS_MergeStrings(ptr_Text->string, ptr_Text->length, ptr_Text->ptr_NextText->string, ptr_Text->ptr_NextText->length, &new_size);

    // Debind follower so it can be destroyed later
    ptr_text_holding = ptr_Text->ptr_NextText;
    ptr_Text->ptr_NextText = ptr_text_holding->ptr_NextText;
    if(ptr_text_holding->ptr_NextText) ptr_text_holding->ptr_NextText->ptr_PrevText = ptr_Text;

    // Destroy Merged and Debinded object
    QTS_DestroyTextOBJ(ptr_text_holding);

    // Update the merge result
    QTS_UpdateTextString(ptr_Text, new_string, new_size, false);
    QTS_UpdateText(ptr_renderer, ptr_Text);

    return true;

  }else{

    // only merge if the formats match
    if(QTS_CompareTextFormats(ptr_Text, ptr_Text->ptr_NextText)){

      // Merge the two strings
      new_string = QTS_MergeStrings(ptr_Text->string, ptr_Text->length, ptr_Text->ptr_NextText->string, ptr_Text->ptr_NextText->length, &new_size);

      // Debind follower so it can be destroyed later
      ptr_text_holding = ptr_Text->ptr_NextText;
      ptr_Text->ptr_NextText = ptr_text_holding->ptr_NextText;
      if(ptr_text_holding->ptr_NextText) ptr_text_holding->ptr_NextText->ptr_PrevText = ptr_Text;

      // Destroy Merged and Debinded object
      QTS_DestroyTextOBJ(ptr_text_holding);

      // Update the merge result
      QTS_UpdateTextString(ptr_Text, new_string, new_size, false);
      QTS_UpdateText(ptr_renderer, ptr_Text);

      return true;

    }else{
      // not merged due to not alining formats
      return false;
    }

  }

  return true;
}


// Function for inserting a string a insision index in the string
// ins_string is copyed
bool QTS_InsertString(struct QTS_Text* ptr_Text, char* ins_string, size_t insision_index){

  // Validate Input
  if(!ptr_Text || !ins_string) return false;

  size_t adj_ins_size = 0;
  size_t new_string_size;

  // count chars in string
  while(ins_string[adj_ins_size]) adj_ins_size++;

  // if text element has no string
  if(!ptr_Text->string) return QTS_UpdateTextString(ptr_Text, ins_string, 0, true);

  // Calculate new string size
  new_string_size = adj_ins_size + ptr_Text->length;// - 1; // -1 because one of the two Null terminations needs to be removed

  // Allocate new string
  //char* new_string = (char*)malloc(sizeof(char) * new_string_size);
  char* new_string = (char*)calloc(new_string_size, sizeof(char));

  // Copy the strings in to new string
  memcpy(new_string, ptr_Text->string, insision_index); // copy text before incission
  memcpy(&(new_string[insision_index]), ins_string, adj_ins_size); // copy incertion text
  // only copy string after incission if there is text after incission
  memcpy(&(new_string[insision_index + adj_ins_size]), &(ptr_Text->string[insision_index]), ptr_Text->length - insision_index); // copy text after incission

  /*
  memcpy(new_string, ptr_Text->string, insision_index - 1);
  memcpy(&(new_string[insision_index]), ins_string, adj_ins_size);
  memcpy(&(new_string[insision_index + adj_ins_size]), ptr_Text->string, new_string_size - (insision_index + adj_ins_size));
  */

  return QTS_UpdateTextString(ptr_Text, new_string, new_string_size, false);
}




// Function for Drawing Bounding Boxes of chars
/*
ptr_renderer: the subject renderer
ptr_Text: the textobject whos boundingboxes are suposed to be drawn
x: x cordinate of text rendering
y: y cordinate of text rendering
Note: draws on the current render targed
*/
bool QTS_DrawBoundingBoxes(SDL_Renderer *ptr_renderer, struct QTS_Text* ptr_Text, float x, float y){

  // Validate Input
  if(!ptr_renderer || !ptr_Text) return false;

  // get the Hight of the text
  int hight = ptr_Text->hight;

  // Create rectangle to render
  SDL_FRect rect;
  rect.x = x;
  rect.y = y;
  rect.h = (float)hight;
  rect.w = (float)(ptr_Text->spacing[0]);

  SDL_SetRenderDrawColor(ptr_renderer, 0x1F, 0x1F, 0xFF, 0xFF);

  for(Uint32 i = 0; i < ptr_Text->codepoints; i++){
    rect.w = (float)(ptr_Text->spacing[i]);
    SDL_RenderRect(ptr_renderer, &rect);
  }

  return true;
}




/*
// Updates the Text of a QTS_Text object by copying the source string src_string and recalculating the spacing (also updates codepoint count)
// Does NOT render the new text. Use QTS_UpdateText to update render
// ptr_Text : pointer to the QTS_Text object to be updated
// src_string : the source string to be copyed in to the structure
// length : the length of the surce string in bytes (0 for NULL terminated string)
bool QTS_UpdateTextString(struct QTS_Text* ptr_Text, char *src_string, size_t length)
{

  // Validate input
  if(!ptr_Text || !src_string) return false;

  // if is NULL terminated find length
  if(length == 0) {

    int i = 0;
    while(src_string[i]) i++; // finnd NULL at end
    i++; // add one for the NULL termination

    // Assing found length
    ptr_Text->length = i;

  } else {

    // Asing given length
    ptr_Text->length = length;

  }

  // Free the current string
  free(ptr_Text->string);

  // Allocate a new string
  ptr_Text->string = (char*)calloc(ptr_Text->length, sizeof(char));
  if(!ptr_Text->string) return false;

  // Copy source string
  memcpy(ptr_Text->string, src_string, ptr_Text->length);


  // General variables for UTF8 stepping
  const char* ptr_string = ptr_Text->string; // will be adjusted by SDL_StepUTF8 function
  size_t adj_length = ptr_Text->length; // will be adjusted by SDL_StepUTF8 function

  // Count code points in new string
  ptr_Text->codepoints = 0; // Reset codepoint count
  while(adj_length > 0 && ptr_string[0]){
    SDL_StepUTF8(&ptr_string, &adj_length);
    ptr_Text->codepoints++;
  }

  if(!ptr_Text->ptr_font) return false;

  // Update spacing
  // Reallocate spacing array
  free(ptr_Text->spacing);
  ptr_Text->spacing = (int*)calloc(ptr_Text->codepoints, sizeof(int));
  if(!ptr_Text->spacing) return false;

  // Recalculate spacings
  // Fill in spacing
  int spacing_holding = 0;
  int kerneling_holding = 0;
  int generic_data; // direct not needed stuff here
  ptr_string = ptr_Text->string;
  adj_length = ptr_Text->length;
  Uint32 code_point = 0;
  Uint32 prev_code_point = SDL_StepUTF8(&ptr_string, &adj_length);

  TTF_GetGlyphMetrics(ptr_Text->ptr_font, prev_code_point, &generic_data, &generic_data, &generic_data, &generic_data, &spacing_holding);
  ptr_Text->spacing[0] = spacing_holding;

  // Iterate through every codepoint
  for(int i = 1; i < ptr_Text->codepoints; i++){

    // Identify the curtrent codepoint
    code_point = SDL_StepUTF8(&ptr_string, &adj_length);

    // Get the glyph spacing
    TTF_GetGlyphMetrics(ptr_Text->ptr_font, code_point, &generic_data, &generic_data, &generic_data, &generic_data, &spacing_holding);
    TTF_GetGlyphKerning(ptr_Text->ptr_font, prev_code_point, code_point, &kerneling_holding);

    ptr_Text->spacing[i] = spacing_holding + kerneling_holding + ptr_Text->spacing[i - 1];

    prev_code_point = code_point;

  } // end of spacing loop

  return true;
}// end of QTS_UpdateTextString function



*/
