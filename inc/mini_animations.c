// Mini Animatios module Code file
/*
Created by Christopher Jung 05.04.2026

This File Contains the Definitions for the Mini animations Expansion Module.
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


// Header file for working with this module
#include "mini_animations.h"


// Function for Loading an Animation from a File
/*
Loads an animation so that it can be deployed with MiAn_Animation after Loading
thereby the textures for the animation are Pregenerated.
renderer: the render context used with the subject textures.
path: a string path to the animation.
RETURNS a pointer to a new MiAn_LoadedAnimation structure for the animation or NULL on failure.
*/
struct MiAn_LoadedAnimation* MiAn_LoadAnimation(SDL_Renderer* renderer, const char* path){

  // Validate Input
  if(!renderer || !path) return NULL;

  // Alocate structure
  struct MiAn_LoadedAnimation* ptr_MiAn_LoadedAnimation = (struct MiAn_LoadedAnimation*)malloc(sizeof(struct MiAn_LoadedAnimation));

  // Load the animation using SDL3_image and put pointer in structure
  ptr_MiAn_LoadedAnimation->ptr_animation = IMG_LoadAnimation(path);

  // Error check
  if(!ptr_MiAn_LoadedAnimation->ptr_animation){
    // failed to load animation
    free(ptr_MiAn_LoadedAnimation);
    return NULL;
  }

  // Pregenerate the textures for the frames
  // allocate pointer array
  ptr_MiAn_LoadedAnimation->frame_texture = (SDL_Texture**)calloc(ptr_MiAn_LoadedAnimation->ptr_animation->count, sizeof(SDL_Texture*));

  // Pre Generate Frame Textures
  for(int i = 0; i < ptr_MiAn_LoadedAnimation->ptr_animation->count; i++){
    ptr_MiAn_LoadedAnimation->frame_texture[i] = SDL_CreateTextureFromSurface(renderer, ptr_MiAn_LoadedAnimation->ptr_animation->frames[i]);
  }

  // Return the created structure
  return ptr_MiAn_LoadedAnimation;
}





// Function for safly destroying a MiAn_LoadedAnimation object
// Note: an animation object can not runn without its MiAn_LoadedAnimation
void MiAn_DestroyLoadedAnimation(struct MiAn_LoadedAnimation* ptr_MiAn_LoadedAnimation){

  if(!ptr_MiAn_LoadedAnimation) return;

  // free the preloaded textures
  for(int i = 0; i < ptr_MiAn_LoadedAnimation->ptr_animation->count; i++){
    SDL_DestroyTexture(ptr_MiAn_LoadedAnimation->frame_texture[i]);
  }

  // free the animation
  IMG_FreeAnimation(ptr_MiAn_LoadedAnimation->ptr_animation);

  // Free the remaining structure
  free(ptr_MiAn_LoadedAnimation);

  return;
}





// Function for Creating a MiAn_Animation instance of a MiAn_LoadedAnimation
/*
requiures a MiAn_LoadedAnimation to make an animation.
Note: possition in presentation rectangle still needs to be set, behavior bools also need to be set if a behavior other than the default is wanted.
      MiAn_Animation can bee freed using a simple free function, as long as the MiAn_LoadedAnimation is considerd seperatly.
ptr_MiAn_LoadedAnimation: a pointer to a ptr_MiAn_LoadedAnimation containing the animation data
RETURNS a pointer to a new MiAn_Animation with default behavior settings.
*/
struct MiAn_Animation* MiAn_CreateAnimation(struct MiAn_LoadedAnimation* ptr_MiAn_LoadedAnimation){

  // Validate Input
  if(!ptr_MiAn_LoadedAnimation) return NULL;

  // Allocate the structure
  struct MiAn_Animation* ptr_MiAn_Animation = (struct MiAn_Animation*)calloc(1, sizeof(struct MiAn_Animation));
  if(!ptr_MiAn_Animation) return NULL;

  // Give animation source
  ptr_MiAn_Animation->ptr_Animation_data = ptr_MiAn_LoadedAnimation;

  // Get size of animation frames and set crop and presentation rectangle
  // crop rectangle
  ptr_MiAn_Animation->crop_rect.x = 0;
  ptr_MiAn_Animation->crop_rect.y = 0;
  ptr_MiAn_Animation->crop_rect.w = (float)ptr_MiAn_LoadedAnimation->frame_texture[0]->w;
  ptr_MiAn_Animation->crop_rect.h = (float)ptr_MiAn_LoadedAnimation->frame_texture[0]->h;

