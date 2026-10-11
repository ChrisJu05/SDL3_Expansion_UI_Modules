// QTS EditLine Functions
/*
This Header Provides the Functionality for QTS_EditLines.
*/

// Include QTS Header
#include "QualityTextSystem.h"

// Include Dynmic Lines (for nicer drawing functions)
#include "dynamic_lines.h"



// Basic function for Drawing the curser in a Edit Line
bool QTS_EditLine_DrawCurser(SDL_Renderer *ptr_renderer, struct QTS_EditLine* ptr_EditLine){

  // Validate Input
  if(!ptr_renderer || !ptr_EditLine) return false;
  if(!ptr_EditLine->ptr_Line) return false;

  // Base variables
  int x_cord, y_cord, length, thikness;

  // Get the current time for Blinking intervall
  Uint32 current_time = SDL_GetTicks();

  // Handle blinking
  if(current_time - ptr_EditLine->last_time > 1000) ptr_EditLine->last_time = current_time;
  if(current_time - ptr_EditLine->last_time > 500) return true;


  // calculate true x cordinate
  x_cord = (int)(ptr_EditLine->line_rect.x) + ptr_EditLine->x_cord;

  // calculate true y cordinate
  // if Incision text element is named then draw curser in accoring size
  if(ptr_EditLine->incission_text_element){
    // draw curser based on specefyed textobject hight
    length = ptr_EditLine->incission_text_element->hight;
    y_cord = (int)QTS_GetAllingmentOffset(ptr_EditLine->ptr_Line, ptr_EditLine->incission_text_element);
    y_cord += (int)(ptr_EditLine->line_rect.y);
  }else{
    // Draw curser with line hight
    length = ptr_EditLine->ptr_Line->hight - ptr_EditLine->ptr_Line->Spaceing_Up - ptr_EditLine->ptr_Line->Spaceing_Down;
    y_cord = (int)(ptr_EditLine->line_rect.y);
  }

  // subtract font accent and decent from length
  //length -= abs(ptr_EditLine->incission_text_element->ascent);
  length -= abs(ptr_EditLine->incission_text_element->descent);

  // the thikness is allways proportional to the hight
  thikness = length / 60;
  // Enforce minimum thikness
  if(thikness < 1) thikness = 1 * ptr_EditLine->scaling * ptr_EditLine->scaling_offset;
  thikness = thikness * ptr_EditLine->scaling * ptr_EditLine->scaling_offset;

  // Draw curser
  if(ptr_EditLine->ptr_cursor_collor) {
    SDL_SetRenderDrawColor(ptr_renderer, ptr_EditLine->ptr_cursor_collor->r, ptr_EditLine->ptr_cursor_collor->g, ptr_EditLine->ptr_cursor_collor->b, ptr_EditLine->ptr_cursor_collor->a);
  }else{
    // Use Default collor
    SDL_SetRenderDrawColor(ptr_renderer, 0x80, 0x80, 0xFF, 0xFF);
  }
  return RenderLineDynamic(ptr_renderer, x_cord, y_cord, x_cord, y_cord + length, thikness);
}



// function for updating Highlight rect of a highlight
bool QTS_EditLine_GetHighlightRect(struct QTS_EditLine* ptr_EditLine){

  // Validate Input
  if(!ptr_EditLine) return false;
  if(!ptr_EditLine->ptr_Line) return false;
  if(!ptr_EditLine->highlight_start_text || !ptr_EditLine->highlight_end_text) return false;


  // create a holding element
  struct QTS_Text* ptr_text_holding = ptr_EditLine->highlight_start_text;
  float val_holding;
  ptr_EditLine->highlight_rect.y = QTS_GetAllingmentOffset(ptr_EditLine->ptr_Line, ptr_text_holding);
  ptr_EditLine->highlight_rect.h = ptr_text_holding->hight;


  // iterate through line untill highlight end element is found
  while(ptr_text_holding != ptr_EditLine->highlight_end_text && ptr_text_holding != NULL){

    // Minimum offset (top left)
    val_holding = QTS_GetAllingmentOffset(ptr_EditLine->ptr_Line, ptr_text_holding);
    if(val_holding < ptr_EditLine->highlight_rect.y) ptr_EditLine->highlight_rect.y = val_holding;

    // bottom right
    val_holding += (float)(ptr_text_holding->hight);
    val_holding -= ptr_EditLine->highlight_rect.y;
    if(val_holding > ptr_EditLine->highlight_rect.h) ptr_EditLine->highlight_rect.h = val_holding;

    // get next text element
    ptr_text_holding = ptr_text_holding->ptr_NextText;

  }

  // set sart and end x cordinates
  ptr_EditLine->highlight_rect.x = (float)(ptr_EditLine->x_cord_start);
  ptr_EditLine->highlight_rect.w = (float)(ptr_EditLine->x_cord_end - ptr_EditLine->x_cord_start);

  return true;
}





