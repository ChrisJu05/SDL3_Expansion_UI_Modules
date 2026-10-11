// Qualety Text System QTS Header by Christopher Jung
// A header used with SDL3 and SDL3 TTF to create dynmic Text elements

// Header Guard:
#ifndef QTS_TEXT_HEADER_GUARD
#define QTS_TEXT_HEADER_GUARD

// Standerd Libuary Headers
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#define _USE_MATH_DEFINES

// Add use of SDL3 Libuarys
#include <SDL3/SDL.h>                   // the core SDL libuary
#include <SDL3/SDL_main.h>              // part of SDL
#include <SDL3_ttf/SDL_ttf.h>           // SDL3 extension for rendering true type fonts (ttf fonts)


// Defined Render method assingment
// (not a regular count so that is compatable with mini_lables header defines)
#define QTS_RENDERMETHOD_SOLID 0
#define QTS_RENDERMETHOD_SHADED 2
#define QTS_RENDERMETHOD_BLENDED 4
#define QTS_RENDERMETHOD_LCD 6
#define QTS_RENDERMETHOD_CUSTOM -1    // For custom render methods via render_func


// Line Text Alingment Enumeration
enum QTS_TextAlignment {
  BASE,     // Default (recomended) Alings text along their base lines
  TOP,      // Allings the top edges of the bounding boxes
  BOTTOM,   // Allings the bottom edges of the bounding boxes
  MIDDLE    // Allings the text objects around their middle hight
};



// Represents a pice of text all in one format (all one line) in witch the size and location of each character can be determined
// these text components can than be aranged together in to a composit text that can have multiple difrent formats
struct QTS_Text
{

  // Text
  char *string;                                         // UTF-8 String of characters (preferably NULL Terminated) (owned)
  size_t length;                                        // the length in bytes of the string ( 0 for NULL Terminated (for dynamic usecase)) (NULL termination includet in length)
  Uint32 codepoints;                                    // the number of codepoint characters in the string
  int *spacing;                                         // Array of spacings from the start of the text to each character including kerneling (size = codepoints) (owned)
  int hight;                                            // The total hight of the font (of the text) (is thehight of the global bounding box )
  int ascent;                                           // The offset from the baseline to the top edge of the global bounding box
  int descent;                                          // The offset from the baseline to the bottom edge of the global bounding box
  Uint32 first_codepoint;                               // first codepoint in the text (for kerneling with other text objects)
  Uint32 last_codepoint;                                // last codepoint in the text (for kerneling with other text objects)
  int pre_kernel_offset;                                // offset to the text object infront in the line

  // special format
  SDL_Texture* (*render_func)(struct QTS_Text*, void*); // for special interpretation like LATEX rendering (passed are the text object itself and a generic pointer that can be assinged with format data)
  void* ptr_format_data;                                // Generic pointer that will be passed to the render func (NOT OWNED)
                                                        // if a function is given that function will be used to render the text otherwise the default render system will be used

  // Foramt and Collor
  TTF_Font *ptr_font;                                   // Pointer to the SDL3 TTF font to be used for the Text (not owned)
  int render_method;                                    // method used for rendering the text (default is RENDERMETHOD_BLENDED = 4 for collor emoji suport)
  SDL_Color *ptr_color_f;                               // The foreground color to be used when rendering (not owned)
  SDL_Color *ptr_color_b;                               // The background color to be used when rendering (not owned)

  enum QTS_TextAlignment alignment;                     // Determins the Allinment of the Textobject in the Line
  bool pre_kernel;                                      // if true then kernel with text infront of text object (enabled by default)

  // Rendering Components
  SDL_Surface *ptr_surface;                             // Surface to render text on (only adjust if actualy updated) (owned)
  SDL_Texture *ptr_texture;                             // Texture for final rendering (only adjust if actualy updated) (owned)

  // Integration components
  // (for integrating in to dynamic text structures)
  struct QTS_Text *ptr_PrevText;                        // pointer to next text object in the line (null if is last) (not owned)
  struct QTS_Text *ptr_NextText;                        // pointer to previus text object in line (null if is first) (not owned)

};


// Represents a single Line of text that is made up of QTS_Text objects
// These can asembled in to a text box
struct QTS_Line
{

  // QTS_Text object List (NULL Terminated)
  struct QTS_Text *ptr_TextElements;    // Pointer to the QTS_Text elements in the line