  // presentation rectangle
  ptr_MiAn_Animation->pres_rect.x = 0;
  ptr_MiAn_Animation->pres_rect.y = 0;
  ptr_MiAn_Animation->pres_rect.w = (float)ptr_MiAn_LoadedAnimation->frame_texture[0]->w;
  ptr_MiAn_Animation->pres_rect.h = (float)ptr_MiAn_LoadedAnimation->frame_texture[0]->h;

  // set target frame to default (last frame)
  ptr_MiAn_Animation->targed_frame = ptr_MiAn_Animation->ptr_Animation_data->ptr_animation->count - 1;
  ptr_MiAn_Animation->last_frame = ptr_MiAn_Animation->targed_frame;

  // Get the time
  ptr_MiAn_Animation->last_time = SDL_GetTicks();

  // Return the completed object
  return ptr_MiAn_Animation;
}




// Function for Destroying a MiAn_Animation instance
// Note: does not free MiAn_LoadedAnimation as that is a seperate component not owned by MiAn_Animation.
void MiAn_DestroyAnimation(struct MiAn_Animation* ptr_animated_obj){
  free(ptr_animated_obj);
}




// Function for Drawing the animation to the current render targed according to its state described by the MiAn_Animation and the given time
/*
renderer: the render context used with the subject textures.
ptr_MiAn_Animation: the animation instance to draw/render
time_now: the current time collected using Uint32 time = SDL_GetTicks();
RETURNS true on sucsess and false on failure.
*/
bool MiAn_RenderAnimation(SDL_Renderer* renderer, struct MiAn_Animation* ptr_MiAn_Animation, Uint32 time_now){

  // Validate Input
  if(!renderer || !ptr_MiAn_Animation) return false;

  // if is paused just draw the current frame
  if(ptr_MiAn_Animation->pause){
    // Draw the assinged frame number
    return SDL_RenderTexture(renderer, ptr_MiAn_Animation->ptr_Animation_data->frame_texture[ptr_MiAn_Animation->cur_frame],
    &(ptr_MiAn_Animation->crop_rect), &(ptr_MiAn_Animation->pres_rect));
  }

  // if it is not yet time to change frame draw the current frame
  if((time_now - ptr_MiAn_Animation->last_time) < (Uint32)(ptr_MiAn_Animation->ptr_Animation_data->ptr_animation->delays[ptr_MiAn_Animation->cur_frame])){
    // Draw the assinged frame number
    return SDL_RenderTexture(renderer, ptr_MiAn_Animation->ptr_Animation_data->frame_texture[ptr_MiAn_Animation->cur_frame],
    &(ptr_MiAn_Animation->crop_rect), &(ptr_MiAn_Animation->pres_rect));
  }

  // Frame must be changed
  int new_last_frame = ptr_MiAn_Animation->cur_frame;
  // ptr_MiAn_Animation->last_frame = ptr_MiAn_Animation->cur_frame;


  if(ptr_MiAn_Animation->backplay){
    // Complex behavior

    // Loop Test
    if(ptr_MiAn_Animation->loop){
      // Looping behavior: A

      // Reverse test
      if(ptr_MiAn_Animation->reverse){

        // Endpoint check (reverse)
        if(ptr_MiAn_Animation->cur_frame >= ptr_MiAn_Animation->targed_frame){

          // Active check
          if(ptr_MiAn_Animation->active){
            // start next loop by going one step
            ptr_MiAn_Animation->cur_frame -= 1;
          }else{
            // do nothing
          }

        }else if(ptr_MiAn_Animation->cur_frame <= 0){
          // if other end
          // reverse direction
          ptr_MiAn_Animation->cur_frame = 1;

        }else{
          // end point not reached
          // Forward_Backward check (note: reverse)
          if(ptr_MiAn_Animation->cur_frame >= ptr_MiAn_Animation->last_frame){
            // is backwarding
            ptr_MiAn_Animation->cur_frame += 1;
          }else{
            // is forwarding
            ptr_MiAn_Animation->cur_frame -= 1;
          }

        }// end of endpoint check
        // end of reverse
      }else{

        // Endpoint check (NON reverse)
        if(ptr_MiAn_Animation->cur_frame <= 0){

          // Active check
          if(ptr_MiAn_Animation->active){
            // start next loop by going one step
            ptr_MiAn_Animation->cur_frame = 1;
          }else{
            // do nothing
          }

        }else if(ptr_MiAn_Animation->cur_frame >= ptr_MiAn_Animation->targed_frame){
          //if other end was reache
          // reverse direction
          ptr_MiAn_Animation->cur_frame = ptr_MiAn_Animation->targed_frame - 1;

        }else{
          // end point not reached
          // Forward_Backward check (note: NON reverse)
          if(ptr_MiAn_Animation->cur_frame <= ptr_MiAn_Animation->last_frame){
            // is backwarding
            ptr_MiAn_Animation->cur_frame -= 1;
          }else{
            // is forwarding
            ptr_MiAn_Animation->cur_frame += 1;
          }

        }// end of endpoint check

      }// end of nonreverse


      // end of Looping behavior
    }else{
      // Non Looping behavior: B

      if(ptr_MiAn_Animation->active){
        // Active case B3 and B4
        if(ptr_MiAn_Animation->reverse){
          // Case B4
          // Endpoint 0 reached
          if(!(ptr_MiAn_Animation->cur_frame <= 0)){
            ptr_MiAn_Animation->cur_frame -= 1;
          }
        }else{
          // Case B3
          // Endpoint targed reached
          if(!(ptr_MiAn_Animation->cur_frame >= ptr_MiAn_Animation->targed_frame)){
            ptr_MiAn_Animation->cur_frame += 1;
          }
        }
      }else{
        // NON Active case B1 and B2
        if(ptr_MiAn_Animation->reverse){
          // Case B2
          // Endpoint targed reached
          if(!(ptr_MiAn_Animation->cur_frame >= ptr_MiAn_Animation->targed_frame)){
            ptr_MiAn_Animation->cur_frame += 1;
          }
        }else{
          // Case B1
          // Endpoint 0 reached
          if(!(ptr_MiAn_Animation->cur_frame <= 0)){
            ptr_MiAn_Animation->cur_frame -= 1;
          }
        }
      }
      // end of Non Looping behavior
    }


    // end of Complex behavior
  }else{
    // Simple behavejor

    //4 cases
    if(ptr_MiAn_Animation->active){
      // Cases 3 and 4
      if(ptr_MiAn_Animation->reverse){
        // Case 4
        // Endpoint check (reverse)
        if(ptr_MiAn_Animation->cur_frame <= 0){
          // Endpoint reached
          // Loop ckeck
          if(ptr_MiAn_Animation->loop){
            // Reset to targed frame
            ptr_MiAn_Animation->cur_frame = ptr_MiAn_Animation->targed_frame;
          }else{
            // stay on frame
          }
        }else{
          // Endpoint NOT reached
          ptr_MiAn_Animation->cur_frame -= 1;
        }

      }else{
        // Case 3
        // Endpoint check (non reverse)
        if(ptr_MiAn_Animation->cur_frame >= ptr_MiAn_Animation->targed_frame){
          // Endpoint reached
          // Loop ckeck
          if(ptr_MiAn_Animation->loop){
            // Reset to first frame
            ptr_MiAn_Animation->cur_frame = 0;
          }else{
            // stay on frame
          }
        }else{
          // Endpoint NOT reached
          ptr_MiAn_Animation->cur_frame += 1;
        }

      }
    }else{
      // cases 1 and 2
      if(ptr_MiAn_Animation->reverse){
        // Case 2
        // Set to targed frame
        ptr_MiAn_Animation->cur_frame = ptr_MiAn_Animation->targed_frame;
      }else{
        // Case 1
        // set to first frame
        ptr_MiAn_Animation->cur_frame = 0;
      }
    }

  // end of Simple behavejor
  }

  // in anny case the time needs to be set if the frame was to be changed
  ptr_MiAn_Animation->last_time = time_now;
  ptr_MiAn_Animation->last_frame = new_last_frame;


  // Draw the assinged frame number
  return SDL_RenderTexture(renderer, ptr_MiAn_Animation->ptr_Animation_data->frame_texture[ptr_MiAn_Animation->cur_frame],
                            &(ptr_MiAn_Animation->crop_rect), &(ptr_MiAn_Animation->pres_rect));

}// end of MiAn_RenderAnimation function