bool QTS_EditLine_DrawHighlight(SDL_Renderer *ptr_renderer, struct QTS_EditLine* ptr_EditLine){

  // Validate input
  if(!ptr_renderer || !ptr_EditLine) return false;
  if(!ptr_EditLine->highlight_present) return true;


  // The rect is provided by the edit line and pre calculated by the event processing function
  SDL_FRect croped_rect = ptr_EditLine->highlight_rect;
  float padding = roundf(croped_rect.h / 15.0f);



  //printf("pre \n");
  // Update x,y cordinates
  croped_rect.x = ptr_EditLine->line_rect.x + ptr_EditLine->highlight_rect.x;
  croped_rect.y = ptr_EditLine->line_rect.y + ptr_EditLine->highlight_rect.y;
  //printf("post \n");

  // remove padding from crop rect
  croped_rect.x += padding;
  croped_rect.y += padding;
  croped_rect.h -= (padding * 2.0f);
  croped_rect.w -= (padding * 2.0f);

  // Set renderer Draw collor
  if(ptr_EditLine->ptr_highlight_color){
    SDL_SetRenderDrawColor(ptr_renderer, ptr_EditLine->ptr_highlight_color->r, ptr_EditLine->ptr_highlight_color->g, ptr_EditLine->ptr_highlight_color->b, ptr_EditLine->ptr_highlight_color->a);
  }else{
    SDL_SetRenderDrawColor(ptr_renderer, 0x7F, 0x7F, 0xFF, 0x90);
  }

  // Render five rectangels
  SDL_FRect Rects[5];
  Rects[0] = croped_rect;

  (Rects[1]).x = croped_rect.x;
  (Rects[1]).y = croped_rect.y - padding;
  (Rects[1]).w = croped_rect.w;
  (Rects[1]).h = padding;

  (Rects[2]).x = croped_rect.x - padding;
  (Rects[2]).y = croped_rect.y;
  (Rects[2]).w = padding;
  (Rects[2]).h = croped_rect.h;

  (Rects[3]).x = croped_rect.x + croped_rect.w;
  (Rects[3]).y = croped_rect.y;
  (Rects[3]).w = padding;
  (Rects[3]).h = croped_rect.h;

  (Rects[4]).x = croped_rect.x;
  (Rects[4]).y = croped_rect.y + croped_rect.h;
  (Rects[4]).w = croped_rect.w;
  (Rects[4]).h = padding;

  SDL_RenderFillRects(ptr_renderer, Rects, 5);

  // Render four rounded corners
  //RenderCircleSectionFilled(ptr_renderer, (int)croped_rect.x, (int)croped_rect.y, (int)padding, PI_FLOAT / 2.0f, PI_FLOAT);
  RenderFillCircleQuater(ptr_renderer, ((int)roundf(croped_rect.x)) - 1, ((int)roundf(croped_rect.y)) - 1, ((int)padding) - 1, 2);
  //RenderCircleSectionFilled(ptr_renderer, (int)(croped_rect.x + croped_rect.w), (int)croped_rect.y, (int)padding, 0.0f, PI_FLOAT / 2.0f);
  RenderFillCircleQuater(ptr_renderer, (int)roundf(croped_rect.x + croped_rect.w), ((int)roundf(croped_rect.y)) - 1, ((int)padding) - 1, 1);
  //RenderCircleSectionFilled(ptr_renderer, (int)croped_rect.x, (int)(croped_rect.y + croped_rect.h), (int)padding, PI_FLOAT, (PI_FLOAT / 2.0f) * 3.0f);
  RenderFillCircleQuater(ptr_renderer, ((int)roundf(croped_rect.x)) - 1, (int)roundf(croped_rect.y + croped_rect.h), ((int)padding) - 1, 3);
  //RenderCircleSectionFilled(ptr_renderer, (int)(croped_rect.x + croped_rect.w), (int)(croped_rect.y + croped_rect.h), (int)padding, (PI_FLOAT / 2.0f) * 3.0f, PI_FLOAT * 2.0f);
  RenderFillCircleQuater(ptr_renderer, (int)roundf(croped_rect.x + croped_rect.w), (int)roundf(croped_rect.y + croped_rect.h), ((int)padding) - 1, 4);


  // Set renderer Draw collor to non transparent version
  if(ptr_EditLine->ptr_highlight_color){
    SDL_SetRenderDrawColor(ptr_renderer, ptr_EditLine->ptr_highlight_color->r, ptr_EditLine->ptr_highlight_color->g, ptr_EditLine->ptr_highlight_color->b, 0xFF);
  }else{
    SDL_SetRenderDrawColor(ptr_renderer, 0x7F, 0x7F, 0xFF, 0xFF);
  }

  // Render Edge Lines

  // Circles in the corners
  RenderCircleQuater(ptr_renderer, ((int)roundf(croped_rect.x)) - 1, ((int)roundf(croped_rect.y)) - 1, ((int)padding) - 1, 2);
  RenderCircleQuater(ptr_renderer, (int)roundf(croped_rect.x + croped_rect.w), ((int)roundf(croped_rect.y)) - 1, ((int)padding) - 1, 1);
  RenderCircleQuater(ptr_renderer, ((int)roundf(croped_rect.x)) - 1, (int)roundf(croped_rect.y + croped_rect.h), ((int)padding) - 1, 3);
  RenderCircleQuater(ptr_renderer, (int)roundf(croped_rect.x + croped_rect.w), (int)roundf(croped_rect.y + croped_rect.h), ((int)padding) - 1, 4);

  // Lines on the edges
  padding -= 1.0f;
  SDL_RenderLine(ptr_renderer, (Rects[1]).x, (Rects[1]).y, (Rects[1]).x + (Rects[1]).w, (Rects[1]).y);
  SDL_RenderLine(ptr_renderer, (Rects[2]).x, (Rects[2]).y, (Rects[2]).x, (Rects[2]).y + (Rects[2]).h);
  SDL_RenderLine(ptr_renderer, (Rects[3]).x + padding, (Rects[3]).y, (Rects[3]).x  + padding, (Rects[3]).y + (Rects[3]).h);
  SDL_RenderLine(ptr_renderer, (Rects[4]).x, (Rects[4]).y + padding, (Rects[4]).x + (Rects[4]).w, (Rects[4]).y + padding);



  return true;
}



