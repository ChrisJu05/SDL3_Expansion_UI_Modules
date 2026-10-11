// SDL3 Project Template V3 by Christopher Jung created 30.08.2026 for GCC compiler.
// Uses SDL3, SDL3_ttf, SDL3_immage (optionaly more libuarys)
/*
This is a SDL3 Project Template for UI applications

 Compillation Notes:
 Needs to be linked against SDL3, SDL3_image, SDL3_ttf, and optionaly SDL3_net

 for installed libuarys:
 gcc <main_file_name> -o <output_name> -lSDL3 -lSDL3_ttf -lSDL3_image -lm

 for compiled libuarys:
 gcc <main_file_name> glad.c -o <output_name> -I./include -L./SDL/build -lSDL3 -L./SDL_ttf/build -lSDL3_ttf -L./SDL_image/build -lSDL3_image -lGL -ldl -lm

Dependencys:

 This template only relys on SDL3 and special sdl3 headers that are included but optional.

 apt packeges:
 libsdl3-dev, libsdl3-ttf-dev, libsdl3-image-dev, (optionaly libsdl3-net-dev but does not yet exsist in apt)

 Other Notes:

   - This Template is made to be as platform independent as posible by reliing on SDL functions.
     However further modifications may be neccecary for platforms like IOS and Android like removing
     the standerd libuarys.

   - you may consider putting the head components of this template in to a main.h file
     keep in mind that the order of the components may be of significance

*/

// PLATFORM IDENTIFICATION
// on diffrent platfroms diffrent defines will have been set by the compiler
// Platfroms: __APPLE__; __linux__; __MINGW32__ (for Windows);

// Include Header
#include "main.h"
#include "error_handler.c"

// Default values are defined here
#include "inc/app_defaults.h"

// Define the Application metadata (optional)
#define APP_METADATA_APPNAME "SDL3_Template_Project"
#define APP_METADATA_VERSION "VERSION: TEMPLATE_REV_2"
#define APP_METADATA_APPIDENTIFYER NULL



// Global Variabels for the Application

// Return values
int i_ret;  // integer return value
bool b_ret; // bool return value

// Application Loop
bool done = false;    // while is false the applicaton loop keeps running
SDL_Event sdl_event;  // Holds the current event procured by SDL_PollEvent