// Function for generating a Linear value motion
// This motion moves directly from start value to end value in a linear motion
float MiAn_LinearMotion(float par_start_value, float par_cur_value, float par_end_value, float pasive_value, float active_value, void* other_parameters){

  // Tell compiler that other_parameters is unused
  (void)(other_parameters);

  if(par_cur_value <= par_start_value) return pasive_value;
  if(par_cur_value >= par_end_value) return active_value;

  float par_range = par_end_value - par_start_value;

  return (1.0f / par_range) * (par_cur_value - par_start_value) * (active_value - pasive_value) + pasive_value;

}





// Function for generating a Sigmoid value Motion
/*
This motion moves smothly and gental from start value to end value
parameter from -5 to 5 is optimal
Note a sigmoid never reaches its targed, jut 99,9% there.
*/
float MiAn_SigmoidMotion(float par_start_value, float par_cur_value, float par_end_value, float pasive_value, float active_value, void* other_parameters){

  // Tell compiler that other_parameters is unused
  (void)(other_parameters);
  
  if(par_cur_value <= par_start_value) return pasive_value;
  if(par_cur_value >= par_end_value) return active_value;

  //float par_range = par_end_value - par_start_value;

  // t (0,1) = (1.0f / par_range) * (par_cur_value - par_start_value)
  // f(x) = 1.0f / (1 + expf(-4x))
  // m = f(2t-1) with x = t
  // return = m * (active_value - pasive_value) + pasive_value

  return (1.0f / (1.0f + expf(-1.0f * par_cur_value))) * (active_value - pasive_value) + pasive_value;
}