  // Line propertys
  int hight;                            // Total hight of the line including spacing up and down
  int length;                           // Totoal length of the line in pixels

  // Text Propertys
  int MaxTextHight;                     // The total hight (not including spacing up or down) of the highest / tallest text in the line.
                                        // NOTE: some fonts may have theri base line at a diffrent relative hight so when calculating the Maximum Tallnes of all the text in the line add the ascent and descent values together
  int MaxTextAscent;                    // The maximum Ascent of anny Text element in the line
  int MaxTextDescent;                   // The maximum Descent of anny Text element in the line

  int Spaceing_Up;                      // The spacing above the line (from the spacing down of the line above to the top of this line)
  int Spaceing_Down;                    // The spacing above the line (from the Descent of this line to the start of the Up spacing of the next line)

  // Integration components
  struct QTS_Line *ptr_NextLine;        // Pointer to the next line
  struct QTS_Line *ptr_PrevLine;        // Pointer to the previus line

};



// Represents a Box of Text made up of QTS_Line objects
struct QTS_Box
{

  // QTS_Line object List (NULL Terminated)
  struct QTS_Line *ptr_LineElements;    // Pointer to the QTS_Line elements in the Text Box

  // Text Box propertys
  int MaxWidth;                         // Max Length of anny Line in the Text Box (in Pixels)
  int MaxHight;                         // Max Hight summ Result of all Lines Hight (including spacings)
  int LineCount;                        // Number of Lines in the Text Box (coresponds to number of objects in ptr_LineElements List)

  bool cross_kernaling;                 // determine if crosskerneling should be enabled
  bool ignore_empty_textelements;       // determine if empty text elements should be ignored during rendering

};




// Represents an editable Line
struct QTS_EditLine
{

  // The line Instance representing the visual Components (owned)
  struct QTS_Line* ptr_Line;

  // Update function for formating (leave NULL if no formatting needed) is called after every eddit to adjust the format
  bool (*format_function)(SDL_Renderer*, struct QTS_Line*, void*);
  void* ptr_foramt_data;                // a pointer passed to format_function that can be set to point to other formating data for the line (not owned)

  // Event processing overide function for custom behavior modifications
  bool (*event_proc_overide)(SDL_Renderer*, struct QTS_EditLine*, SDL_Event*, void*); // if returns false then regular event processing happens
  void* ptr_event_overide_data;         // an optional pointer to be passed to event_proc_overide function (not owned)

  // Accsesability settings
  bool visible;                         // Determines if the Line is renderd visible
  bool enable_highlight;                // if is disabled then no hilighting takes place
  bool enable_edit;                     // if is not enabled than no editing shal tae place
  bool selected;                        // determins if the line is selected

  // Render cordinates
  SDL_FRect line_rect;                  // defines render cordinates and size of interaction fild
  float offset_x, offset_y;             // Offset Cordinates of event processing
  float scaling;                        // is udes to scale event cordinates to streen cordinates (mouse cordinates * scaling = texture pixel cordinates)
  float scaling_offset;                 // used to scale local cordinates (on widget)

  // Render settings
  bool ignore_empty_textelements;
  bool crosskerneling;

  // Curser Holding data (codepoint text object ,...)
  int x_cord;                           // the x cordinate of the curser releative to the beginning of the line
  int back_codepoints;                  // Number of Codepoints before the text object inwitch the Incision point is
  int incision_codepoint;               // the codepoint at witch the Incision point is in the text object in that the Incision point is. (-1 = very start of text object)
  size_t incision_index;                // the string incission index
  struct QTS_Text* incission_text_element; // the textelement in witch the incission point is (NOT owned)
  SDL_Color* ptr_cursor_collor;         // Collor use for the curser (a blue default collor will be used if this is NULL (asuming the default cursor function QTS_EditLine_DrawCurser is used))
  Uint32 last_time;                     // the last time the blinking status was updated
  bool (*curser_render_func)(SDL_Renderer*, struct QTS_EditLine*);// The function used to draw the curser
  void* ptr_data_curser;                // pointer to optional aditional data used for to curser render func (not owned)

