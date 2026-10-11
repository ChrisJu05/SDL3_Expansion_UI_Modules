// Qualety Text Core by Christopher Jung
// for Creating Dynamic text elements with SDL3 and SDL3 TTF

#include "QualityTextSystem.h"


// Function for Updating the parameters of a box
bool QTS_UpdateBoxParameters(struct QTS_Box* ptr_box){

  // Validate Input
  if(!ptr_box) return false;

  // If box is empty
  if(!ptr_box->ptr_LineElements){
    ptr_box->MaxWidth = 0;
    ptr_box->MaxHight = 0;
    ptr_box->LineCount = 0;
    return true;
  }


  // itterate through Linest and update parameters
  int MaxWidth = 0;
  int MaxHight = 0;
  int LineCount = 0;

  struct QTS_Line* ptr_line_holding = ptr_box->ptr_LineElements;
  
  while(ptr_line_holding){

    QTS_UpdateLine(ptr_line_holding);

    if(MaxWidth < ptr_line_holding->length) MaxWidth = ptr_line_holding->length;
    MaxHight += ptr_line_holding->hight;
    LineCount++;

    ptr_line_holding = ptr_line_holding->ptr_NextLine;

  }

  // Give parameters to Box
  ptr_box->MaxWidth = MaxWidth;
  ptr_box->MaxHight = MaxHight;
  ptr_box->LineCount = LineCount;

  return true;
}




struct QTS_Box* QTS_CreateBox(struct QTS_Line *ptr_LineElements){

  // Allocate Box
  struct QTS_Box* ptr_box = (struct QTS_Box*)calloc(1, sizeof(struct QTS_Box));
  if(!ptr_box) return NULL;

  // Give Line elements
  ptr_box->ptr_LineElements = ptr_LineElements;

  // Update parameters
  QTS_UpdateBoxParameters(ptr_box);

  // set Defaults
  ptr_box->ignore_empty_textelements = true;
  ptr_box->cross_kernaling = true;

  return ptr_box;
}





// Function for safely destroying a text box (including all child line and text objects)
void QTS_DestroyBox(struct QTS_Box* ptr_box){

  if(!ptr_box) return;

  // Destroy all the Lines in the Box
  struct QTS_Line* ptr_line_holding = ptr_box->ptr_LineElements;
  struct QTS_Line* ptr_line_free;

  while(ptr_line_holding){
    ptr_line_free = ptr_line_holding;
    ptr_line_holding = ptr_line_holding->ptr_NextLine;
    QTS_DestroyLine(ptr_line_free);
  }

  // Free the box itself
  free(ptr_box);
}



// Function for Rendering the Text box to the current render targed
bool QTS_RenderBox(SDL_Renderer *ptr_renderer, struct QTS_Box* ptr_Box, float x, float y){

  // Validate Input
  if(!ptr_renderer || !ptr_Box) return false;

  // Holding variables for rendering the Line
  struct QTS_Line* ptr_Line_holding = ptr_Box->ptr_LineElements;
  float y_adjusted = y;

  while(ptr_Line_holding){

    // Render The Line
    QTS_RenderLine(ptr_renderer, ptr_Line_holding, x, y_adjusted, ptr_Box->ignore_empty_textelements, ptr_Box->cross_kernaling);

    // Increment y possition
    y_adjusted += (float)(ptr_Line_holding->hight);

    // Get Next Line
    ptr_Line_holding = ptr_Line_holding->ptr_NextLine;

  }

  return true;
}





// Function for adding a Line after a nother line (Note: only adds that line ignores its followers)
bool QTS_AddLineToBox(struct QTS_Box* ptr_Box, struct QTS_Line* ptr_line_root, struct QTS_Line* ptr_line_add){

  // Validate Input
  if(!ptr_Box || !ptr_line_add) return false;

  // if root line is NULL add as first Line
  if(!ptr_line_root){

    // configure Line
    ptr_line_add->ptr_NextLine = ptr_Box->ptr_LineElements;
    ptr_line_add->ptr_PrevLine = NULL;

    // Add to text Box
    ptr_Box->ptr_LineElements = ptr_line_add;

  }else{

    // configure Line
    ptr_line_add->ptr_NextLine = ptr_line_root->ptr_NextLine;
    ptr_line_add->ptr_PrevLine = ptr_line_root;

    // add in to list
    if(ptr_line_root->ptr_NextLine) ptr_line_root->ptr_NextLine->ptr_PrevLine = ptr_line_add;
    ptr_line_root->ptr_NextLine = ptr_line_add;

  }

  return QTS_UpdateBoxParameters(ptr_Box);
}