// Function for setting the Value animation to its passive state (instantly)
// recomended to call to initialize the animation to passive value
void MiAn_SetAnimationToPasiveValue(struct MiAn_ValueAnimaton *ptr_MiAn_Animation){

  // Validate Input
  if(!ptr_MiAn_Animation) return;

  // Set each value to passive
  for(int i = 0; i < ptr_MiAn_Animation->Value_count; i++){
    ptr_MiAn_Animation->ptr_cur_values[i] = ptr_MiAn_Animation->ptr_pasive_values[i];
  }

  // set parameter value
  ptr_MiAn_Animation->cur_par = ptr_MiAn_Animation->par_start;

  return;
}



// Function for setting the value animation in to its active state (instantly)
// recomonded to call to initioalize the animation to its active value
void MiAn_SetAnimationToActiveValue(struct MiAn_ValueAnimaton *ptr_MiAn_Animation){

  // Validate Input
  if(!ptr_MiAn_Animation) return;

  // Set each value to active value
  for(int i = 0; i < ptr_MiAn_Animation->Value_count; i++){
    ptr_MiAn_Animation->ptr_cur_values[i] = ptr_MiAn_Animation->ptr_active_values[i];
  }

  // Set parameter value
  ptr_MiAn_Animation->cur_par = ptr_MiAn_Animation->par_end;

  return;
}





// Function for Creating a value animation structure in memmory
/*
a movment function should be given or the update animation function will fail, as a default use MiAn_LinearMotion or MiAn_SigmoidMotion.
Note: active and passive values still need to be enterd.
Value_count: How manny values are being animated with this instance (these values will share one animation state).
par_start: parametrization start value (is used with the movment_function to animate the values).
par_end: parametrization end value (is used with the movment_function to animate the values).
animation_time: how long the animation is in milliseconds.
movment_function: a movment function used to animate the values (typicaly MiAn_SigmoidMotion or MiAn_LinearMotion but a custom function can also be used).
RETURNS a pointer to a new MiAn_ValueAnimaton on sucsess and NULL on failure.
*/
struct MiAn_ValueAnimaton* MiAn_CreateValueAnimation(int Value_count, float par_start, float par_end, void *ptr_other_par, Uint32 animation_time, float (*movment_function)(float, float, float, float, float, void*)){

  // allocate structure itself
  struct MiAn_ValueAnimaton *ptr_ValueAnimation = (struct MiAn_ValueAnimaton*)malloc(sizeof(struct MiAn_ValueAnimaton));
  if(!ptr_ValueAnimation) return NULL;