  // Highlight Data
  bool marking_active;                  // is true if mousbutton is down
  bool marking_backwards;               // if is true then the marking is done backwards and the start and end where fliped
  bool highlight_present;               // determines if a highlight is present
  int highlight_start;                  // the starting codepoint of the highlight
  int highlight_end;                    // the end codepoint of the highlight
  int x_cord_start;                     // The start cordinate of the highlight relative to the start of the line
  int x_cord_end;                       // the end cordinate of the highlight relative to the start of the line
  SDL_FRect highlight_rect;             // rect describing the area to be highlighted
  struct QTS_Text* highlight_start_text;// text element in witch the highlight starts
  struct QTS_Text* highlight_end_text;  // text element in witch the highlight ends
  SDL_Color* ptr_highlight_color;       // The collor to be used for the highlight
  bool (*highlight_render_func)(SDL_Renderer*, struct QTS_EditLine*); // the function to be used to draw the highlight
  void* ptr_data_highlight;             // pointer to optional additional data to be used for the highlight function (not owned)

};






// Represents an Editable Box
struct QTS_EditBox{

  // The Box Instance representing the visual Components (owned)
  struct QTS_Box* ptr_Box;

  // Update function for formating (leave NULL if no formatting needed) is called after every eddit to adjust the format
  bool (*format_function)(SDL_Renderer*, struct QTS_Box*, void*);
  void* ptr_foramt_data;                // a pointer passed to format_function that can be set to point to other formating data for the box (not owned)

  // Event processing overide function for custom behavior modifications
  bool (*event_proc_overide)(SDL_Renderer*, struct QTS_EditBox*, SDL_Event*, void*); // if returns false then regular event processing happens
  void* ptr_event_overide_data;         // an optional pointer to be passed to event_proc_overide function (not owned)

  // Accsesability settings
  bool visible;                         // Determines if the box is renderd visible
  bool enable_highlight;                // if is disabled then no hilighting takes place
  bool enable_edit;                     // if is not enabled than no editing shal tae place
  bool selected;                        // determins if the line is selected
  bool crosskerneling;

  // Render cordinates
  SDL_FRect box_rect;                  // defines render cordinates and size of interaction fild
  float offset_x, offset_y;             // Offset Cordinates of event processing
  float scaling;                        // is udes to scale event cordinates to streen cordinates (mouse cordinates * scaling = texture pixel cordinates)
  float scaling_offset;                 // used to scale local cordinates (on widget)

  // Curser Handeling data
  int x_cord, y_cord;                   // The X and Y cordinate relative to the top left corner of the edit box
  int back_codepoints;                  // Number of Codepoints before the text object inwitch the Incision point is (of the line)
  int incision_codepoint;               // the codepoint at witch the Incision point is in the text object in that the Incision point is. (-1 = very start of text object)
  size_t incision_index;                // the string incission index
  struct QTS_Text* incission_text_element; // the textelement in witch the incission point is (NOT owned)
  struct QTS_Line* ptr_incission_line;
  SDL_Color* ptr_cursor_collor;         // Collor use for the curser (a blue default collor will be used if this is NULL (asuming the default cursor function QTS_EditLine_DrawCurser is used))
  Uint32 last_time;                     // the last time the blinking status was updated
  bool (*curser_render_func)(SDL_Renderer*, struct QTS_EditBox*);// The function used to draw the curser
  void* ptr_data_curser;                // pointer to optional aditional data used for to curser render func (not owned)

  // Hilight Handeling function
  bool marking_active;                  // is true if mousbutton is down
  bool marking_backwards;               // if is true then the marking is done backwards and the start and end where fliped
  bool highlight_present;               // determines if a highlight is present
  int highlight_start;                  // the starting codepoint of the highlight
  int highlight_end;                    // the end codepoint of the highlight
  int x_cord_start;                     // The start cordinate of the highlight relative to the start of the line
  int x_cord_end;                       // the end cordinate of the highlight relative to the start of the line
  SDL_FRect highlight_rect_start;       // rect describing the area to be highlighted
  struct QTS_Line* hilight_start_line;  // the line in witch the hilight starts
  struct QTS_Text* highlight_start_text;// text element in witch the highlight starts
  SDL_FRect highlight_rect_end;         // rect describing the area to be highlighted
  struct QTS_Line* hilight_end_line;    // the line in witch the hilight ends
  struct QTS_Text* highlight_end_text;  // text element in witch the highlight ends
  SDL_Color* ptr_highlight_color;       // The collor to be used for the highlight
  bool (*highlight_render_func)(SDL_Renderer*, struct QTS_EditBox*, SDL_FRect); // the function to be used to draw the highlight
  void* ptr_data_highlight;             // pointer to optional additional data to be used for the highlight function (not owned)

};





