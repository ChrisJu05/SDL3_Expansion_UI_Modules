// Dynamic Lines module Code file
/*
Created by Christopher Jung 05.04.2026

This File Contains the Definitions for the Dynamic Lines Expansion Module.
This module is suposed to provide functions for more easyly drawing shapes.
*/

#include "dynamic_lines.h"


// Draws the outline of a circle
int RenderDrawCircle(SDL_Renderer * renderer, int x, int y, int radius)
{
    int offsetx, offsety, d;
    int status;

    //CHECK_RENDERER_MAGIC(renderer, -1);

    offsetx = 0;
    offsety = radius;
    d = radius -1;
    status = 0;

    while (offsety >= offsetx) {
        status += SDL_RenderPoint(renderer, x + offsetx, y + offsety);
        status += SDL_RenderPoint(renderer, x + offsety, y + offsetx);
        status += SDL_RenderPoint(renderer, x - offsetx, y + offsety);
        status += SDL_RenderPoint(renderer, x - offsety, y + offsetx);
        status += SDL_RenderPoint(renderer, x + offsetx, y - offsety);
        status += SDL_RenderPoint(renderer, x + offsety, y - offsetx);
        status += SDL_RenderPoint(renderer, x - offsetx, y - offsety);
        status += SDL_RenderPoint(renderer, x - offsety, y - offsetx);

        if (status < 0) {
            status = -1;
            break;
        }

        if (d >= 2*offsetx) {
            d -= 2*offsetx + 1;
            offsetx +=1;
        }
        else if (d < 2 * (radius - offsety)) {
            d += 2 * offsety - 1;
            offsety -= 1;
        }
        else {
            d += 2 * (offsety - offsetx - 1);
            offsety -= 1;
            offsetx += 1;
        }
    }

    return status;
}


// Draws a filled circle
bool RenderFillCircle(SDL_Renderer * renderer, int x, int y, int radius)
{

    int offsetx = 0;
    int offsety = radius;
    int d = radius - 1;
    bool status = true;

    while (offsety >= offsetx) {
        // SDL3 rendering functions strictly take float coordinates.
        // We cast our integer coordinates to floats to suppress compiler warnings.

        // Draw the top and bottom octants
        status &= SDL_RenderLine(renderer, (float)(x - offsety), (float)(y - offsetx),
                                           (float)(x + offsety), (float)(y - offsetx));
        status &= SDL_RenderLine(renderer, (float)(x - offsety), (float)(y + offsetx),
                                           (float)(x + offsety), (float)(y + offsetx));

        // Draw the left and right octants
        status &= SDL_RenderLine(renderer, (float)(x - offsetx), (float)(y - offsety),
                                           (float)(x + offsetx), (float)(y - offsety));
        status &= SDL_RenderLine(renderer, (float)(x - offsetx), (float)(y + offsety),
                                           (float)(x + offsetx), (float)(y + offsety));

        // Midpoint algorithm core
        if (d >= 2 * offsetx) {
            d -= 2 * offsetx + 1;
            offsetx += 1;
        } else if (d < 2 * (radius - offsety)) {
            d += 2 * offsety - 1;
            offsety -= 1;
        } else {
            d += 2 * (offsety - offsetx - 1);
            offsety -= 1;
            offsetx += 1;
        }
    }

    return status;
}