// Function for Handeling mouse button down
// returns true if evnt was procesed and false if event did not affect the line
bool QTS_EditLine_HandleMouseButtonDown(struct QTS_EditLine* ptr_EditLine, SDL_Event* ptr_Event){

  // Validate Input
  if(!ptr_EditLine || !ptr_Event) return false;

  // return holding
  bool b_ret;

  // Test if event occured in interaction fild
  SDL_FRect interaction_rect;
  float scaling = ptr_EditLine->scaling * ptr_EditLine->scaling_offset;
  interaction_rect.x = ptr_EditLine->line_rect.x + ptr_EditLine->offset_x;
  interaction_rect.y = ptr_EditLine->line_rect.y + ptr_EditLine->offset_y;
  interaction_rect.w = ptr_EditLine->line_rect.w;
  interaction_rect.h = ptr_EditLine->line_rect.h;

  if(!is_in_FRect((int)(ptr_Event->button.x * scaling), (int)(ptr_Event->button.y * scaling), &interaction_rect)){
    ptr_EditLine->selected = false;
    ptr_EditLine->marking_active = false;
    return false;
  }

  // Test if is leftclick
  if(!(ptr_Event->button.button == SDL_BUTTON_LEFT)) return false;

  // set selected to true
  ptr_EditLine->selected = true;

  // set curser and hilight data points
  if(ptr_EditLine->enable_highlight){
    ptr_EditLine->marking_active = true;
    ptr_EditLine->highlight_present = false;
    ptr_EditLine->marking_backwards = false;
  }


  // get curser data
  b_ret = QTS_GetLineIncisionPoint(ptr_EditLine->ptr_Line, (int)((ptr_Event->button.x * scaling) - interaction_rect.x), &(ptr_EditLine->x_cord), &(ptr_EditLine->incission_text_element), &(ptr_EditLine->back_codepoints), &(ptr_EditLine->incision_codepoint), &(ptr_EditLine->incision_index), ptr_EditLine->crosskerneling);
  if(!b_ret) return false;

  // set curser last time to current time so that curser is imediatly visible
  ptr_EditLine->last_time = SDL_GetTicks();

  // set this point as hilight start point
  if(ptr_EditLine->incision_codepoint == -1){
    ptr_EditLine->highlight_start = ptr_EditLine->back_codepoints;
  }else{
    ptr_EditLine->highlight_start = ptr_EditLine->back_codepoints + ptr_EditLine->incision_codepoint + 1;
  }
  ptr_EditLine->x_cord_start = ptr_EditLine->x_cord;

  // set hilight start text object
  ptr_EditLine->highlight_start_text = ptr_EditLine->incission_text_element;

  return true;

}




// Function for handeling mouse button up
bool QTS_EditLine_HandleMouseButtonUp(struct QTS_EditLine* ptr_EditLine, SDL_Event* ptr_Event){

  // Validate Input
  if(!ptr_EditLine || !ptr_Event) return false;

  // no longer actively marking
  ptr_EditLine->marking_active = false;


  return true;
}



// Function for Handeling mouse motion
bool QTS_EditLine_HadleMoueMotion(struct QTS_EditLine* ptr_EditLine, SDL_Event* ptr_Event){

  // Validate Input
  if(!ptr_EditLine || !ptr_Event) return false;

  // If is not selected ignore
  if(!ptr_EditLine->selected || !ptr_EditLine->enable_highlight) return false;

  if(ptr_EditLine->marking_active){

    // update end of hilight
    // remember case where the hiolight start and end are backwards (in withc case they need to be flipped)

    // Create holding variables
    int highlight_end;
    int hilight_end_back_codepoints, hilight_end_ins_codepoint;
    int x_cord_end;
    struct QTS_Text* highlight_end_text;

    // Get mouse x possition relative to start of line
    float mouse_x;
    SDL_GetMouseState(&mouse_x, NULL);
    mouse_x = mouse_x * ptr_EditLine->scaling * ptr_EditLine->scaling_offset;
    mouse_x -= ptr_EditLine->line_rect.x;
    mouse_x += ptr_EditLine->offset_x;

    // get possition form event
    QTS_GetLineIncisionPoint(ptr_EditLine->ptr_Line, (int)mouse_x, &x_cord_end, &highlight_end_text, &hilight_end_back_codepoints, &hilight_end_ins_codepoint, NULL, ptr_EditLine->crosskerneling);

    // calculate hilight end codepoint
    if(hilight_end_ins_codepoint == -1){
      highlight_end = hilight_end_back_codepoints;
    }else{
      highlight_end = hilight_end_back_codepoints + hilight_end_ins_codepoint + 1;
    }

    // if the "end" is befor the beginning flip
    if(highlight_end <= ptr_EditLine->highlight_start){

      // flip
      // flip is was not already fliped
      if(!ptr_EditLine->marking_backwards){
        // end = start
        ptr_EditLine->highlight_end = ptr_EditLine->highlight_start;
        ptr_EditLine->x_cord_end = ptr_EditLine->x_cord_start;
        ptr_EditLine->highlight_end_text = ptr_EditLine->highlight_start_text;
      }
      ptr_EditLine->marking_backwards = true;

      // enter as begining
      ptr_EditLine->highlight_start = highlight_end;
      ptr_EditLine->x_cord_start = x_cord_end;
      ptr_EditLine->highlight_start_text = highlight_end_text;

    }else{

      // flip back
      if(ptr_EditLine->marking_backwards){
        // start = end
        ptr_EditLine->highlight_start = ptr_EditLine->highlight_end;
        ptr_EditLine->x_cord_start = ptr_EditLine->x_cord_end;
        ptr_EditLine->highlight_start_text = ptr_EditLine->highlight_end_text;
      }
      ptr_EditLine->marking_backwards = false;

      // enter as beginning
      ptr_EditLine->highlight_end = highlight_end;
      ptr_EditLine->x_cord_end = x_cord_end;
      ptr_EditLine->highlight_end_text = highlight_end_text;
    }

    // draw hilight if a fi a highlight is present (remember to calculate hilight with function first)
    // (a hilight is present if the hilighting encompases more than one codepoint)

    if(abs(ptr_EditLine->highlight_end - ptr_EditLine->highlight_start) < 1){
      // no hilight present
      ptr_EditLine->highlight_present = false;
      return true;
    }else{
      ptr_EditLine->highlight_present = true;
    }

    // Claculate hilight size
    QTS_EditLine_GetHighlightRect(ptr_EditLine);

    return true;
  }
  return false;
}