// Functions Decloration

// QTS_Utils.c
char* QTS_MergeStrings(char* string_A, size_t size_A, char* string_B, size_t size_B, size_t* ptr_size_new);
int QTS_CountCodepoints(char* string);
bool QTS_RemoveNewLineSymbols(char* string);
struct QTS_Text* QTS_CloneTextFormat(SDL_Renderer *ptr_renderer, struct QTS_Text* ptr_source);
void QTS_PrintLineComposition(struct QTS_Line* ptr_Line);
int QTS_CountCodepointsInLine(struct QTS_Line* ptr_Line);
bool QTS_IsSeperationChar(char symb);
int QTS_IndexToCodepoint(char* string, size_t index);
int QTS_CountStringLines(char* text);
int QTS_GetStringLineLength(char* text);
int QTS_GetMaxLineLength(char* text);

// QTS_Text.c
struct QTS_Text* QTS_CreateText(SDL_Renderer *ptr_renderer, TTF_Font *ptr_font, char *string, size_t pslen, int render_method, SDL_Color *ptr_color_f, SDL_Color *ptr_color_b, SDL_Texture* (render_func)(struct QTS_Text*, void*), void* ptr_format_data);
int QTS_GetPreKernelOffset(struct QTS_Text* ptr_Text);
void QTS_DestroyTextOBJ(struct QTS_Text* ptr_Text);
void QTS_DestroyTextChain(struct QTS_Text* ptr_Text);
bool QTS_RenderText(SDL_Renderer *ptr_renderer, struct QTS_Text* ptr_Text, float x, float y);
bool QTS_UpdateText(SDL_Renderer *ptr_renderer, struct QTS_Text* ptr_Text);
bool QTS_GetGlyphKerning(TTF_Font* ptr_Font, Uint32 ch1, Uint32 ch2, int* ptr_kerneling);
bool QTS_UpdateTextString(struct QTS_Text* ptr_Text, char* new_string, size_t strlen, bool copy);
struct QTS_Text* QTS_SplitText(SDL_Renderer *ptr_renderer, struct QTS_Text* ptr_Text, int SplitCodepoint);
bool QTS_CompareTextFormats(struct QTS_Text* ptr_Text_A, struct QTS_Text* ptr_Text_B);
bool QTS_MergeText(SDL_Renderer *ptr_renderer, struct QTS_Text* ptr_Text, bool force_merge);
bool QTS_InsertString(struct QTS_Text* ptr_Text, char* ins_string, size_t Incision_index);
bool QTS_DrawBoundingBoxes(SDL_Renderer *ptr_renderer, struct QTS_Text* ptr_Text, float x, float y);