// Draws a dynamic line that has rounded edges and can have diffrent thiknesses
// returns true on sucsess and false on fail
bool RenderLineDynamic(SDL_Renderer* renderer, int Ax, int Ay, int Bx, int By, int thikness){

  // Validate Input
  if(!renderer) return false;

  // Fallback to standard fast line drawing for thin lines
    if (thikness <= 1) {
        return SDL_RenderLine(renderer, (float)Ax, (float)Ay, (float)Bx, (float)By);
    }

    int radius = thikness / 2;

    // If start and end points are identical, the line is just a single dot
    if (Ax == Bx && Ay == By) {
        return RenderFillCircle(renderer, Ax, Ay, radius);
    }

    bool status = true;

    // 1. Draw the rounded end caps
    status &= RenderFillCircle(renderer, Ax, Ay, radius);
    status &= RenderFillCircle(renderer, Bx, By, radius);

    // 2. Calculate the rotated rectangular body
    float dx = (float)(Bx - Ax);
    float dy = (float)(By - Ay);
    float length = sqrtf(dx * dx + dy * dy);

    // Calculate normalized perpendicular vectors to shift the rectangle edges outward
    float nx = -dy / length;
    float ny = dx / length;

    float fRadius = (float)radius; // Match the integer radius of the circle caps
    float offsetX = nx * fRadius;
    float offsetY = ny * fRadius;

    // Fetch the current render state color to apply to our geometry vertices
    float r, g, b, a;
    SDL_GetRenderDrawColorFloat(renderer, &r, &g, &b, &a);
    SDL_FColor color = { r, g, b, a };

    // 3. Define the 4 corners of the quad
    SDL_Vertex verts[4];

    // Top-Left (relative to line trajectory)
    verts[0].position.x = Ax + offsetX;
    verts[0].position.y = Ay + offsetY;
    verts[0].color = color;

    // Bottom-Left
    verts[1].position.x = Ax - offsetX;
    verts[1].position.y = Ay - offsetY;
    verts[1].color = color;

    // Top-Right
    verts[2].position.x = Bx + offsetX;
    verts[2].position.y = By + offsetY;
    verts[2].color = color;

    // Bottom-Right
    verts[3].position.x = Bx - offsetX;
    verts[3].position.y = By - offsetY;
    verts[3].color = color;

    // 4. Render the two triangles that make up the rectangle body
    const int indices[6] = { 0, 1, 2, 1, 3, 2 };
    status &= SDL_RenderGeometry(renderer, NULL, verts, 4, indices, 6);

    return status;
}


// Function for parameterising a cordinate in a line from value A to B; X is element from [0; 1].
float parameterize_line_cord(float x, float val_A, float val_B){
  return (x * (val_B - val_A)) + val_A;
}


// function for mirroring a cordinate componetnt to a point
int mirror_cord_to_point(int point, int mirror_point){
  return ((point - mirror_point) * -1 ) + mirror_point;
}




