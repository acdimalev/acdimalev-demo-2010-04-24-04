#include "SDL.h"
#include <cairo.h>

int fps = 30;
int width  = 320;
int height = 240;

int scale = 16;

int main(int argc, char **argv) {
  SDL_Surface *sdl_surface;
  cairo_t *cr;

  Uint32 next_frame;

  int running;

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
    running = 1;
  }

  SDL_LockSurface(sdl_surface);
  while (running) {
    { /* Render Frame */
      int x, y;
      float xo = -scale/2.0+0.5, yo = -scale/2.0+0.5;

      cairo_set_operator(cr, CAIRO_OPERATOR_CLEAR);
      cairo_paint(cr);
      cairo_set_operator(cr, CAIRO_OPERATOR_OVER);

      for (y = 0; y < scale; y = y + 1) {
        for (x = 0; x < scale; x = x + 1) {
          cairo_move_to(cr, x+xo-0.5, y+xo-0.5);
          cairo_line_to(cr, x+xo-0.5, y+xo+0.5);
          cairo_line_to(cr, x+xo+0.5, y+xo+0.5);
          cairo_line_to(cr, x+xo+0.5, y+xo-0.5);
          cairo_close_path(cr);
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

      SDL_PumpEvents();
      keystate = SDL_GetKeyState(NULL);
      if (keystate[SDLK_q]) {
        running = 0;
      }
    }

  }
  SDL_UnlockSurface(sdl_surface);

  cairo_destroy(cr);
  SDL_Quit();

  return 0;
}
