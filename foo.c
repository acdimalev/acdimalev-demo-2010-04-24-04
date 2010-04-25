#include "SDL.h"
#include <cairo.h>

const int fps = 30;
const int width  = 320;
const int height = 240;
// st float aspect = (float)width / height;
const float aspect = 1.33333333333333333333;

const int scale = 16;
// st int pattern_width  = ceil(scale * aspect) + 1;
// st int pattern_width  = 23;
const int pattern_width  = 32;
// st int pattern_height = scale;
const int pattern_height = 16;

int main(int argc, char **argv) {
  SDL_Surface *sdl_surface;
  cairo_t *cr;

  Uint32 next_frame;

  int running;
  int pattern[pattern_width * pattern_height];
  float scroll, scroll_velocity;

  /* Initialize SDL */
  SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
  SDL_ShowCursor(0);
  SDL_SetVideoMode(width, height, 32, 0);
  sdl_surface = SDL_GetVideoSurface();

  { /* Initialize Cairo Canvas */
    cairo_surface_t *surface;
    surface = cairo_image_surface_create_for_data(
      sdl_surface->pixels,
      CAIRO_FORMAT_RGB24,
      sdl_surface->w,
      sdl_surface->h,
      sdl_surface->pitch
      );
    cr = cairo_create(surface);
    // Reduce surface refcount.
    // Surface will be freed when canvas is destroyed.
    cairo_surface_destroy(surface);
  }

  // Cartesian
  cairo_translate(cr, width/2.0, height/2.0);
  cairo_scale(cr, 1, -1);

  // scale
  cairo_scale(cr, height / scale, height / scale);

  /* Initialize Delay */
  next_frame = 1024.0 / fps;

  { /* Game Logic */
    int x, y;

    running = 1;
    scroll = 0;
    scroll_velocity = -4;

    for (x = 0; x < pattern_width; x = x + 1) {
      if (x % 2) {
        pattern[0*pattern_width+x] = 0;
        pattern[1*pattern_width+x] = 1;
      } else {
        pattern[0*pattern_width+x] = 1;
        pattern[1*pattern_width+x] = 0;
      }
      for (y = 2; y < pattern_height; y = y + 1) {
        pattern[y*pattern_width+x] = 0;
      }
    }
  }

  SDL_LockSurface(sdl_surface);
  while (running) {
    { /* Render Frame */
      int x, y;
      float xo = -pattern_width/2.0+0.5+scroll, yo = -pattern_height/2.0+0.5;

      cairo_set_operator(cr, CAIRO_OPERATOR_CLEAR);
      cairo_paint(cr);
      cairo_set_operator(cr, CAIRO_OPERATOR_OVER);

      for (y = 0; y < pattern_height; y = y + 1) {
        for (x = 0; x < pattern_width; x = x + 1) {
          if (pattern[y*pattern_width+x]) {
            cairo_move_to(cr, x+xo-0.5, y+yo-0.5);
            cairo_line_to(cr, x+xo-0.5, y+yo+0.5);
            cairo_line_to(cr, x+xo+0.5, y+yo+0.5);
            cairo_line_to(cr, x+xo+0.5, y+yo-0.5);
            cairo_close_path(cr);
          }
          xo = xo + pattern_width;
          if (pattern[y*pattern_width+x]) {
            cairo_move_to(cr, x+xo-0.5, y+yo-0.5);
            cairo_line_to(cr, x+xo-0.5, y+yo+0.5);
            cairo_line_to(cr, x+xo+0.5, y+yo+0.5);
            cairo_line_to(cr, x+xo+0.5, y+yo-0.5);
            cairo_close_path(cr);
          }
          xo = xo - pattern_width;
        }
      }
      cairo_set_source_rgb(cr, 1.0, 1.0, 1.0);
      cairo_fill(cr);
    }

    { /* Update Display */
      SDL_UnlockSurface(sdl_surface);
      SDL_Flip(sdl_surface);
      SDL_LockSurface(sdl_surface);
    }

    { /* Delay */
      Uint32 now;
      now = SDL_GetTicks();
      if (now < next_frame) {
        SDL_Delay(next_frame - now);
      }
      next_frame = next_frame + 1024.0 / fps;
    }

    { /* Game Logic */
      Uint8 *keystate;
      float old_scroll;
      int x, y;

      SDL_PumpEvents();
      keystate = SDL_GetKeyState(NULL);
      if (keystate[SDLK_q]) {
        running = 0;
      }

      old_scroll = scroll;
      scroll = scroll + scroll_velocity / fps;
      if (scroll < -pattern_width) {
        scroll = scroll + pattern_width;
      }
      if ( (int)scroll % pattern_width != (int)old_scroll % pattern_width ) {
        x = -(int)scroll % pattern_width;
        if (x % 2) {
          pattern[0*pattern_width+x] = 0;
          pattern[1*pattern_width+x] = 1;
        } else {
          pattern[0*pattern_width+x] = 1;
          pattern[1*pattern_width+x] = 0;
        }
        for (y = 2; y < pattern_height; y = y + 1) {
          pattern[y*pattern_width+x] = 0;
        }
      }
    }

  }
  SDL_UnlockSurface(sdl_surface);

  cairo_destroy(cr);
  SDL_Quit();

  return 0;
}
