// Qualety Text Core by Christopher Jung
// for Creating Dynamic text elements with SDL3 and SDL3 TTF

#include "QualityTextSystem.h"


// Creates a QTS_Line struct in memmory with all parameters initialized to zero
// Returns NULL if the allocation failed
struct QTS_Line* QTS_CreateLine()
{

  // Create a Line struct in memmory
  struct QTS_Line* ptr_line = (struct QTS_Line*)calloc(1, sizeof(struct QTS_Line));
  if(!ptr_line) return NULL;

  return ptr_line;
}// end of QTS_CreateLine function




// Adds QTS_Text element after a given element
// ptr_text_root is the element in the line behind witch ptr_text_add and its following elements are added (NULL for add as first element)
// if the QTS_Text Element has a list connected, the list is added to
bool QTS_AddTextToLine_After(struct QTS_Line* ptr_line, struct QTS_Text* ptr_text_root, struct QTS_Text* ptr_text_add)
{
  // Validate Input
  if(!ptr_line || !ptr_text_add) return false;

  // Create a Holding pointer for remainder of list
  struct QTS_Text* ptr_text_remain;
  struct QTS_Text* ptr_text_holding;

  // if ptr_text_root is NULL then add as first section
  if(!ptr_text_root) {

    // get currently first element
    ptr_text_remain = ptr_line->ptr_TextElements;

    // Add ptr_text_add as first element in line
    ptr_line->ptr_TextElements = ptr_text_add;
    ptr_text_add->ptr_PrevText = NULL;

  } else {

    // Get remainder of line
    ptr_text_remain = ptr_text_root->ptr_NextText;

    // Add ptr_text_add after ptr_text_root
    ptr_text_root->ptr_NextText = ptr_text_add;
    ptr_text_add->ptr_PrevText = ptr_text_root;

  }

  // Itterate through Added elements and adjust line parameters as needet
  ptr_text_holding = ptr_text_add;

  // in case ptr_text_add is a single element
  // Adjust parameters
  if(ptr_line->MaxTextHight < ptr_text_add->hight) ptr_line->MaxTextHight = ptr_text_add->hight;
  if(ptr_line->MaxTextAscent < ptr_text_add->ascent) ptr_line->MaxTextAscent = ptr_text_add->ascent;
  if(ptr_line->MaxTextDescent < ptr_text_add->descent) ptr_line->MaxTextDescent = ptr_text_add->descent;

  if(ptr_text_add->ptr_texture) ptr_line->length += ptr_text_add->ptr_texture->w;

  while(ptr_text_holding->ptr_NextText) {

    // Get next element
    ptr_text_holding = ptr_text_holding->ptr_NextText;

    // Adjust parameters
    if(ptr_line->MaxTextHight < ptr_text_holding->hight) ptr_line->MaxTextHight = ptr_text_holding->hight;
    if(ptr_line->MaxTextAscent < ptr_text_holding->ascent) ptr_line->MaxTextAscent = ptr_text_holding->ascent;
    if(ptr_line->MaxTextDescent < ptr_text_holding->descent) ptr_line->MaxTextDescent = ptr_text_holding->descent;

    if(ptr_text_holding->ptr_texture) ptr_line->length += ptr_text_holding->ptr_texture->w;

  }

  // Recalculate Remaining general Values
  ptr_line->hight = ptr_line->MaxTextAscent + ptr_line->MaxTextDescent + ptr_line->Spaceing_Up + ptr_line->Spaceing_Down;

  // Add Remains of Line after the added section
  ptr_text_holding->ptr_NextText = ptr_text_remain;
  if(ptr_text_remain) ptr_text_remain->ptr_PrevText = ptr_text_holding;

  // Adjust pre kerneling
  ptr_text_add->pre_kernel_offset = QTS_GetPreKernelOffset(ptr_text_add);

  return true;
}




// Function for Updating a Line after a QTS_Text object has been eddited
bool QTS_UpdateLine(struct QTS_Line* ptr_line)
{
  // Validate input
  if(!ptr_line) return false;

  // Itterate through Added elements and adjust line parameters as needet
  struct QTS_Text* ptr_text_holding = ptr_line->ptr_TextElements;
  if(!ptr_text_holding) return true;

  // In case the line is empty
  if(!ptr_text_holding){
    return true;
  }

  // Reset Parameters of line
  ptr_line->MaxTextHight = 0;
  ptr_line->MaxTextAscent = 0;
  ptr_line->MaxTextDescent = 0;
  ptr_line->length = 0;

  // in case ptr_text_add is a single element
  // Adjust parameters
  if(ptr_line->MaxTextHight < ptr_text_holding->hight) ptr_line->MaxTextHight = ptr_text_holding->hight;
  if(ptr_line->MaxTextAscent < ptr_text_holding->ascent) ptr_line->MaxTextAscent = ptr_text_holding->ascent;
  if(ptr_line->MaxTextDescent < ptr_text_holding->descent) ptr_line->MaxTextDescent = ptr_text_holding->descent;

  if(ptr_text_holding->ptr_texture) ptr_line->length += ptr_text_holding->ptr_texture->w;

  while(ptr_text_holding->ptr_NextText) {

    // Get next element
    ptr_text_holding = ptr_text_holding->ptr_NextText;

    // Adjust parameters
    if(ptr_line->MaxTextHight < ptr_text_holding->hight) ptr_line->MaxTextHight = ptr_text_holding->hight;
    if(ptr_line->MaxTextAscent < ptr_text_holding->ascent) ptr_line->MaxTextAscent = ptr_text_holding->ascent;
    if(ptr_line->MaxTextDescent < ptr_text_holding->descent) ptr_line->MaxTextDescent = ptr_text_holding->descent;

    if(ptr_text_holding->ptr_texture) ptr_line->length += ptr_text_holding->ptr_texture->w;

    // Adjust pre kerneling
    ptr_text_holding->pre_kernel_offset = QTS_GetPreKernelOffset(ptr_text_holding);

  }

  // Recalculate Remaining general Values
  ptr_line->hight = ptr_line->MaxTextAscent + ptr_line->MaxTextDescent + ptr_line->Spaceing_Up + ptr_line->Spaceing_Down;

  return true;
}





