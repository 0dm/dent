#ifndef DENT_GFX_H
#define DENT_GFX_H
#if defined(WIN32)
  #include "..\External\mingw-std-threads\mingw.thread.h" // Can't assume POSIX threads available...
#else
  #include <thread>
#endif
#include <SDL2/SDL.h>
#include "../CPU/CPU.h"

#define keysym key.keysym.sym

static uint8_t keyPad[0x10] = {
    SDLK_x, // 0
    SDLK_1, // 1
    SDLK_2, // 2
    SDLK_3, // 3
    SDLK_q, // 4
    SDLK_w, // 5
    SDLK_e, // 6
    SDLK_a, // 7
    SDLK_s, // 8
    SDLK_d, // 9
    SDLK_z, // A
    SDLK_c, // B
    SDLK_4, // C
    SDLK_r, // D
    SDLK_f, // E
    SDLK_v  // F
};

class GFX
{
private:
    // Device
    CPU device;

    // Dimensions
    int width;
    int height;

    int delay;
    int queueTitle;
    bool fullscreen;
    std::string title;
    void sendMsg(std::string msg);


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