// Function for selecting the entiure Line
bool QTS_EditLine_HilightAll(struct QTS_EditLine* ptr_EditLine){

  // Validate input
  if(!ptr_EditLine) return false;
  if(!ptr_EditLine->ptr_Line->ptr_TextElements) return true;

  // configure backwards parameter to false
  ptr_EditLine->marking_backwards = false;

  // set hilight start to 0
  ptr_EditLine->highlight_start = 0;
  ptr_EditLine->x_cord_start = 0;
  ptr_EditLine->highlight_start_text = ptr_EditLine->ptr_Line->ptr_TextElements;

  // set hilight end to end of line
  int back_codepoints;
  int ins_codepoint;
  QTS_GetLineIncisionPoint(ptr_EditLine->ptr_Line, ptr_EditLine->ptr_Line->length, &(ptr_EditLine->x_cord_end), &(ptr_EditLine->highlight_end_text), &back_codepoints, &ins_codepoint, NULL, ptr_EditLine->crosskerneling);
  if(ins_codepoint == -1){
    ptr_EditLine->highlight_end = back_codepoints;
  }else{
    ptr_EditLine->highlight_end = back_codepoints + ins_codepoint + 1;
  }

  if(ptr_EditLine->highlight_end > 0){
    ptr_EditLine->highlight_present = true;
    // Claculate hilight size
    QTS_EditLine_GetHighlightRect(ptr_EditLine);
  }else{
    ptr_EditLine->highlight_present = false;
  }

  return true;
}




// Function for Copying selected text to Clippboard
bool QTS_EditLine_CopySelectedTextToClipboard(struct QTS_EditLine* ptr_EditLine){

  // Validate input
  if(!ptr_EditLine) return false;

  // check if there is a hilight present
  if(!ptr_EditLine->highlight_present) return true;

  // Return value holding variable
  bool b_ret;

  // Get the string from the hilight
  char* text;
  b_ret = QTS_GetLineSectionString(ptr_EditLine->ptr_Line, ptr_EditLine->highlight_start, ptr_EditLine->highlight_end, &text, NULL);
  if(!b_ret){
    free(text);
    return false;
  }

  // Set clippboard text
  SDL_SetClipboardText(text);
  free(text);

  return true;
}



// Function for Removing Hilighted Text
bool QTS_EditLine_DestroyHilightedText(SDL_Renderer *ptr_renderer, struct QTS_EditLine* ptr_EditLine){
  // update line after
  // leave an empty text element at beginning by cloning first text element without text


  // Validate Input
  if(!ptr_EditLine || !ptr_renderer) return false;
  if(!ptr_EditLine->highlight_present) return false;
  if(!ptr_EditLine->ptr_Line) return false;
  if(ptr_EditLine->highlight_start == ptr_EditLine->highlight_end){
    ptr_EditLine->highlight_present = false;
    return true;
  }

  // return holding variable
  bool b_ret;

  // 1. remove codepoint to codepoints
  int start_codepoint;
  int end_codepoint;
  struct QTS_Text* ptr_TextChain_removed = NULL;

  // enshore the right orientation of the values
  if(ptr_EditLine->highlight_start < ptr_EditLine->highlight_end){
    start_codepoint = ptr_EditLine->highlight_start;
    end_codepoint = ptr_EditLine->highlight_end;
  }else{
    start_codepoint = ptr_EditLine->highlight_end;
    end_codepoint = ptr_EditLine->highlight_start;
  }
  b_ret = QTS_RemoveSectionFromLine(ptr_renderer, ptr_EditLine->ptr_Line, start_codepoint,  end_codepoint, &ptr_TextChain_removed);
  if(!b_ret) {
    // Destroy the removed text objects
    QTS_DestroyTextChain(ptr_TextChain_removed); // to make shore there is no disconnected memmory
    return false;
  }
  if(!ptr_TextChain_removed) return false;

  // 2. check if line is empty
  //    if line is empty clone first removed text object without text and add it to the line
  if(!ptr_EditLine->ptr_Line->ptr_TextElements){

    // Create Clone
    //struct QTS_Text* ptr_TextElement_Clone = QTS_CreateText(ptr_renderer, ptr_TextChain_removed->ptr_font, NULL, 0, ptr_TextChain_removed->render_method, ptr_TextChain_removed->ptr_color_f, ptr_TextChain_removed->ptr_color_b, ptr_TextChain_removed->render_func, ptr_TextChain_removed->ptr_format_data);
    struct QTS_Text* ptr_TextElement_Clone = QTS_CloneTextFormat(ptr_renderer, ptr_TextChain_removed);

    // Add clone to Line
    QTS_AddTextToLine_After(ptr_EditLine->ptr_Line, NULL, ptr_TextElement_Clone);
  }

  // 3. Destroy the removed text elements
  QTS_DestroyTextChain(ptr_TextChain_removed);

  // 3.5. Update Line parameters
  QTS_UpdateLine(ptr_EditLine->ptr_Line);

  // 4. Set Hilight Predent to false and Last time to now
  ptr_EditLine->highlight_present = false;
  ptr_EditLine->marking_active = false;
  ptr_EditLine->last_time = SDL_GetTicks();

  // 5. set curser to where hilight was
  QTS_GetLineIncisionPoint_FromCodepoint(ptr_EditLine->ptr_Line, start_codepoint, &(ptr_EditLine->incission_text_element), &(ptr_EditLine->x_cord), &(ptr_EditLine->back_codepoints), &(ptr_EditLine->incision_codepoint), &(ptr_EditLine->incision_index), ptr_EditLine->crosskerneling);

  // 6. Nullify previus hilight text pointers (to avoid segmentation fault)
  ptr_EditLine->highlight_start_text = NULL;
  ptr_EditLine->highlight_end_text = NULL;

  return true;
}