// Function for Removing a QTS_Text object from a Line safely (the removed object is not freed)
bool QTS_RemoveTextFromLine(struct QTS_Line* ptr_line, struct QTS_Text* ptr_text_remove)
{
  // Validate Input
  if(!ptr_line || !ptr_text_remove) return false;

  // Remove ptr_text_remove from List of text objects in line
  if(ptr_text_remove->ptr_PrevText){
    ptr_text_remove->ptr_PrevText->ptr_NextText = ptr_text_remove->ptr_NextText;
  } else{
    // is first element in line
    ptr_line->ptr_TextElements = ptr_text_remove->ptr_NextText;
  }
  if(ptr_text_remove->ptr_NextText) ptr_text_remove->ptr_NextText->ptr_PrevText = ptr_text_remove->ptr_PrevText;

  // Reset Removed objects pointers to null for safety
  ptr_text_remove->ptr_PrevText = NULL;
  ptr_text_remove->ptr_NextText = NULL;

  // finaly Update the line
  return QTS_UpdateLine(ptr_line);
}




// Function for creating a Text object splitt in a line at a given code point (returns QTS_Text element starting with split codepoint)
// if a split already exsists at the specefyed codepoint the function does nothing and returns the requested struct QTS_Text object
struct QTS_Text* QTS_SplitLineAt(SDL_Renderer *ptr_renderer, struct QTS_Line* ptr_line, int split_codepoint){

  // Validate Input
  if(!ptr_renderer || !ptr_line || split_codepoint < 0) return NULL;

  // if line is empty return NULL
  if(!ptr_line->ptr_TextElements) return NULL;

  // If the split location is 0 then return the first element
  if(split_codepoint == 0) return ptr_line->ptr_TextElements;

  struct QTS_Text* ptr_text_holding;
  struct QTS_Text* ptr_text_split;
  int ref_count = 0;

  ptr_text_holding = ptr_line->ptr_TextElements;

  while(ptr_text_holding){

    // Add codepoint count
    ref_count += ptr_text_holding->codepoints;

    if(ref_count > split_codepoint){

      // Split QTS_Text object ptr_text_holding at given codepoint
      ptr_text_split = QTS_SplitText(ptr_renderer, ptr_text_holding, split_codepoint - (ref_count - ptr_text_holding->codepoints));
      // Adjust pre kerneling
      ptr_text_split->pre_kernel_offset = QTS_GetPreKernelOffset(ptr_text_split);

      return ptr_text_split;

    } else if(ref_count == split_codepoint){

      // No splitting neccecary as there is allready a division of the text objects between those codepoints
      return ptr_text_holding->ptr_NextText;

    }

    // Get NExt element
    ptr_text_holding = ptr_text_holding->ptr_NextText;
  }

  return NULL;
}




// Funtion for removing a section of codepoints from a Line (split location is the Incision point before the char)(if end_code_point = 0 then remove untill end)
// ptr_line is the line to remove the section from
// start_code_point / end_code_point set the range of what characters to remove
// ptr_ptr_text_remove is the addres off a pointer where the removed QTS_Text objects will be placed (NULL for Destroy)
bool QTS_RemoveSectionFromLine(SDL_Renderer *ptr_renderer, struct QTS_Line* ptr_line, int start_code_point,  int end_code_point, struct QTS_Text** ptr_ptr_text_remove)
{
  // Validate input
  if(!ptr_line || start_code_point < 0 || end_code_point < 0) return false;
  if(end_code_point < start_code_point && end_code_point != 0) return false;
  if(!ptr_line->ptr_TextElements) return true;

  struct QTS_Text* ptr_text_rmv_start = NULL; // the first QTS_Text object that needs to be removed
  struct QTS_Text* ptr_text_remainder = NULL; // the remainder that stays in the line

  // split at start and end codepoint
  ptr_text_rmv_start = QTS_SplitLineAt(ptr_renderer, ptr_line, start_code_point);
  //QTS_PrintLineComposition(ptr_line);

  // Splitt at endcodepoint
  if(end_code_point != 0) ptr_text_remainder = QTS_SplitLineAt(ptr_renderer, ptr_line, end_code_point);

  if(!ptr_text_rmv_start) return true; // implys that codepoint out of range for line


  // Remove section
  // Unbind front
  if(ptr_text_rmv_start->ptr_PrevText){
    ptr_text_rmv_start->ptr_PrevText->ptr_NextText = ptr_text_remainder;
  }


  // Disconnect last removed text object from line on back side
  if(ptr_text_remainder) ptr_text_remainder->ptr_PrevText->ptr_NextText = NULL; // this is where things go wrong


  // Unbind back (if needed)
  if(ptr_text_remainder){
    ptr_text_remainder->ptr_PrevText = ptr_text_rmv_start->ptr_PrevText;
  }


  // Nullyfy connections to line of the removed section (for safty)
  ptr_text_rmv_start->ptr_PrevText = NULL;
  //if(ptr_text_remainder) ptr_text_remainder->ptr_NextText = NULL;


  // if first element was removed
  if(ptr_text_rmv_start == ptr_line->ptr_TextElements){
    ptr_line->ptr_TextElements = ptr_text_remainder;
    if(ptr_text_remainder) ptr_text_remainder->ptr_PrevText = NULL;
  }


  // give removed section away
  if(ptr_ptr_text_remove){

    *ptr_ptr_text_remove = ptr_text_rmv_start;

  }else{
    // Destroy the removed section
    QTS_DestroyTextChain(ptr_text_rmv_start);
    /*
    struct QTS_Text* ptr_text_holding;
    while(ptr_text_rmv_start){
      ptr_text_holding = ptr_text_rmv_start->ptr_NextText;
      QTS_DestroyTextOBJ(ptr_text_rmv_start);
      ptr_text_rmv_start = ptr_text_holding;
    }
    */

  }

  // Update Line to adjust the parameters
  return QTS_UpdateLine(ptr_line);
}