// Function for rendering a bezian (quadratic) curve
// a bezian curve requiures 3 points (start point, connecting point, endpoint)
// returns true on sucsess and false on fail
bool RenderBezianCurve(SDL_Renderer* renderer, int Ax, int Ay, int Bx, int By, int Cx, int Cy, int thikness){

  // Validate Input
  if(!renderer || thikness <= 0) return false;
  /*
  bool status = true;

    // Estimate the length of the curve by summing the distances between the control points.
    // This allows us to dynamically adjust the curve's resolution based on its size.
    float distAB = sqrtf((float)((Bx - Ax) * (Bx - Ax) + (By - Ay) * (By - Ay)));
    float distBC = sqrtf((float)((Cx - Bx) * (Cx - Bx) + (Cy - By) * (Cy - By)));
    float approx_length = distAB + distBC;

    // Determine the number of segments.
    // Roughly 1 segment per 5 pixels provides a smooth look while keeping GPU overhead low.
    int num_segments = (int)(approx_length / 5.0f);
    if (num_segments < 10) num_segments = 10; // Enforce a minimum resolution

    float t_step = 1.0f / (float)num_segments;

    // Start at the origin point (A)
    float prev_x = (float)Ax;
    float prev_y = (float)Ay;

    for (int i = 1; i <= num_segments; i++) {
        float t = (float)i * t_step;
        float u = 1.0f - t;

        // Quadratic Bezier formula: B(t) = (1-t)^2*P0 + 2*(1-t)*t*P1 + t^2*P2
        float curr_x = (u * u * Ax) + (2.0f * u * t * Bx) + (t * t * Cx);
        float curr_y = (u * u * Ay) + (2.0f * u * t * By) + (t * t * Cy);

        // Render the current segment
        status &= RenderLineDynamic(renderer,
                                    (int)roundf(prev_x), (int)roundf(prev_y),
                                    (int)roundf(curr_x), (int)roundf(curr_y),
                                    thikness);

        prev_x = curr_x;
        prev_y = curr_y;
    }

    return status;
    */

    bool status = true;

    // 1. Dynamic Resolution: Calculate rough length using Manhattan distance
    // of the control polygon to avoid an expensive square root.
    float approx_length = fabsf((float)(Ax - Bx)) + fabsf((float)(Ay - By)) +
                          fabsf((float)(Bx - Cx)) + fabsf((float)(By - Cy));

    // Assign roughly 1 line segment per 10 pixels of curve length
    int segments = (int)(approx_length / 10.0f);
    if (segments < 10) segments = 10;
    if (segments > 100) segments = 100; // Cap to prevent performance hits on huge curves

    float step = 1.0f / (float)segments;
    int last_x = Ax;
    int last_y = Ay;

    // 2. Step through the curve parameter 't' from 0.0 to 1.0
    for (int i = 1; i <= segments; ++i) {
        float t = i * step;
        float u = 1.0f - t;

        // Quadratic Bezier interpolation polynomial
        // P(t) = (1-t)^2 * A + 2(1-t)t * B + t^2 * C
        float u2 = u * u;
        float t2 = t * t;
        float ut2 = 2.0f * u * t;

        int curr_x = (int)roundf(u2 * Ax + ut2 * Bx + t2 * Cx);
        int curr_y = (int)roundf(u2 * Ay + ut2 * By + t2 * Cy);

        // Draw the thick line segment; the rounded caps will smooth the joints automatically
        status &= RenderLineDynamic(renderer, last_x, last_y, curr_x, curr_y, thikness);

        last_x = curr_x;
        last_y = curr_y;
    }

    return status;
}




// Function for rendering a spline
// A spline requiures 4 points (start point, midalingment point, midpoint, endpoint)
bool RenderSpline(SDL_Renderer* renderer, int* ptr_points, int thikness){
  // A normal spline is just two bezian curves connected together

  bool ret;

  // render first bezian curve
  ret = RenderBezianCurve(renderer, ptr_points[0], ptr_points[1], ptr_points[2], ptr_points[3], ptr_points[4], ptr_points[5], thikness);

  if(!ret) return false;

  // Render second Bezian curve by mirroring midalingment point over the midpoint
  ret = RenderBezianCurve(renderer, ptr_points[4], ptr_points[5],
                        mirror_cord_to_point(ptr_points[2], ptr_points[4]),
                        mirror_cord_to_point(ptr_points[3], ptr_points[5]),
                        ptr_points[6], ptr_points[7], thikness);
  return ret;
}






// Function for rendering circle sections (py definition)
// sections are in multiples of py and should not be negative (you can use PI_FLOAT to multiply with py)
// return true on sucsess and false on fail
bool RenderCircleSection(SDL_Renderer* renderer, int center_x, int center_y, int radius, float start_angle, float end_angle, int thikness){

  // Validate Input
  if(!renderer) return false;

  // set parameters
  float stepping_distance = 1 / (2 * radius * PI_FLOAT);
  //float current_angle = start_angle;
  int rad_thikness = thikness / 2;

  float true_start_angle = start_angle;
  float true_end_angle = end_angle;

  // if end angle is less thatn start angle go once around circle
  if(end_angle < start_angle){
    //end_angle += PI_FLOAT * 2;
    true_start_angle = end_angle;
    true_end_angle = start_angle;
  }
  float current_angle = true_start_angle;

  // if thikness is to low use points instead of circles
  if(thikness > 1){
    while(current_angle <= true_end_angle){
      // draw a circle at radius * sin(angle) + center ...(same with cos)
      // X: (radius * cosf(current_angle)) + center_x
      // Y: (radius * sinf(current_angle)) + center_y
      RenderFillCircle(renderer, (radius * cosf(current_angle)) + center_x, -(radius * sinf(current_angle)) + center_y, rad_thikness);

      current_angle += stepping_distance;

    }
  }else{
    while(current_angle <= true_end_angle){
      // draw a circle at radius * sin(angle) + center ...(same with cos)
      // X: (radius * cosf(current_angle)) + center_x
      // Y: (radius * sinf(current_angle)) + center_y
      SDL_RenderPoint(renderer, (radius * cosf(current_angle)) + center_x, -(radius * sinf(current_angle)) + center_y);

      current_angle += stepping_distance;
    }
  }

  return true;
}

