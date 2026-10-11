// Dynamic Lines Header
/*
Created by Christopher Jung 05.04.2026

This Header File Contains the Declorations for the Dynamic Lines Expansion Module.
This module is suposed to provide functions for more easyly drawing shapes.
*/

// Header Guard
#ifndef DYNAMIC_LINES_HEADER_GUARD
#define DYNAMIC_LINES_HEADER_GUARD

#include <stdlib.h>
#include <stdio.h>

#include <math.h>
#define _USE_MATH_DEFINES

#include <SDL3/SDL.h>                   // the core SDL libuary
#include <SDL3/SDL_main.h>              // part of SDL


#define PI_FLOAT 3.141592653589793238f

// Declared Functions
bool RenderLineDynamic(SDL_Renderer* renderer, int Ax, int Ay, int Bx, int By, int thikness); // Draws a dynamic line that has rounded edges and can have diffrent thiknesses
float parameterize_line_cord(float x, float val_A, float val_B); // Function for parameterising a cordinate in a line from value A to B; X is element from [0; 1].
int mirror_cord_to_point(int point, int mirror_point); // function for mirroring a cordinate componetnt to a point
bool RenderBezianCurve(SDL_Renderer* renderer, int Ax, int Ay, int Bx, int By, int Cx, int Cy, int thikness); // Function for rendering a bezian (quadratic) curve
bool RenderSpline(SDL_Renderer* renderer, int* ptr_points, int thikness); // Function for rendering a spline
bool RenderCircleSection(SDL_Renderer* renderer, int center_x, int center_y, int radius, float start_angle, float end_angle, int thikness); // Function for rendering circle sections (py definition)
bool RenderCircleSectionFilled(SDL_Renderer* renderer, int center_x, int center_y, int radius, float start_angle, float end_angle); // function for rendering a filled crcle section
bool RenderFillCircleQuater(SDL_Renderer* renderer, int center_x, int center_y, int radius, int quater);
bool RenderCircleQuater(SDL_Renderer* renderer, int center_x, int center_y, int radius, int quater);
bool GetLineOffsetPoint(float Ax, float Ay, float Bx, float By, float offset, float* ptr_point_x, float* ptr_point_y); // Function for getting a point on a line with an offset
bool RenderTeardrop_inclined(SDL_Renderer* renderer, float Start_x, float Start_y, float Start_radius, float End_x, float End_y, float End_radius); // Function for rendering a teardrop efect out from a circle
int RenderDrawCircle(SDL_Renderer * renderer, int x, int y, int radius); // Draws the outline of a circle
bool RenderFillCircle(SDL_Renderer * renderer, int x, int y, int radius); // Draws a filled circle
bool is_in_FRect(int x, int y, SDL_FRect* ptr_FRect); // Function for checking if a point is in a rectangle
bool is_in_Radius(int x, int y, int center_x, int center_y, int radius); // Function for checking if a point is in a circle (in range)

#endif