// Function for getting the number of codepoints in the line
// NOTE: This may be diffrent to the string size of the line due to UTF8 standard use QTS_GetLineSize_ST to get the size of the string.
int QTS_GetLineSize_CP(struct QTS_Line* ptr_line){

  // Validate input
  if(!ptr_line) return 0;

  // Itterater through list and add up codepoints
  int count = 0;
  struct QTS_Text* ptr_text_holding = ptr_line->ptr_TextElements;

  while(ptr_text_holding){
    count += ptr_text_holding->codepoints;
    ptr_text_holding = ptr_text_holding->ptr_NextText;
  }

  return count;
}


// Function for getting the size of a string in bytes to fitt all characters in the line + NULL termination a the end
size_t QTS_GetLineSize_ST(struct QTS_Line* ptr_line){

  // Validate input
  if(!ptr_line) return 0;

  // Itterater through list and add up codepoints
  size_t count = 0;
  struct QTS_Text* ptr_text_holding = ptr_line->ptr_TextElements;
  while(ptr_text_holding){
    if(ptr_text_holding->string) count += (ptr_text_holding->length - 1); // -1 to remove NULL Termination
    ptr_text_holding = ptr_text_holding->ptr_NextText;
  }

  return count;
}


// Function for Creating a string that holds all the text in the line (is a new string allocation not owned)
// Return true on sucsess and false on fail, if the line is empty the string pointer wil be set to NULL and true is returned
// ptr_line Pointer to the line where the string is devived from
// ptr_ptr_string is the address of the pointer so the pointer can be set to the location of the newly created string
// ptr_codepoints a pointer to an integer where the number of codepoints in the line will be written to (put NULL if not needed)
// ptr_size a pointer to a size variable where the size in bytes of the string wil be written (put NULL if not needed) (size includes NULL Termination)
bool QTS_GetLineString(struct QTS_Line* ptr_line, char** ptr_ptr_string, int* ptr_codepoints, size_t* ptr_size){

  // Validate Input
  if(!ptr_line || !ptr_ptr_string) return false;

  // If line is empty
  if(!ptr_line->ptr_TextElements){
    *ptr_ptr_string = NULL;
    *ptr_codepoints = 0;
    *ptr_size = 0;
    return true;
  }

  // Create a string for the line
  size_t string_size = QTS_GetLineSize_ST(ptr_line);
  int codepoint_count = QTS_GetLineSize_CP(ptr_line);
  char* ptr_string = (char*)malloc(string_size);

  if(string_size == 0){
    if(ptr_ptr_string) *ptr_ptr_string = NULL;
    if(ptr_codepoints) *ptr_codepoints = 0;
    if(ptr_size) *ptr_size = 0;
    return true;
  }


  // Give size and codepoint count
  if(ptr_codepoints) *ptr_codepoints = codepoint_count;
  if(ptr_size) *ptr_size = string_size;

  // Itterate through the Line and copy strings in to the created string
  struct QTS_Text* ptr_text_holding = ptr_line->ptr_TextElements;
  size_t byte_count = 0;
  while(ptr_text_holding){

    if(ptr_text_holding->string){
      // Copy string from QTS_Text object in to Created string
      memcpy(&(ptr_string[byte_count]), ptr_text_holding->string, ptr_text_holding->length - 1); // -1 because NULL termination not copyed over

      // Increment bytecounter by the number of bytes copyed
      byte_count += (ptr_text_holding->length - 1);
    }
    /*
    // Copy string from QTS_Text object in to Created string
    memcpy(&(ptr_string[byte_count]), ptr_text_holding->string, ptr_text_holding->length - 1); // -1 because NULL termination not copyed over

    // Increment bytecounter by the number of bytes copyed
    byte_count += (ptr_text_holding->length - 1);
    */
    // Get next QTS_Text Object
    ptr_text_holding = ptr_text_holding->ptr_NextText;

  }

  // Give the created string
  *ptr_ptr_string = ptr_string;

  return true;
}



// Function for Printing the Line string
void QTS_PrintLine(struct QTS_Line* ptr_line){

  // Validate input
  if(!ptr_line) return;

  // return holding variable
  bool b_ret;

  // Get the string
  char* str = NULL;
  b_ret = QTS_GetLineString(ptr_line, &str, NULL, NULL);
  if(!b_ret){
    if(str) free(str);
    printf("FAILED TO GET LINE STRING \n");
    return;
  }

  // print the Line string
  printf("%s", str);
  fflush(stdout);

  // free the string
  free(str);

  return;
}

// Function for getting a string from a section from a line
// return true on sucsess and false on fail
// ptr_line is the line to get the string from
// start_code_point / end_code_point are the codepoints at witch to start and end the string (if end_code_point = 0 then the string from start codepoint to the end of the line is extracted)
// ptr_ptr_string is the adress of the pointer that will be set to point to the newly allocated string
// ptr_size a pointer to the size variable that wil be set to the number of bytes the extracted string is long (including the NULL termination) put NULL if not needet
bool QTS_GetLineSectionString(struct QTS_Line* ptr_line, int start_code_point,  int end_code_point, char** ptr_ptr_string, size_t* ptr_size){

  // Validate Input
  if(!ptr_line || !ptr_ptr_string || start_code_point < 0 || end_code_point < 0) return false;

  // Get string of whole Line
  char* full_string;
  size_t full_length;
  int codepoints;
  QTS_GetLineString(ptr_line, &full_string, &codepoints, &full_length);

  if(codepoints < start_code_point){
    free(full_string);
    return false;
  }

  // Copy out section in to other string
  // Variables for UTF8 stepping
  const char* ptr_string_mov = full_string;
  //char* ptr_string_start;
  size_t adj_length = full_length;
  size_t copy_size;

  // Get starting Addres
  for(int i = 0; i < start_code_point; i++) SDL_StepUTF8(&ptr_string_mov, &adj_length);

  copy_size = adj_length; // size of remaining string after startcodepoint
  const char* ptr_string_start = ptr_string_mov;

  if((end_code_point != 0) && (end_code_point < codepoints)){ // if end_code_point = 0 copy untill end of line
    for(int i = 0; i < end_code_point - start_code_point; i++) SDL_StepUTF8(&ptr_string_mov, &adj_length);
    copy_size -= adj_length;
  }

  // Create new string and copy extract in to it
  char* new_string = (char*)malloc(copy_size + 1); // +1 because of Null termination
  memcpy(new_string, ptr_string_start, copy_size);
  new_string[copy_size] = 0x00; // add Null termination

  // free the full string
  free(full_string);

  // Give size and string out
  if(ptr_size) *ptr_size = copy_size + 1;
  *ptr_ptr_string = new_string;

  // return sucsesfull
  return true;
}