// QTS_Line.c
struct QTS_Line* QTS_CreateLine();
bool QTS_AddTextToLine_After(struct QTS_Line* ptr_line, struct QTS_Text* ptr_text_root, struct QTS_Text* ptr_text_add);
bool QTS_UpdateLine(struct QTS_Line* ptr_line);
bool QTS_RemoveTextFromLine(struct QTS_Line* ptr_line, struct QTS_Text* ptr_text_remove);
struct QTS_Text* QTS_SplitLineAt(SDL_Renderer *ptr_renderer, struct QTS_Line* ptr_line, int split_codepoint);
bool QTS_RemoveSectionFromLine(SDL_Renderer *ptr_renderer, struct QTS_Line* ptr_line, int start_code_point,  int end_code_point, struct QTS_Text** ptr_ptr_text_remove);
int QTS_GetLineSize_CP(struct QTS_Line* ptr_line);
size_t QTS_GetLineSize_ST(struct QTS_Line* ptr_line);
bool QTS_GetLineString(struct QTS_Line* ptr_line, char** ptr_ptr_string, int* ptr_codepoints, size_t* ptr_size);
void QTS_PrintLine(struct QTS_Line* ptr_line);
bool QTS_GetLineSectionString(struct QTS_Line* ptr_line, int start_code_point,  int end_code_point, char** ptr_ptr_string, size_t* ptr_size);
bool QTS_CutLineOffAfter(SDL_Renderer *ptr_renderer, struct QTS_Line* ptr_line, int cut_codepoint, struct QTS_Text** ptr_ptr_text_remove);
float QTS_GetAllingmentOffset(struct QTS_Line* ptr_line, struct QTS_Text* ptr_Text);
bool QTS_RenderLine(SDL_Renderer *ptr_renderer, struct QTS_Line* ptr_line, float x, float y, bool ignore_empty_textelements, bool cross_kernaling);
int QTS_GetCodepointFromPos(struct QTS_Line* ptr_line, int x_pos);
bool QTS_GetLineIncisionPoint_FromCodepoint(struct QTS_Line* ptr_line, int pre_selected_codepoint, struct QTS_Text** ptr_ptr_Incision_text, int* x_pos, int* ptr_back_codepoints, int* ptr_insison_codepoint, size_t* ptr_Incision_index, bool adjust_for_prekerneling);
bool QTS_GetLineIncisionPoint_FromPos(struct QTS_Line* ptr_line, int x_pos, struct QTS_Text** ptr_ptr_Incision_text, int* ptr_back_codepoints, int* ptr_insison_codepoint, size_t* ptr_Incision_index, bool adjust_for_prekerneling);
bool QTS_ConsolidateLine(SDL_Renderer *ptr_renderer, struct QTS_Line* ptr_line, bool force_merge);
bool QTS_GetLineIncisionPoint(struct QTS_Line* ptr_line, int x_pos, int* ins_x_pos, struct QTS_Text** ptr_ptr_Incision_text, int* ptr_back_codepoints, int* ptr_insison_codepoint, size_t* ptr_Incision_index, bool adjust_for_prekerneling);
bool QTS_MoveIncisionPoint(struct QTS_Line* ptr_line, int mov_codepoints, struct QTS_Text** ptr_ptr_Incision_text, int* x_pos, int* ptr_back_codepoints, int* ptr_insison_codepoint, size_t* ptr_Incision_index, bool adjust_for_prekerneling);
void QTS_DestroyLine(struct QTS_Line* ptr_line);
void QTS_DestroyLineChain(struct QTS_Line* ptr_line);
bool QTS_FormatLineSection(SDL_Renderer *ptr_renderer, struct QTS_Line* ptr_line, int start_codepoint_in, int end_codepoint_in, struct QTS_Text* ptr_format_input);
struct QTS_Line* QTS_ConvertTextToLines(SDL_Renderer *ptr_renderer, struct QTS_Text* ptr_FormatReference, char* text, struct QTS_Line** ptr_ptr_last_line, struct QTS_Text** ptr_ptr_last_text);

// QTS_Box.c
bool QTS_UpdateBoxParameters(struct QTS_Box* ptr_box);
struct QTS_Box* QTS_CreateBox(struct QTS_Line *ptr_LineElements);
void QTS_DestroyBox(struct QTS_Box* ptr_box);
bool QTS_RenderBox(SDL_Renderer *ptr_renderer, struct QTS_Box* ptr_Box, float x, float y);
bool QTS_AddLineToBox(struct QTS_Box* ptr_Box, struct QTS_Line* ptr_line_root, struct QTS_Line* ptr_line_add);
bool QTS_AddLineCahinToBox(struct QTS_Box* ptr_Box, struct QTS_Line* ptr_line_root, struct QTS_Line* ptr_line_add);
bool QTS_RemoveLineFromBox(struct QTS_Box* ptr_Box, struct QTS_Line* ptr_line_remove);
bool QTS_SpiltLineChain(struct QTS_Line* ptr_chain_split);
char* QTS_GetBoxText(struct QTS_Box* ptr_Box, size_t* ptr_size);

