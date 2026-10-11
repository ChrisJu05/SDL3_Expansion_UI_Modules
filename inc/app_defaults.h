// app_default.h header file
/*
This Headerfile contains definitions for default values for the Application
*/

#ifndef APP_DEFAULTS_HEADER_GUARD
#define APP_DEFAULTS_HEADER_GUARD
// Standerd Parameters definition
#define DEFAULT_WINDOW_WIDTH 1200                                               // The default window width upon the creation of the window
#define DEFAULT_WINDOW_HIGHT 800                                                // The default window hight upon the creation of the window
#define DEFAULT_WINDOW_RESIZABLE true                                           // The default setting if the window should be resizable
#define DEFAULT_USER_SCALER 1.0f                                                // a default UI scaling value (optional use)
//#define DEFAULT_SCALE 5.0f                                                       // How large the texture is relatice to the window by default

#define DEFAULT_PIXELFORMAT SDL_PIXELFORMAT_RGBA8888                            // standerd Pixelformat (Note: Platforms like apple may have a diffrent preference)
//#define DEFAULT_IMPLEMENT_GPU false
//#define DEFAULT_USE_GPU_RENDERER false


#endif