// Function for cutting the line off after a given codepoint
// ptr_line is the line to be cut
// cut_codepoint is the codepoint at witch to cut (cut at Incision point before that codepoint)
bool QTS_CutLineOffAfter(SDL_Renderer *ptr_renderer, struct QTS_Line* ptr_line, int cut_codepoint, struct QTS_Text** ptr_ptr_text_remove){
  return QTS_RemoveSectionFromLine(ptr_renderer, ptr_line, cut_codepoint,  0, ptr_ptr_text_remove);
}






// function for getting the y offset for allingment in rendering
float QTS_GetAllingmentOffset(struct QTS_Line* ptr_line, struct QTS_Text* ptr_Text){
  switch(ptr_Text->alignment){

    case BASE:
      return (float)(ptr_line->MaxTextAscent - ptr_Text->ascent);
      break;

    case TOP:
      return 0.0f; // no offset needed
      break;

    case BOTTOM:
      return (float)(ptr_line->MaxTextHight - ptr_Text->hight);
      break;

    case MIDDLE:
      return (float)((ptr_line->MaxTextHight - ptr_Text->hight) / 2);
      break;
  }

  return 0.0f;
}



// function for Rendering Line to current render targed
// ptr_renderer: subject renderer
// ptr_line: pointer to line to be renderd
// ignore_empty_textelements: if true texts with no texture will be skipped in the rendering process if false texts with no texture will cause return fail.
// cross_kernaling: determines if kerneling is applyed between two text objects following each other in the line
// Returns true on sucsess and false on fail
bool QTS_RenderLine(SDL_Renderer *ptr_renderer, struct QTS_Line* ptr_line, float x, float y, bool ignore_empty_textelements, bool cross_kernaling){

  // Validate Input
  if(!ptr_renderer || !ptr_line) return false;

  // Holding variables
  struct QTS_Text* ptr_Text = ptr_line->ptr_TextElements;
  float x_adjusted = x;
  float y_adjusted = y;

  // Render All Text Elements
  if(ignore_empty_textelements){

    while(ptr_Text){

      // decide if skip
      if(ptr_Text->ptr_texture){

        // Adjust Y cordinate given allingment
        y_adjusted = y + QTS_GetAllingmentOffset(ptr_line, ptr_Text);

        // Handle kerneling with text infront
        if(cross_kernaling && ptr_Text->pre_kernel) x_adjusted += (float)(ptr_Text->pre_kernel_offset);

        // Render Text object
        QTS_RenderText(ptr_renderer, ptr_Text, x_adjusted, y_adjusted);
        //QTS_DrawBoundingBoxes(ptr_renderer, ptr_Text, x_adjusted, y_adjusted);

        // Adjust x cordinate for next text object
        x_adjusted += (float)(ptr_Text->ptr_texture->w);

      }

      // Get next Text element
      ptr_Text = ptr_Text->ptr_NextText;

    }

  }else{

    while(ptr_Text){

      // Validate Element
      if(!(ptr_Text->ptr_texture)) return false;

      // Adjust Y cordinate given allingment
      y_adjusted = y + QTS_GetAllingmentOffset(ptr_line, ptr_Text);

      // Handle kerneling with text infront
      if(cross_kernaling && ptr_Text->pre_kernel) x_adjusted += (float)(ptr_Text->pre_kernel_offset);

      // Render Text object
      QTS_RenderText(ptr_renderer, ptr_Text, x_adjusted, y_adjusted);

      // Adjust x cordinate for next text object
      x_adjusted += (float)(ptr_Text->ptr_texture->w);

      // Get next Text element
      ptr_Text = ptr_Text->ptr_NextText;

    }

  }

  return true;
}




// function for consolidating Textobjects (of same format) in a line together
bool QTS_ConsolidateLine(SDL_Renderer *ptr_renderer, struct QTS_Line* ptr_line, bool force_merge){

  // Validate Input
  if(!ptr_renderer || !ptr_line) return false;
  if(!(ptr_line->ptr_TextElements)) return true; // if line is empty it counts as consolidated

  // merge sucsess tracker
  bool merged = true;

  // Get first text object
  struct QTS_Text* ptr_text = ptr_line->ptr_TextElements;

  while(ptr_text){
    while(merged){
      merged = QTS_MergeText(ptr_renderer, ptr_text, force_merge);
    }
    merged = true;
    ptr_text = ptr_text->ptr_NextText;
  }

  return QTS_UpdateLine(ptr_line);
}