// Function for Inserting text in to an edit line at the current curser possition
// text is a Null terminated string that will be copyed in to the line
// this function will not mve the incission point or prepare the string in annyway
bool QTS_EditLine_InsertText(SDL_Renderer *ptr_renderer, struct QTS_EditLine* ptr_EditLine, char* text){

  // Validate Input
  if(!ptr_renderer || !ptr_EditLine || !text) return false;
  if(!text[0]) return false;

  // Check if there is a curser text obj pointer
  if(!ptr_EditLine->incission_text_element) return false;

  // Return Holding variable
  bool b_ret;

  // Insert text in to text obj
  b_ret = QTS_InsertString(ptr_EditLine->incission_text_element, text, ptr_EditLine->incision_index);

  // Update the affected text element
  if(!QTS_UpdateText(ptr_renderer, ptr_EditLine->incission_text_element)){
    return false;
  }

  return b_ret;
}




// Function for resetting the Curser after an edditation has been made
// this updates the incission_text_element is the edit line
bool QTS_EditLine_UpdateCurser(struct QTS_EditLine* ptr_EditLine){

  // Validate Input
  if(!ptr_EditLine) return false;

  // Calculate Codepoint
  int codepoint;
  if(ptr_EditLine->incision_codepoint == -1){
    codepoint = ptr_EditLine->back_codepoints;
  }else{
    codepoint = ptr_EditLine->back_codepoints + ptr_EditLine->incision_codepoint + 1;
  }

  // special case if is at very beginning of line
  if(ptr_EditLine->incision_codepoint == -1 && ptr_EditLine->back_codepoints <= 0){
    ptr_EditLine->x_cord = 0;
    ptr_EditLine->incision_index = 0;
    ptr_EditLine->incission_text_element = ptr_EditLine->ptr_Line->ptr_TextElements;
    return true;
  }

  // Recalculate all values
  if(!QTS_GetLineIncisionPoint_FromCodepoint(ptr_EditLine->ptr_Line, codepoint, &(ptr_EditLine->incission_text_element), &(ptr_EditLine->x_cord), &(ptr_EditLine->back_codepoints), &(ptr_EditLine->incision_codepoint), &(ptr_EditLine->incision_index), ptr_EditLine->crosskerneling)){
    return false;
  }

  // Recalculate back codepoint
  ptr_EditLine->back_codepoints = codepoint - ptr_EditLine->incision_codepoint - 1;

  return true;
}



// Function for Updating the Hilight by codepoints
bool QTS_EditLine_UpdateHilight(struct QTS_EditLine* ptr_EditLine){

  // Validate Input
  if(!ptr_EditLine) return false;

  // return holding variable
  bool b_ret;

  // Nullify hilight text pointers
  ptr_EditLine->highlight_start_text = NULL;
  ptr_EditLine->highlight_end_text = NULL;

  // check if there is even suposed to be a hilight
  if(ptr_EditLine->highlight_start == ptr_EditLine->highlight_end){
    ptr_EditLine->highlight_present = false;
    return true;
  }

  ptr_EditLine->highlight_present = true;

  // Recalculate values
  b_ret = QTS_GetLineIncisionPoint_FromCodepoint(ptr_EditLine->ptr_Line, ptr_EditLine->highlight_start, &(ptr_EditLine->highlight_start_text), &(ptr_EditLine->x_cord_start), NULL, NULL, NULL, ptr_EditLine->crosskerneling);
  if(!b_ret) return false;

  b_ret = QTS_GetLineIncisionPoint_FromCodepoint(ptr_EditLine->ptr_Line, ptr_EditLine->highlight_end, &(ptr_EditLine->highlight_end_text), &(ptr_EditLine->x_cord_end), NULL, NULL, NULL, ptr_EditLine->crosskerneling);
  if(!b_ret) return false;

  // Recalculate Hilight Rect
  b_ret = QTS_EditLine_GetHighlightRect(ptr_EditLine);
  if(!b_ret) return false;

  return true;
}





// Function for Moving the Curser by a number of codepoints
bool QTS_EditLine_MoveCurser(struct QTS_EditLine* ptr_EditLine, int codepoints){

  // Validate input
  if(!ptr_EditLine) return false;
  if(codepoints == 0) return true;

  if(!QTS_MoveIncisionPoint(ptr_EditLine->ptr_Line, codepoints, &(ptr_EditLine->incission_text_element), &(ptr_EditLine->x_cord), &(ptr_EditLine->back_codepoints), &(ptr_EditLine->incision_codepoint), &(ptr_EditLine->incision_index), ptr_EditLine->crosskerneling)){
    return false;
  }

  // set curser time to now
  ptr_EditLine->last_time = SDL_GetTicks();

  return true;
}




bool QTS_EditLine_PasetTextFromClipboard(SDL_Renderer *ptr_renderer, struct QTS_EditLine* ptr_EditLine){

  // Validate Input
  if(!ptr_renderer ||!ptr_EditLine) return false;
  if(!ptr_EditLine->incission_text_element) return false;
  if(!ptr_EditLine->enable_edit) return false;

  // Return Holding variable
  bool b_ret;

  // if there is a highlight destroy that hight
  if(ptr_EditLine->highlight_present){
    b_ret = QTS_EditLine_DestroyHilightedText(ptr_renderer, ptr_EditLine);
    if(!b_ret) return false;
  }

  // obtain text from clippboard
  char* insert_text = SDL_GetClipboardText();
  if(!insert_text) return false;

  // remove new line symbols from string
  QTS_RemoveNewLineSymbols(insert_text);

  // count the codepoints in the string
  int count = QTS_CountCodepoints(insert_text);
  if(count <= 0){
    SDL_free(insert_text);
    return false;
  }

  // Insert the text at current curser location
  b_ret = QTS_EditLine_InsertText(ptr_renderer, ptr_EditLine, insert_text);
  if(!b_ret){
    SDL_free(insert_text);
    return false;
  }

  // move curser by the number of codepoints
  b_ret = QTS_EditLine_MoveCurser(ptr_EditLine, count);
  if(!b_ret){
    SDL_free(insert_text);
    return false;
  }

  // free text
  SDL_free(insert_text);

  return true;
}