// Function for rendering circle sections filled (py definition)
// sections are in multiples of py and should not be negative (you can use PI_FLOAT to multiply with py)
// return true on sucsess and false on fail
bool RenderCircleSectionFilled(SDL_Renderer* renderer, int center_x, int center_y, int radius, float start_angle, float end_angle){

  // Validate Input
  if(!renderer) return false;

  // set parameters
  float stepping_distance = 1 / (2 * radius * PI_FLOAT);
  //float current_angle = start_angle;
  //int rad_thikness = thikness / 2;

  float true_start_angle = start_angle;
  float true_end_angle = end_angle;

  // if end angle is less thatn start angle go once around circle
  if(end_angle < start_angle){
    //end_angle += PI_FLOAT * 2;
    true_start_angle = end_angle;
    true_end_angle = start_angle;
  }
  float current_angle = true_start_angle;

  while(current_angle <= true_end_angle){
    // draw a circle at radius * sin(angle) + center ...(same with cos)
    // X: (radius * cosf(current_angle)) + center_x
    // Y: (radius * sinf(current_angle)) + center_y
    //RenderFillCircle(renderer, (radius * cosf(current_angle)) + center_x, -(radius * sinf(current_angle)) + center_y, rad_thikness);
    SDL_RenderLine(renderer, (float)center_x, (float)center_y, (radius * cosf(current_angle)) + (float)center_x, -(radius * sinf(current_angle)) + (float)center_y);

    current_angle += stepping_distance;

  }

  return true;
}



// Function for rendering a quater of a circle (filled)
bool RenderFillCircleQuater(SDL_Renderer* renderer, int center_x, int center_y, int radius, int quater){

  // Veryfy input
  if(!renderer) return false;

  // Help variables
  float angle;
  float length;
  float f_radius = (float)radius;

  for(int i = 0; i < radius; i++){
    // Calculate length of line
    angle = acosf((float)i / f_radius);
    length = sinf(angle) * f_radius;

    // Draw line
    //SDL_RenderLine(renderer, (float)center_x + i, (float)center_y, (float)center_x + i, (float)center_y + length);

    switch(quater){
      case 1:
        SDL_RenderLine(renderer, (float)center_x + i, (float)center_y, (float)center_x + i, (float)center_y - length);
        break;

      case 2:
        SDL_RenderLine(renderer, (float)center_x - i, (float)center_y, (float)center_x - i, (float)center_y - length);
        break;

      case 3:
        SDL_RenderLine(renderer, (float)center_x - i, (float)center_y, (float)center_x - i, (float)center_y + length);
        break;

      case 4:
        SDL_RenderLine(renderer, (float)center_x + i, (float)center_y, (float)center_x + i, (float)center_y + length);
        break;
      default:
        SDL_RenderLine(renderer, (float)center_x + i, (float)center_y, (float)center_x + i, (float)center_y - length);
        break;
    }
  }

  return true;
}