// QTS_EditLine_Functions.c
bool QTS_EditLine_DrawCurser(SDL_Renderer *ptr_renderer, struct QTS_EditLine* ptr_EditLine);
bool QTS_EditLine_GetHighlightRect(struct QTS_EditLine* ptr_EditLine);
bool QTS_EditLine_DrawHighlight(SDL_Renderer *ptr_renderer, struct QTS_EditLine* ptr_EditLine);
bool QTS_EditLine_HandleMouseButtonDown(struct QTS_EditLine* ptr_EditLine, SDL_Event* ptr_Event);
bool QTS_EditLine_HandleMouseButtonUp(struct QTS_EditLine* ptr_EditLine, SDL_Event* ptr_Event);
bool QTS_EditLine_HadleMoueMotion(struct QTS_EditLine* ptr_EditLine, SDL_Event* ptr_Event);
bool QTS_EditLine_HilightAll(struct QTS_EditLine* ptr_EditLine);
bool QTS_EditLine_CopySelectedTextToClipboard(struct QTS_EditLine* ptr_EditLine);
bool QTS_EditLine_DestroyHilightedText(SDL_Renderer *ptr_renderer, struct QTS_EditLine* ptr_EditLine);
bool QTS_EditLine_InsertText(SDL_Renderer *ptr_renderer, struct QTS_EditLine* ptr_EditLine, char* text);
bool QTS_EditLine_UpdateCurser(struct QTS_EditLine* ptr_EditLine);
bool QTS_EditLine_UpdateHilight(struct QTS_EditLine* ptr_EditLine);
bool QTS_EditLine_MoveCurser(struct QTS_EditLine* ptr_EditLine, int codepoints);
bool QTS_EditLine_PasetTextFromClipboard(SDL_Renderer *ptr_renderer, struct QTS_EditLine* ptr_EditLine);
bool QTS_EditLine_InputText(SDL_Renderer *ptr_renderer, struct QTS_EditLine* ptr_EditLine, char* ptr_text);
bool QTS_EditLine_HandleTextInput(SDL_Renderer *ptr_renderer, struct QTS_EditLine* ptr_EditLine, SDL_Event* ptr_Event);
bool QTS_EditLine_CutSelectedText(SDL_Renderer *ptr_renderer, struct QTS_EditLine* ptr_EditLine);
int QTS_EditLine_GetCurserCodepoint(struct QTS_EditLine* ptr_EditLine);
bool QTS_EditLine_Backspace(SDL_Renderer *ptr_renderer, struct QTS_EditLine* ptr_EditLine);
bool QTS_EditLine_BackspaceToBeginning(SDL_Renderer *ptr_renderer, struct QTS_EditLine* ptr_EditLine);
bool QTS_EditLine_Delete(SDL_Renderer *ptr_renderer, struct QTS_EditLine* ptr_EditLine);
bool QTS_EditLine_DeleteToEnd(SDL_Renderer *ptr_renderer, struct QTS_EditLine* ptr_EditLine);
bool QTS_EditLine_Escape(struct QTS_EditLine* ptr_EditLine);
bool QTS_EditLine_MoveCurserToBegining(struct QTS_EditLine* ptr_EditLine);
bool QTS_EditLine_MoveCurserToEnd(struct QTS_EditLine* ptr_EditLine);
bool QTS_EditLine_HilightWord(struct QTS_EditLine* ptr_EditLine);

// QTS_EditLine.c
struct QTS_EditLine* QTS_CreateEditLine(struct QTS_Line* ptr_Line, float x, float y, float w, float h, float scaling);
void QTS_DestroyEditLine(struct QTS_EditLine* ptr_EditLine, bool destroy_line_instance);
bool QTS_RednerEditLine(SDL_Renderer *ptr_renderer, struct QTS_EditLine* ptr_EditLine);
bool QTS_EditLine_HandleEvent(SDL_Renderer *ptr_renderer, struct QTS_EditLine* ptr_EditLine, SDL_Event* ptr_Event);