// Function for inputting text at the curser location and moving the curser accordingly
bool QTS_EditLine_InputText(SDL_Renderer *ptr_renderer, struct QTS_EditLine* ptr_EditLine, char* ptr_text){

  // Validate Input
  if(!ptr_renderer || !ptr_EditLine || !ptr_text) return false;

  // Return Holding variable
  bool b_ret;

  // count the text codepoints
  int codepoints = QTS_CountCodepoints(ptr_text);
  if(codepoints <= 0) return true;

  // remove new line symbols from string
  QTS_RemoveNewLineSymbols(ptr_text);

  // Insert the Text
  b_ret = QTS_EditLine_InsertText(ptr_renderer, ptr_EditLine, ptr_text);
  if(!b_ret) return false;

  // Move the curser by the number of codepoints
  b_ret = QTS_EditLine_MoveCurser(ptr_EditLine, codepoints);
  if(!b_ret) return false;

  return true;
}




// Function for Handeling Text Input Event
bool QTS_EditLine_HandleTextInput(SDL_Renderer *ptr_renderer, struct QTS_EditLine* ptr_EditLine, SDL_Event* ptr_Event){

  // Validate Input
  if(!ptr_renderer || !ptr_EditLine || !ptr_Event) return false;
  if(!ptr_EditLine->enable_edit) return false;

  // Create a copy of the text input string
  char* text_copy = SDL_strdup(ptr_Event->text.text);

  // return Holding variable
  bool b_ret;

  b_ret = QTS_RemoveNewLineSymbols(text_copy);
  if(!b_ret){
    SDL_free(text_copy);
    return false;
  }

  // If there is a Hilight present delet hilight and then insert text
  if(ptr_EditLine->highlight_present){
    b_ret = QTS_EditLine_DestroyHilightedText(ptr_renderer, ptr_EditLine);
    if(!b_ret){
      SDL_free(text_copy);
      return false;
    }
  }

  // Insert the string
  b_ret = QTS_EditLine_InputText(ptr_renderer, ptr_EditLine, text_copy);

  SDL_free(text_copy);
  return b_ret;
}



// Function for Cutting the selected text (copy to clipboard and remove from line)
bool QTS_EditLine_CutSelectedText(SDL_Renderer *ptr_renderer, struct QTS_EditLine* ptr_EditLine){

  // Validate Input
  if(!ptr_renderer || !ptr_EditLine) return false;
  if(!ptr_EditLine->enable_edit) return false;

  // only do something if hilight is present
  if(!ptr_EditLine->highlight_present) return false;

  // Return Holding variable
  bool b_ret;

  // Copy selected text to clipboard
  b_ret = QTS_EditLine_CopySelectedTextToClipboard(ptr_EditLine);
  if(!b_ret) return false;

  // Destroy selected text
  b_ret = QTS_EditLine_DestroyHilightedText(ptr_renderer, ptr_EditLine);
  if(!b_ret) return false;

  return b_ret;
}




// Function for calculatin the curser codepoint
int QTS_EditLine_GetCurserCodepoint(struct QTS_EditLine* ptr_EditLine){

  // Validate Input
  if(!ptr_EditLine) return 0;

  // Calculate codepoint
  int codepoint = 0;
  if(ptr_EditLine->incision_codepoint == -1){
    codepoint = ptr_EditLine->back_codepoints;
  }else{
    codepoint = ptr_EditLine->back_codepoints + ptr_EditLine->incision_codepoint + 1;
  }

  return codepoint;
}



// Function for Handeling Backspace
bool QTS_EditLine_Backspace(SDL_Renderer *ptr_renderer, struct QTS_EditLine* ptr_EditLine){

  // Validate Input
  if(!ptr_renderer || !ptr_EditLine) return false;
  if(!ptr_EditLine->ptr_Line) return false;
  if(!ptr_EditLine->ptr_Line->ptr_TextElements) return false;
  if(!ptr_EditLine->enable_edit) return false;

  // Special case if line is empty
  if(!ptr_EditLine->ptr_Line->ptr_TextElements->string || !ptr_EditLine->ptr_Line->ptr_TextElements->spacing){
    if(!ptr_EditLine->ptr_Line->ptr_TextElements->ptr_NextText){
      return false;
    }
  }

  // Return Holding variable
  bool b_ret;

  // if a hilight is present then delete the hilight
  if(ptr_EditLine->highlight_present){
    b_ret = QTS_EditLine_DestroyHilightedText(ptr_renderer, ptr_EditLine);
    if(!b_ret) return false;
  }

  // Calculate true curser codepoint
  int curser_codepoint = QTS_EditLine_GetCurserCodepoint(ptr_EditLine);

  // Delete section

  // if is at beginning of line
  if(curser_codepoint == 0) return true;

  // remove section from line
  struct QTS_Text* ptr_removed_element = NULL;
  b_ret = QTS_RemoveSectionFromLine(ptr_renderer, ptr_EditLine->ptr_Line, curser_codepoint - 1,  curser_codepoint, &ptr_removed_element);
  if(!b_ret){
    QTS_DestroyTextOBJ(ptr_removed_element);
    return false;
  }

  // Move curser by one codepoint to the left
  QTS_EditLine_MoveCurser(ptr_EditLine, -1);

  // if line is empty insert format clone
  if(!(ptr_EditLine->ptr_Line->ptr_TextElements)){
    struct QTS_Text* ptr_format_clone = QTS_CloneTextFormat(ptr_renderer, ptr_removed_element);
    b_ret = QTS_AddTextToLine_After(ptr_EditLine->ptr_Line, NULL, ptr_format_clone);
    if(!b_ret){
      QTS_DestroyTextOBJ(ptr_format_clone);
      QTS_DestroyTextOBJ(ptr_removed_element);
      return false;
    }
  }

  // Destroy the removed part
  QTS_DestroyTextOBJ(ptr_removed_element);

  // Update
  ptr_EditLine->last_time = SDL_GetTicks();
  QTS_EditLine_UpdateCurser(ptr_EditLine);

  return true;
}






