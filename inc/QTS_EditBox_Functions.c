// Include QTS_Header
#include "QualityTextSystem.h"
#include "dynamic_lines.h"


// Default Function for drawing the curser in a QTS EditBox
bool QTS_EditBox_DrawCurser(SDL_Renderer* ptr_renderer, struct QTS_EditBox* ptr_EditBox){

  // Validate Input
  if(!ptr_renderer || !ptr_EditBox) return false;

  // Base variables
  int x_cord, y_cord, length, thikness;

  // Get the current time for blinking intervall
  Uint32 current_time = SDL_GetTicks();

  // Handle blinking
  if(current_time - ptr_EditBox->last_time > 1000) ptr_EditBox->last_time = current_time;
  if(current_time - ptr_EditBox->last_time > 500) return true;

  // Calculate the X and Y cordinate
  x_cord = (int)ptr_EditBox->box_rect.x + ptr_EditBox->x_cord;
  y_cord = (int)ptr_EditBox->box_rect.y + ptr_EditBox->y_cord;

  // if the incission text element is named than draw the curser acording to its size
  if(ptr_EditBox->incission_text_element){
    // Get parameters from text element
    length = ptr_EditBox->incission_text_element->hight;
    y_cord += (int)QTS_GetAllingmentOffset(ptr_EditBox->ptr_incission_line, ptr_EditBox->incission_text_element);
  }else{
    // Get the parameters from the line
    length = ptr_EditBox->ptr_incission_line->hight - ptr_EditBox->ptr_incission_line->Spaceing_Up - ptr_EditBox->ptr_incission_line->Spaceing_Down;
    y_cord += ptr_EditBox->ptr_incission_line->Spaceing_Up;
  }

  // subtract font accent and decent from length
  //length -= abs(ptr_EditBox->incission_text_element->ascent);
  length -= abs(ptr_EditBox->incission_text_element->descent);

  // the thikness is allways proportional to the hight
  thikness = length / 60;
  // Enforce minimum thikness
  if(thikness < 1) thikness = 1 * ptr_EditBox->scaling * ptr_EditBox->scaling_offset;
  thikness = thikness * ptr_EditBox->scaling * ptr_EditBox->scaling_offset;

  // Draw curser
  if(ptr_EditBox->ptr_cursor_collor) {
    SDL_SetRenderDrawColor(ptr_renderer, ptr_EditBox->ptr_cursor_collor->r, ptr_EditBox->ptr_cursor_collor->g, ptr_EditBox->ptr_cursor_collor->b, ptr_EditBox->ptr_cursor_collor->a);
  }else{
    // Use Default collor
    SDL_SetRenderDrawColor(ptr_renderer, 0x80, 0x80, 0xFF, 0xFF);
  }
  return RenderLineDynamic(ptr_renderer, x_cord, y_cord, x_cord, y_cord + length, thikness);
}