// Function for getting the Incision point from a x possition relative to start of line
// ptr_line: the line in witch to serch for a Incision point
// x_pos: the x possition to witch to find the closest inssision point
// ptr_ptr_Incision_text: address of a pointer to be filled with the address of the text element containing the found insission point
// ptr_back_codepoints: (may be -1 to represent very beginning of text object) the summ of the codepoints of the prior text elements up to the insission containing text element
// ptr_insison_codepoint: the codepoint in the text element at witch the Incision point is (not including ptr_back_codepoints)
// ptr_Incision_index: the string index at witch the Incision point is (if an Incision is made the char would be written to this index)
bool QTS_GetLineIncisionPoint_FromPos(struct QTS_Line* ptr_line, int x_pos, struct QTS_Text** ptr_ptr_Incision_text, int* ptr_back_codepoints, int* ptr_insison_codepoint, size_t* ptr_Incision_index, bool adjust_for_prekerneling){

  // Validate Input
  if(!ptr_line || x_pos < 0) return false;

  // Case: empty line
  if(!(ptr_line->ptr_TextElements)){
    if(ptr_ptr_Incision_text) *ptr_ptr_Incision_text = NULL;
    if(ptr_back_codepoints) *ptr_back_codepoints = 0;
    if(ptr_insison_codepoint) *ptr_insison_codepoint = 0;
    if(ptr_Incision_index) *ptr_Incision_index = 0;
    return true;
  }

  // Special case Only one empty text element present
  if(!(ptr_line->ptr_TextElements->string) || !(ptr_line->ptr_TextElements->spacing)){
    // Then give the empty text element as incission point
    if(ptr_ptr_Incision_text) *ptr_ptr_Incision_text = ptr_line->ptr_TextElements;
    if(ptr_back_codepoints) *ptr_back_codepoints = 0;
    if(ptr_insison_codepoint) *ptr_insison_codepoint = 0;
    if(ptr_Incision_index) *ptr_Incision_index = 0;
    return true;
  }



  // First find the Text Element that contains the Incision location
  struct QTS_Text* ptr_text = ptr_line->ptr_TextElements;
  int ref_holding = 0;
  int back_codepoints = 0;


  // move values to end of text element
  ref_holding += ptr_text->spacing[ptr_text->codepoints - 1];
  back_codepoints += ptr_text->codepoints;


  // Adjust for prekernaling
  if(adjust_for_prekerneling && ptr_text->pre_kernel){
    ref_holding += ptr_text->pre_kernel_offset;
  }

  // Find textelement containing Incision point
  while(ref_holding < x_pos && ptr_text->ptr_NextText){

    // Get next text element
    ptr_text = ptr_text->ptr_NextText;

    // move values to end of text element
    ref_holding += ptr_text->spacing[ptr_text->codepoints - 1];
    back_codepoints += ptr_text->codepoints;

    // Adjust for prekernaling
    if(adjust_for_prekerneling && ptr_text->pre_kernel){
      ref_holding += ptr_text->pre_kernel_offset;
    }
  }
  // backwind values to start of Text Element
  ref_holding -= ptr_text->spacing[ptr_text->codepoints - 1];
  back_codepoints -= ptr_text->codepoints;



  // Find the codepoint in that text element that represents the area that x_pos is in
  int insison_codepoint = 0;

  while(ref_holding + ptr_text->spacing[insison_codepoint] < x_pos && insison_codepoint < (int)(ptr_text->codepoints)){
    insison_codepoint ++;
  }

  // Decide if the Incision point before of after the current Incision codepoint area is closer to x_pos
  int distance_to_back = abs((ref_holding + ptr_text->spacing[insison_codepoint]) - x_pos);
  int distance_to_front;
  if(insison_codepoint == 0) distance_to_front = abs(x_pos - ref_holding); else distance_to_front = abs(x_pos - ref_holding - ptr_text->spacing[insison_codepoint - 1]);

  if(distance_to_front < distance_to_back){
    // front codepoint
    insison_codepoint -= 1; // may be -1 representing start of text
  }else{
    // back (current) codepoint
  }



  // Find the Incision index
  size_t Incision_index;
  size_t string_length = ptr_text->length;
  const char* ptr_string = ptr_text->string;

  if(insison_codepoint == -1){
    // if Incision is a very beginning of tex element
    Incision_index = 0;

  }else if(insison_codepoint >= (int)(ptr_text->codepoints - 1)){
    // if Incision is at very end of the text element
    Incision_index = ptr_text->length - 1; // -1 because indexing starts at 0 => ends up on null termination char

  }else{
    // UTF8 stepp to index
    for(int i = 0; i <= insison_codepoint; i++){
      SDL_StepUTF8(&ptr_string, &string_length);
    }
    // calculate index
    Incision_index = ptr_text->length - string_length;
  }

  // Return values
  if(ptr_ptr_Incision_text) *ptr_ptr_Incision_text = ptr_text;
  if(ptr_back_codepoints) *ptr_back_codepoints = back_codepoints;
  if(ptr_insison_codepoint) *ptr_insison_codepoint = insison_codepoint;
  if(ptr_Incision_index) *ptr_Incision_index = Incision_index;

  return true;
}