// Function for Handeling Backspace to beginning
bool QTS_EditLine_BackspaceToBeginning(SDL_Renderer *ptr_renderer, struct QTS_EditLine* ptr_EditLine){

  // Validate Input
  if(!ptr_renderer || !ptr_EditLine) return false;
  if(!ptr_EditLine->ptr_Line) return false;
  if(!ptr_EditLine->ptr_Line->ptr_TextElements) return false;
  if(!ptr_EditLine->enable_edit) return false;

  // Special case if line is empty
  if(!ptr_EditLine->ptr_Line->ptr_TextElements->string || !ptr_EditLine->ptr_Line->ptr_TextElements->spacing){
    if(!ptr_EditLine->ptr_Line->ptr_TextElements->ptr_NextText){
      return false;
    }
  }

  // Return Holding variable
  bool b_ret;

  // if a hilight is present then delete the hilight
  if(ptr_EditLine->highlight_present){
    b_ret = QTS_EditLine_DestroyHilightedText(ptr_renderer, ptr_EditLine);
    if(!b_ret) return false;
  }

  // Calculate true curser codepoint
  int curser_codepoint = QTS_EditLine_GetCurserCodepoint(ptr_EditLine);

  // Delete section

  // if is at beginning of line
  if(curser_codepoint == 0) return true;

  // remove section from line
  struct QTS_Text* ptr_removed_element = NULL;
  b_ret = QTS_RemoveSectionFromLine(ptr_renderer, ptr_EditLine->ptr_Line, 0,  curser_codepoint, &ptr_removed_element);
  if(!b_ret){
    QTS_DestroyTextChain(ptr_removed_element);
    return false;
  }

  // Move curser by one codepoint to the left
  QTS_EditLine_MoveCurser(ptr_EditLine, curser_codepoint * (-1));

  // if line is empty insert format clone
  if(!(ptr_EditLine->ptr_Line->ptr_TextElements)){
    struct QTS_Text* ptr_format_clone = QTS_CloneTextFormat(ptr_renderer, ptr_removed_element);
    b_ret = QTS_AddTextToLine_After(ptr_EditLine->ptr_Line, NULL, ptr_format_clone);
    if(!b_ret){
      QTS_DestroyTextOBJ(ptr_format_clone);
      QTS_DestroyTextChain(ptr_removed_element);
      return false;
    }
  }

  // Destroy the removed part
  QTS_DestroyTextChain(ptr_removed_element);

  // Update
  ptr_EditLine->last_time = SDL_GetTicks();
  QTS_EditLine_UpdateCurser(ptr_EditLine);

  return true;
}









// Function for Handeling Delete (removes next character)
bool QTS_EditLine_Delete(SDL_Renderer *ptr_renderer, struct QTS_EditLine* ptr_EditLine){

  // Validate Input
  if(!ptr_renderer || !ptr_EditLine) return false;
  if(!ptr_EditLine->ptr_Line) return false;
  if(!ptr_EditLine->ptr_Line->ptr_TextElements) return false;
  if(!ptr_EditLine->enable_edit) return false;

  // Special case if line is empty
  if(!ptr_EditLine->ptr_Line->ptr_TextElements->string || !ptr_EditLine->ptr_Line->ptr_TextElements->spacing){
    if(!ptr_EditLine->ptr_Line->ptr_TextElements->ptr_NextText){
      return false;
    }
  }

  // Return Holding variable
  bool b_ret;

  // if a hilight is present then delete the hilight
  if(ptr_EditLine->highlight_present){
    b_ret = QTS_EditLine_DestroyHilightedText(ptr_renderer, ptr_EditLine);
    if(!b_ret) return false;
  }

  // Calculate true curser codepoint
  int curser_codepoint = QTS_EditLine_GetCurserCodepoint(ptr_EditLine);

  // Delete section

  // if is at beginning of line
  //if(curser_codepoint == 0) return true;

  // remove section from line
  struct QTS_Text* ptr_removed_element = NULL;
  b_ret = QTS_RemoveSectionFromLine(ptr_renderer, ptr_EditLine->ptr_Line, curser_codepoint,  curser_codepoint + 1, &ptr_removed_element);
  if(!b_ret){
    QTS_DestroyTextOBJ(ptr_removed_element);
    return false;
  }

  // Move curser by one codepoint to the left
  //QTS_EditLine_MoveCurser(ptr_EditLine, 1);

  // if line is empty insert format clone
  if(!(ptr_EditLine->ptr_Line->ptr_TextElements)){
    struct QTS_Text* ptr_format_clone = QTS_CloneTextFormat(ptr_renderer, ptr_removed_element);
    b_ret = QTS_AddTextToLine_After(ptr_EditLine->ptr_Line, NULL, ptr_format_clone);
    if(!b_ret){
      QTS_DestroyTextOBJ(ptr_format_clone);
      QTS_DestroyTextOBJ(ptr_removed_element);
      return false;
    }
  }

  // Destroy the removed part
  QTS_DestroyTextOBJ(ptr_removed_element);

  // Update
  ptr_EditLine->last_time = SDL_GetTicks();
  QTS_EditLine_UpdateCurser(ptr_EditLine);

  return true;
}




