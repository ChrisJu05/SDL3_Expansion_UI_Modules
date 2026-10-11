// Mini Animations Header
/*
Created by Christopher Jung 05.04.2026

This Header File Contains Declorations for the Mini animations Expansion Module.
This module is intended to provide tools for easyly implementing animations and
microinteractions in to a ui application.

Uses SDL3 and SDL3_image

Image Animated objects concist of two parts the MiAn_LoadedAnimation witch is
animation itself witch can be used for multiple animations on screen and the
MiAn_Animation witch represents an animation on screen and its behavior.

Value animations concist of just an animation status

Animation structures are allocated using dedicated functions and also get freed
with dedicated functions

The system also needs an initioalized time witch can be done like this
// get the time in ms since init was called
Uint32 time = SDL_GetTicks();
*/

// Header Guard
#ifndef MINI_ANIMATIONS_HEADER_GUARD
#define MINI_ANIMATIONS_HEADER_GUARD

#include <stdlib.h>
#include <stdio.h>

#include <math.h>
#define _USE_MATH_DEFINES

#include <SDL3/SDL.h>                   // the core SDL libuary
#include <SDL3/SDL_main.h>              // part of SDL
#include <SDL3_image/SDL_image.h>       // SDL3 extenion for rendering immages and animations


// Structure containing the Data for an animation like frame times and preloaded frame textures
// Represents the data for an animation but not an animation itself, so can be used for multiple of
// the same animation on screen each with their individual state represented by MiAn_Animation.
struct MiAn_LoadedAnimation{

  // SDL3 IMG animation
  IMG_Animation* ptr_animation;

  // Array of Texture pointers each representing a frame
  SDL_Texture** frame_texture;
};


// Represents the state of an animation directly, as well as its behavior
// the Animation obj is linked to a Loaded animation the drawing of an animation is done using the Loaded animation.
struct MiAn_Animation{

  // Pointer to the MiAn_LoadedAnimation where the frames come from
  struct MiAn_LoadedAnimation* ptr_Animation_data; // NOT OWNED BY MiAn_Animation

  // Current frame and Time of the last frame change for animating
  int cur_frame;
  Uint32 last_time;
  int targed_frame; // the frame to move to
  int last_frame;   // the frame that was displayed before this one

  // Crop and presentation rectangle
  SDL_FRect crop_rect;
  SDL_FRect pres_rect;

  // Primary behavior parameters
  bool active;    // animation is playing or has finished playing and is on its final frame
  bool loop;      // if true animation should play in loop (if backplay is true the animation will move back and forth)
  bool backplay;  // go through frames in reverse when deactivated or looping
  bool pause;     // if is paused the animation will stay on the current frame
  bool reverse;   // will make the animation act in reverse (including behavior)

};


// Value animation animates a set of values from a passive to a active value with definable animation Behavior
// Animation will follow a animation motion like linear or sigmoid motion (via movment_function)
// Value animation can be used for all sorts of animations like UI movments or collor changes , ...
struct MiAn_ValueAnimaton{

  // Values
  int Value_count;          // how manny values are being animated
  float *ptr_cur_values;    // the current values as of the last update
  float *ptr_active_values; // value for active state
  float *ptr_pasive_values; // value for passive state

  // the movment motion is done by parameterizing a movment via movment function
  // this defines the start, end and current parameter values
  // parmaeter values will move linearly between start and end
  // parameter Range (par_start < par_end)
  float par_start, par_end, cur_par;  //cur_par moves linearly with time
  float step_size;                    // for evry ms
  void *ptr_other_parameters;         // other animation parameters can be submitted via this void pointer witch will be submitted to the movment function

  // function for getting the curent values using cur_par
  // Movinf the value in the desiured way
  // returns cur_value, intput: parameter start value, parameter end value, passive value, active value, other parameters via void pointer
  float (*movment_function)(float, float, float, float, float, void*);

  // Targed state to animate of value animation
  bool active;

  // How long to chage from one collor to the other in miliseconds
  Uint32 animation_time;

  // Time of activation
  Uint32 last_time;
  float last_par_value;

  // Behavior definition
  bool loop;      // loop value animation if active
  bool backplay;  // reverse play the animation if active to passive
  bool pause;     // pauses the animation in its currnet state
  //bool is_backplaying;
  bool reverse;   // Makes the animation act in reverse

};


// Function Declorations
struct MiAn_LoadedAnimation* MiAn_LoadAnimation(SDL_Renderer* renderer, const char* path); // Function for Loading an Animation from a File
void MiAn_DestroyLoadedAnimation(struct MiAn_LoadedAnimation* ptr_MiAn_LoadedAnimation); // Function for safly destroying a MiAn_LoadedAnimation object
struct MiAn_Animation* MiAn_CreateAnimation(struct MiAn_LoadedAnimation* ptr_MiAn_LoadedAnimation); // Function for Creating a MiAn_Animation instance of a MiAn_LoadedAnimation
void MiAn_DestroyAnimation(struct MiAn_Animation* ptr_animated_obj); // Function for Destroying a MiAn_Animation instance
bool MiAn_RenderAnimation(SDL_Renderer* renderer, struct MiAn_Animation* ptr_MiAn_Animation, Uint32 time_now); // Function for Drawing the animation to the current render targed according to its state described by the MiAn_Animation and the given time
float MiAn_LinearMotion(float par_start_value, float par_cur_value, float par_end_value, float pasive_value, float active_value, void* other_parameters); // Function for generating a Linear value motion
float MiAn_SigmoidMotion(float par_start_value, float par_cur_value, float par_end_value, float pasive_value, float active_value, void* other_parameters); // Function for generating a Sigmoid value Motion
void MiAn_SetAnimationToPasiveValue(struct MiAn_ValueAnimaton *ptr_MiAn_Animation); // Function for setting the Value animation to its passive state (instantly)
void MiAn_SetAnimationToActiveValue(struct MiAn_ValueAnimaton *ptr_MiAn_Animation); // Function for setting the value animation in to its active state (instantly)
struct MiAn_ValueAnimaton* MiAn_CreateValueAnimation(int Value_count, float par_start, float par_end, void *ptr_other_par, Uint32 animation_time, float (*movment_function)(float, float, float, float, float, void*)); // Function for Creating a value animation structure in memmory
void MiAn_DestroyValueAnimation(struct MiAn_ValueAnimaton *Value_Animation); // Function for Freeing a value animation structure
bool SDL_UpdateValueAnmation(struct MiAn_ValueAnimaton *ptr_MiAn_Animation, Uint32 time_now); // Function for updating the value animation (only needs to be called if a change occurs) but can be called every frame


#endif