  // Assing values
  ptr_ValueAnimation->Value_count = Value_count;

  ptr_ValueAnimation->ptr_cur_values = (float*)calloc(Value_count, sizeof(float));
  if(!(ptr_ValueAnimation->ptr_cur_values)) return NULL;

  ptr_ValueAnimation->ptr_active_values = (float*)calloc(Value_count, sizeof(float));
  if(!(ptr_ValueAnimation->ptr_active_values)) return NULL;

  ptr_ValueAnimation->ptr_pasive_values = (float*)calloc(Value_count, sizeof(float));
  if(!(ptr_ValueAnimation->ptr_pasive_values)) return NULL;

  ptr_ValueAnimation->par_start = par_start;
  ptr_ValueAnimation->par_end = par_end;
  ptr_ValueAnimation->cur_par = par_start;
  ptr_ValueAnimation->ptr_other_parameters = ptr_other_par;
  ptr_ValueAnimation->last_par_value = par_start;
  ptr_ValueAnimation->animation_time = animation_time;
  ptr_ValueAnimation->last_time = 0;

  // Assing movment function
  ptr_ValueAnimation->movment_function = movment_function;

  ptr_ValueAnimation->active = false;
  ptr_ValueAnimation->loop = false;
  ptr_ValueAnimation->backplay = false;
  ptr_ValueAnimation->pause = false;
  ptr_ValueAnimation->reverse = false;

  ptr_ValueAnimation->step_size = (ptr_ValueAnimation->par_end - ptr_ValueAnimation->par_start) / ((float)animation_time);
  //printf("step siue %f \n", ptr_ValueAnimation->step_size);
  return ptr_ValueAnimation;
}




// Function for Freeing a value animation structure
// (asuming it was allocated with the MiAn_CreateValueAnimation function)
void MiAn_DestroyValueAnimation(struct MiAn_ValueAnimaton *Value_Animation){

  // Validate Input
  if(!Value_Animation) return;

  free(Value_Animation->ptr_cur_values);
  free(Value_Animation->ptr_active_values);
  free(Value_Animation->ptr_pasive_values);

  free(Value_Animation);
}