// Function for rendering a quater of a circle (only the edg of the circle)
bool RenderCircleQuater(SDL_Renderer* renderer, int center_x, int center_y, int radius, int quater){

  // Veryfy input
  if(!renderer) return false;

  // Help variables
  float angle;
  float hight;
  float prev_hight = 0.0f;
  float f_radius = (float)radius;

  for(int i = radius - 1; i >= 0; i--){
    // Calculate hight of line
    angle = acosf((float)i / f_radius);
    hight = sinf(angle) * f_radius;
    //if(length < 1.0f) length = 1.0f;

    // Draw line
    //SDL_RenderLine(renderer, (float)center_x + i, (float)center_y, (float)center_x + i, (float)center_y + hight);

    switch(quater){
      case 1:
        SDL_RenderLine(renderer, (float)center_x + i, (float)center_y - prev_hight, (float)center_x + i, (float)center_y - hight);
        break;

      case 2:
        SDL_RenderLine(renderer, (float)center_x - i, (float)center_y - prev_hight, (float)center_x - i, (float)center_y - hight);
        break;

      case 3:
        SDL_RenderLine(renderer, (float)center_x - i, (float)center_y + prev_hight, (float)center_x - i, (float)center_y + hight);
        break;

      case 4:
        SDL_RenderLine(renderer, (float)center_x + i, (float)center_y + prev_hight, (float)center_x + i, (float)center_y + hight);
        break;
      default:
        SDL_RenderLine(renderer, (float)center_x + i, (float)center_y - prev_hight, (float)center_x + i, (float)center_y - hight);
        break;
    }
    // set new previus hight
    prev_hight = hight;
  }

  return true;
}



// Function for getting a point on a line with an offset
// Line goes from A to B with the point being offset far away from B
// Point B is the one with an offset
// B is the point with the offset
bool GetLineOffsetPoint(float Ax, float Ay, float Bx, float By, float offset, float* ptr_point_x, float* ptr_point_y){

  // Vector from B to A
  float BAx = Ax - Bx;
  float BAy = Ay - By;

  // calculate length of vector
  float length = sqrtf((BAx * BAx) + (BAy * BAy));

  // Reuse variables to create point with offset
  BAx = (BAx * (offset / length)) + Bx;
  BAy = (BAy * (offset / length)) + By;

  // Draw line
  //SDL_RenderLine(renderer, (int)BAx, (int)BAy, (int)Ax, (int)Ay);
  ptr_point_x[0] = BAx;
  ptr_point_y[0] = BAy;
  return true;
}