// Function for Adding a chain of lines to a text box
bool QTS_AddLineCahinToBox(struct QTS_Box* ptr_Box, struct QTS_Line* ptr_line_root, struct QTS_Line* ptr_line_add){

  // Validate Input
  if(!ptr_Box || !ptr_line_add) return false;

  // Get end of line chain
  struct QTS_Line* ptr_LineCahin_end = ptr_line_add;
  while(ptr_LineCahin_end->ptr_NextLine) ptr_LineCahin_end = ptr_LineCahin_end->ptr_NextLine;

  // if root line is NULL add as first Line
  if(!ptr_line_root){

    ptr_LineCahin_end->ptr_NextLine = ptr_Box->ptr_LineElements;
    if(ptr_LineCahin_end->ptr_NextLine) ptr_LineCahin_end->ptr_NextLine->ptr_PrevLine = ptr_LineCahin_end;
    ptr_Box->ptr_LineElements = ptr_line_add;
    ptr_line_add->ptr_PrevLine = NULL;

  }else{

    ptr_LineCahin_end->ptr_NextLine = ptr_line_root->ptr_NextLine;
    ptr_line_add->ptr_PrevLine = ptr_line_root;

    ptr_line_root->ptr_NextLine = ptr_line_add;
    if(ptr_LineCahin_end->ptr_NextLine) ptr_LineCahin_end->ptr_NextLine->ptr_PrevLine = ptr_LineCahin_end;

  }

  return QTS_UpdateBoxParameters(ptr_Box);
}




// Function for removing a line from a box
bool QTS_RemoveLineFromBox(struct QTS_Box* ptr_Box, struct QTS_Line* ptr_line_remove){

  // Validate Input
  if(!ptr_line_remove || !ptr_Box) return false;

  // if line is first in box
  if(ptr_Box->ptr_LineElements == ptr_line_remove){

    ptr_Box->ptr_LineElements = ptr_line_remove->ptr_NextLine;
    ptr_Box->ptr_LineElements->ptr_PrevLine = NULL;

    ptr_line_remove->ptr_NextLine = NULL;
    ptr_line_remove->ptr_PrevLine = NULL;

  }else{

    if(!ptr_line_remove->ptr_PrevLine) return false;

    ptr_line_remove->ptr_PrevLine->ptr_NextLine = ptr_line_remove->ptr_NextLine;
    ptr_line_remove->ptr_NextLine->ptr_PrevLine = ptr_line_remove->ptr_PrevLine;

    ptr_line_remove->ptr_NextLine = NULL;
    ptr_line_remove->ptr_PrevLine = NULL;

  }

  return QTS_UpdateBoxParameters(ptr_Box);
}





// Function for Splitting a Line List
// ptr_chain_split the start of the split off part
bool QTS_SpiltLineChain(struct QTS_Line* ptr_chain_split){

  // Validate Input
  if(!ptr_chain_split) return false;

  // Decupple
  if(ptr_chain_split->ptr_PrevLine) ptr_chain_split->ptr_PrevLine->ptr_NextLine = NULL;
  ptr_chain_split->ptr_PrevLine = NULL;

  return true;
}




// Function for Getting the Text Inside a Textbox
// ptr_size is a pointer to a size_t where the size of the string will be written (including the NULL termination). is allowd to be NULL.
char* QTS_GetBoxText(struct QTS_Box* ptr_Box, size_t* ptr_size){

  // Validate Input
  if(!ptr_Box) return NULL;
  if(!ptr_Box->ptr_LineElements) return NULL;

  // Calculate the size of the Box string
  struct QTS_Line* ptr_line_holding = ptr_Box->ptr_LineElements;
  size_t str_size = 0;
  size_t holding = 0;

  while(ptr_line_holding){

    // Get Line string size
    holding = QTS_GetLineSize_ST(ptr_line_holding);
    //if(holding == 0) holding = 1; // because if a line is empty then a new line symbol should be added.
    holding += 1;

    str_size += holding;

    // Get Next Line
    ptr_line_holding = ptr_line_holding->ptr_NextLine;
  }

  // Allocate a string of size
  char* box_str = (char*)calloc(str_size, sizeof(char));

  // Copy Line strings in to the box string
  ptr_line_holding = ptr_Box->ptr_LineElements;
  char* holding_str = NULL;
  size_t copy_count = 0;

  while(ptr_line_holding){

    // Get the string of the line
    QTS_GetLineString(ptr_line_holding, &holding_str, NULL, &holding);
    //if(holding == 0) holding = 1;
    holding += 1;

    // only do this if a string was read
    if(holding_str){

      // Copy the string in to the final string and increment copy count by size
      memcpy(&(box_str[copy_count]), holding_str, holding);

      // free the string generated by QTS_GetLineString function
      free(holding_str);
      holding_str = NULL;
    }

    // increment copy count
    copy_count += holding;

    // Add new line symbol
    box_str[copy_count - 1] = '\n';

    // Get Next Line
    ptr_line_holding = ptr_line_holding->ptr_NextLine;
  }

  // Add NULL Termination
  box_str[((int)copy_count) - 1] = 0x00;

  // Return Values
  if(ptr_size) *ptr_size = str_size;
  return box_str;
}





// END
