// QTS Edit Line Functions
/*
This Header Provides the Primmary functions for QTS Edit Lines.
*/

// Include QTS_Header
#include "QualityTextSystem.h"

// Function for Creating Edit line
// leave ptr_Line NULL if a line is suposed to be created for the edit line
struct QTS_EditLine* QTS_CreateEditLine(struct QTS_Line* ptr_Line, float x, float y, float w, float h, float scaling){

  // Allocate Edit Line structure
  struct QTS_EditLine* ptr_EditLine = (struct QTS_EditLine*)calloc(1, sizeof(struct QTS_EditLine));
  if(!ptr_EditLine) return NULL;

  // Initialize default values
  ptr_EditLine->visible = true;
  ptr_EditLine->enable_highlight = true;
  ptr_EditLine->enable_edit = true;
  ptr_EditLine->selected = false;
  ptr_EditLine->ignore_empty_textelements = true;
  ptr_EditLine->crosskerneling = true;
  ptr_EditLine->curser_render_func = QTS_EditLine_DrawCurser;
  ptr_EditLine->highlight_render_func = QTS_EditLine_DrawHighlight;
  ptr_EditLine->scaling_offset = 1.0f;
  if(scaling <= 0.0f){
    ptr_EditLine->scaling = 1.0f;
  }else{
    ptr_EditLine->scaling = scaling;
  }

  // Initialize line rect
  ptr_EditLine->line_rect.x = x;
  ptr_EditLine->line_rect.y = y;
  ptr_EditLine->line_rect.w = w;
  ptr_EditLine->line_rect.h = h;
  ptr_EditLine->offset_x = 0.0f;
  ptr_EditLine->offset_y = 0.0f;

  // Add Line
  if(!ptr_Line){
    // Create Line
    ptr_EditLine->ptr_Line = QTS_CreateLine();
  }else{
    // Assing Line
    ptr_EditLine->ptr_Line = ptr_Line;
  }

  return ptr_EditLine;
}


// Function for Destroying an edit Line
// if destroy_line_instance is true the Line instance that the Edit line is representing is also destroyed
void QTS_DestroyEditLine(struct QTS_EditLine* ptr_EditLine, bool destroy_line_instance){

  // Validate Input to avoid segmentation fault
  if(!ptr_EditLine) return;

  // Destroy line instance
  if(destroy_line_instance) QTS_DestroyLine(ptr_EditLine->ptr_Line);

  // Free Edit Line Structure
  free(ptr_EditLine);

  return;
}


// Function for Rendering the EditLine to the current render targed
bool QTS_RednerEditLine(SDL_Renderer *ptr_renderer, struct QTS_EditLine* ptr_EditLine){

  // Validate Input
  if(!ptr_renderer || !ptr_EditLine) return false;
  if(!ptr_EditLine->visible) return true;

  // Return holding variable
  bool b_ret;

  // Render Line
  b_ret = QTS_RenderLine(ptr_renderer, ptr_EditLine->ptr_Line, ptr_EditLine->line_rect.x, ptr_EditLine->line_rect.y, ptr_EditLine->ignore_empty_textelements, ptr_EditLine->crosskerneling);
  if(!b_ret) return false;

  // Render Highlight
  if(ptr_EditLine->highlight_present && ptr_EditLine->enable_highlight){
    b_ret = ptr_EditLine->highlight_render_func(ptr_renderer, ptr_EditLine);
  }


  // Render Curser
  // Only render cursor if is selected and edit is enabled
  if(ptr_EditLine->selected && ptr_EditLine->enable_edit && !ptr_EditLine->highlight_present){
    b_ret = ptr_EditLine->curser_render_func(ptr_renderer, ptr_EditLine);
  }


  return b_ret;
}


// Function for Updating the Line after an external edit has been made (typicaly by formatting function)
bool QTS_EditLine_Update(SDL_Renderer *ptr_renderer, struct QTS_EditLine* ptr_EditLine, bool Consolidate_Line){

  // Validate Input
  if(!ptr_renderer || !ptr_EditLine) return false;
  if(!ptr_EditLine->ptr_Line) return false;

  // if consolidate line is called, consolidate the line and update the curser and hilight text element pointers
  if(Consolidate_Line) QTS_ConsolidateLine(ptr_renderer, ptr_EditLine->ptr_Line, false);

  // Update Line parameters
  return QTS_UpdateLine(ptr_EditLine->ptr_Line);

  //return true;
}



