// This HEader Contains basic universal Utiletys for the Quality Text system

// Include Quality Text system Header
#include "QualityTextSystem.h"



// Function for Merging two strings (both Null terminated) in to one (both strings Null terminated) the returned string is a new allocation also NULL Terminated
// if one of the srings is empty the other will simply be coppyed; if both are empty Null is returned
// ptr_size_new is a pointer to the variable where the new size of the new string will be written to
char* QTS_MergeStrings(char* string_A, size_t size_A, char* string_B, size_t size_B, size_t* ptr_size_new){

  // Validate input
  if(!string_A && !string_B) return NULL;
  //if(size_A < 0 || size_B < 0) return NULL;

  char* new_string = NULL;
  size_t new_size;

  // Three cases
  if(!string_A){

    // String A is NULL => copy string B
    new_string = (char*)malloc(size_B);
    memcpy(new_string, string_B, size_B);
    new_size = size_B;

  }else if(!string_B){

    // string B is Null => copy string A
    new_string = (char*)malloc(size_A);
    memcpy(new_string, string_A, size_A);
    new_size = size_A;

  }else{

    // Expand Strings
    new_string = (char*)malloc(size_A + size_B - 1); // -1 because one of the Nullterminations needs to be removed
    memcpy(new_string, string_A, size_A);
    memcpy(&(new_string[size_A - 1]), string_B, size_B); // -1 to override the the Nulltermination of the frist string that was copyed in
    new_size = size_A + size_B - 1;

    // write Null termination at end
    new_string[size_A + size_B - 2] = 0x00;
  }

  if(ptr_size_new) *ptr_size_new = new_size;
  return new_string;
}



// Function for Counting the Codepoints in a Null Terminated String (not including NULL)
int QTS_CountCodepoints(char* string){

  // validate Input
  if(!string) return 0;

  // create Holding variables
  const char* stp_string = string;
  int count = 0;
  while(string[count]){
    SDL_StepUTF8(&stp_string, NULL);
    count++;
  }
  return count;
}





// Function for Removing new Line symbols from a string (Null terminated)
bool QTS_RemoveNewLineSymbols(char* string){

  // Validate Input
  if(!string) return false;

  // itterate through string
  int i = 0;
  while(string[i]){
    if(string[i] == '\n'){
      string[i] = ' ';
    }
    i++;
  }

  return true;
}



// Function for Cloning an Empty Text element
struct QTS_Text* QTS_CloneTextFormat(SDL_Renderer *ptr_renderer, struct QTS_Text* ptr_source){

  // Validate Input
  if(!ptr_source) return NULL;

  // Create a new text element with parameters from source but no text
  struct QTS_Text* ptr_clone = QTS_CreateText(ptr_renderer, ptr_source->ptr_font, NULL, 0, ptr_source->render_method, ptr_source->ptr_color_f, ptr_source->ptr_color_b, ptr_source->render_func, ptr_source->ptr_format_data);
  if(!ptr_clone) return NULL;

  return ptr_clone;
}



// Function for Printing the composition of a line
void QTS_PrintLineComposition(struct QTS_Line* ptr_Line){

  // Validate Input
  if(!ptr_Line) return;

  // Print basic Line composition
  struct QTS_Text *ptr_Text_holding = ptr_Line->ptr_TextElements;

  printf("Line: %p \n", ptr_Text_holding);

  while(ptr_Text_holding){
    printf("<%s>", ptr_Text_holding->string);
    ptr_Text_holding = ptr_Text_holding->ptr_NextText;
  }

  printf("\n");
  return;
}



int QTS_CountCodepointsInLine(struct QTS_Line* ptr_Line){

  // Validate input
  if(!ptr_Line) return 0;

  // grab first text element
  struct QTS_Text* ptr_text_holding = ptr_Line->ptr_TextElements;
  int codepoint_count = 0;
  while(ptr_text_holding){
    codepoint_count += ptr_text_holding->codepoints;
    ptr_text_holding = ptr_text_holding->ptr_NextText;
  }

  return codepoint_count;
}



// Function for checking if a character is a seperation character
bool QTS_IsSeperationChar(char symb){

  Uint8 isymb = symb;

  // First ASCII Region of seperation characters (32 - 47)
  if(isymb >= 32 && isymb <= 47) return true;

  // Second ASCII Region of seperation characters (58 - 64)
  if(isymb >= 58 && isymb <= 64) return true;

  // Third ASCII Region of seperation characters (91 - 94)
  if(isymb >= 91 && isymb <= 94) return true;

  // Forth ASCII Region of seperation characters (96)
  if(isymb == 96) return true;

  // Fith ASCII Region of seperation characters (123 - 126)
  if(isymb >= 123 && isymb <= 126) return true;

  return false;
}



// Function for getting the codepoint to a string index (return value is the codepoint in array indexing starting with 0)
int QTS_IndexToCodepoint(char* string, size_t index){

  // Validate input
  if(!string) return 0;
  if(!string[0]) return 0;
  if(index == 0) return 0;

  // Get the string Length
  size_t length = 0;
  while(string[length]) length++;

  // set up UTF8 stepping
  const char* con_string = string;
  size_t adj_length = length;
  int codepoint_count = 0;

  while((length - adj_length) < index && adj_length > 0 && codepoint_count < (int)length){
    SDL_StepUTF8(&con_string, &adj_length);
    codepoint_count++;
  }

  return codepoint_count;
}




// Function for counting the Lines a string has
int QTS_CountStringLines(char* text){

  // Validate Input
  if(!text) return 0;
  if(!text[0]) return 0;

  // Count Lines
  int LineCount = 1;

  for(int i = 1; text[i]; i++) if(text[i] == '\n') LineCount++;

  return LineCount;
}



// Function for Getting the length of the first line of a string (length does not include new line symbol or NULL termination)
int QTS_GetStringLineLength(char* text){

  // Validate Input
  if(!text) return 0;
  if(!text[0]) return 0;

  // return holding variable
  int count = 0;

  // itterate throught text untill new line symbol is found
  for(int i = 0; text[i] && text[i] != '\n'; i++) count++;

  return count;
}



// Function for Getting the Maximum Line Length in a string
int QTS_GetMaxLineLength(char* text){

  // Validate Input
  if(!text) return 0;
  if(!text[0]) return 0;

  // Holding Variables
  int max_length = QTS_GetStringLineLength(text);
  int length_holding = 0;
  int offset = 0;

  while(text[offset]){
    length_holding = QTS_GetStringLineLength(&(text[offset]));
    if(length_holding > max_length) max_length = length_holding;
    offset += length_holding;
    if(!text[offset]) break; // if reached NULL
    offset++; // Add one for new line symbol
  }

  return max_length;
}










// END