// QTS_EditBox_Functions.c
bool QTS_EditBox_DrawCurser(SDL_Renderer* ptr_renderer, struct QTS_EditBox* ptr_EditBox);
bool QTS_EditBox_DrawHighlightRect(SDL_Renderer* ptr_renderer, struct QTS_EditBox* ptr_EditBox, SDL_FRect Hilight_Rect);
struct QTS_Line* QTS_EditBox_GetLineFromCordinate(struct QTS_Box* ptr_Box, int y_cord);
int QTS_EditBox_GetLineIndex(struct QTS_Box* ptr_Box, struct QTS_Line* ptr_line);
int QTS_EditBox_GetLineOffset(struct QTS_Box* ptr_Box, struct QTS_Line* ptr_Line);
bool QTS_EditBox_CalculateHighlightRects(struct QTS_EditBox* ptr_EditBox);
bool QTS_EditBox_DrawHilights(SDL_Renderer* ptr_renderer, struct QTS_EditBox* ptr_EditBox);
void QTS_EditBox_FlipHilight(struct QTS_EditBox* ptr_EditBox);
bool QTS_EditBox_HandleMouseButtonDown(struct QTS_EditBox* ptr_EditBox, SDL_Event* ptr_Event);
bool QTS_EditBox_HandleMouseButtonUp(struct QTS_EditBox* ptr_EditBox, SDL_Event* ptr_Event);
bool QTS_EditBox_HadleMoueMotion(struct QTS_EditBox* ptr_EditBox, SDL_Event* ptr_Event);
bool QTS_EditBox_HilightAll(struct QTS_EditBox* ptr_EditBox);
char* QTS_EditBox_GetSelectedText(struct QTS_EditBox* ptr_EditBox, size_t* ptr_size);
bool QTS_EditBox_CopySelectedTextToClipboard(struct QTS_EditBox* ptr_EditBox);
bool QTS_EditBox_DestroyHilightedText(SDL_Renderer *ptr_renderer, struct QTS_EditBox* ptr_EditBox);
bool QTS_EditBox_InsertText(SDL_Renderer *ptr_renderer, struct QTS_EditBox* ptr_EditBox, char* text);
bool QTS_EditBox_UpdateCurser(struct QTS_EditBox* ptr_EditBox);
bool QTS_EditBox_UpdateHilight(struct QTS_EditBox* ptr_EditBox);
bool QTS_EditBox_MoveCurser(struct QTS_EditBox* ptr_EditBox, int codepoints);
bool QTS_EditBox_PasetTextFromClipboard(SDL_Renderer *ptr_renderer, struct QTS_EditBox* ptr_EditBox);
bool QTS_EditBox_InputText(SDL_Renderer *ptr_renderer, struct QTS_EditBox* ptr_EditBox, char* ptr_text);
bool QTS_EditBox_HandleTextInput(SDL_Renderer *ptr_renderer, struct QTS_EditBox* ptr_EditBox, SDL_Event* ptr_Event);
bool QTS_EditBox_CutSelectedText(SDL_Renderer *ptr_renderer, struct QTS_EditBox* ptr_EditBox);
int QTS_EditBox_GetCurserCodepoint(struct QTS_EditBox* ptr_EditBox);
bool QTS_EditBox_Backspace(SDL_Renderer *ptr_renderer, struct QTS_EditBox* ptr_EditBox);
bool QTS_EditBox_BackspaceToBeginning(SDL_Renderer *ptr_renderer, struct QTS_EditBox* ptr_EditBox);
bool QTS_EditBox_Delete(SDL_Renderer *ptr_renderer, struct QTS_EditBox* ptr_EditBox);
bool QTS_EditBox_DeleteToEnd(SDL_Renderer *ptr_renderer, struct QTS_EditBox* ptr_EditBox);
bool QTS_EditBox_Escape(struct QTS_EditBox* ptr_EditBox);
bool QTS_EditBox_MoveCurserToBegining(struct QTS_EditBox* ptr_EditBox);
bool QTS_EditBox_MoveCurserToEnd(struct QTS_EditBox* ptr_EditBox);
bool QTS_EditBox_MoveCurserUp(struct QTS_EditBox* ptr_EditBox);
bool QTS_EditBox_MoveCurserDown(struct QTS_EditBox* ptr_EditBox);
bool QTS_EditBox_SwitchCurserLineUp(struct QTS_EditBox* ptr_EditBox);
bool QTS_EditBox_SwitchCurserLineDown(struct QTS_EditBox* ptr_EditBox);
bool QTS_EditBox_HilightWord(struct QTS_EditBox* ptr_EditBox);
bool QTS_EditBox_HandleReturn(SDL_Renderer *ptr_renderer, struct QTS_EditBox* ptr_EditBox);
bool QTS_EditBox_HilightSelectedLine(struct QTS_EditBox* ptr_EditBox);

// QTS_EditBox.c
struct QTS_EditBox* QTS_CreateEditBox(struct QTS_Box* ptr_Box, float x, float y, float w, float h, float scaling);
void QTS_DestroyEditBox(struct QTS_EditBox* ptr_EditBox);
bool QTS_RenderEditBox(SDL_Renderer *ptr_renderer, struct QTS_EditBox* ptr_EditBox);
bool QTS_EditBox_HandleEvent(SDL_Renderer *ptr_renderer, struct QTS_EditBox* ptr_EditBox, SDL_Event* ptr_Event);



// End of Header Guard
#endif

// END
