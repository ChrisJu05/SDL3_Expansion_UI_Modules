// main.h header file

// Header Guard
#ifndef main_header_guard
#define main_header_guard

// Compilation Relevant Defines
#define USE_SDL_NET false           // Only include SDL3 Net if it is actualy needed for the project

// Include SDL3 Libuarys
#include <SDL3/SDL.h>                   // the core SDL libuary
#include <SDL3/SDL_main.h>              // part of SDL
#include <SDL3_ttf/SDL_ttf.h>           // SDL3 extension for rendering true type fonts (ttf fonts)
#include <SDL3_image/SDL_image.h>       // SDL3 extenion for rendering immages and animations

#if (USE_SDL_NET == true)               // Only include SDL3 net if it is actualy needed for the project
#include <SDL3_net>
#endif

// Standerd Libuarys needed for the Project
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#define _USE_MATH_DEFINES
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <time.h>



// Expansion Modules includes
#include "inc/dynamic_lines.h"
#include "inc/edit_box.h"
#include "inc/glass.h"
#include "inc/mini_animations.h"
#include "inc/mini_labels.h"
#include "inc/window.h"
#include "inc/QualityTextSystem.h"






#endif
