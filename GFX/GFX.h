#ifndef DENT_GFX_H
#define DENT_GFX_H
#if defined(WIN32)
  #include "..\External\mingw-std-threads\mingw.thread.h"
#else
  #include <thread>
#endif
#include "SDL2/SDL.h"
#include "../CPU/CPU.h"

#define keysym key.keysym.sym

static uint8_t keyPad[0x10] = {
        SDLK_x,
        SDLK_1,
        SDLK_2,
        SDLK_3,
        SDLK_q,
        SDLK_w,
        SDLK_e,
        SDLK_a,
        SDLK_s,
        SDLK_d,
        SDLK_z,
        SDLK_c,
        SDLK_4,
        SDLK_r,
        SDLK_f,
        SDLK_v,
};

class GFX
{
private:
    // Device
    CPU device;

    // Dimensions
    int width;
    int height;

public:
    GFX(int width, int height, CPU device);

    // SDL
    SDL_Window *window{};
    SDL_Renderer *renderer{};
    SDL_Texture *sdlTexture{};

    //
    void initialize();

    [[noreturn]] void update();

};


#endif //DENT_GFX_H