// Function for Handeling Delete to End (removes next characters to End)
bool QTS_EditLine_DeleteToEnd(SDL_Renderer *ptr_renderer, struct QTS_EditLine* ptr_EditLine){

  // Validate Input
  if(!ptr_renderer || !ptr_EditLine) return false;
  if(!ptr_EditLine->ptr_Line) return false;
  if(!ptr_EditLine->ptr_Line->ptr_TextElements) return false;
  if(!ptr_EditLine->enable_edit) return false;

  // Special case if line is empty
  if(!ptr_EditLine->ptr_Line->ptr_TextElements->string || !ptr_EditLine->ptr_Line->ptr_TextElements->spacing){
    if(!ptr_EditLine->ptr_Line->ptr_TextElements->ptr_NextText){
      return false;
    }
  }

  // Return Holding variable
  bool b_ret;

  // if a hilight is present then delete the hilight
  if(ptr_EditLine->highlight_present){
    b_ret = QTS_EditLine_DestroyHilightedText(ptr_renderer, ptr_EditLine);
    if(!b_ret) return false;
  }

  // Calculate true curser codepoint
  int curser_codepoint = QTS_EditLine_GetCurserCodepoint(ptr_EditLine);

  // Delete section

  // if is at beginning of line
  //if(curser_codepoint == 0) return true;

  // remove section from line
  struct QTS_Text* ptr_removed_element = NULL;  // (if end_code_point = 0 then remove untill end)
  b_ret = QTS_RemoveSectionFromLine(ptr_renderer, ptr_EditLine->ptr_Line, curser_codepoint,  0, &ptr_removed_element);
  if(!b_ret){
    QTS_DestroyTextChain(ptr_removed_element);
    return false;
  }

  // Move curser by one codepoint to the left
  //QTS_EditLine_MoveCurser(ptr_EditLine, 1);

  // if line is empty insert format clone
  if(!(ptr_EditLine->ptr_Line->ptr_TextElements)){
    struct QTS_Text* ptr_format_clone = QTS_CloneTextFormat(ptr_renderer, ptr_removed_element);
    b_ret = QTS_AddTextToLine_After(ptr_EditLine->ptr_Line, NULL, ptr_format_clone);
    if(!b_ret){
      QTS_DestroyTextOBJ(ptr_format_clone);
      QTS_DestroyTextChain(ptr_removed_element);
      return false;
    }
  }

  // Destroy the removed part
  QTS_DestroyTextChain(ptr_removed_element);

  // Update
  ptr_EditLine->last_time = SDL_GetTicks();
  QTS_EditLine_UpdateCurser(ptr_EditLine);

  return true;
}



// Function for Escapint the edit Line
bool QTS_EditLine_Escape(struct QTS_EditLine* ptr_EditLine){

  // Validate Input
  if(!ptr_EditLine) return false;

  // Unselect and stop marking
  ptr_EditLine->marking_active = false;
  ptr_EditLine->selected = false;

  return true;
}



// Function for Moving the curser to the Beginning of the line
bool QTS_EditLine_MoveCurserToBegining(struct QTS_EditLine* ptr_EditLine){

  // Validate Input
  if(!ptr_EditLine) return false;

  // Move curser to beginning
  int true_codepoint = QTS_EditLine_GetCurserCodepoint(ptr_EditLine);

  if(true_codepoint <= 0) return true;

  QTS_EditLine_MoveCurser(ptr_EditLine, true_codepoint * (-1));

  // Update
  ptr_EditLine->last_time = SDL_GetTicks();
  QTS_EditLine_UpdateCurser(ptr_EditLine);

  return true;
}



// Function for Moving the cursert to the end of the line
bool QTS_EditLine_MoveCurserToEnd(struct QTS_EditLine* ptr_EditLine){

  // Validate Input
  if(!ptr_EditLine) return false;

  // Move curser to beginning
  int codepoint = QTS_CountCodepointsInLine(ptr_EditLine->ptr_Line);

  if(codepoint <= 0) return true;

  QTS_EditLine_MoveCurser(ptr_EditLine, codepoint);

  // Update
  ptr_EditLine->last_time = SDL_GetTicks();
  QTS_EditLine_UpdateCurser(ptr_EditLine);

  return true;
}





// Function for Hilighting the Word that the curser is currently in
bool QTS_EditLine_HilightWord(struct QTS_EditLine* ptr_EditLine){

  // Validate Input
  if(!ptr_EditLine) return false;
  if(!ptr_EditLine->incission_text_element) return false;

  // Return Holding variable
  bool b_ret;

  // Get the string from the Line
  char* string;
  b_ret = QTS_GetLineString(ptr_EditLine->ptr_Line, &string, NULL, NULL);
  if(!b_ret || !string[0]) return false;

  // Get the string length
  size_t srg_length = 0;
  while(string[srg_length]) srg_length++;

  // calculate true Incission index (for full line string)
  size_t true_incision_index = 0;
  struct QTS_Text* ptr_text_holding = ptr_EditLine->ptr_Line->ptr_TextElements;

  while(ptr_text_holding != ptr_EditLine->incission_text_element && ptr_text_holding){
    true_incision_index += ptr_text_holding->length - 1;
    ptr_text_holding = ptr_text_holding->ptr_NextText;
  }

  true_incision_index += ptr_EditLine->incision_index;

  // identify the words range in the string
  size_t index_start = true_incision_index;
  size_t index_end = true_incision_index;

  if(QTS_IsSeperationChar(string[true_incision_index])){
    free(string);
    return false;
  }

  while(!QTS_IsSeperationChar(string[index_start]) && index_start > 0) index_start--;
  while(!QTS_IsSeperationChar(string[index_end]) && index_end < srg_length) index_end++;

  if(index_start > 0) index_start++;

  // reverse process the indexes to codepoints
  int start_codepoint = QTS_IndexToCodepoint(string, index_start);
  int end_codepoint = QTS_IndexToCodepoint(string, index_end);

  // free the string
  free(string);

  // Hilight the codepoint range
  ptr_EditLine->highlight_start = start_codepoint;
  ptr_EditLine->highlight_end = end_codepoint;

  return QTS_EditLine_UpdateHilight(ptr_EditLine);
}






// End
