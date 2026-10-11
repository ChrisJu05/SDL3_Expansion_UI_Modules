// Include QTS_Header
#include "QualityTextSystem.h"

// Function for Creating an Edit Box
// ptr_Box is a box that can be passed to the edit box to fill it with content (may be null if a box is to be allocated)
struct QTS_EditBox* QTS_CreateEditBox(struct QTS_Box* ptr_Box, float x, float y, float w, float h, float scaling){

  // Allocate the EditBox structure
  struct QTS_EditBox* ptr_EditBox = (struct QTS_EditBox*)calloc(1, sizeof(struct QTS_EditBox));
  if(!ptr_EditBox) return NULL;

  // if no box is passed allocate a new box
  if(!ptr_Box){

    // Create Box
    ptr_EditBox->ptr_Box = QTS_CreateBox(NULL);
    if(!ptr_EditBox->ptr_Box) QTS_DestroyEditBox(ptr_EditBox);

  }else{

    ptr_EditBox->ptr_Box = ptr_Box;

  }

  // Configure Default settings
  ptr_EditBox->visible = true;
  ptr_EditBox->enable_highlight = true;
  ptr_EditBox->enable_edit = true;
  ptr_EditBox->crosskerneling = true;

  // Configure rect
  ptr_EditBox->box_rect.x = x;
  ptr_EditBox->box_rect.y = y;
  ptr_EditBox->box_rect.w = w;
  ptr_EditBox->box_rect.h = h;
  ptr_EditBox->scaling = scaling;
  ptr_EditBox->scaling_offset = 1.0f;

  // Asinging default functions
  ptr_EditBox->curser_render_func = QTS_EditBox_DrawCurser;
  ptr_EditBox->highlight_render_func = QTS_EditBox_DrawHighlightRect;


  return ptr_EditBox;
}




// Function for Destroying a Edid Box
void QTS_DestroyEditBox(struct QTS_EditBox* ptr_EditBox){

  // Validate input
  if(!ptr_EditBox) return;

  // Destroy the Box inside
  QTS_DestroyBox(ptr_EditBox->ptr_Box);

  // Destroy the EditBox structure
  free(ptr_EditBox);

  return;
}




// Function for Rendering the QTS_EditBox
bool QTS_RenderEditBox(SDL_Renderer *ptr_renderer, struct QTS_EditBox* ptr_EditBox){

  // Validate Input
  if(!ptr_renderer || !ptr_EditBox) return false;
  if(!ptr_EditBox->ptr_Box) return false;
  if(!ptr_EditBox->visible) return true;

  // Holding return variable
  bool b_ret;

  // Render the box
  b_ret = QTS_RenderBox(ptr_renderer, ptr_EditBox->ptr_Box, ptr_EditBox->box_rect.x, ptr_EditBox->box_rect.y);
  if(!b_ret) return false;

  // Render the Hilights
  b_ret = QTS_EditBox_DrawHilights(ptr_renderer, ptr_EditBox);
  if(!b_ret) return false;

  // Render the curser
  // only render the curser if selected and edit is enabled
  if(ptr_EditBox->selected && ptr_EditBox->enable_edit && !ptr_EditBox->highlight_present){
    b_ret = QTS_EditBox_DrawCurser(ptr_renderer, ptr_EditBox);
    if(!b_ret) return false;
  }

  return true;
}