// Default function for Drawing a Hiligt in a QTS_EditBox
bool QTS_EditBox_DrawHighlightRect(SDL_Renderer* ptr_renderer, struct QTS_EditBox* ptr_EditBox, SDL_FRect Hilight_Rect){

  // Validate input
  if(!ptr_renderer) return false;

  // The rect is provided by the edit line and pre calculated by the event processing function
  SDL_FRect croped_rect = Hilight_Rect;
  float padding = roundf(croped_rect.h / 15.0f);

  // remove padding from crop rect
  croped_rect.x += padding;
  croped_rect.y += padding;
  croped_rect.h -= (padding * 2.0f);
  croped_rect.w -= (padding * 2.0f);

  // Set renderer Draw collor
  if(ptr_EditBox->ptr_highlight_color){
    SDL_SetRenderDrawColor(ptr_renderer, ptr_EditBox->ptr_highlight_color->r, ptr_EditBox->ptr_highlight_color->g, ptr_EditBox->ptr_highlight_color->b, ptr_EditBox->ptr_highlight_color->a);
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
  if(ptr_EditBox->ptr_highlight_color){
    SDL_SetRenderDrawColor(ptr_renderer, ptr_EditBox->ptr_highlight_color->r, ptr_EditBox->ptr_highlight_color->g, ptr_EditBox->ptr_highlight_color->b, 0xFF);
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




// Function for Getting the Line from a y cordinate
// note: y cordiante must be relative to the box (top Left corner of the Box)
struct QTS_Line* QTS_EditBox_GetLineFromCordinate(struct QTS_Box* ptr_Box, int y_cord){

  // Veryfy input
  if(!ptr_Box) return NULL;
  if(!ptr_Box->ptr_LineElements) return NULL;
  if(y_cord <= 0) return ptr_Box->ptr_LineElements;

  // Itterate through Lines until Line or End is found
  struct QTS_Line* ptr_line_holding = ptr_Box->ptr_LineElements;
  int y_ref = 0;

  while(ptr_line_holding->ptr_NextLine && (y_ref + ptr_line_holding->hight) <= y_cord){

    // Add the hight of the Line to the reference count
    y_ref += ptr_line_holding->hight;

    // Move to next Line
    ptr_line_holding = ptr_line_holding->ptr_NextLine;

  }


  return ptr_line_holding;
}



// Function for Getting the Index of a Line in a Text Box
int QTS_EditBox_GetLineIndex(struct QTS_Box* ptr_Box, struct QTS_Line* ptr_line){

  // Validate Input
  if(!ptr_Box || !ptr_line) return 0;

  // itterate throught line untill is found or reached end
  struct QTS_Line* ptr_line_holding = ptr_Box->ptr_LineElements;
  int count = 0;

  while(ptr_line_holding && ptr_line_holding != ptr_line){

    count++;
    ptr_line_holding = ptr_line_holding->ptr_NextLine;

  }

  return count;
}



// Function for Getting a lines y cordinate relative to box
int QTS_EditBox_GetLineOffset(struct QTS_Box* ptr_Box, struct QTS_Line* ptr_Line){

  // Validate Input
  if(!ptr_Box || !ptr_Line) return 0;

  // Create Holding variabls
  struct QTS_Line* ptr_line_holding = ptr_Box->ptr_LineElements;
  int y_ref = 0;

  // Iterate Through Line untill found Line
  while(ptr_line_holding && ptr_line_holding != ptr_Line){

    // increment y ref by the hight
    y_ref += ptr_line_holding->hight;

    // Get next Line
    ptr_line_holding = ptr_line_holding->ptr_NextLine;
  }

  return y_ref;
}





// Function for Calculating the Hilight Rectangle
bool QTS_EditBox_CalculateHighlightRects(struct QTS_EditBox* ptr_EditBox){

  // Validate Input
  if(!ptr_EditBox) return false;
  if(!ptr_EditBox->hilight_start_line || !ptr_EditBox->highlight_start_text) return false;

  // Holding variables
  struct QTS_Text* ptr_text_holding = ptr_EditBox->highlight_start_text;
  float val_holding = 0.0f;

  // case Hilight is only in one Line
  if(ptr_EditBox->hilight_start_line == ptr_EditBox->hilight_end_line){

    // Calculate Hilight size from codepoint to codepoint for Hilight start

    ptr_EditBox->highlight_rect_start.y = QTS_GetAllingmentOffset(ptr_EditBox->hilight_start_line, ptr_text_holding);
    ptr_EditBox->highlight_rect_start.h = ptr_text_holding->hight;

    // Iterate through line untill hilight end element is found
    while(ptr_text_holding != ptr_EditBox->highlight_end_text && ptr_text_holding->ptr_NextText){

      // get next text element
      ptr_text_holding = ptr_text_holding->ptr_NextText;

      // Minimum offset (top left)
      val_holding = QTS_GetAllingmentOffset(ptr_EditBox->hilight_start_line, ptr_text_holding);
      if(val_holding < ptr_EditBox->highlight_rect_start.y) ptr_EditBox->highlight_rect_start.y = val_holding;

      // bottom right
      val_holding += (float)(ptr_text_holding->hight);
      val_holding -= ptr_EditBox->highlight_rect_start.y;
      if(val_holding > ptr_EditBox->highlight_rect_start.h) ptr_EditBox->highlight_rect_start.h = val_holding;

    }

    ptr_EditBox->highlight_rect_start.y += QTS_EditBox_GetLineOffset(ptr_EditBox->ptr_Box, ptr_EditBox->hilight_start_line);

    // set sart and end x cordinates
    ptr_EditBox->highlight_rect_start.x = (float)(ptr_EditBox->x_cord_start);
    ptr_EditBox->highlight_rect_start.w = (float)(ptr_EditBox->x_cord_end - ptr_EditBox->x_cord_start);


  }else{
    // Hilight start and end on diffreent lines

    // start rect goes from start text codepoint to end of line
    ptr_EditBox->highlight_rect_start.x = (float)(ptr_EditBox->x_cord_start);
    ptr_EditBox->highlight_rect_start.w = (float)(ptr_EditBox->hilight_start_line->length) - ptr_EditBox->highlight_rect_start.x;

    ptr_EditBox->highlight_rect_start.y = QTS_GetAllingmentOffset(ptr_EditBox->hilight_start_line, ptr_text_holding);
    ptr_EditBox->highlight_rect_start.h = 0.0f;

    // Minimize rect
    while(ptr_text_holding){

      // Minimum offset (top left)
      val_holding = QTS_GetAllingmentOffset(ptr_EditBox->hilight_start_line, ptr_text_holding);
      if(val_holding < ptr_EditBox->highlight_rect_start.y) ptr_EditBox->highlight_rect_start.y = val_holding;

      // bottom right
      val_holding += (float)(ptr_text_holding->hight);
      val_holding -= ptr_EditBox->highlight_rect_start.y;
      if(val_holding > ptr_EditBox->highlight_rect_start.h) ptr_EditBox->highlight_rect_start.h = val_holding;

      // get next text element
      ptr_text_holding = ptr_text_holding->ptr_NextText;
    }

    ptr_EditBox->highlight_rect_start.y += QTS_EditBox_GetLineOffset(ptr_EditBox->ptr_Box, ptr_EditBox->hilight_start_line);
    //ptr_EditBox->highlight_rect_start.h = (float)(ptr_EditBox->hilight_start_line->hight);



    // End rect goes from begining of line to end text

    // reset holding variables
    ptr_text_holding = ptr_EditBox->hilight_end_line->ptr_TextElements;
    val_holding = 0.0f;



    ptr_EditBox->highlight_rect_end.x = 0.0f;
    ptr_EditBox->highlight_rect_end.w = (float)(ptr_EditBox->x_cord_end);

    ptr_EditBox->highlight_rect_end.y = QTS_GetAllingmentOffset(ptr_EditBox->hilight_end_line, ptr_text_holding);

    // Minimum offset (top left)
    val_holding = QTS_GetAllingmentOffset(ptr_EditBox->hilight_end_line, ptr_text_holding);
    if(val_holding < ptr_EditBox->highlight_rect_end.y) ptr_EditBox->highlight_rect_end.y = val_holding;

    // bottom right
    val_holding += (float)(ptr_text_holding->hight);
    val_holding -= ptr_EditBox->highlight_rect_end.y;
    ptr_EditBox->highlight_rect_end.h = val_holding;

    // Minimize Rect
    while(ptr_text_holding->ptr_NextText && ptr_text_holding != ptr_EditBox->highlight_end_text){

      // get next text element
      ptr_text_holding = ptr_text_holding->ptr_NextText;

      // Minimum offset (top left)
      val_holding = QTS_GetAllingmentOffset(ptr_EditBox->hilight_end_line, ptr_text_holding);
      if(val_holding < ptr_EditBox->highlight_rect_end.y) ptr_EditBox->highlight_rect_end.y = val_holding;

      // bottom right
      val_holding += (float)(ptr_text_holding->hight);
      val_holding -= ptr_EditBox->highlight_rect_end.y;
      if(val_holding > ptr_EditBox->highlight_rect_end.h) ptr_EditBox->highlight_rect_end.h = val_holding;

    }

    ptr_EditBox->highlight_rect_end.y += QTS_EditBox_GetLineOffset(ptr_EditBox->ptr_Box, ptr_EditBox->hilight_end_line);
    //ptr_EditBox->highlight_rect_end.h = (float)(ptr_EditBox->hilight_end_line->hight);

  }

  return true;
}





// Function for Drawing the Hilights
bool QTS_EditBox_DrawHilights(SDL_Renderer* ptr_renderer, struct QTS_EditBox* ptr_EditBox){

  // Validate the Input
  if(!ptr_renderer || !ptr_EditBox) return false;
  if(!ptr_EditBox->highlight_present) return true;

  // Holding variable
  SDL_FRect holding_line_rect;

  // Render start Rect
  holding_line_rect = ptr_EditBox->highlight_rect_start;
  holding_line_rect.x += ptr_EditBox->box_rect.x;
  holding_line_rect.y += ptr_EditBox->box_rect.y;

  // in case Line is Empty
  if(holding_line_rect.w <= 0) holding_line_rect.w = holding_line_rect.h / 2.0f;


  if(ptr_EditBox->highlight_render_func) ptr_EditBox->highlight_render_func(ptr_renderer, ptr_EditBox, holding_line_rect);

  // if only one line Hilight
  if(ptr_EditBox->hilight_start_line == ptr_EditBox->hilight_end_line) return true;

  // Hilight all the lines in between
  struct QTS_Line* ptr_line_holding = ptr_EditBox->hilight_start_line;
  float offset = (float)QTS_EditBox_GetLineOffset(ptr_EditBox->ptr_Box, ptr_line_holding);

  // increment offset
  offset += (float)(ptr_line_holding->hight);

  while(ptr_line_holding->ptr_NextLine != ptr_EditBox->hilight_end_line && ptr_line_holding->ptr_NextLine){

    // get next Line
    ptr_line_holding = ptr_line_holding->ptr_NextLine;

    // make rect the size and possition of line
    holding_line_rect.x = ptr_EditBox->box_rect.x;
    holding_line_rect.y = ptr_EditBox->box_rect.y + offset;// + (float)(ptr_line_holding->Spaceing_Up);

    holding_line_rect.w = ptr_line_holding->length;
    holding_line_rect.h = ptr_line_holding->hight;// - ptr_line_holding->Spaceing_Up - ptr_line_holding->Spaceing_Down;

    // in case Line is Empty
    if(holding_line_rect.w <= 0) holding_line_rect.w = holding_line_rect.h / 2.0f;

    // Draw
    if(ptr_EditBox->highlight_render_func) ptr_EditBox->highlight_render_func(ptr_renderer, ptr_EditBox, holding_line_rect);

    // increment offset
    offset += (float)(ptr_line_holding->hight);

  }


  // Hilight End line
  holding_line_rect = ptr_EditBox->highlight_rect_end;
  holding_line_rect.x += ptr_EditBox->box_rect.x;
  holding_line_rect.y += ptr_EditBox->box_rect.y;
  /*
  SDL_RenderRect(ptr_renderer, &holding_line_rect);
  */
  if(ptr_EditBox->highlight_render_func) ptr_EditBox->highlight_render_func(ptr_renderer, ptr_EditBox, holding_line_rect);

  return true;
}



// Function for Flipping the EditBox Hilight (does not touch ptr_EditBox->marking_backwards or ptr_EditBox->highlight_present)
void QTS_EditBox_FlipHilight(struct QTS_EditBox* ptr_EditBox){

  // Validate Input
  if(!ptr_EditBox) return;

  // create and set Holding variables
  int highlight_start = ptr_EditBox->highlight_start;
  int x_cord_start = ptr_EditBox->x_cord_start;
  SDL_FRect highlight_rect_start = ptr_EditBox->highlight_rect_start;
  struct QTS_Line* hilight_start_line = ptr_EditBox->hilight_start_line;
  struct QTS_Text* highlight_start_text = ptr_EditBox->highlight_start_text;

  // Assing start, end
  ptr_EditBox->highlight_start = ptr_EditBox->highlight_end;
  ptr_EditBox->x_cord_start = ptr_EditBox->x_cord_end;
  ptr_EditBox->highlight_rect_start = ptr_EditBox->highlight_rect_end;
  ptr_EditBox->hilight_start_line = ptr_EditBox->hilight_end_line;
  ptr_EditBox->highlight_start_text = ptr_EditBox->highlight_end_text;

  ptr_EditBox->highlight_end = highlight_start;
  ptr_EditBox->x_cord_end = x_cord_start;
  ptr_EditBox->highlight_rect_end = highlight_rect_start;
  ptr_EditBox->hilight_end_line = hilight_start_line;
  ptr_EditBox->highlight_end_text = highlight_start_text;

  return;
}






















// Function for Handeling mouse button down
// returns true if evnt was procesed and false if event did not affect the line
bool QTS_EditBox_HandleMouseButtonDown(struct QTS_EditBox* ptr_EditBox, SDL_Event* ptr_Event){

  // Validate Input
  if(!ptr_EditBox || !ptr_Event) return false;

  // return holding
  bool b_ret;

  // Test if event occured in interaction fild
  SDL_FRect interaction_rect;
  float scaling = ptr_EditBox->scaling * ptr_EditBox->scaling_offset;
  interaction_rect.x = ptr_EditBox->box_rect.x + ptr_EditBox->offset_x;
  interaction_rect.y = ptr_EditBox->box_rect.y + ptr_EditBox->offset_y;
  interaction_rect.w = ptr_EditBox->box_rect.w;
  interaction_rect.h = ptr_EditBox->box_rect.h;


  if(!is_in_FRect((int)(ptr_Event->button.x * scaling), (int)(ptr_Event->button.y * scaling), &interaction_rect)){
    ptr_EditBox->selected = false;
    ptr_EditBox->marking_active = false;
    return false;
  }

  // Test if is leftclick
  if(!(ptr_Event->button.button == SDL_BUTTON_LEFT)) return false;

  // set selected to true
  ptr_EditBox->selected = true;

  // set curser and hilight data points
  if(ptr_EditBox->enable_highlight){
    ptr_EditBox->marking_active = true;
    ptr_EditBox->highlight_present = false;
    ptr_EditBox->marking_backwards = false;
  }

  // Get the Line that was ckicked
  ptr_EditBox->ptr_incission_line = QTS_EditBox_GetLineFromCordinate(ptr_EditBox->ptr_Box, (int)((ptr_Event->button.y * scaling) - interaction_rect.y));
  ptr_EditBox->y_cord = QTS_EditBox_GetLineOffset(ptr_EditBox->ptr_Box, ptr_EditBox->ptr_incission_line);


  // get curser data
  b_ret = QTS_GetLineIncisionPoint(ptr_EditBox->ptr_incission_line, (int)((ptr_Event->button.x * scaling) - interaction_rect.x), &(ptr_EditBox->x_cord), &(ptr_EditBox->incission_text_element), &(ptr_EditBox->back_codepoints), &(ptr_EditBox->incision_codepoint), &(ptr_EditBox->incision_index), ptr_EditBox->crosskerneling);
  if(!b_ret) return false;

  // set curser last time to current time so that curser is imediatly visible
  ptr_EditBox->last_time = SDL_GetTicks();

  // set this point as hilight start point
  if(ptr_EditBox->incision_codepoint == -1){
    ptr_EditBox->highlight_start = ptr_EditBox->back_codepoints;
  }else{
    ptr_EditBox->highlight_start = ptr_EditBox->back_codepoints + ptr_EditBox->incision_codepoint + 1;
  }
  ptr_EditBox->x_cord_start = ptr_EditBox->x_cord;

  // set hilight start text object and Line
  ptr_EditBox->highlight_start_text = ptr_EditBox->incission_text_element;
  ptr_EditBox->hilight_start_line = ptr_EditBox->ptr_incission_line;
  ptr_EditBox->hilight_end_line = NULL;

  // if line is empty
  if(!ptr_EditBox->incission_text_element->string){
    ptr_EditBox->incision_codepoint = -1;
  }

  return true;
}




// Function for handeling mouse button up
bool QTS_EditBox_HandleMouseButtonUp(struct QTS_EditBox* ptr_EditBox, SDL_Event* ptr_Event){

  // Validate Input
  if(!ptr_EditBox || !ptr_Event) return false;

  // no longer actively marking
  ptr_EditBox->marking_active = false;

  return true;
}



// Function for Handeling mouse motion
bool QTS_EditBox_HadleMoueMotion(struct QTS_EditBox* ptr_EditBox, SDL_Event* ptr_Event){

  // Validate Input
  if(!ptr_EditBox || !ptr_Event) return false;

  // If is not selected ignore
  if(!ptr_EditBox->selected || !ptr_EditBox->enable_highlight) return false;

  if(ptr_EditBox->marking_active){

    // update end of hilight
    // remember case where the hiolight start and end are backwards (in withc case they need to be flipped)

    // Create holding variables
    int highlight_end;
    int hilight_end_back_codepoints, hilight_end_ins_codepoint;
    int x_cord_end;
    struct QTS_Text* highlight_end_text;
    struct QTS_Line* highlight_end_line;
    //struct QTS_Line* highlight_start_line;
    //SDL_FRect holding_rect;

    // Get mouse x possition relative to start of line
    float mouse_x, mouse_y;
    SDL_GetMouseState(&mouse_x, &mouse_y);

    mouse_x = mouse_x * ptr_EditBox->scaling * ptr_EditBox->scaling_offset;
    mouse_x -= ptr_EditBox->box_rect.x;
    mouse_x += ptr_EditBox->offset_x;

    if(mouse_x < 0) return false;

    mouse_y = mouse_y * ptr_EditBox->scaling * ptr_EditBox->scaling_offset;
    mouse_y -= ptr_EditBox->box_rect.y;
    mouse_y += ptr_EditBox->offset_y;


    // Get the Line
    highlight_end_line = QTS_EditBox_GetLineFromCordinate(ptr_EditBox->ptr_Box, (int)mouse_y);

    // get possition form event
    QTS_GetLineIncisionPoint(highlight_end_line, (int)mouse_x, &x_cord_end, &highlight_end_text, &hilight_end_back_codepoints, &hilight_end_ins_codepoint, NULL, ptr_EditBox->crosskerneling);

    // calculate hilight end codepoint
    if(hilight_end_ins_codepoint == -1){
      highlight_end = hilight_end_back_codepoints;
    }else{
      highlight_end = hilight_end_back_codepoints + hilight_end_ins_codepoint + 1;
    }



    // if was marking backwards flip
    if(ptr_EditBox->marking_backwards){
      QTS_EditBox_FlipHilight(ptr_EditBox);
      ptr_EditBox->marking_backwards = false;
    }

    int start_line_index = QTS_EditBox_GetLineIndex(ptr_EditBox->ptr_Box, ptr_EditBox->hilight_start_line);
    int end_line_index   = QTS_EditBox_GetLineIndex(ptr_EditBox->ptr_Box, ptr_EditBox->hilight_end_line);


    // if hilight start and end are in same line
    if(ptr_EditBox->hilight_start_line == highlight_end_line){

      // Like in edit Line

      // Enter forward and optionaly flip later
      // enter as end
      ptr_EditBox->highlight_end      = highlight_end;
      ptr_EditBox->x_cord_end         = x_cord_end;
      ptr_EditBox->highlight_end_text = highlight_end_text;

      // Pass Line to EditBox (same line so assimgment order doesent matter)
      ptr_EditBox->hilight_start_line = highlight_end_line;
      ptr_EditBox->hilight_end_line = highlight_end_line;

      // if (endline is before start line) then flip the hilight
      if(ptr_EditBox->highlight_end < ptr_EditBox->highlight_start){
        QTS_EditBox_FlipHilight(ptr_EditBox);
        ptr_EditBox->marking_backwards = true;
      }

      // draw hilight if a if a highlight is present (remember to calculate hilight with function first)
      // (a hilight is present if the hilighting encompases more than one codepoint)

      if(abs(ptr_EditBox->highlight_end - ptr_EditBox->highlight_start) < 1){
        // no hilight present
        ptr_EditBox->highlight_present = false;
        return true;
      }else{
        ptr_EditBox->highlight_present = true;
      }


    }else{
      // Hilighting acrose two or more lines

      // new solution

      // Always enter forward and if marking backwards then flip

      // enter as end
      ptr_EditBox->highlight_end      = highlight_end;
      ptr_EditBox->x_cord_end         = x_cord_end;
      ptr_EditBox->hilight_end_line   = highlight_end_line;
      ptr_EditBox->highlight_end_text = highlight_end_text;

      // if (endline is before start line) then flip the hilight
      if(end_line_index < start_line_index){
        QTS_EditBox_FlipHilight(ptr_EditBox);
        ptr_EditBox->marking_backwards = true;
      }

      // Determine if a Hilight is present
      ptr_EditBox->highlight_present = true;

    }

    // calculate Hilight rectangles
    QTS_EditBox_CalculateHighlightRects(ptr_EditBox);

    return true;
  }
  return false;
}































// Function for selecting the entiure Line
bool QTS_EditBox_HilightAll(struct QTS_EditBox* ptr_EditBox){

  // Validate Input
  if(!ptr_EditBox) return false;
  if(!(ptr_EditBox->ptr_Box)) return false;
  if(!(ptr_EditBox->ptr_Box->ptr_LineElements)) return false;
  if(!(ptr_EditBox->ptr_Box->ptr_LineElements->ptr_TextElements)) return false;

  // Configure Hilight parameters
  ptr_EditBox->marking_backwards = false;

  // Hilight Start
  ptr_EditBox->highlight_start = 0;
  ptr_EditBox->x_cord_start = 0;
  ptr_EditBox->hilight_start_line = ptr_EditBox->ptr_Box->ptr_LineElements;
  ptr_EditBox->highlight_start_text = ptr_EditBox->ptr_Box->ptr_LineElements->ptr_TextElements;

  // Highlight End

  // Get last Line and Text Element
  struct QTS_Line* ptr_line_end = ptr_EditBox->hilight_start_line;
  //struct QTS_Text* ptr_text_end = NULL;

  // Get Last Line
  while(ptr_line_end->ptr_NextLine) ptr_line_end = ptr_line_end->ptr_NextLine;
  ptr_EditBox->hilight_end_line = ptr_line_end;

  // set hilight end to end of line
  int back_codepoints;
  int ins_codepoint;
  QTS_GetLineIncisionPoint(ptr_line_end, ptr_line_end->length, &(ptr_EditBox->x_cord_end), &(ptr_EditBox->highlight_end_text), &back_codepoints, &ins_codepoint, NULL, ptr_EditBox->crosskerneling);
  if(ins_codepoint == -1){
    ptr_EditBox->highlight_end = back_codepoints;
  }else{
    ptr_EditBox->highlight_end = back_codepoints + ins_codepoint + 1;
  }

  if(ptr_EditBox->highlight_end > 0 || (ptr_EditBox->hilight_start_line != ptr_EditBox->hilight_end_line)){
    ptr_EditBox->highlight_present = true;
    // Claculate hilight size
    QTS_EditBox_CalculateHighlightRects(ptr_EditBox);
  }else{
    ptr_EditBox->highlight_present = false;
  }

  return true;
}







// Function that creates a string that contains the selected text in the Edit box
// ptr_size is where the size of the string will be written. (is allowd to be NULL)
char* QTS_EditBox_GetSelectedText(struct QTS_EditBox* ptr_EditBox, size_t* ptr_size){

  // Validate Input
  if(!ptr_EditBox) return NULL;
  if(!ptr_EditBox->ptr_Box) return NULL;
  if(!ptr_EditBox->ptr_Box->ptr_LineElements) return NULL;
  if(!ptr_EditBox->highlight_present) return NULL;

  // Holding variables
  bool b_ret;

  // calculate size of section
  char* start_section = NULL;
  size_t size_start_section = 0;
  char* end_section = NULL;
  size_t size_end_section = 0;

  // in case is single line Hilight
  if(ptr_EditBox->hilight_start_line == ptr_EditBox->hilight_end_line){
    QTS_GetLineSectionString(ptr_EditBox->hilight_start_line, ptr_EditBox->highlight_start, ptr_EditBox->highlight_end, &start_section, &size_start_section);
    if(ptr_size) *ptr_size = size_start_section;
    return start_section;
  }

  // is Multiline Hilight

  // Get codepoint lengths
  int start_cp_length = QTS_GetLineSize_CP(ptr_EditBox->hilight_start_line);
  //int end_cp_length = QTS_GetLineSize_CP(ptr_EditBox->hilight_end_line);

  // Get start and end section
  b_ret = QTS_GetLineSectionString(ptr_EditBox->hilight_start_line, ptr_EditBox->highlight_start, start_cp_length, &start_section, &size_start_section);
  if(!b_ret) return NULL;

  b_ret = QTS_GetLineSectionString(ptr_EditBox->hilight_end_line, 0, ptr_EditBox->highlight_end, &end_section, &size_end_section);
  if(!b_ret){
    free(start_section);
    return NULL;
  }

  // Add up the Lengths of the Lines inbetween
  size_t length = 0;
  struct QTS_Line* ptr_line_holding = ptr_EditBox->hilight_start_line->ptr_NextLine;

  while(ptr_line_holding->ptr_NextLine && ptr_line_holding->ptr_NextLine != ptr_EditBox->hilight_end_line){

    // get the string Length of the Line
    length += QTS_GetLineSize_ST(ptr_line_holding);
    length += 1; // for new line symbol

    ptr_line_holding = ptr_line_holding->ptr_NextLine;
  }


  // Add the Line Lengths of start and end
  length += size_start_section + 1;
  length += size_end_section + 1;

  // Allocate a string
  char* section_string = (char*)calloc(length, sizeof(char));

  // copy string together
  size_t copy_count = 0;
  size_t holding = 0;
  char* str_holding = NULL;


  // copy start
  memcpy(section_string, start_section, size_start_section);
  copy_count += size_start_section; // may need to add  +1 here

  // Add new line
  section_string[copy_count - 1] = '\n';

  // copy lines inbetween
  ptr_line_holding = ptr_EditBox->hilight_start_line->ptr_NextLine;
  while(ptr_line_holding && ptr_line_holding != ptr_EditBox->hilight_end_line){

    // Get the string of the Line (allocates a string to str_holding)
    QTS_GetLineString(ptr_line_holding, &str_holding, NULL, &holding);

    // increment holding
    holding += 1;

    // only copy something if a string was given
    if(str_holding){

      // Copy the string in to the final string
      memcpy(&(section_string[copy_count]), str_holding, holding);

      // free the holding string allocated by QTS_GetLineString function
      free(str_holding);
      str_holding = NULL;
    }

    // increment copy count
    copy_count += holding;

    // Add new line symbol
    section_string[copy_count - 1] = '\n';

    // Get next Line
    ptr_line_holding = ptr_line_holding->ptr_NextLine;
  }

  // copy End
  if(end_section) memcpy(&(section_string[copy_count]), end_section, size_end_section);
  copy_count += size_end_section; // may need to add +1 Here.

  // Add null termination
  section_string[copy_count - 1] = 0x00;

  // Free start and end section strings
  free(start_section);
  free(end_section);

  // start and end strings that where allocated earlyer
  if(ptr_size) *ptr_size = length;
  return section_string;
}




// Function for Copying selected text to Clippboard
bool QTS_EditBox_CopySelectedTextToClipboard(struct QTS_EditBox* ptr_EditBox){

  // Validate Input
  if(!ptr_EditBox) return false;
  if(!ptr_EditBox->highlight_present) return false;

  // Get the selected Text
  char* section_string = QTS_EditBox_GetSelectedText(ptr_EditBox, NULL);
  if(!section_string) return false;

  // Set as clipboard Text
  SDL_SetClipboardText(section_string);

  // free the string
  free(section_string);

  // return false (no edits made)
  return true;
}



// Function for Removing Hilighted Text
bool QTS_EditBox_DestroyHilightedText(SDL_Renderer *ptr_renderer, struct QTS_EditBox* ptr_EditBox){
  // update Box after
  // leave an empty text element at beginning by cloning first text element without text

  // Validate input
  if(!ptr_renderer || !ptr_EditBox) return false;
  if(!ptr_EditBox->hilight_start_line) return false;
  if(!ptr_EditBox->highlight_present) return false;

  // return holding variable
  bool b_ret;

  // 1. remove codepoint to codepoints
  int start_codepoint = ptr_EditBox->highlight_start;
  int end_codepoint = ptr_EditBox->highlight_end;
  struct QTS_Text* ptr_TextChain_removed = NULL;
  struct QTS_Line* ptr_line_holding = NULL;
  struct QTS_Text* ptr_TextElement_Clone = NULL;

  // Special case: single Line Hilight
  if(ptr_EditBox->hilight_start_line == ptr_EditBox->hilight_end_line){

    b_ret = QTS_RemoveSectionFromLine(ptr_renderer, ptr_EditBox->hilight_start_line, start_codepoint,  end_codepoint, &ptr_TextChain_removed);
    if(!b_ret) {
      // Destroy the removed text objects
      QTS_DestroyTextChain(ptr_TextChain_removed); // to make shore there is no disconnected memmory
      return false;
    }
    if(!ptr_TextChain_removed) return false;

    // 2. check if line is empty
    //    if line is empty clone first removed text object without text and add it to the line
    if(!ptr_EditBox->hilight_start_line->ptr_TextElements){

      // Create Clone so that Edit Box is not completly empty and has a format to use
      //struct QTS_Text* ptr_TextElement_Clone = QTS_CreateText(ptr_renderer, ptr_TextChain_removed->ptr_font, NULL, 0, ptr_TextChain_removed->render_method, ptr_TextChain_removed->ptr_color_f, ptr_TextChain_removed->ptr_color_b, ptr_TextChain_removed->render_func, ptr_TextChain_removed->ptr_format_data);
      ptr_TextElement_Clone = QTS_CloneTextFormat(ptr_renderer, ptr_TextChain_removed);

      // Add clone to Line
      QTS_AddTextToLine_After(ptr_EditBox->hilight_start_line, NULL, ptr_TextElement_Clone);

    } // end of Line is Empty

    // 3. Destroy the removed text elements
    QTS_DestroyTextChain(ptr_TextChain_removed);

    // 3.5. Update Line parameters
    QTS_UpdateLine(ptr_EditBox->hilight_start_line); //

    // 4. Set Hilight Present to false and Last time to now
    ptr_EditBox->highlight_present = false;
    ptr_EditBox->marking_active = false;
    ptr_EditBox->last_time = SDL_GetTicks();

    // 5. set curser to where hilight was
    QTS_GetLineIncisionPoint_FromCodepoint(ptr_EditBox->hilight_start_line, start_codepoint, &(ptr_EditBox->incission_text_element), &(ptr_EditBox->x_cord), &(ptr_EditBox->back_codepoints), &(ptr_EditBox->incision_codepoint), &(ptr_EditBox->incision_index), ptr_EditBox->crosskerneling);
    ptr_EditBox->y_cord = QTS_EditBox_GetLineOffset(ptr_EditBox->ptr_Box, ptr_EditBox->hilight_start_line);

    // 6. Nullify previus hilight text pointers (to avoid segmentation fault)
    ptr_EditBox->highlight_start_text = NULL;
    ptr_EditBox->highlight_end_text = NULL;

    return true;
  }// End of Special case: single Line Hilight



  // Normal Case: Multiline Hilight
  if(!ptr_EditBox->hilight_start_line || !ptr_EditBox->hilight_end_line) return false;

  // Remove Lines in between start and end Line
  ptr_line_holding = NULL;

  // if there is a section
  if(ptr_EditBox->hilight_start_line->ptr_NextLine != ptr_EditBox->hilight_end_line){

    ptr_line_holding = ptr_EditBox->hilight_start_line->ptr_NextLine;

    // Nullify Pointers for Full disconection
    ptr_EditBox->hilight_start_line->ptr_NextLine->ptr_PrevLine = NULL;
    ptr_EditBox->hilight_end_line->ptr_PrevLine->ptr_NextLine = NULL;

    ptr_EditBox->hilight_start_line->ptr_NextLine = ptr_EditBox->hilight_end_line;
    ptr_EditBox->hilight_end_line->ptr_PrevLine = ptr_EditBox->hilight_start_line;

    // Destroy the Line chain that was Removed
    QTS_DestroyLineChain(ptr_line_holding);

  }

  ptr_EditBox->ptr_incission_line = ptr_EditBox->hilight_start_line;


  // Remove section from start Line
  // only do if line is not empty
  if(ptr_EditBox->hilight_start_line->ptr_TextElements->string){
    b_ret = QTS_RemoveSectionFromLine(ptr_renderer, ptr_EditBox->hilight_start_line, start_codepoint,  0, &ptr_TextChain_removed); // end codepoint = 0 implys remove untill end of line
    if(!b_ret || !ptr_TextChain_removed) {
      // Destroy the removed text objects
      QTS_DestroyTextChain(ptr_TextChain_removed); // to make shore there is no disconnected memmory
      return false;
    }
  }

  // if line is empty enter an empty format clone
  if(!ptr_EditBox->hilight_start_line->ptr_TextElements){
    ptr_TextElement_Clone = QTS_CloneTextFormat(ptr_renderer, ptr_TextChain_removed);
    QTS_AddTextToLine_After(ptr_EditBox->hilight_start_line, NULL, ptr_TextElement_Clone);
  }

  // Destroy Text Chain
  QTS_DestroyTextChain(ptr_TextChain_removed);
  ptr_TextChain_removed = NULL;



  // Remove section from end Line
  // only do if line is not empty
  if(ptr_EditBox->hilight_end_line->ptr_TextElements->string){
    b_ret = QTS_RemoveSectionFromLine(ptr_renderer, ptr_EditBox->hilight_end_line, 0,  ptr_EditBox->highlight_end, &ptr_TextChain_removed); // end codepoint = 0 implys remove untill end of line
    if(!b_ret || !ptr_TextChain_removed) {
      // Destroy the removed text objects
      QTS_DestroyTextChain(ptr_TextChain_removed); // to make shore there is no disconnected memmory
      return false;
    }
  }

  // if line is empty enter an empty format clone
  if(!ptr_EditBox->hilight_end_line->ptr_TextElements){
    ptr_TextElement_Clone = QTS_CloneTextFormat(ptr_renderer, ptr_TextChain_removed);
    QTS_AddTextToLine_After(ptr_EditBox->hilight_end_line, NULL, ptr_TextElement_Clone);
  }

  // Destroy Text Chain
  QTS_DestroyTextChain(ptr_TextChain_removed);

  // Update Curser parameters
  ptr_EditBox->highlight_present = false;
  ptr_EditBox->marking_active = false;
  ptr_EditBox->last_time = SDL_GetTicks();

  // get new curser possition
  QTS_GetLineIncisionPoint_FromCodepoint(ptr_EditBox->hilight_start_line, start_codepoint, &(ptr_EditBox->incission_text_element), &(ptr_EditBox->x_cord), &(ptr_EditBox->back_codepoints), &(ptr_EditBox->incision_codepoint), &(ptr_EditBox->incision_index), ptr_EditBox->crosskerneling);
  ptr_EditBox->y_cord = QTS_EditBox_GetLineOffset(ptr_EditBox->ptr_Box, ptr_EditBox->hilight_start_line);

  // Nullyfy pointers
  ptr_EditBox->highlight_start_text = NULL;
  ptr_EditBox->highlight_end_text = NULL;

  return true;
}




// Function for Inserting text in to an edit line at the current curser possition
// text is a Null terminated string that will be copyed in to the line
// this function will not mve the incission point or prepare the string in annyway
bool QTS_EditBox_InsertText(SDL_Renderer *ptr_renderer, struct QTS_EditBox* ptr_EditBox, char* text){

  // Validate Input
  if(!ptr_renderer || !ptr_EditBox || !text) return false;
  if(!text[0]) return false;

  // Check if there is a curser text obj pointer
  if(!ptr_EditBox->incission_text_element) return false;

  // Return Holding variable
  bool b_ret;

  // two cases: single line or multiline Incission
  int line_count = QTS_CountStringLines(text);
  if(line_count <= 0) return false;

  // Case single line incission
  if(line_count == 1){

    if(ptr_EditBox->incision_index == 0) ptr_EditBox->incision_codepoint = -1;

    // Insert text in to text obj
    b_ret = QTS_InsertString(ptr_EditBox->incission_text_element, text, ptr_EditBox->incision_index);

    // Update the affected text element
    if(!QTS_UpdateText(ptr_renderer, ptr_EditBox->incission_text_element)){
      return false;
    }

    return b_ret;
  }// end of single line incission

  // Case: Multiline Incission

  // Holding variables
  struct QTS_Text* ptr_text_holding = NULL;
  struct QTS_Text* ptr_text_end_line_holding = NULL;
  struct QTS_Text* ptr_text_first_line = NULL;
  int inc_line_codepoints = QTS_GetLineSize_CP(ptr_EditBox->ptr_incission_line);
  int first_line_length = 0;
  char* first_line_str = NULL;

  // calculate true codepoint
  int true_incision_codepoint = ptr_EditBox->back_codepoints;
  if(ptr_EditBox->incision_codepoint >= -1) true_incision_codepoint += ptr_EditBox->incision_codepoint + 1;


  // extract the first line of incission text and add it at incission point
  first_line_length = QTS_GetStringLineLength(text);
  if(first_line_length > 0){
    first_line_str = (char*)calloc(first_line_length + 1, sizeof(char)); // will be passed to new text object therefore does not need to be freed!
    memcpy(first_line_str, text, (size_t)first_line_length);

    //ptr_text_first_line = QTS_CloneTextFormat(ptr_renderer, ptr_text_holding);
    ptr_text_first_line = QTS_CloneTextFormat(ptr_renderer, ptr_EditBox->incission_text_element);
    QTS_UpdateTextString(ptr_text_first_line, first_line_str, 0, false);
    QTS_UpdateText(ptr_renderer, ptr_text_first_line);


    // special case if incission point is at the end of the line
    if(true_incision_codepoint >= inc_line_codepoints){

      // add first line at the end of incission line
      ptr_text_end_line_holding = ptr_EditBox->ptr_incission_line->ptr_TextElements;
      while(ptr_text_end_line_holding->ptr_NextText) ptr_text_end_line_holding = ptr_text_end_line_holding->ptr_NextText; // gets last text element in line
      ptr_text_end_line_holding->ptr_NextText = ptr_text_first_line;
      ptr_text_first_line->ptr_PrevText = ptr_text_end_line_holding;

    }else{


      // split line of at incission point and hold line rest
      ptr_text_holding = QTS_SplitLineAt(ptr_renderer, ptr_EditBox->ptr_incission_line, true_incision_codepoint);
      if(!ptr_text_holding) return false;

      // hold rest and add first line
      ptr_text_first_line->ptr_PrevText = ptr_text_holding->ptr_PrevText;
      if(ptr_text_holding->ptr_PrevText){
        ptr_text_holding->ptr_PrevText->ptr_NextText = ptr_text_first_line;
      } else {
        ptr_EditBox->ptr_incission_line->ptr_TextElements = ptr_text_first_line;
      }

    }
  }


  // Generate Lines from the rest of text
  struct QTS_Line* ptr_last_generated_line = NULL;
  struct QTS_Text* ptr_last_text = NULL;
  //struct QTS_Line* ptr_lines_generated = QTS_ConvertTextToLines(ptr_renderer, ptr_text_holding, &(text[first_line_length + 1]), &ptr_last_generated_line, &ptr_last_text);
  struct QTS_Line* ptr_lines_generated = QTS_ConvertTextToLines(ptr_renderer, ptr_EditBox->incission_text_element, &(text[first_line_length + 1]), &ptr_last_generated_line, &ptr_last_text);


  // add the generated Lines from rest of text
  ptr_last_generated_line->ptr_NextLine = ptr_EditBox->ptr_incission_line->ptr_NextLine;
  if(ptr_last_generated_line->ptr_NextLine) ptr_last_generated_line->ptr_NextLine->ptr_PrevLine = ptr_last_generated_line;

  ptr_EditBox->ptr_incission_line->ptr_NextLine = ptr_lines_generated;
  ptr_lines_generated->ptr_PrevLine = ptr_EditBox->ptr_incission_line;

  // add rest to the end of last incerted Line
  // case text element is empty:
  if(!ptr_last_text->string){

    ptr_last_generated_line->ptr_TextElements = ptr_text_holding;
    if(ptr_text_holding) ptr_text_holding->ptr_PrevText = NULL;
    QTS_DestroyTextOBJ(ptr_last_text);

  }else{

    ptr_last_text->ptr_NextText = ptr_text_holding;
    if(ptr_text_holding) ptr_text_holding->ptr_PrevText = ptr_last_text;

  }

  return true;
}




// Function for resetting the Curser after an edditation has been made
// this updates the incission_text_element is the edit line
bool QTS_EditBox_UpdateCurser(struct QTS_EditBox* ptr_EditBox){

  // Validate Input
  if(!ptr_EditBox) return false;

  // Calculate Codepoint
  int codepoint;
  if(ptr_EditBox->incision_codepoint == -1){
    codepoint = ptr_EditBox->back_codepoints;
  }else{
    codepoint = ptr_EditBox->back_codepoints + ptr_EditBox->incision_codepoint + 1;
  }

  // special case if is at very beginning of line
  if(ptr_EditBox->incision_codepoint == -1 && ptr_EditBox->back_codepoints <= 0){
    ptr_EditBox->x_cord = 0;
    ptr_EditBox->incision_index = 0;
    ptr_EditBox->incission_text_element = ptr_EditBox->ptr_incission_line->ptr_TextElements;
    ptr_EditBox->y_cord = QTS_EditBox_GetLineOffset(ptr_EditBox->ptr_Box, ptr_EditBox->ptr_incission_line);
    return true;
  }

  // get y cord
  ptr_EditBox->y_cord = QTS_EditBox_GetLineOffset(ptr_EditBox->ptr_Box, ptr_EditBox->ptr_incission_line);

  //if(ptr_EditBox->ptr_incission_line->ptr_PrevLine) ptr_EditBox->y_cord += ptr_EditBox->ptr_incission_line->ptr_PrevLine->hight;

  // Recalculate all values
  if(!QTS_GetLineIncisionPoint_FromCodepoint(ptr_EditBox->ptr_incission_line, codepoint, &(ptr_EditBox->incission_text_element), &(ptr_EditBox->x_cord), &(ptr_EditBox->back_codepoints), &(ptr_EditBox->incision_codepoint), &(ptr_EditBox->incision_index), ptr_EditBox->crosskerneling)){
    return false;
  }



  // Recalculate back codepoint
  ptr_EditBox->back_codepoints = codepoint - ptr_EditBox->incision_codepoint - 1;

  return true;
}



// Function for Updating the Hilight by codepoints
bool QTS_EditBox_UpdateHilight(struct QTS_EditBox* ptr_EditBox){

  // Validate Input
  if(!ptr_EditBox) return false;

  // return holding variable
  bool b_ret;

  // Nullify hilight text pointers
  ptr_EditBox->highlight_start_text = NULL;
  ptr_EditBox->highlight_end_text = NULL;

  // check if there is even suposed to be a hilight
  if(ptr_EditBox->highlight_start == ptr_EditBox->highlight_end){
    ptr_EditBox->highlight_present = false;
    return true;
  }

  ptr_EditBox->highlight_present = true;

  // Recalculate values
  b_ret = QTS_GetLineIncisionPoint_FromCodepoint(ptr_EditBox->hilight_start_line, ptr_EditBox->highlight_start, &(ptr_EditBox->highlight_start_text), &(ptr_EditBox->x_cord_start), NULL, NULL, NULL, ptr_EditBox->crosskerneling);
  if(!b_ret) return false;

  b_ret = QTS_GetLineIncisionPoint_FromCodepoint(ptr_EditBox->hilight_end_line, ptr_EditBox->highlight_end, &(ptr_EditBox->highlight_end_text), &(ptr_EditBox->x_cord_end), NULL, NULL, NULL, ptr_EditBox->crosskerneling);
  if(!b_ret) return false;

  // Recalculate Hilight Rect
  b_ret = QTS_EditBox_CalculateHighlightRects(ptr_EditBox);
  //b_ret = QTS_EditBox_GetHighlightRect(ptr_EditBox);
  if(!b_ret) return false;

  return true;
}





// Function for Moving the Curser by a number of codepoints
bool QTS_EditBox_MoveCurser(struct QTS_EditBox* ptr_EditBox, int codepoints){

  // Validate input
  if(!ptr_EditBox) return false;
  if(codepoints == 0) return true;

  // Calculate true Incission codepoint
  int true_inc_codepoint = ptr_EditBox->back_codepoints;

  if(ptr_EditBox->incision_codepoint >= 0) true_inc_codepoint += ptr_EditBox->incision_codepoint + 1;

  int move_count = codepoints;
  int line_codepoints = QTS_GetLineSize_CP(ptr_EditBox->ptr_incission_line);

  if(move_count > 0){
    // movinf forward

    // detect if move to next line
    if(true_inc_codepoint + move_count > line_codepoints && ptr_EditBox->ptr_incission_line->ptr_NextLine){

      // increment move_count
      move_count -= (line_codepoints - true_inc_codepoint);
      move_count--;

      // set next incission line an incission line
      ptr_EditBox->y_cord += ptr_EditBox->ptr_incission_line->hight;
      ptr_EditBox->ptr_incission_line = ptr_EditBox->ptr_incission_line->ptr_NextLine;
      ptr_EditBox->back_codepoints = 0;
      ptr_EditBox->incision_codepoint = -1;
      ptr_EditBox->incision_index = 0;
      ptr_EditBox->incission_text_element = ptr_EditBox->ptr_incission_line->ptr_TextElements;

      line_codepoints = QTS_GetLineSize_CP(ptr_EditBox->ptr_incission_line);

      // itterate through Lines untill move count is depleted
      while(move_count > line_codepoints && ptr_EditBox->ptr_incission_line->ptr_NextLine){

        // deplete move_count
        move_count -= line_codepoints;
        move_count--;

        // Go to next Line
        ptr_EditBox->y_cord += ptr_EditBox->ptr_incission_line->hight;
        ptr_EditBox->ptr_incission_line = ptr_EditBox->ptr_incission_line->ptr_NextLine;

        // Get Line codepoints
        line_codepoints = QTS_GetLineSize_CP(ptr_EditBox->ptr_incission_line);

      }
      ptr_EditBox->incission_text_element = ptr_EditBox->ptr_incission_line->ptr_TextElements;
    }
  }else if(move_count < 0){
    // moving backwards

    if(!ptr_EditBox->ptr_incission_line->ptr_TextElements->string) true_inc_codepoint--;
    // detect if move to previus line
    if(true_inc_codepoint + move_count < 0 && ptr_EditBox->ptr_incission_line->ptr_PrevLine){

      // increment move_count
      move_count += true_inc_codepoint;
      move_count++;

      // set next incission line an incission line
      ptr_EditBox->y_cord -= ptr_EditBox->ptr_incission_line->hight;
      ptr_EditBox->ptr_incission_line = ptr_EditBox->ptr_incission_line->ptr_PrevLine;

      // Get Line codepoints
      line_codepoints = QTS_GetLineSize_CP(ptr_EditBox->ptr_incission_line);

      // set incission point at the end of the line
      QTS_GetLineIncisionPoint_FromCodepoint(ptr_EditBox->ptr_incission_line, line_codepoints, &(ptr_EditBox->incission_text_element), &(ptr_EditBox->x_cord), &(ptr_EditBox->back_codepoints), &(ptr_EditBox->incision_codepoint), &(ptr_EditBox->incision_index), ptr_EditBox->crosskerneling);

      // itterate through Lines untill move count is depleted
      while(abs(move_count) > line_codepoints && ptr_EditBox->ptr_incission_line->ptr_PrevLine){

        // deplete move_count
        move_count += line_codepoints;
        move_count++;

        // Go to prev Line
        ptr_EditBox->y_cord -= ptr_EditBox->ptr_incission_line->hight;
        ptr_EditBox->ptr_incission_line = ptr_EditBox->ptr_incission_line->ptr_PrevLine;

        // Get Line codepoints
        line_codepoints = QTS_GetLineSize_CP(ptr_EditBox->ptr_incission_line);

      }
      ptr_EditBox->incission_text_element = ptr_EditBox->ptr_incission_line->ptr_TextElements;
      QTS_GetLineIncisionPoint_FromCodepoint(ptr_EditBox->ptr_incission_line, line_codepoints, &(ptr_EditBox->incission_text_element), &(ptr_EditBox->x_cord), &(ptr_EditBox->back_codepoints), &(ptr_EditBox->incision_codepoint), &(ptr_EditBox->incision_index), ptr_EditBox->crosskerneling);
    }
  }

  // move count for movement across lines
  if(!QTS_MoveIncisionPoint(ptr_EditBox->ptr_incission_line, move_count, &(ptr_EditBox->incission_text_element), &(ptr_EditBox->x_cord), &(ptr_EditBox->back_codepoints), &(ptr_EditBox->incision_codepoint), &(ptr_EditBox->incision_index), ptr_EditBox->crosskerneling)){
    return false;
  }

  QTS_EditBox_UpdateCurser(ptr_EditBox);

  // if line is empty
  if(!ptr_EditBox->incission_text_element->string){
    ptr_EditBox->incision_codepoint = -1;
  }

  // set curser time to now
  ptr_EditBox->last_time = SDL_GetTicks();

  return true;
}




bool QTS_EditBox_PasetTextFromClipboard(SDL_Renderer *ptr_renderer, struct QTS_EditBox* ptr_EditBox){

  // Validate Input
  if(!ptr_renderer ||!ptr_EditBox) return false;
  if(!ptr_EditBox->incission_text_element) return false;
  if(!ptr_EditBox->enable_edit) return false;

  // Return Holding variable
  bool b_ret;

  // if there is a highlight destroy that hight
  if(ptr_EditBox->highlight_present){
    b_ret = QTS_EditBox_DestroyHilightedText(ptr_renderer, ptr_EditBox);
    if(!b_ret) return false;
  }

  // obtain text from clippboard
  char* insert_text = SDL_GetClipboardText();
  if(!insert_text) return false;

  // remove new line symbols from string
  //QTS_RemoveNewLineSymbols(insert_text);

  // count the codepoints in the string
  int count = QTS_CountCodepoints(insert_text);
  if(count <= 0){
    SDL_free(insert_text);
    return false;
  }

  // Insert the text at current curser location
  b_ret = QTS_EditBox_InsertText(ptr_renderer, ptr_EditBox, insert_text);
  if(!b_ret){
    SDL_free(insert_text);
    return false;
  }

  // move curser by the number of codepoints
  b_ret = QTS_EditBox_MoveCurser(ptr_EditBox, count);
  if(!b_ret){
    SDL_free(insert_text);
    return false;
  }

  // free text
  SDL_free(insert_text);

  return true;
}




// Function for inputting text at the curser location and moving the curser accordingly
bool QTS_EditBox_InputText(SDL_Renderer *ptr_renderer, struct QTS_EditBox* ptr_EditBox, char* ptr_text){

  // Validate Input
  if(!ptr_renderer || !ptr_EditBox || !ptr_text) return false;

  // Return Holding variable
  bool b_ret;

  // count the text codepoints
  int codepoints = QTS_CountCodepoints(ptr_text);
  if(codepoints <= 0) return true;

  // remove new line symbols from string
  //QTS_RemoveNewLineSymbols(ptr_text);

  // Insert the Text
  b_ret = QTS_EditBox_InsertText(ptr_renderer, ptr_EditBox, ptr_text);
  if(!b_ret) return false;

  // Move the curser by the number of codepoints
  b_ret = QTS_EditBox_MoveCurser(ptr_EditBox, codepoints);
  if(!b_ret) return false;

  return true;
}




// Function for Handeling Text Input Event
bool QTS_EditBox_HandleTextInput(SDL_Renderer *ptr_renderer, struct QTS_EditBox* ptr_EditBox, SDL_Event* ptr_Event){

  // Validate Input
  if(!ptr_renderer || !ptr_EditBox || !ptr_Event) return false;
  if(!ptr_EditBox->enable_edit) return false;

  // Create a copy of the text input string
  char* text_copy = SDL_strdup(ptr_Event->text.text);

  // return Holding variable
  bool b_ret;

  /*
  b_ret = QTS_RemoveNewLineSymbols(text_copy);
  if(!b_ret){
    SDL_free(text_copy);
    return false;
  }
  */


  // If there is a Hilight present delet hilight and then insert text
  if(ptr_EditBox->highlight_present){
    b_ret = QTS_EditBox_DestroyHilightedText(ptr_renderer, ptr_EditBox);
    if(!b_ret){
      SDL_free(text_copy);
      return false;
    }
  }

  // Insert the string
  b_ret = QTS_EditBox_InputText(ptr_renderer, ptr_EditBox, text_copy);

  SDL_free(text_copy);
  return b_ret;
}



// Function for Cutting the selected text (copy to clipboard and remove from line)
bool QTS_EditBox_CutSelectedText(SDL_Renderer *ptr_renderer, struct QTS_EditBox* ptr_EditBox){

  // Validate Input
  if(!ptr_renderer || !ptr_EditBox) return false;
  if(!ptr_EditBox->enable_edit) return false;

  // only do something if hilight is present
  if(!ptr_EditBox->highlight_present) return false;

  // Return Holding variable
  bool b_ret;

  // Copy selected text to clipboard
  b_ret = QTS_EditBox_CopySelectedTextToClipboard(ptr_EditBox);
  if(!b_ret) return false;

  // Destroy selected text
  b_ret = QTS_EditBox_DestroyHilightedText(ptr_renderer, ptr_EditBox);
  if(!b_ret) return false;

  return b_ret;
}




// Function for calculatin the curser codepoint
int QTS_EditBox_GetCurserCodepoint(struct QTS_EditBox* ptr_EditBox){

  // Validate Input
  if(!ptr_EditBox) return 0;

  // Calculate codepoint
  int codepoint = 0;
  if(ptr_EditBox->incision_codepoint == -1){
    codepoint = ptr_EditBox->back_codepoints;
  }else{
    codepoint = ptr_EditBox->back_codepoints + ptr_EditBox->incision_codepoint + 1;
  }

  return codepoint;
}



// Function for Handeling Backspace
bool QTS_EditBox_Backspace(SDL_Renderer *ptr_renderer, struct QTS_EditBox* ptr_EditBox){

  // Validate Input
  if(!ptr_renderer || !ptr_EditBox) return false;
  if(!ptr_EditBox->ptr_incission_line) return false;
  if(!ptr_EditBox->ptr_incission_line->ptr_TextElements) return false;
  if(!ptr_EditBox->enable_edit) return false;

  // Special case if line is empty
  /*
  if(!ptr_EditBox->ptr_incission_line->ptr_TextElements->string || !ptr_EditBox->ptr_incission_line->ptr_TextElements->spacing){
    if(!ptr_EditBox->ptr_incission_line->ptr_TextElements->ptr_NextText){
      return false;
    }
  } */

  // Return Holding variable
  bool b_ret;

  // if a hilight is present then delete the hilight
  if(ptr_EditBox->highlight_present){
    b_ret = QTS_EditBox_DestroyHilightedText(ptr_renderer, ptr_EditBox);
    if(!b_ret) return false;
    return true;
  }

  // Calculate true curser codepoint
  int curser_codepoint = QTS_EditBox_GetCurserCodepoint(ptr_EditBox);

  // if line is empty
  if(!ptr_EditBox->incission_text_element->string){
    ptr_EditBox->incision_codepoint = -1;
  }

  // Delete section

  // if is at beginning of line
  if(curser_codepoint == 0){

    // if is first line do nothing
    if(!ptr_EditBox->ptr_incission_line->ptr_PrevLine) return true;

    // get the number of codepoints in the prev line
    int prev_line_codepoints = QTS_GetLineSize_CP(ptr_EditBox->ptr_incission_line->ptr_PrevLine);

    // if current line is empty
    if(!ptr_EditBox->incission_text_element->string){
      // Remove the line from the Box
      ptr_EditBox->ptr_incission_line->ptr_PrevLine->ptr_NextLine = ptr_EditBox->ptr_incission_line->ptr_NextLine;
      if(ptr_EditBox->ptr_incission_line->ptr_NextLine) ptr_EditBox->ptr_incission_line->ptr_NextLine->ptr_PrevLine = ptr_EditBox->ptr_incission_line->ptr_PrevLine;

      struct QTS_Line* ptr_line_free = ptr_EditBox->ptr_incission_line;
      ptr_EditBox->ptr_incission_line = ptr_EditBox->ptr_incission_line->ptr_PrevLine;

      // Destroy the removed Line structure
      QTS_DestroyLine(ptr_line_free);

      // Set curser to end of that line
      QTS_GetLineIncisionPoint_FromCodepoint(ptr_EditBox->ptr_incission_line, prev_line_codepoints, &(ptr_EditBox->incission_text_element), &(ptr_EditBox->x_cord), &(ptr_EditBox->back_codepoints), &(ptr_EditBox->incision_codepoint), &(ptr_EditBox->incision_index), ptr_EditBox->crosskerneling);
      if(prev_line_codepoints == 0) ptr_EditBox->incision_codepoint = -1;
      // update the cursers y possition
      ptr_EditBox->y_cord = QTS_EditBox_GetLineOffset(ptr_EditBox->ptr_Box, ptr_EditBox->ptr_incission_line);

      return true;
    }

    // seperate the text objects from the Line structure
    struct QTS_Text* ptr_text_remainder = ptr_EditBox->ptr_incission_line->ptr_TextElements;
    ptr_EditBox->ptr_incission_line->ptr_TextElements = NULL;
    ptr_text_remainder->ptr_PrevText = NULL;

    // Get end of prev line
    struct QTS_Text* ptr_text_holding = ptr_EditBox->ptr_incission_line->ptr_PrevLine->ptr_TextElements;
    while(ptr_text_holding->ptr_NextText) ptr_text_holding = ptr_text_holding->ptr_NextText;

    // Add the Text objects to the end of the previus line
    if(!(ptr_EditBox->ptr_incission_line->ptr_PrevLine->ptr_TextElements->string)){

      QTS_DestroyTextOBJ(ptr_EditBox->ptr_incission_line->ptr_PrevLine->ptr_TextElements);
      ptr_EditBox->ptr_incission_line->ptr_PrevLine->ptr_TextElements = ptr_text_remainder;

    }else{

      ptr_text_holding->ptr_NextText = ptr_text_remainder;
      ptr_text_remainder->ptr_PrevText = ptr_text_holding;

    }

    // Remove the line from the Box
    ptr_EditBox->ptr_incission_line->ptr_PrevLine->ptr_NextLine = ptr_EditBox->ptr_incission_line->ptr_NextLine;
    if(ptr_EditBox->ptr_incission_line->ptr_NextLine) ptr_EditBox->ptr_incission_line->ptr_NextLine->ptr_PrevLine = ptr_EditBox->ptr_incission_line->ptr_PrevLine;

    struct QTS_Line* ptr_line_free = ptr_EditBox->ptr_incission_line;
    ptr_EditBox->ptr_incission_line = ptr_EditBox->ptr_incission_line->ptr_PrevLine;

    // Destroy the removed Line structure
    QTS_DestroyLine(ptr_line_free);

    // Set curser to end of that line
    QTS_GetLineIncisionPoint_FromCodepoint(ptr_EditBox->ptr_incission_line, prev_line_codepoints, &(ptr_EditBox->incission_text_element), &(ptr_EditBox->x_cord), &(ptr_EditBox->back_codepoints), &(ptr_EditBox->incision_codepoint), &(ptr_EditBox->incision_index), ptr_EditBox->crosskerneling);
    if(prev_line_codepoints == 0) ptr_EditBox->incision_codepoint = -1;
    // update the cursers y possition
    ptr_EditBox->y_cord = QTS_EditBox_GetLineOffset(ptr_EditBox->ptr_Box, ptr_EditBox->ptr_incission_line);

    return true;

  }// end of case beginning of line


  if(!ptr_EditBox->ptr_incission_line->ptr_TextElements->string || !ptr_EditBox->ptr_incission_line->ptr_TextElements->spacing){
    if(!ptr_EditBox->ptr_incission_line->ptr_TextElements->ptr_NextText){
      return false;
    }
  }


  // remove section from line
  struct QTS_Text* ptr_removed_element = NULL;
  b_ret = QTS_RemoveSectionFromLine(ptr_renderer, ptr_EditBox->ptr_incission_line, curser_codepoint - 1,  curser_codepoint, &ptr_removed_element);
  if(!b_ret){
    QTS_DestroyTextOBJ(ptr_removed_element);
    return false;
  }

  // Move curser by one codepoint to the left
  //QTS_EditBox_MoveCurser(ptr_EditBox, -1);

  // if line is empty insert format clone
  if(!(ptr_EditBox->ptr_incission_line->ptr_TextElements)){
    struct QTS_Text* ptr_format_clone = QTS_CloneTextFormat(ptr_renderer, ptr_removed_element);
    b_ret = QTS_AddTextToLine_After(ptr_EditBox->ptr_incission_line, NULL, ptr_format_clone);
    if(!b_ret){
      QTS_DestroyTextOBJ(ptr_format_clone);
      QTS_DestroyTextOBJ(ptr_removed_element);
      return false;
    }
    // set curser data acordingly
    ptr_EditBox->incission_text_element = ptr_format_clone;
    ptr_EditBox->x_cord = 0;
    ptr_EditBox->back_codepoints = 0;
    ptr_EditBox->incision_codepoint = -1;

  }else{
    QTS_EditBox_MoveCurser(ptr_EditBox, -1);
  }

  //QTS_EditBox_MoveCurser(ptr_EditBox, -1);
  // Destroy the removed part
  QTS_DestroyTextOBJ(ptr_removed_element);

  // Update
  ptr_EditBox->last_time = SDL_GetTicks();
  QTS_EditBox_UpdateCurser(ptr_EditBox);

  return true;
}






// Function for Handeling Backspace to beginning
bool QTS_EditBox_BackspaceToBeginning(SDL_Renderer *ptr_renderer, struct QTS_EditBox* ptr_EditBox){

  // Validate Input
  if(!ptr_renderer || !ptr_EditBox) return false;
  if(!ptr_EditBox->ptr_incission_line) return false;
  if(!ptr_EditBox->ptr_incission_line->ptr_TextElements) return false;
  if(!ptr_EditBox->enable_edit) return false;

  // Special case if line is empty
  if(!ptr_EditBox->ptr_incission_line->ptr_TextElements->string || !ptr_EditBox->ptr_incission_line->ptr_TextElements->spacing){
    if(!ptr_EditBox->ptr_incission_line->ptr_TextElements->ptr_NextText){
      return false;
    }
  }

  // Return Holding variable
  bool b_ret;

  // if a hilight is present then delete the hilight
  if(ptr_EditBox->highlight_present){
    b_ret = QTS_EditBox_DestroyHilightedText(ptr_renderer, ptr_EditBox);
    if(!b_ret) return false;
  }

  // Calculate true curser codepoint
  int curser_codepoint = QTS_EditBox_GetCurserCodepoint(ptr_EditBox);

  // Delete section

  // if is at beginning of line
  if(curser_codepoint == 0) return true;

  // remove section from line
  struct QTS_Text* ptr_removed_element = NULL;
  b_ret = QTS_RemoveSectionFromLine(ptr_renderer, ptr_EditBox->ptr_incission_line, 0,  curser_codepoint, &ptr_removed_element);
  if(!b_ret){
    QTS_DestroyTextChain(ptr_removed_element);
    return false;
  }

  // Move curser codepoint to the left
  QTS_EditBox_MoveCurser(ptr_EditBox, curser_codepoint * (-1));

  // if line is empty insert format clone
  if(!(ptr_EditBox->ptr_incission_line->ptr_TextElements)){
    struct QTS_Text* ptr_format_clone = QTS_CloneTextFormat(ptr_renderer, ptr_removed_element);
    b_ret = QTS_AddTextToLine_After(ptr_EditBox->ptr_incission_line, NULL, ptr_format_clone);
    if(!b_ret){
      QTS_DestroyTextOBJ(ptr_format_clone);
      QTS_DestroyTextChain(ptr_removed_element);
      return false;
    }
  }

  // Destroy the removed part
  QTS_DestroyTextChain(ptr_removed_element);

  // Update
  ptr_EditBox->last_time = SDL_GetTicks();
  QTS_EditBox_UpdateCurser(ptr_EditBox);

  return true;
}









// Function for Handeling Delete (removes next character)
bool QTS_EditBox_Delete(SDL_Renderer *ptr_renderer, struct QTS_EditBox* ptr_EditBox){

  // Validate Input
  if(!ptr_renderer || !ptr_EditBox) return false;
  if(!ptr_EditBox->ptr_incission_line) return false;
  if(!ptr_EditBox->ptr_incission_line->ptr_TextElements) return false;
  if(!ptr_EditBox->enable_edit) return false;

  // Special case if line is empty
  if(!ptr_EditBox->ptr_incission_line->ptr_TextElements->string || !ptr_EditBox->ptr_incission_line->ptr_TextElements->spacing){
    if(!ptr_EditBox->ptr_incission_line->ptr_TextElements->ptr_NextText && !ptr_EditBox->ptr_incission_line->ptr_NextLine){
      return false;
    }
  }

  // Return Holding variable
  bool b_ret;

  // if a hilight is present then delete the hilight
  if(ptr_EditBox->highlight_present){
    b_ret = QTS_EditBox_DestroyHilightedText(ptr_renderer, ptr_EditBox);
    if(!b_ret) return false;
  }

  // Calculate true curser codepoint
  int curser_codepoint = QTS_EditBox_GetCurserCodepoint(ptr_EditBox);
  int line_codepoints = QTS_GetLineSize_CP(ptr_EditBox->ptr_incission_line);

  // special case if is at the end of the line
  if(curser_codepoint == line_codepoints){

    // Test if has a follower
    if(ptr_EditBox->ptr_incission_line->ptr_NextLine){

      // move forward one
      QTS_EditBox_MoveCurser(ptr_EditBox, 1);

      // backspace
      return QTS_EditBox_Backspace(ptr_renderer, ptr_EditBox);

    }
  }

  // Delete section

  // if is at beginning of line
  //if(curser_codepoint == 0) return true;

  // remove section from line
  struct QTS_Text* ptr_removed_element = NULL;
  b_ret = QTS_RemoveSectionFromLine(ptr_renderer, ptr_EditBox->ptr_incission_line, curser_codepoint,  curser_codepoint + 1, &ptr_removed_element);
  if(!b_ret){
    QTS_DestroyTextOBJ(ptr_removed_element);
    return false;
  }

  // Move curser by one codepoint to the left
  //QTS_EditBox_MoveCurser(ptr_EditBox, 1);

  // if line is empty insert format clone
  if(!(ptr_EditBox->ptr_incission_line->ptr_TextElements)){
    struct QTS_Text* ptr_format_clone = QTS_CloneTextFormat(ptr_renderer, ptr_removed_element);
    b_ret = QTS_AddTextToLine_After(ptr_EditBox->ptr_incission_line, NULL, ptr_format_clone);
    if(!b_ret){
      QTS_DestroyTextOBJ(ptr_format_clone);
      QTS_DestroyTextOBJ(ptr_removed_element);
      return false;
    }
  }

  // Destroy the removed part
  QTS_DestroyTextOBJ(ptr_removed_element);

  // Update
  ptr_EditBox->last_time = SDL_GetTicks();
  QTS_EditBox_UpdateCurser(ptr_EditBox);

  return true;
}




// Function for Handeling Delete to End (removes next characters to End)
bool QTS_EditBox_DeleteToEnd(SDL_Renderer *ptr_renderer, struct QTS_EditBox* ptr_EditBox){

  // Validate Input
  if(!ptr_renderer || !ptr_EditBox) return false;
  if(!ptr_EditBox->ptr_incission_line) return false;
  if(!ptr_EditBox->ptr_incission_line->ptr_TextElements) return false;
  if(!ptr_EditBox->enable_edit) return false;

  // Special case if line is empty
  if(!ptr_EditBox->ptr_incission_line->ptr_TextElements->string || !ptr_EditBox->ptr_incission_line->ptr_TextElements->spacing){
    if(!ptr_EditBox->ptr_incission_line->ptr_TextElements->ptr_NextText){
      return false;
    }
  }

  // Return Holding variable
  bool b_ret;

  // if a hilight is present then delete the hilight
  if(ptr_EditBox->highlight_present){
    b_ret = QTS_EditBox_DestroyHilightedText(ptr_renderer, ptr_EditBox);
    if(!b_ret) return false;
  }

  // Calculate true curser codepoint
  int curser_codepoint = QTS_EditBox_GetCurserCodepoint(ptr_EditBox);

  // Delete section

  // if is at beginning of line
  //if(curser_codepoint == 0) return true;

  // remove section from line
  struct QTS_Text* ptr_removed_element = NULL;  // (if end_code_point = 0 then remove untill end)
  b_ret = QTS_RemoveSectionFromLine(ptr_renderer, ptr_EditBox->ptr_incission_line, curser_codepoint,  0, &ptr_removed_element);
  if(!b_ret){
    QTS_DestroyTextChain(ptr_removed_element);
    return false;
  }

  // Move curser by one codepoint to the left
  //QTS_EditBox_MoveCurser(ptr_EditBox, 1);

  // if line is empty insert format clone
  if(!(ptr_EditBox->ptr_incission_line->ptr_TextElements)){
    struct QTS_Text* ptr_format_clone = QTS_CloneTextFormat(ptr_renderer, ptr_removed_element);
    b_ret = QTS_AddTextToLine_After(ptr_EditBox->ptr_incission_line, NULL, ptr_format_clone);
    if(!b_ret){
      QTS_DestroyTextOBJ(ptr_format_clone);
      QTS_DestroyTextChain(ptr_removed_element);
      return false;
    }
  }

  // Destroy the removed part
  QTS_DestroyTextChain(ptr_removed_element);

  // Update
  ptr_EditBox->last_time = SDL_GetTicks();
  QTS_EditBox_UpdateCurser(ptr_EditBox);

  return true;
}



// Function for Escapint the edit Line
bool QTS_EditBox_Escape(struct QTS_EditBox* ptr_EditBox){

  // Validate Input
  if(!ptr_EditBox) return false;

  // Unselect and stop marking
  ptr_EditBox->marking_active = false;
  ptr_EditBox->selected = false;

  return true;
}



// Function for Moving the curser to the Beginning of the line
bool QTS_EditBox_MoveCurserToBegining(struct QTS_EditBox* ptr_EditBox){

  // Validate Input
  if(!ptr_EditBox) return false;

  // Move curser to beginning
  int true_codepoint = QTS_EditBox_GetCurserCodepoint(ptr_EditBox);

  if(true_codepoint <= 0){
    QTS_EditBox_MoveCurser(ptr_EditBox, -1);
    return true;
  }


  QTS_EditBox_MoveCurser(ptr_EditBox, true_codepoint * (-1));

  // Update
  ptr_EditBox->last_time = SDL_GetTicks();
  QTS_EditBox_UpdateCurser(ptr_EditBox);

  return true;
}



// Function for Moving the cursert to the end of the line
bool QTS_EditBox_MoveCurserToEnd(struct QTS_EditBox* ptr_EditBox){

  // Validate Input
  if(!ptr_EditBox) return false;

  // Move curser to beginning
  int codepoint = QTS_CountCodepointsInLine(ptr_EditBox->ptr_incission_line);
  int curser_codepoint = QTS_EditBox_GetCurserCodepoint(ptr_EditBox);

  //if(codepoint <= 0) return true;
  if(curser_codepoint == codepoint){
    QTS_EditBox_MoveCurser(ptr_EditBox, 1);
    return true;
  }

  QTS_EditBox_MoveCurser(ptr_EditBox, codepoint - curser_codepoint);

  // Update
  ptr_EditBox->last_time = SDL_GetTicks();
  QTS_EditBox_UpdateCurser(ptr_EditBox);

  return true;
}



// Function for moving the curser up one line
bool QTS_EditBox_MoveCurserUp(struct QTS_EditBox* ptr_EditBox){

  // Validate Input
  if(!ptr_EditBox) return false;
  if(!ptr_EditBox->ptr_incission_line) return false;
  if(!ptr_EditBox->ptr_incission_line->ptr_PrevLine) return false;

  // Get true codepoint
  int true_inc_codepoint = QTS_EditBox_GetCurserCodepoint(ptr_EditBox);

  // set incission line to prev line
  ptr_EditBox->ptr_incission_line = ptr_EditBox->ptr_incission_line->ptr_PrevLine;

  // Set curser
  QTS_GetLineIncisionPoint_FromCodepoint(ptr_EditBox->ptr_incission_line, true_inc_codepoint, &(ptr_EditBox->incission_text_element), &(ptr_EditBox->x_cord), &(ptr_EditBox->back_codepoints), &(ptr_EditBox->incision_codepoint), &(ptr_EditBox->incision_index), ptr_EditBox->crosskerneling);

  // Update
  ptr_EditBox->last_time = SDL_GetTicks();
  QTS_EditBox_UpdateCurser(ptr_EditBox);

  // if line is empty
  if(!ptr_EditBox->incission_text_element->string){
    ptr_EditBox->incision_codepoint = -1;
  }

  return true;
}



// Function for Moving the curser down one line
bool QTS_EditBox_MoveCurserDown(struct QTS_EditBox* ptr_EditBox){

  // Validate Input
  if(!ptr_EditBox) return false;
  if(!ptr_EditBox->ptr_incission_line) return false;
  if(!ptr_EditBox->ptr_incission_line->ptr_NextLine) return false;

  // Get true codepoint
  int true_inc_codepoint = QTS_EditBox_GetCurserCodepoint(ptr_EditBox);

  // set incission line to prev line
  ptr_EditBox->ptr_incission_line = ptr_EditBox->ptr_incission_line->ptr_NextLine;

  // Set curser
  QTS_GetLineIncisionPoint_FromCodepoint(ptr_EditBox->ptr_incission_line, true_inc_codepoint, &(ptr_EditBox->incission_text_element), &(ptr_EditBox->x_cord), &(ptr_EditBox->back_codepoints), &(ptr_EditBox->incision_codepoint), &(ptr_EditBox->incision_index), ptr_EditBox->crosskerneling);

  // Update
  ptr_EditBox->last_time = SDL_GetTicks();
  QTS_EditBox_UpdateCurser(ptr_EditBox);

  // if line is empty
  if(!ptr_EditBox->incission_text_element->string){
    ptr_EditBox->incision_codepoint = -1;
  }

  return true;
}



// Function for switching the cursers line with the one above it
bool QTS_EditBox_SwitchCurserLineUp(struct QTS_EditBox* ptr_EditBox){

  // Validate Input
  if(!ptr_EditBox) return false;
  if(!ptr_EditBox->ptr_incission_line) return false;
  if(!ptr_EditBox->ptr_incission_line->ptr_PrevLine) return false;

  struct QTS_Line* ptr_Line_A = ptr_EditBox->ptr_incission_line;
  struct QTS_Line* ptr_Line_B = ptr_EditBox->ptr_incission_line->ptr_PrevLine;

  // if Line B is first Line
  if(!ptr_Line_B->ptr_PrevLine) ptr_EditBox->ptr_Box->ptr_LineElements = ptr_Line_A;

  if(ptr_Line_B->ptr_PrevLine) ptr_Line_B->ptr_PrevLine->ptr_NextLine = ptr_Line_A;
  if(ptr_Line_A->ptr_NextLine) ptr_Line_A->ptr_NextLine->ptr_PrevLine = ptr_Line_B;

  ptr_Line_A->ptr_PrevLine = ptr_Line_B->ptr_PrevLine;
  ptr_Line_B->ptr_NextLine = ptr_Line_A->ptr_NextLine;

  ptr_Line_A->ptr_NextLine = ptr_Line_B;
  ptr_Line_B->ptr_PrevLine = ptr_Line_A;

  // Update
  ptr_EditBox->last_time = SDL_GetTicks();
  QTS_EditBox_UpdateCurser(ptr_EditBox);

  return true;
}



// Function for switching the crser line with the one below it
bool QTS_EditBox_SwitchCurserLineDown(struct QTS_EditBox* ptr_EditBox){

  // Validate Input
  if(!ptr_EditBox) return false;
  if(!ptr_EditBox->ptr_incission_line) return false;
  if(!ptr_EditBox->ptr_incission_line->ptr_NextLine) return false;

  struct QTS_Line* ptr_Line_A = ptr_EditBox->ptr_incission_line->ptr_NextLine;
  struct QTS_Line* ptr_Line_B = ptr_EditBox->ptr_incission_line;

  // if Line B is first Line
  if(!ptr_Line_B->ptr_PrevLine) ptr_EditBox->ptr_Box->ptr_LineElements = ptr_Line_A;

  if(ptr_Line_B->ptr_PrevLine) ptr_Line_B->ptr_PrevLine->ptr_NextLine = ptr_Line_A;
  if(ptr_Line_A->ptr_NextLine) ptr_Line_A->ptr_NextLine->ptr_PrevLine = ptr_Line_B;

  ptr_Line_A->ptr_PrevLine = ptr_Line_B->ptr_PrevLine;
  ptr_Line_B->ptr_NextLine = ptr_Line_A->ptr_NextLine;

  ptr_Line_A->ptr_NextLine = ptr_Line_B;
  ptr_Line_B->ptr_PrevLine = ptr_Line_A;

  // Update
  ptr_EditBox->last_time = SDL_GetTicks();
  QTS_EditBox_UpdateCurser(ptr_EditBox);

  return true;
}





// Function for Hilighting the Word that the curser is currently in
bool QTS_EditBox_HilightWord(struct QTS_EditBox* ptr_EditBox){

  // Validate Input
  if(!ptr_EditBox) return false;
  if(!ptr_EditBox->incission_text_element) return false;
  if(!ptr_EditBox->ptr_incission_line) return false;

  // in case line is empty do nothing
  if(!ptr_EditBox->incission_text_element->string) return false;

  // Return Holding variable
  bool b_ret;

  // Get the string from the Line
  char* string;
  b_ret = QTS_GetLineString(ptr_EditBox->ptr_incission_line, &string, NULL, NULL);
  if(!b_ret || !string[0]) return false;

  // Get the string length
  size_t srg_length = 0;
  while(string[srg_length]) srg_length++;

  // calculate true Incission index (for full line string)
  size_t true_incision_index = 0;
  struct QTS_Text* ptr_text_holding = ptr_EditBox->ptr_incission_line->ptr_TextElements;

  while(ptr_text_holding != ptr_EditBox->incission_text_element && ptr_text_holding){
    true_incision_index += ptr_text_holding->length - 1;
    ptr_text_holding = ptr_text_holding->ptr_NextText;
  }

  true_incision_index += ptr_EditBox->incision_index;

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
  ptr_EditBox->highlight_start = start_codepoint;
  ptr_EditBox->highlight_end = end_codepoint;

  // set hilight lines
  ptr_EditBox->hilight_start_line = ptr_EditBox->ptr_incission_line;
  ptr_EditBox->hilight_end_line = ptr_EditBox->ptr_incission_line;

  return QTS_EditBox_UpdateHilight(ptr_EditBox);
}






// Function for Handeling Return key
bool QTS_EditBox_HandleReturn(SDL_Renderer *ptr_renderer, struct QTS_EditBox* ptr_EditBox){

  // Validate input
  if(!ptr_EditBox) return false;
  if(!ptr_EditBox->ptr_incission_line) return false;

  // split current incission line at the current codepoint
  int curser_codepoint = QTS_EditBox_GetCurserCodepoint(ptr_EditBox);
  struct QTS_Text* ptr_Text_holding = QTS_SplitLineAt(ptr_renderer, ptr_EditBox->ptr_incission_line, curser_codepoint);
  if(ptr_Text_holding){
    if(ptr_Text_holding->ptr_PrevText) ptr_Text_holding->ptr_PrevText->ptr_NextText = NULL;
    ptr_Text_holding->ptr_PrevText = NULL;
  }

  // if ptr_Text_holding is first element in current line
  if(ptr_EditBox->ptr_incission_line->ptr_TextElements == ptr_Text_holding){
    // put in an empty clone to replace it
    ptr_EditBox->ptr_incission_line->ptr_TextElements = QTS_CloneTextFormat(ptr_renderer, ptr_EditBox->incission_text_element);
  }

  // Create a line and add it after incission line
  struct QTS_Line* ptr_new_line = QTS_CreateLine();
  QTS_AddLineToBox(ptr_EditBox->ptr_Box, ptr_EditBox->ptr_incission_line, ptr_new_line);

  // if there is text to be added to the new line
  if(ptr_Text_holding){

    // add the text to the new line
    ptr_new_line->ptr_TextElements = ptr_Text_holding;

  }else{

    // add empty clone of incission text element
    ptr_Text_holding = QTS_CloneTextFormat(ptr_renderer, ptr_EditBox->incission_text_element);
    ptr_new_line->ptr_TextElements = ptr_Text_holding;

  }

  // Update Line
  QTS_UpdateLine(ptr_new_line);

  // set new line as incission line and set curser to beginning of that line
  ptr_EditBox->ptr_incission_line = ptr_new_line;
  ptr_EditBox->x_cord = 0;
  ptr_EditBox->back_codepoints = 0;
  ptr_EditBox->incision_codepoint = -1;
  ptr_EditBox->incision_index = 0;
  ptr_EditBox->incission_text_element = ptr_new_line->ptr_TextElements;

  // update curser
  // Update
  ptr_EditBox->last_time = SDL_GetTicks();
  QTS_EditBox_UpdateCurser(ptr_EditBox);

  return true;
}



// Function for Hilighting the current line
bool QTS_EditBox_HilightSelectedLine(struct QTS_EditBox* ptr_EditBox){

  // Validate Input
  if(!ptr_EditBox) return false;
  if(!ptr_EditBox->ptr_incission_line) return false;

  // set the hilight Lines
  ptr_EditBox->hilight_start_line = ptr_EditBox->ptr_incission_line;
  ptr_EditBox->hilight_end_line = ptr_EditBox->ptr_incission_line;

  ptr_EditBox->highlight_start = 0;
  ptr_EditBox->highlight_end = QTS_CountCodepointsInLine(ptr_EditBox->ptr_incission_line);

  ptr_EditBox->x_cord_start = 0;
  ptr_EditBox->x_cord_end = ptr_EditBox->ptr_incission_line->length;

  return QTS_EditBox_UpdateHilight(ptr_EditBox);
}





// End