// Function for rendering a teardrop efect out from a circle
// For rendering a circle bloging out of a nother circle in a smoth way
// returns true on sucsess and false on fail
bool RenderTeardrop_inclined(SDL_Renderer* renderer, float Start_x, float Start_y, float Start_radius, float End_x, float End_y, float End_radius){

  if(!renderer) return false;

  //if(alpha >= (M_PI / 2.0f) || alpha <= asinf(End_radius / Start_radius)) return false;

  // determine point C_a and C_b where the lines will be drawn from
  // Start End vector: (End_x - Start_x)   // perpendicular vector: (x, y) -> (-y, x)
  float point_Cax = -1.0f * (End_y - Start_y);
  float point_Cay = (End_x - Start_x);

  float point_Cbx = -1.0f * point_Cax;
  float point_Cby = -1.0f * point_Cay;

  // scale  Vectors by tan(alpha)
  //float scaling = tanf(alpha);
  float scaling = 1.0f / (sqrtf((point_Cax * point_Cax) + (point_Cay * point_Cay)));
  point_Cax = point_Cax * scaling;
  point_Cay = point_Cay * scaling;

  scaling = 1.0f / (sqrtf((point_Cbx * point_Cbx) + (point_Cby * point_Cby)));
  point_Cbx = point_Cbx * scaling;
  point_Cby = point_Cby * scaling;

  // Calculate length of distance between points
  float length_SE = sqrtf(powf(End_x - Start_x, 2) + powf(End_y - Start_y, 2));

  // calculate radius of third circle
  float radius = (length_SE * length_SE) + (End_radius * End_radius) - (Start_radius * Start_radius);
  radius = radius / (2.0f * (Start_radius - End_radius));

  // scale vectors up
  scaling = radius + End_radius;

  point_Cax = point_Cax * scaling;
  point_Cay = point_Cay * scaling;
  point_Cbx = point_Cbx * scaling;
  point_Cby = point_Cby * scaling;

  // Add vectors to end point to get points
  point_Cax += End_x;
  point_Cay += End_y;
  point_Cbx += End_x;
  point_Cby += End_y;
  /*
  // for testing render points
  SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
  RenderFillCircle(renderer, Start_x, Start_y, Start_radius);

  SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
  RenderFillCircle(renderer, End_x, End_y, End_radius);

  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  RenderFillCircle(renderer, point_Cax, point_Cay, 10);

  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  RenderFillCircle(renderer, point_Cbx, point_Cby, 10);

  RenderDrawCircle(renderer, point_Cax, point_Cay, radius);
  RenderDrawCircle(renderer, point_Cbx, point_Cby, radius);
  */

  // Create geometry to fill in area
  SDL_Vertex vert[3];
  // first point is start point
  // second point is end point
  // third oint is offset point

  Uint8 ired, igreen, iblue, ialpha;
  SDL_GetRenderDrawColor(renderer, &ired, &igreen, &iblue, &ialpha);

  float fred, fgreen, fblue, falpha;
  fred   = ((float)ired  ) / 255.0f;
  fgreen = ((float)igreen) / 255.0f;
  fblue  = ((float)iblue ) / 255.0f;
  falpha = ((float)ialpha) / 255.0f;

  vert[0].position.x = Start_x;
  vert[0].position.y = Start_y;
  vert[0].color.r = fred;
  vert[0].color.g = fgreen;
  vert[0].color.b = fblue;
  vert[0].color.a = falpha;
  // left
  vert[1].position.x = End_x;
  vert[1].position.y = End_y;
  vert[1].color.r = fred;
  vert[1].color.g = fgreen;
  vert[1].color.b = fblue;
  vert[1].color.a = falpha;
  //right
  vert[2].position.x = Start_x;
  vert[2].position.y = Start_y;
  vert[2].color.r = fred;
  vert[2].color.g = fgreen;
  vert[2].color.b = fblue;
  vert[2].color.a = falpha;

  float SE_x = End_x - Start_x;
  float SE_y = End_y - Start_y;

  // Parameterize connection line
  for(float i = 0.0f; i < 1.0; i += 1.0f / length_SE){
    // Upper side
    GetLineOffsetPoint((i * SE_x) + Start_x, (i * SE_y) + Start_y, point_Cax, point_Cay, radius, &(vert[2].position.x), &(vert[2].position.y));
    vert[1].position.x = (i * SE_x) + Start_x;
    vert[1].position.y = (i * SE_y) + Start_y;
    SDL_RenderGeometry(renderer, NULL, vert, 3, NULL, 0);
    // Lowwer side
    GetLineOffsetPoint((i * SE_x) + Start_x, (i * SE_y) + Start_y, point_Cbx, point_Cby, radius, &(vert[2].position.x), &(vert[2].position.y));
    SDL_RenderGeometry(renderer, NULL, vert, 3, NULL, 0);
  }


 return true;
}


// Function for checking if a point is in a rectangle
bool is_in_FRect(int x, int y, SDL_FRect* ptr_FRect){

  if(!ptr_FRect) return false;

  if(x < (int)(ptr_FRect->x)) return false;
  if(y < (int)(ptr_FRect->y)) return false;

  if(x > (int)(ptr_FRect->x + ptr_FRect->w)) return false;
  if(y > (int)(ptr_FRect->y + ptr_FRect->h)) return false;

  return true;
}



// Function for checking if a point is in a circle (in range)
bool is_in_Radius(int x, int y, int center_x, int center_y, int radius){

  // check if point is in vecinety of circle
  if(x < (center_x - radius)) return false;
  if(x > (center_x + radius)) return false;
  if(y < (center_y - radius)) return false;
  if(y > (center_y + radius)) return false;

  // point is within square of circle

  // Expression of distance from center to point
  // SDL_sqrtf(powf((float)(x - center_x), 2) + powf((float)(y - center_y), 2))
  if(SDL_sqrtf(powf((float)(x - center_x), 2) + powf((float)(y - center_y), 2)) > (float)radius) return false;

  return true;
}