// Function for Handeling Events
bool QTS_EditBox_HandleEvent(SDL_Renderer *ptr_renderer, struct QTS_EditBox* ptr_EditBox, SDL_Event* ptr_Event){

  // Validate Input
  if(!ptr_renderer || !ptr_EditBox || !ptr_Event) return false;

  // if is not enambled
  if(!ptr_EditBox->enable_edit && !ptr_EditBox->enable_highlight) return true;

  // Return Holding variable
  bool b_ret = false;

  // Current selection status
  bool pre_select = ptr_EditBox->selected;

  // Event overide Function
  if(ptr_EditBox->event_proc_overide){
    // if an Event processing overide function exsists then runn it
    b_ret = ptr_EditBox->event_proc_overide(ptr_renderer, ptr_EditBox, ptr_Event, ptr_EditBox->ptr_event_overide_data);
  }

  // only Process the event if was allowed by event_proc_overide function
  if(!b_ret){

    // Process Event
    switch(ptr_Event->type) {
      case SDL_EVENT_MOUSE_BUTTON_DOWN:
        // Remember to include offset (is already included in default QTS_Function)
        if(ptr_Event->button.clicks >= 3){

          // Select box and hilight entiure Line
          b_ret = QTS_EditBox_HandleMouseButtonDown(ptr_EditBox, ptr_Event);
          //b_ret = QTS_EditBox_HilightAll(ptr_EditBox);
          b_ret = QTS_EditBox_HilightSelectedLine(ptr_EditBox);

        } else if(ptr_Event->button.clicks == 2){
          // Doubble click
          // replace with hilight ckicked word
          b_ret = QTS_EditBox_HandleMouseButtonDown(ptr_EditBox, ptr_Event);
          b_ret = QTS_EditBox_HilightWord(ptr_EditBox);

        }else{
          // single cklck
          b_ret = QTS_EditBox_HandleMouseButtonDown(ptr_EditBox, ptr_Event);
        }
        break;

      case SDL_EVENT_MOUSE_MOTION:
        b_ret = QTS_EditBox_HadleMoueMotion(ptr_EditBox, ptr_Event);
        break;

      case SDL_EVENT_MOUSE_BUTTON_UP:
        b_ret = QTS_EditBox_HandleMouseButtonUp(ptr_EditBox, ptr_Event);
        break;

      case SDL_EVENT_TEXT_INPUT:
        //printf("text %s \n", ptr_Event->text.text);
        if(ptr_EditBox->selected){
          b_ret = QTS_EditBox_HandleTextInput(ptr_renderer, ptr_EditBox, ptr_Event);
        }
        break;

      case SDL_EVENT_KEY_DOWN:

        // if is not selected dont do annything
        if(!ptr_EditBox->selected) break;

        // if is selected process Command input
        switch (ptr_Event->key.key) {
          case SDLK_A:
            if(ptr_Event->key.mod & SDL_KMOD_CTRL) {
              // Hilight full Line
              b_ret = QTS_EditBox_HilightAll(ptr_EditBox);
            }
            break;

          case SDLK_C:
            if(ptr_Event->key.mod & SDL_KMOD_CTRL) {
              // Copy selected text string to clipboard
              b_ret = QTS_EditBox_CopySelectedTextToClipboard(ptr_EditBox);
            }
            break;

          case SDLK_V:
            if(ptr_Event->key.mod & SDL_KMOD_CTRL) {
              // Paste text from Clipboard at inssisionpoint
              b_ret = QTS_EditBox_PasetTextFromClipboard(ptr_renderer, ptr_EditBox);
            }
            break;

          case SDLK_X:
            if(ptr_Event->key.mod & SDL_KMOD_CTRL) {
              // cut selected text (copy on to clipboard and delete from line)
              b_ret = QTS_EditBox_CutSelectedText(ptr_renderer, ptr_EditBox);
            }
            break;

          case SDLK_LEFT:
            if(ptr_Event->key.mod & SDL_KMOD_CTRL) {
              // Move curser to beginning of line
              b_ret = QTS_EditBox_MoveCurserToBegining(ptr_EditBox);
            }else{
              // Move Curser one codepoint to the left
              b_ret = QTS_EditBox_MoveCurser(ptr_EditBox, -1);
            }
            break;

          case SDLK_RIGHT:
            if(ptr_Event->key.mod & SDL_KMOD_CTRL) {
              // move curser to end of line
              b_ret = QTS_EditBox_MoveCurserToEnd(ptr_EditBox);
            }else{
              // move curser one codepoint to the right
              b_ret = QTS_EditBox_MoveCurser(ptr_EditBox, 1);
            }
            break;

          case SDLK_UP:
            if(ptr_Event->key.mod & SDL_KMOD_CTRL) {
              // switch line with the line above it
              b_ret = QTS_EditBox_SwitchCurserLineUp(ptr_EditBox);
            }else{
              // move curser one line up
              QTS_EditBox_MoveCurserUp(ptr_EditBox);
            }
            break;

          case SDLK_DOWN:
            if(ptr_Event->key.mod & SDL_KMOD_CTRL) {
              // switch line with the line below it
              b_ret = QTS_EditBox_SwitchCurserLineDown(ptr_EditBox);
            }else{
              // move curser one Line down
              QTS_EditBox_MoveCurserDown(ptr_EditBox);
            }
            break;

          case SDLK_HOME:
            //Move cursor to beginning of the line
            b_ret = QTS_EditBox_MoveCurserToBegining(ptr_EditBox);
            break;

          case SDLK_END:
            // Move curser to end of line
            b_ret = QTS_EditBox_MoveCurserToEnd(ptr_EditBox);
            break;

          case SDLK_BACKSPACE:
            if(ptr_Event->key.mod & SDL_KMOD_CTRL){
              // backspace to beginning
              b_ret = QTS_EditBox_BackspaceToBeginning(ptr_renderer, ptr_EditBox);
            }else{
              b_ret = QTS_EditBox_Backspace(ptr_renderer, ptr_EditBox);
              b_ret = QTS_EditBox_DestroyHilightedText(ptr_renderer, ptr_EditBox);
            }
            break;

          case SDLK_DELETE:
            if(ptr_Event->key.mod & SDL_KMOD_CTRL){
              // Delete to end
              b_ret = QTS_EditBox_DeleteToEnd(ptr_renderer, ptr_EditBox);
            }else{
              // Delete
              b_ret = QTS_EditBox_Delete(ptr_renderer, ptr_EditBox);
            }
            break;

          case SDLK_RETURN:
            // Process return (posibly with special return function)
            b_ret = QTS_EditBox_HandleReturn(ptr_renderer, ptr_EditBox);
            break;

          case SDLK_ESCAPE:
            // Unselect Editbox
            b_ret = QTS_EditBox_Escape(ptr_EditBox);
            break;

          default:
            break;
        } // end of process key input

  /*
      case SDL_EVENT_TEXT_INPUT:
        // if not selected ignore
        // Insert text ptr_Event->text.text
        if(ptr_EditBox->selected){
          b_ret = QTS_EditBox_HandleTextInput(ptr_renderer, ptr_EditBox, ptr_Event);
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
  if(ptr_EditBox->selected && !SDL_TextInputActive(SDL_GetRenderWindow(ptr_renderer))){
    SDL_StartTextInput(SDL_GetRenderWindow(ptr_renderer));
  }

  // if was selected start text input else stop text input
  if(!pre_select && ptr_EditBox->selected){
    SDL_StartTextInput(SDL_GetRenderWindow(ptr_renderer));
  }else if(pre_select && !ptr_EditBox->selected){
    SDL_StopTextInput(SDL_GetRenderWindow(ptr_renderer));
  }

  // Update Line if a change occured
  if(b_ret){

    // Update the Box
    QTS_UpdateBoxParameters(ptr_EditBox->ptr_Box);

    // Call Reformating Function
    if(ptr_EditBox->format_function) b_ret = ptr_EditBox->format_function(ptr_renderer, ptr_EditBox->ptr_Box, ptr_EditBox->ptr_foramt_data);

  }

  return b_ret;
}