// Function for Processing Events
// Returns true if event affected the edit box and false if not
bool QTS_EditLine_HandleEvent(SDL_Renderer *ptr_renderer, struct QTS_EditLine* ptr_EditLine, SDL_Event* ptr_Event){

  // Validate Input
  if(!ptr_EditLine || !ptr_Event) return false;

  // if is not enabled
  if(!ptr_EditLine->enable_edit && !ptr_EditLine->enable_highlight) return true;

  // Return Holding variable (true => an edit has been made; false => Event did not affect the edit line)
  bool b_ret = false;
  //bool body_edit = false; // is set to true if an edit occured that requiures that the reformating function be called

  // current selection status
  bool pre_select = ptr_EditLine->selected;

  // Event overide function here
  if(ptr_EditLine->event_proc_overide){
    // if an event processing override function exsists then runn it
    b_ret = ptr_EditLine->event_proc_overide(ptr_renderer, ptr_EditLine, ptr_Event, ptr_EditLine->ptr_event_overide_data);
  }

  // Only process event if was allowed by event_proc_overide function
  if(!b_ret){

    // Process Event
    switch(ptr_Event->type) {
      case SDL_EVENT_MOUSE_BUTTON_DOWN:
        // Remember to include offset (is already included in default QTS_Function)
        if(ptr_Event->button.clicks >= 3){

          // Select box and hilight entiure Line
          b_ret = QTS_EditLine_HandleMouseButtonDown(ptr_EditLine, ptr_Event);
          if(ptr_EditLine->selected) b_ret = QTS_EditLine_HilightAll(ptr_EditLine);

        } else if(ptr_Event->button.clicks == 2){
          // Doubble click
          // replace with hilight ckicked word
          b_ret = QTS_EditLine_HandleMouseButtonDown(ptr_EditLine, ptr_Event);
          if(ptr_EditLine->selected) b_ret = QTS_EditLine_HilightWord(ptr_EditLine);

        }else{
          // single cklck
          b_ret = QTS_EditLine_HandleMouseButtonDown(ptr_EditLine, ptr_Event);
        }
        break;

      case SDL_EVENT_MOUSE_MOTION:
        b_ret = QTS_EditLine_HadleMoueMotion(ptr_EditLine, ptr_Event);
        break;

      case SDL_EVENT_MOUSE_BUTTON_UP:
        b_ret = QTS_EditLine_HandleMouseButtonUp(ptr_EditLine, ptr_Event);
        break;

      case SDL_EVENT_TEXT_INPUT:
        //printf("text %s \n", ptr_Event->text.text);
        if(ptr_EditLine->selected){
          b_ret = QTS_EditLine_HandleTextInput(ptr_renderer, ptr_EditLine, ptr_Event);
        }
        break;

      case SDL_EVENT_KEY_DOWN:

        // if is not selected dont do annything
        if(!ptr_EditLine->selected) break;

        // if is selected process Command input
        switch (ptr_Event->key.key) {
          case SDLK_A:
            if(ptr_Event->key.mod & SDL_KMOD_CTRL) {
              // Hilight full Line
              b_ret = QTS_EditLine_HilightAll(ptr_EditLine);
            }
            break;

          case SDLK_C:
            if(ptr_Event->key.mod & SDL_KMOD_CTRL) {
              // Copy selected text string to clipboard
              b_ret = QTS_EditLine_CopySelectedTextToClipboard(ptr_EditLine);
            }
            break;

          case SDLK_V:
            if(ptr_Event->key.mod & SDL_KMOD_CTRL) {
              // Paste text from Clipboard at inssisionpoint
              b_ret = QTS_EditLine_PasetTextFromClipboard(ptr_renderer, ptr_EditLine);
            }
            break;

          case SDLK_X:
            if(ptr_Event->key.mod & SDL_KMOD_CTRL) {
              // cut selected text (copy on to clipboard and delete from line)
              b_ret = QTS_EditLine_CutSelectedText(ptr_renderer, ptr_EditLine);
            }
            break;

          case SDLK_LEFT:
            if(ptr_Event->key.mod & SDL_KMOD_CTRL) {
              // Move curser to beginning of line
              b_ret = QTS_EditLine_MoveCurserToBegining(ptr_EditLine);
            }else{
              // Move Curser one codepoint to the left
              b_ret = QTS_EditLine_MoveCurser(ptr_EditLine, -1);
            }
            break;

          case SDLK_RIGHT:
            if(ptr_Event->key.mod & SDL_KMOD_CTRL) {
              // move curser to end of line
              b_ret = QTS_EditLine_MoveCurserToEnd(ptr_EditLine);
            }else{
              // move curser one codepoint to the right
              b_ret = QTS_EditLine_MoveCurser(ptr_EditLine, 1);
            }
            break;
  /*
          // These cases are not needed because Editline is one line
          case SDLK_UP:
            if(ptr_Event->key.mod & SDL_KMOD_CTRL) {
              // move curser to end of line
            }else{
              // move curser one codepoint to the left
            }
            break;

          case SDLK_DOWN:
            if(ptr_Event->key.mod & SDL_KMOD_CTRL) {
              // move curser to end of line
            }else{
              // move curser one codepoint to the left
            }
            break;
  */
          case SDLK_HOME:
            //Move cursor to beginning of the line
            b_ret = QTS_EditLine_MoveCurserToBegining(ptr_EditLine);
            break;

          case SDLK_END:
            // Move curser to end of line
            b_ret = QTS_EditLine_MoveCurserToEnd(ptr_EditLine);
            break;

          case SDLK_BACKSPACE:
            if(ptr_Event->key.mod & SDL_KMOD_CTRL){
              // backspace to beginning
              b_ret = QTS_EditLine_BackspaceToBeginning(ptr_renderer, ptr_EditLine);
            }else{
              b_ret = QTS_EditLine_Backspace(ptr_renderer, ptr_EditLine);
            }
            break;

          case SDLK_DELETE:
            if(ptr_Event->key.mod & SDL_KMOD_CTRL){
              // Delete to end
              b_ret = QTS_EditLine_DeleteToEnd(ptr_renderer, ptr_EditLine);
            }else{
              // Delete
              b_ret = QTS_EditLine_Delete(ptr_renderer, ptr_EditLine);
            }
            break;

          case SDLK_RETURN:
            // Process return (posibly with special return function)

            break;

          case SDLK_ESCAPE:
            // Unselect Editbox
            b_ret = QTS_EditLine_Escape(ptr_EditLine);
            break;

          default:
            break;
        } // end of process key input

  /*
      case SDL_EVENT_TEXT_INPUT:
        // if not selected ignore
        // Insert text ptr_Event->text.text
        if(ptr_EditLine->selected){
          b_ret = QTS_EditLine_HandleTextInput(ptr_renderer, ptr_EditLine, ptr_Event);
        }
        break;
  */
      case SDL_EVENT_TEXT_EDITING:
        // if not selected ignore
        // Handle composition
        // Accesing this event may cause segmentation fault. if so then move this event more towards the top of the switch, witch works for some reson.
        break;

      case SDL_EVENT_TEXT_EDITING_CANDIDATES:
        // if not selected ignore
        // figure out what this is
        // Accesing this event may cause segmentation fault. if so then move this event more towards the top of the switch, witch works for some reson.
        break;

      default:
        break;
    }// End of event processing switch

  } // end of allowed by event override statment


  // Enable text input if is selected
  if(ptr_EditLine->selected && !SDL_TextInputActive(SDL_GetRenderWindow(ptr_renderer))){
    SDL_StartTextInput(SDL_GetRenderWindow(ptr_renderer));
  }


  // if was selected start text input else stop text input
  if(!pre_select && ptr_EditLine->selected){
    SDL_StartTextInput(SDL_GetRenderWindow(ptr_renderer));
  }else if(pre_select && !ptr_EditLine->selected){
    SDL_StopTextInput(SDL_GetRenderWindow(ptr_renderer));
  }

  // Update Line , curser and Hilight
  if(b_ret){
    // An eddit was appplyed
    QTS_UpdateLine(ptr_EditLine->ptr_Line);

    // Call Reformating function
    if(ptr_EditLine->format_function) b_ret = ptr_EditLine->format_function(ptr_renderer, ptr_EditLine->ptr_Line, ptr_EditLine->ptr_foramt_data);

  }

  // Return fase by default
  return b_ret;
}






// END