// Function for getting the Insission point location from a codepoint
// ptr_line: the line on witch to move the insission point
// codepoint: the codepoint location (may be -1 to represent very beginning of the line)
// ptr_ptr_Incision_text: the text object inwitch the insission is
// x_pos: pointer to variable containing x possition of the insission location
// ptr_insison_codepoint: pointer to inssion codepoint variable (the index at witch the Incision point is in the text object)
// ptr_Incision_index: the index of the string where an insission would start writing in the text object
bool QTS_GetLineIncisionPoint_FromCodepoint(struct QTS_Line* ptr_line, int pre_selected_codepoint, struct QTS_Text** ptr_ptr_Incision_text, int* x_pos, int* ptr_back_codepoints, int* ptr_insison_codepoint, size_t* ptr_Incision_index, bool adjust_for_prekerneling){

  // Validate Input
  if(!ptr_line) return false;
  int selected_codepoint = pre_selected_codepoint;
  if(selected_codepoint < -1) selected_codepoint = -1;

  // Case Empty Line
  if(!(ptr_line->ptr_TextElements)){
    if(ptr_ptr_Incision_text) *ptr_ptr_Incision_text = NULL;
    if(x_pos) *x_pos = 0;
    if(ptr_insison_codepoint) *ptr_insison_codepoint = 0;
    if(ptr_Incision_index) *ptr_Incision_index = 0;
    if(ptr_back_codepoints) *ptr_back_codepoints = 0;
    return true;
  }

  // Case -1
  if(selected_codepoint == -1){
    if(ptr_ptr_Incision_text) *ptr_ptr_Incision_text = ptr_line->ptr_TextElements;
    if(x_pos) *x_pos = 0;
    if(ptr_insison_codepoint) *ptr_insison_codepoint = -1;
    if(ptr_Incision_index) *ptr_Incision_index = 0;
    if(ptr_back_codepoints) *ptr_back_codepoints = 0;
    return true;
  }

  // Special case Only one empty text element present
  if(!(ptr_line->ptr_TextElements->string) || !(ptr_line->ptr_TextElements->spacing)){
    // Then give the empty text element as incission point
    if(ptr_ptr_Incision_text) *ptr_ptr_Incision_text = ptr_line->ptr_TextElements;
    if(x_pos) *x_pos = 0;
    if(ptr_back_codepoints) *ptr_back_codepoints = 0;
    if(ptr_insison_codepoint) *ptr_insison_codepoint = 0;
    if(ptr_Incision_index) *ptr_Incision_index = 0;
    if(ptr_back_codepoints) *ptr_back_codepoints = 0;
    return true;
  }


  // Case more codepoints then there are in line
  int codepoint = selected_codepoint;
  int codepoints_in_line = QTS_GetLineSize_CP(ptr_line);
  if(codepoint > codepoints_in_line){
    codepoint = codepoints_in_line;
  }



  // First find the Text Element that contains the Incision location
  struct QTS_Text* ptr_text = ptr_line->ptr_TextElements;
  int ref_holding = 0;
  int back_codepoints = 0;


  // move values to end of text element
  ref_holding += ptr_text->spacing[ptr_text->codepoints - 1];
  back_codepoints += ptr_text->codepoints;

  // Adjust for prekernaling
  if(adjust_for_prekerneling && ptr_text->pre_kernel){
    ref_holding += ptr_text->pre_kernel_offset;
  }

  // Find textelement containing Incision point
  while(back_codepoints < codepoint && ptr_text->ptr_NextText){

    // Get next text element
    ptr_text = ptr_text->ptr_NextText;

    // move values to end of text element
    ref_holding += ptr_text->spacing[ptr_text->codepoints - 1];
    back_codepoints += ptr_text->codepoints;

    // Adjust for prekernaling
    if(adjust_for_prekerneling && ptr_text->pre_kernel){
      ref_holding += ptr_text->pre_kernel_offset;
    }
  }
  // backwind values to start of Text Element
  ref_holding -= ptr_text->spacing[ptr_text->codepoints - 1];
  back_codepoints -= ptr_text->codepoints;


  // codepoint - back_codepoints = insison_codepoint
  int insison_codepoint = codepoint - back_codepoints - 1;


  // Find the Incision index
  size_t Incision_index;
  size_t string_length = ptr_text->length;
  const char* ptr_string = ptr_text->string;

  if(insison_codepoint == -1){
    // if Incision is a very beginning of tex element
    Incision_index = 0;

  }else if(insison_codepoint >= (int)(ptr_text->codepoints - 1)){
    // if Incision is at very end of the text element
    Incision_index = ptr_text->length - 1; // -1 because indexing starts at 0 => ends up on null termination char

  }else{
    // UTF8 stepp to index
    for(int i = 0; i <= insison_codepoint; i++){
      SDL_StepUTF8(&ptr_string, &string_length);
    }
    // calculate index
    Incision_index = ptr_text->length - string_length;
  }

  // claculate x position (in to ref_holding)
  if(insison_codepoint != -1) ref_holding += ptr_text->spacing[insison_codepoint];


  // Return vaues
  if(ptr_ptr_Incision_text) *ptr_ptr_Incision_text = ptr_text;
  if(x_pos) *x_pos = ref_holding;
  if(ptr_back_codepoints) *ptr_back_codepoints = back_codepoints;
  if(ptr_insison_codepoint) *ptr_insison_codepoint = insison_codepoint;
  if(ptr_Incision_index) *ptr_Incision_index = Incision_index;

  // Return sucsesfully
  return true;
}