// Function for updating the value animation (only needs to be called if a change occurs) but can be called every frame
/*
will update the animation acording to its set behavior and state.
ptr_MiAn_Animation: the MiAn_ValueAnimaton to be updated.
time_now: the current time collected using Uint32 time = SDL_GetTicks();
RETURNS true on sucsess and false on failure.
*/
bool SDL_UpdateValueAnmation(struct MiAn_ValueAnimaton *ptr_MiAn_Animation, Uint32 time_now){

  if(!ptr_MiAn_Animation) return false;
  if(!(ptr_MiAn_Animation->movment_function)) return false;

  // if is paused do nothing
  if(ptr_MiAn_Animation->pause) return true;

  // store for later
  float new_last_par_value = ptr_MiAn_Animation->cur_par;

  // calculate elapsed time (in ms)
  Uint32 elapsed_time = time_now - ptr_MiAn_Animation->last_time;



  if(ptr_MiAn_Animation->backplay){
    // Complex behavior

    // Loop Test
    if(ptr_MiAn_Animation->loop){
      // Looping behavior: A

      // Reverse test
      if(ptr_MiAn_Animation->reverse){

        // Endpoint check (reverse)
        //if(ptr_MiAn_Animation->cur_frame >= ptr_MiAn_Animation->targed_frame){
        if(ptr_MiAn_Animation->cur_par >= ptr_MiAn_Animation->par_end){

          // Active check
          if(ptr_MiAn_Animation->active){
            // start next loop by going one step
            //ptr_MiAn_Animation->cur_frame -= 1;
            ptr_MiAn_Animation->cur_par -= (ptr_MiAn_Animation->step_size * (float)elapsed_time);
          }else{
            // do nothing
          }

        //}else if(ptr_MiAn_Animation->cur_frame <= 0){
        }else if(ptr_MiAn_Animation->cur_par <= ptr_MiAn_Animation->par_start){
          // if other end
          // reverse direction
          //ptr_MiAn_Animation->cur_frame = 1;
          ptr_MiAn_Animation->cur_par = (ptr_MiAn_Animation->step_size * (float)elapsed_time) + ptr_MiAn_Animation->par_start;

        }else{
          // end point not reached
          // Forward_Backward check (note: reverse)
          //if(ptr_MiAn_Animation->cur_frame >= ptr_MiAn_Animation->last_frame){
          if(ptr_MiAn_Animation->cur_par >= ptr_MiAn_Animation->last_par_value){
            // is backwarding
            //ptr_MiAn_Animation->cur_frame += 1;
            ptr_MiAn_Animation->cur_par += (ptr_MiAn_Animation->step_size * (float)elapsed_time);
          }else{
            // is forwarding
            //ptr_MiAn_Animation->cur_frame -= 1;
            ptr_MiAn_Animation->cur_par -= (ptr_MiAn_Animation->step_size * (float)elapsed_time);
          }

        }// end of endpoint check
        // end of reverse
      }else{

        // Endpoint check (NON reverse)
        //if(ptr_MiAn_Animation->cur_frame <= 0){
        if(ptr_MiAn_Animation->cur_par <= ptr_MiAn_Animation->par_start){

          // Active check
          if(ptr_MiAn_Animation->active){
            // start next loop by going one step
            //ptr_MiAn_Animation->cur_frame = 1;
            ptr_MiAn_Animation->cur_par = (ptr_MiAn_Animation->step_size * (float)elapsed_time) + ptr_MiAn_Animation->par_start;
          }else{
            // do nothing
          }

        //}else if(ptr_MiAn_Animation->cur_frame >= ptr_MiAn_Animation->targed_frame){
        }else if(ptr_MiAn_Animation->cur_par >= ptr_MiAn_Animation->par_end){
          //if other end was reache
          // reverse direction
          //ptr_MiAn_Animation->cur_frame = ptr_MiAn_Animation->targed_frame - 1;
          ptr_MiAn_Animation->cur_par = ptr_MiAn_Animation->par_end - (ptr_MiAn_Animation->step_size * (float)elapsed_time);

        }else{
          // end point not reached
          // Forward_Backward check (note: NON reverse)
          //if(ptr_MiAn_Animation->cur_frame <= ptr_MiAn_Animation->last_frame){
          if(ptr_MiAn_Animation->cur_par <= ptr_MiAn_Animation->last_par_value){
            // is backwarding
            //ptr_MiAn_Animation->cur_frame -= 1;
            ptr_MiAn_Animation->cur_par -= (ptr_MiAn_Animation->step_size * (float)elapsed_time);
          }else{
            // is forwarding
            //ptr_MiAn_Animation->cur_frame += 1;
            ptr_MiAn_Animation->cur_par += (ptr_MiAn_Animation->step_size * (float)elapsed_time);
          }

        }// end of endpoint check

      }// end of nonreverse


      // end of Looping behavior
    }else{
      // Non Looping behavior: B

      if(ptr_MiAn_Animation->active){
        // Active case B3 and B4
        if(ptr_MiAn_Animation->reverse){
          // Case B4
          // Endpoint 0 reached
          //if(!(ptr_MiAn_Animation->cur_frame <= 0)){
          if(!(ptr_MiAn_Animation->cur_par <= ptr_MiAn_Animation->par_start)){
            //ptr_MiAn_Animation->cur_frame -= 1;
            ptr_MiAn_Animation->cur_par -= (ptr_MiAn_Animation->step_size * (float)elapsed_time);
          }
        }else{
          // Case B3
          // Endpoint targed reached
          //if(!(ptr_MiAn_Animation->cur_frame >= ptr_MiAn_Animation->targed_frame)){
          if(!(ptr_MiAn_Animation->cur_par >= ptr_MiAn_Animation->par_end)){
            //ptr_MiAn_Animation->cur_frame += 1;
            ptr_MiAn_Animation->cur_par += (ptr_MiAn_Animation->step_size * (float)elapsed_time);
          }
        }
      }else{
        // NON Active case B1 and B2
        if(ptr_MiAn_Animation->reverse){
          // Case B2
          // Endpoint targed reached
          //if(!(ptr_MiAn_Animation->cur_frame >= ptr_MiAn_Animation->targed_frame)){
          if(!(ptr_MiAn_Animation->cur_par >= ptr_MiAn_Animation->par_end)){
            //ptr_MiAn_Animation->cur_frame += 1;
            ptr_MiAn_Animation->cur_par += (ptr_MiAn_Animation->step_size * (float)elapsed_time);
          }
        }else{
          // Case B1
          // Endpoint 0 reached
          //if(!(ptr_MiAn_Animation->cur_frame <= 0)){
          if(!(ptr_MiAn_Animation->cur_par <= ptr_MiAn_Animation->par_start)){
            //ptr_MiAn_Animation->cur_frame -= 1;
            ptr_MiAn_Animation->cur_par -= (ptr_MiAn_Animation->step_size * (float)elapsed_time);
          }
        }
      }
      // end of Non Looping behavior
    }


    // end of Complex behavior
  }else{
    // Simple behavejor

    //4 cases
    if(ptr_MiAn_Animation->active){
      // Cases 3 and 4
      if(ptr_MiAn_Animation->reverse){
        // Case 4
        // Endpoint check (reverse)
        //if(ptr_MiAn_Animation->cur_frame <= 0){
        if(ptr_MiAn_Animation->cur_par <= ptr_MiAn_Animation->par_start){
          // Endpoint reached
          // Loop ckeck
          if(ptr_MiAn_Animation->loop){
            // Reset to targed frame
            //ptr_MiAn_Animation->cur_frame = ptr_MiAn_Animation->targed_frame;
            ptr_MiAn_Animation->cur_par = ptr_MiAn_Animation->par_end;
          }else{
            // stay on frame
          }
        }else{
          // Endpoint NOT reached
          //ptr_MiAn_Animation->cur_frame -= 1;
          ptr_MiAn_Animation->cur_par -= (ptr_MiAn_Animation->step_size * (float)elapsed_time);
        }

      }else{
        // Case 3
        // Endpoint check (non reverse)
        //if(ptr_MiAn_Animation->cur_frame >= ptr_MiAn_Animation->targed_frame){
        if(ptr_MiAn_Animation->cur_par >= ptr_MiAn_Animation->par_end){
          // Endpoint reached
          // Loop ckeck
          if(ptr_MiAn_Animation->loop){
            // Reset to first frame
            //ptr_MiAn_Animation->cur_frame = 0;
            ptr_MiAn_Animation->cur_par = ptr_MiAn_Animation->par_start;
          }else{
            // stay on frame
          }
        }else{
          // Endpoint NOT reached
          //ptr_MiAn_Animation->cur_frame += 1;
          ptr_MiAn_Animation->cur_par += (ptr_MiAn_Animation->step_size * (float)elapsed_time);
        }

      }
    }else{
      // cases 1 and 2
      if(ptr_MiAn_Animation->reverse){
        // Case 2
        // Set to targed frame
        //ptr_MiAn_Animation->cur_frame = ptr_MiAn_Animation->targed_frame;
        ptr_MiAn_Animation->cur_par = ptr_MiAn_Animation->par_end;
      }else{
        // Case 1
        // set to first frame
        //ptr_MiAn_Animation->cur_frame = 0;
        ptr_MiAn_Animation->cur_par = ptr_MiAn_Animation->par_start;
      }
    }

  // end of Simple behavejor
  }

  // in case of overstepping
  if(ptr_MiAn_Animation->cur_par > ptr_MiAn_Animation->par_end) ptr_MiAn_Animation->cur_par = ptr_MiAn_Animation->par_end;
  if(ptr_MiAn_Animation->cur_par < ptr_MiAn_Animation->par_start) ptr_MiAn_Animation->cur_par = ptr_MiAn_Animation->par_start;

  // Update all values using dedicated function
  for(int i = 0; i < ptr_MiAn_Animation->Value_count; i++){
    ptr_MiAn_Animation->ptr_cur_values[i] = ptr_MiAn_Animation->movment_function(ptr_MiAn_Animation->par_start, ptr_MiAn_Animation->cur_par, ptr_MiAn_Animation->par_end,
                                          ptr_MiAn_Animation->ptr_pasive_values[i], ptr_MiAn_Animation->ptr_active_values[i], ptr_MiAn_Animation->ptr_other_parameters);
  }


  // in anny case the time needs to be set if the frame was to be changed
  ptr_MiAn_Animation->last_time = time_now;
  ptr_MiAn_Animation->last_par_value = new_last_par_value;

  return true;

}

// END