// Main Function
int main(int argc, char *argv[]) {

  // Start by setting the Application Metadata
  b_ret = SDL_SetAppMetadata(APP_METADATA_APPNAME, APP_METADATA_VERSION, APP_METADATA_APPIDENTIFYER);
  if(!b_ret) if(ERROR_HANDLER(0)) return 1; // Is A noncritical error but should be noted

  // optional: print Version and Revision of Linked SDL Libuary
  printf("Initialized SDL3 Version %d Revision %s \n", SDL_GetVersion(), SDL_GetRevision());



  // Initialization of SDL and its components for one window

  // Initialize SDL3
  // The SDL_INIT_VIDEO was chose as it initializes the events subsistem that is critical for the application.
  // Note: after SDL sucsesfully Initialized, SDL must be quit before the application exits.
  b_ret = SDL_Init(SDL_INIT_VIDEO);
  if(!b_ret) if(ERROR_HANDLER(1)) goto CLEAN_UP;
  // Is a critical error as the SDL system is a primarry part of the application


  // Initialize SDL3 TTF Libuarry
  // (does not take flags as input)
  // Note: after TTF sucsesfully Initialized, TTF must be quit before the application exsits.
  b_ret = TTF_Init();
  if(!b_ret) if(ERROR_HANDLER(2)) goto CLEAN_UP;
  // Is a critical error as the TTF system is needed th the application


  // SDL3_image Does Not requiure Initialization


  // Only include if application directly uses network functions REMEMBER NET_Quit()
  #if (USE_SDL_NET == true)

    // SDL3 NET for Network components of an application
    // Note: After SDL NET sucsesfully Initialized it must be closed with NET_Quit() befor the application closes
    // NET_Init() does not take flags as input
    b_ret = NET_Init();
    if(!b_ret) if(ERROR_HANDLER(3)) goto CLEAN_UP;
    // May be critical to application if not comment out return 1 or use ERROR Handler

  #endif


  // Create the Application window
  // Window and Renderer are held within this struct
  struct AplicationWindow Prim_Window;
  InitWindowStruct(&Prim_Window); // Initializes struct to acceptable starting values

  // Also sets the window title (can also be changed later), default window size and window flags (can be changed later). (Sets window to Resizable and High pixel densitiy flag for full display use)
  Prim_Window.ptr_window = SDL_CreateWindow(APP_METADATA_APPNAME, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HIGHT, SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY);
  if(!Prim_Window.ptr_window) if(ERROR_HANDLER(4)) goto CLEAN_UP;

  // Sett Window Floags
  // Set default window flags here like Transparency, Borderd, Fullsecreen, ...





  // Create a Renderer (2D Rendering context) for the Window  (ALTERNATIVELY a GPU Renderer can be created)
  // Note "Primary_Window_Renderer" is the nmae of the renderer (can be null) is usefull to name if multiple renderers exist.
  Prim_Window.ptr_renderer = SDL_CreateRenderer(Prim_Window.ptr_window, NULL);
  if(!(Prim_Window.ptr_renderer)) if(ERROR_HANDLER(5)) goto CLEAN_UP;
  // Is a critical error as the application cannot draw annything without a renderer
/*
  // Device = NULL => Create GPU device
  Prim_Window.ptr_renderer = SDL_CreateGPURenderer(NULL, Prim_Window.ptr_window);
  if(!(Prim_Window.ptr_renderer)) if(ERROR_HANDLER(5)) goto CLEAN_UP;
*/


  // Confugure The Renderer Defaults:

  // Set default Blend Mode
  SDL_SetRenderDrawBlendMode(Prim_Window.ptr_renderer, SDL_BLENDMODE_BLEND);

  // Set the renderer Vsync (Recomended 1)
  // Vsync is set to 1 => Update with every vertiacal refresh
  // The vsync parameter can be 1 to synchronize present with every vertical refresh, 2 to synchronize present with every second vertical refresh, etc.,
  // Alternatively it can be set to SDL_RENDERER_VSYNC_ADAPTIVE for adaptive Vsync; or SDL_RENDERER_VSYNC_DISABLED
  // Note: Depending on the platform this setting may cause performance prebblems
  b_ret = SDL_SetRenderVSync(Prim_Window.ptr_renderer, 1);
  if(!b_ret) if(ERROR_HANDLER(6)) goto CLEAN_UP;



  // Configure the Window structure with default values:

  // start by setting up the Window Texture (struct AplicationWindow *ptr_Window, int upscaling_factor, float usr_scale, bool use_pointsize)
  b_ret = SetUpWindowTexture(&Prim_Window, 2, 1.0f, true); // only even numbers make sens for upsacling values
  if(!b_ret) if(ERROR_HANDLER(6)) goto CLEAN_UP;

  Prim_Window.background_color.r    = 30;
  Prim_Window.background_color.g    = 30;
  Prim_Window.background_color.b    = 30;
  Prim_Window.background_color.a    = 255;
  // Print the window Parameters (for trouble shooting purposes)
  //printf("Window Pixel Density: %f \nWindow Display scale: %f \nPixel Density: %f \n", SDL_GetWindowPixelDensity(Prim_Window.ptr_window), SDL_GetWindowDisplayScale(Prim_Window.ptr_window), SDL_GetWindowPixelDensity(Prim_Window.ptr_window));

  // Test if pointsize should be used (optional)
  if(Prim_Window.window_pixel_size.w == Prim_Window.window_logical_size.w && Prim_Window.window_pixel_size.h == Prim_Window.window_logical_size.h) Prim_Window.use_pointsize = false;


  // Initialization for SDL and one window complete.






  // Space for setting up the Rest of the Application






  // Space for Main Loop Relevant Components

  // Create mosue variables
  float mouse_fx, mouse_fy;

  // Vrtual Grid
  float vgrid_x = 0.0f;
  float vgrid_y = 0.0f;
  float prev_vgrid_x, prev_vgrid_y;
  float prev_x;
  float prev_y;
  bool right_mousebutton_down;

  prev_vgrid_x = 0.0f;
  prev_vgrid_y = 0.0f;


  // Main Application Loop
  while(!done){

    // Start by Processing SDL Event (may be done as a function)

    // The Event case structure has been pre configured to be compatible with the platfroms listed above

    while(SDL_PollEvent(&sdl_event)){

      // Convert Event cordinates to render cordinates so that cordinates do not deed to be maped from logical to render
      // Various event types are converted with this function: mouse, touch, pen, etc.
      // Keep in mind the upsacling value multiplys the drawing texture size.
      b_ret = SDL_ConvertEventToRenderCoordinates(Prim_Window.ptr_renderer, &sdl_event);
      if(!b_ret) if(ERROR_HANDLER(12)) goto CLEAN_UP;


      // Event type switch (For General Application Events)
      switch (sdl_event.type){

        case SDL_EVENT_TERMINATING:
          /* Terminate the app.
            Shut everything down before returning from this function.
          */
          done = true;
          break;

        case SDL_EVENT_LOW_MEMORY:
          /* You will get this when your app is paused and iOS wants more memory.
            Release as much memory as possible.
          */
          break;

        case SDL_EVENT_WILL_ENTER_BACKGROUND:
          /* Prepare your app to go into the background.  Stop loops, etc.
           This gets called when the user hits the home button, or gets a call.
          */
          break;

        case SDL_EVENT_DID_ENTER_BACKGROUND:
          /* This will get called if the user accepted whatever sent your app to the background.
            If the user got a phone call and canceled it, you'll instead get an SDL_EVENT_DID_ENTER_FOREGROUND event and restart your loops.
            When you get this, you have 5 seconds to save all your state or the app will be terminated.
            Your app is NOT active at this point.
          */

          break;

        case SDL_EVENT_WILL_ENTER_FOREGROUND:
          /* This call happens when your app is coming back to the foreground.
            Restore all your state here.
          */
          break;

        case SDL_EVENT_DID_ENTER_FOREGROUND:
          /* Restart your loops here.
            Your app is interactive and getting CPU again.
          */
          break;

        case SDL_EVENT_WINDOW_MAXIMIZED:
          /*
          This gets triggerd when the window enters full screen mode (primerly for windows and Linux)
          */
          break;

        case SDL_EVENT_WINDOW_RESIZED:
          /*
          This gets triggerd when thw window has been resized in anny way
          */
          // Function for handeling the resizing of the window (including adjusting the window texture)
          i_ret = HandlWindowResizing(&Prim_Window);
          if(i_ret != 0) if(ERROR_HANDLER(i_ret)) goto CLEAN_UP;
          vgrid_x = 0.0f;
          vgrid_y = 0.0f;
          break;

        case SDL_EVENT_WINDOW_DISPLAY_SCALE_CHANGED:
          /*
          Displays now have a content display scale, which is the expected scale for content based on the DPI (Dots Per Inch (Pixel density)) settings of the display.
          This gets triggerd when the scale changes.
          */
          // Adjust the scale
          // Prim_Window.usr_scale = SDL_GetWindowDisplayScale(Prim_Window.ptr_window);
          // Adjust content scale
          Prim_Window.display_content_scale = SDL_GetWindowDisplayScale(Prim_Window.ptr_window); //SDL_GetDisplayContentScale(SDL_GetDisplayForWindow(Prim_Window.ptr_window));
          // The new scale may demand a change in the window texture
          i_ret = HandlWindowResizing(&Prim_Window);
          if(i_ret != 0) if(ERROR_HANDLER(i_ret)) goto CLEAN_UP;
          // Update UI scale
          UpdateWindowUserScale(&Prim_Window, Prim_Window.usr_scale);

          break;

        case SDL_EVENT_DISPLAY_CONTENT_SCALE_CHANGED:
          /*
          This gets triggerd when the display content scale changed
          */
          // Adjust content scale
          Prim_Window.display_content_scale = SDL_GetWindowDisplayScale(Prim_Window.ptr_window); //SDL_GetDisplayContentScale(SDL_GetDisplayForWindow(Prim_Window.ptr_window));
          // Adjust the scale
          // Prim_Window.scale = SDL_GetWindowDisplayScale(Prim_Window.ptr_window);
          // The new scale may demand a change in the window texture
          i_ret = HandlWindowResizing(&Prim_Window);
          if(i_ret != 0) if(ERROR_HANDLER(i_ret)) goto CLEAN_UP;
          // Update UI scale
          UpdateWindowUserScale(&Prim_Window, Prim_Window.usr_scale);

          break;

        case SDL_EVENT_MOUSE_BUTTON_DOWN:

          if(sdl_event.button.button == SDL_BUTTON_RIGHT){
            right_mousebutton_down = true;
            prev_x = mouse_fx;
            prev_y = mouse_fy;
            //printf("V_Grid %f  %f  \n", prev_x, prev_y);
          }

          break;

        case SDL_EVENT_MOUSE_BUTTON_UP:

          right_mousebutton_down = false;
          prev_vgrid_x = vgrid_x;
          prev_vgrid_y = vgrid_y;

          break;

        case SDL_EVENT_QUIT:
          /*
          Application was quit, so shut everything down permanently (primarly for windows and Linux)
          */
          done = true;
          break;


        default:
          /* No special processing, add it to the event queue */

          break;

      }// End of Event type switch

      // Sub Component Event Processing Here
      // Pass on event to Important components (like edit boxes)





    }// End of Event processing Loop


    // Space for Processing direct Inputs Here (like Touch and mouse):

    SDL_GetMouseState(&mouse_fx, &mouse_fy); // Note: if a scale is applyed then these cordinates must be transoformed with that scale
    mouse_fx = mouse_fx * Prim_Window.upscaling_factor;
    mouse_fy = mouse_fy * Prim_Window.upscaling_factor;



    // Space for Recalculating UI components based on Input:

    // Handle Virtual Grid
    if(right_mousebutton_down){
      vgrid_x = prev_vgrid_x + (mouse_fx - prev_x);
      vgrid_y = prev_vgrid_y + (mouse_fy - prev_y);
    }



    // Start of Application Drawing process:

    // clear the areas that need to be updated (or the whole screen)
    SDL_SetRenderDrawColor(Prim_Window.ptr_renderer, Prim_Window.background_color.r, Prim_Window.background_color.g, Prim_Window.background_color.b, Prim_Window.background_color.a);
    //SDL_SetRenderDrawColor(Prim_Window.ptr_renderer, 0, 0, 0, 0);
    SDL_RenderClear(Prim_Window.ptr_renderer);
    //SDL_RenderRect(Prim_Window.ptr_renderer, NULL);

    // Application loop start complete
    //-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // Place Rendering stuff here

    // Example Circle
    SDL_SetRenderDrawColor(Prim_Window.ptr_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    RenderFillCircle(Prim_Window.ptr_renderer, 1800 + vgrid_x, 1800 + vgrid_y, 300);


    SDL_SetRenderDrawColor(Prim_Window.ptr_renderer, 0xFF, 0x1F, 0x1F, 0xFF);
    RenderLineDynamic(Prim_Window.ptr_renderer, 200, 200, 1800 + vgrid_x, 1800 + vgrid_y, 20);

    RenderBezianCurve(Prim_Window.ptr_renderer, 200, 200, 1800 + vgrid_x, 1800 + vgrid_y, 2100, 2100, 20);


    // End of Rendering stuff
    //-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

    // Render Presentation Component:

    // Update the window frame buffer
    b_ret = UpdateWindow(&Prim_Window);
    if(!b_ret) if(ERROR_HANDLER(i_ret)) goto CLEAN_UP;



  }// End of Application Loop


  //-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  CLEAN_UP:
  printf("QUITTING! \n");
  // Applicaton is Ending
  // Put Cleanup Here -> shut everything down completly (free all the memmory)
  // SDL shutdown will be handeled by exit_application function
  // SDL_Quit, ... is handled last





  DestroyAplicationWindow_stack(&Prim_Window); // stack because Prim_Window is a stack variable
  //-----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
  // Final shutdown: destroying the window and its components

  // Quit SDL libs last
  #if (USE_SDL_NET == true)
  NET_Quit();
  #endif
  TTF_Quit();
  SDL_Quit();

  if(i_ret != 0 || !b_ret){
    return 1;
  }else{
    return 0;
  }
  // OBLIGATORY RETURN BECAUSE XCODE SUCKS
  return 1;
}




// END