// Function for Getting the Incision point of a lline from a x cordinate relative to the start of the line
bool QTS_GetLineIncisionPoint(struct QTS_Line* ptr_line, int x_pos, int* ins_x_pos, struct QTS_Text** ptr_ptr_Incision_text, int* ptr_back_codepoints, int* ptr_insison_codepoint, size_t* ptr_Incision_index, bool adjust_for_prekerneling){

  // Validate Input
  if(!ptr_line || x_pos < 0 ) return false;

  // Case: empty line
  if(!(ptr_line->ptr_TextElements)){
    if(ptr_ptr_Incision_text) *ptr_ptr_Incision_text = NULL;
    if(ptr_back_codepoints) *ptr_back_codepoints = 0;
    if(ptr_insison_codepoint) *ptr_insison_codepoint = 0;
    if(ptr_Incision_index) *ptr_Incision_index = 0;
    return true;
  }

  // Special case Only one empty text element present
  if(!(ptr_line->ptr_TextElements->string) || !(ptr_line->ptr_TextElements->spacing)){
    // Then give the empty text element as incission point
    if(ins_x_pos) *ins_x_pos = 0;
    if(ptr_ptr_Incision_text) *ptr_ptr_Incision_text = ptr_line->ptr_TextElements;
    if(ptr_back_codepoints) *ptr_back_codepoints = 0;
    if(ptr_insison_codepoint) *ptr_insison_codepoint = 0;
    if(ptr_Incision_index) *ptr_Incision_index = 0;
    return true;
  }



  // First find the Text Element that contains the Incision location
  struct QTS_Text* ptr_text = ptr_line->ptr_TextElements;
  int ref_holding = 0;
  int back_codepoints = 0;


  // move values to end of text element
  ref_holding += ptr_text->spacing[ptr_text->codepoints - 1];
  back_codepoints += ptr_text->codepoints;


  // Adjust for prekernaling
  if(adjust_for_prekerneling && ptr_text->pre_kernel){
    ref_holding += ptr_text->pre_kernel_offset;
  }

  // Find textelement containing Incision point
  while(ref_holding < x_pos && ptr_text->ptr_NextText){

    // Get next text element
    ptr_text = ptr_text->ptr_NextText;

    // move values to end of text element
    ref_holding += ptr_text->spacing[ptr_text->codepoints - 1];
    back_codepoints += ptr_text->codepoints;

    // Adjust for prekernaling
    if(adjust_for_prekerneling && ptr_text->pre_kernel){
      ref_holding += ptr_text->pre_kernel_offset;
    }
  }
  // backwind values to start of Text Element
  ref_holding -= ptr_text->spacing[ptr_text->codepoints - 1];
  back_codepoints -= ptr_text->codepoints;



  // Find the codepoint in that text element that represents the area that x_pos is in
  int insison_codepoint = 0;

  while(ref_holding + ptr_text->spacing[insison_codepoint] < x_pos && insison_codepoint < (int)(ptr_text->codepoints)){
    insison_codepoint ++;
  }

  // Decide if the Incision point before of after the current Incision codepoint area is closer to x_pos
  int distance_to_back = abs((ref_holding + ptr_text->spacing[insison_codepoint]) - x_pos);
  int distance_to_front;
  if(insison_codepoint == 0) distance_to_front = abs(x_pos - ref_holding); else distance_to_front = abs(x_pos - ref_holding - ptr_text->spacing[insison_codepoint - 1]);

  if(distance_to_front < distance_to_back){
    // front codepoint
    insison_codepoint -= 1; // may be -1 representing start of text
  }else{
    // back (current) codepoint
  }


  // Adjust ref_holding
  if(insison_codepoint >= 0){
    ref_holding = ref_holding + ptr_text->spacing[insison_codepoint];
  }



  // Find the Incision index
  size_t Incision_index;
  size_t string_length = ptr_text->length;
  const char* ptr_string = ptr_text->string;

  if(insison_codepoint == -1){
    // if Incision is a very beginning of tex element
    Incision_index = 0;

  }else if(insison_codepoint >= (int)(ptr_text->codepoints - 1)){
    // if Incision is at very end of the text element
    Incision_index = ptr_text->length - 1; // -1 because indexing starts at 0 => ends up on null termination char

  }else{
    // UTF8 stepp to index
    for(int i = 0; i <= insison_codepoint; i++){
      SDL_StepUTF8(&ptr_string, &string_length);
    }
    // calculate index
    Incision_index = ptr_text->length - string_length;
  }

  // Return values
  if(ptr_ptr_Incision_text) *ptr_ptr_Incision_text = ptr_text;
  if(ptr_back_codepoints) *ptr_back_codepoints = back_codepoints;
  if(ptr_insison_codepoint) *ptr_insison_codepoint = insison_codepoint;
  if(ptr_Incision_index) *ptr_Incision_index = Incision_index;
  if(ins_x_pos) *ins_x_pos = ref_holding;

  return true;
}





// Function for moving the insission Location by codepoints
bool QTS_MoveIncisionPoint(struct QTS_Line* ptr_line, int mov_codepoints, struct QTS_Text** ptr_ptr_Incision_text, int* x_pos, int* ptr_back_codepoints, int* ptr_insison_codepoint, size_t* ptr_Incision_index, bool adjust_for_prekerneling){

  // Validate Input
  if(!ptr_line || !ptr_ptr_Incision_text || !ptr_back_codepoints || !ptr_insison_codepoint) return false;
  if(mov_codepoints == 0) return true;

  // Return values
  bool b_ret;
  int codepoint; // = *ptr_back_codepoints + *ptr_insison_codepoint + mov_codepoints;

  // calculate true incission point
  if(*ptr_insison_codepoint == -1){
    codepoint = *ptr_back_codepoints + mov_codepoints;
  }else{
    codepoint = *ptr_back_codepoints + mov_codepoints + *ptr_insison_codepoint + 1;
  }

  b_ret = QTS_GetLineIncisionPoint_FromCodepoint(ptr_line, codepoint, ptr_ptr_Incision_text, x_pos, ptr_back_codepoints, ptr_insison_codepoint, ptr_Incision_index, adjust_for_prekerneling);
  if(!b_ret) return false;

  return b_ret;
}






// Function for Destroying a QTS_Line
void QTS_DestroyLine(struct QTS_Line* ptr_line){

  if(!ptr_line) return;

  // Destroy all Text objects in the line
  struct QTS_Text* ptr_text_holding = ptr_line->ptr_TextElements;
  struct QTS_Text* ptr_text_free = ptr_text_holding;

  while(ptr_text_holding){
    ptr_text_holding = ptr_text_holding->ptr_NextText;
    QTS_DestroyTextOBJ(ptr_text_free);
    ptr_text_free = ptr_text_holding;
  }

  // Free Line itself
  free(ptr_line);
}


void QTS_DestroyLineChain(struct QTS_Line* ptr_line){

  if(!ptr_line) return;

  // Holding var
  struct QTS_Line* ptr_line_holding = ptr_line;
  struct QTS_Line* ptr_line_free;

  while(ptr_line_holding){
    ptr_line_free = ptr_line_holding;
    ptr_line_holding = ptr_line_holding->ptr_NextLine;
    QTS_DestroyLine(ptr_line_free);
  }

  return;
}




// Function for force formating a section of text in a Line
// The line will be spitt at codepoints and consecatively merged with format
// if ptr_format is NULL the starting codepoints format will be used
bool QTS_FormatLineSection(SDL_Renderer *ptr_renderer, struct QTS_Line* ptr_line, int start_codepoint_in, int end_codepoint_in, struct QTS_Text* ptr_format_input){

  // Validat Input
  if(!ptr_renderer || !ptr_line || start_codepoint_in == end_codepoint_in) return false;

  // Return Holding variable
  bool b_ret = false;

  // input Holding variables
  struct QTS_Text* ptr_merge_start = NULL;
  int start_codepoint = start_codepoint_in;
  int end_codepoint = end_codepoint_in;
  Uint32 codepoint_count;

  // Enshore corect orienataion
  if(start_codepoint > end_codepoint){
    start_codepoint = end_codepoint_in;
    end_codepoint = start_codepoint_in;
  }

  codepoint_count = end_codepoint - start_codepoint;

  // split line at start codepoint
  ptr_merge_start = QTS_SplitLineAt(ptr_renderer, ptr_line, start_codepoint);

  // split line at end codepoint
  QTS_SplitLineAt(ptr_renderer, ptr_line, end_codepoint);

  // if there is a given format insert it before the start and make it the start
  if(ptr_format_input){
    // insert in front of current start
    b_ret = QTS_AddTextToLine_After(ptr_line, ptr_merge_start->ptr_PrevText, ptr_format_input);
    if(!b_ret) return false;
    ptr_merge_start = ptr_format_input;
  }

  // Merge untill codepoint range has been reached or no more elements exsist in line
  while(ptr_merge_start->ptr_NextText && ptr_merge_start->codepoints < codepoint_count){
    b_ret = QTS_MergeText(ptr_renderer, ptr_merge_start, true);
    if(!b_ret) return false;
  }

  return true;
}




// Function for Turning Text in to Lines
// ptr_FormatReference is a reference format that will be cloned and used for all the new text elements
// ptr_ptr_last_line is a pointer that will be filled with the address of the last line in the created line chain
// ptr_ptr_last_text is a pointer that will be filled with th last text element in the last line
struct QTS_Line* QTS_ConvertTextToLines(SDL_Renderer *ptr_renderer, struct QTS_Text* ptr_FormatReference, char* text, struct QTS_Line** ptr_ptr_last_line, struct QTS_Text** ptr_ptr_last_text){

  // Validate Input
  if(!ptr_renderer || !ptr_FormatReference || !text) return NULL;

  // return Holding variable
  bool b_ret;

  // Get max Line length
  int max_line_length = QTS_GetMaxLineLength(text);
  if(max_line_length <= 0) return NULL;
  max_line_length++; // Add one for Null termination

  // Allocate a buffer with max line size
  char* buffer = (char*)calloc(max_line_length, sizeof(char));

  // offset variable for moving through string
  int offset = 0;
  int length_holding = 0;

  // lines
  struct QTS_Line* ptr_start_line = NULL;
  struct QTS_Line* ptr_line_holding = NULL;
  struct QTS_Line* ptr_line_new = NULL;
  struct QTS_Text* ptr_text_holding = NULL;

  // itterate through lines
  while(text[offset]){

    // Get the length of the line
    length_holding = QTS_GetStringLineLength(&(text[offset]));

    // two cases
    if(length_holding <= 0){

      // Line length = 0
      // Create Line with text from buffer
      ptr_line_new = QTS_CreateLine();
      ptr_text_holding = QTS_CloneTextFormat(ptr_renderer, ptr_FormatReference);

      b_ret = QTS_AddTextToLine_After(ptr_line_new, NULL, ptr_text_holding);
      if(!b_ret){
        // error case: free everything and return NULL
        free(buffer);
        QTS_DestroyTextOBJ(ptr_text_holding);
        QTS_DestroyLine(ptr_line_new);
        QTS_DestroyLineChain(ptr_start_line);
        return NULL;
      }

    }else{
      // line length is > 0

      // Copy the Line in to the buffer
      memcpy(buffer, &(text[offset]), length_holding);

      // Add Null termination to Buffer
      buffer[length_holding] = 0x00;

      // Create Line with text from buffer
      ptr_line_new = QTS_CreateLine();
      ptr_text_holding = QTS_CloneTextFormat(ptr_renderer, ptr_FormatReference);

      b_ret = QTS_UpdateTextString(ptr_text_holding, buffer, 0, true);
      if(!b_ret){
        // error case: free everything and return NULL
        free(buffer);
        QTS_DestroyTextOBJ(ptr_text_holding);
        QTS_DestroyLine(ptr_line_new);
        QTS_DestroyLineChain(ptr_start_line);
        return NULL;
      }
      b_ret = QTS_UpdateText(ptr_renderer, ptr_text_holding);
      if(!b_ret){
        // error case: free everything and return NULL
        free(buffer);
        QTS_DestroyTextOBJ(ptr_text_holding);
        QTS_DestroyLine(ptr_line_new);
        QTS_DestroyLineChain(ptr_start_line);
        return NULL;
      }
      b_ret = QTS_AddTextToLine_After(ptr_line_new, NULL, ptr_text_holding);
      if(!b_ret){
        // error case: free everything and return NULL
        free(buffer);
        QTS_DestroyTextOBJ(ptr_text_holding);
        QTS_DestroyLine(ptr_line_new);
        QTS_DestroyLineChain(ptr_start_line);
        return NULL;
      }
    }// end of case line length > 0


    // Add to line chain
    if(!ptr_start_line){
      // is first Line generated

      // set as start line and holding line
      ptr_start_line = ptr_line_new;
      ptr_line_holding = ptr_line_new;

    }else{
      // add ptr_line_new behind line holding and then set line holding to next
      ptr_line_holding->ptr_NextLine = ptr_line_new;
      ptr_line_new->ptr_PrevLine = ptr_line_holding;

      ptr_line_holding = ptr_line_new;

    }

    // Move Offset to next Line
    offset += length_holding;
    if(text[offset]) offset++; // to skip new line symbol
  }

  // free the Allocated Buffer
  free(buffer);

  // Return the Values
  if(ptr_ptr_last_line) *ptr_ptr_last_line = ptr_line_holding;
  if(ptr_ptr_last_text) *ptr_ptr_last_text = ptr_text_holding;

  return ptr_start_line;
}



// END
