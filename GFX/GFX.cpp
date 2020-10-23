#include "GFX.h"

GFX::GFX(int width, int height, CPU device) : delay(1100), fullscreen(false), queueTitle(0)
{
  this->width = width;
  this->height = height;
  this->device = std::move(device);
}


[[noreturn]] void GFX::update()
{
  uint32_t pixels[0x800];

  // Emulation loop
  while (true)
  {

    SDL_Event SDLe;
    while (SDL_PollEvent(&SDLe))
    {
      switch (SDLe.type)
      {
        case SDL_KEYDOWN:
        {
          switch (SDLe.keysym)
          {

            case SDLK_F11:
              if (!fullscreen)
              {
                SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
                fullscreen = !fullscreen;
                sendMsg("FULLSCREEN: ON");
              } else
              {
                SDL_SetWindowFullscreen(window, 0);
                fullscreen = !fullscreen;
                sendMsg("FULLSCREEN: OFF");
              }
              break;

            case SDLK_F1: // Pause Game
              if (!device.paused)
              {
                device.paused = true;
                sendMsg("GAME PAUSED");
              } else
              {
                device.paused = false;
                sendMsg("RESUME");
              }
              break;

            case SDLK_F2:
              this->device.reload();
              sendMsg("RELOADED: " + device.currentGame);
              break;

            case SDLK_UP:
              delay += 100;
              sendMsg("DELAY: " + std::to_string(delay));
              break;

            case SDLK_DOWN:
              if (delay >= 1100)
              {
                delay -= 100;
                sendMsg("DELAY: " + std::to_string(delay));
              }
              break;

            case SDLK_F4:
              exit(0);
          }
          for (int i = 0; i < 0x10; ++i)
          {
            if (SDLe.keysym == keyPad[i])
            {
              this->device.setKey(i, 1);
            }
          }
        }
          break;

        case SDL_KEYUP:
        {
          for (int i = 0; i < 0x10; ++i)
          {
            if (SDLe.keysym == keyPad[i])
            {
              this->device.setKey(i, 0);
            }
          }
        }
          break;
      }
    }
    if (!device.paused)
    {
      this->device.runCycle();
      if (this->device.readDrawFlag())
      {
        this->device.setDrawFlag(false);
        for (int i = 0; i < 0x800; ++i)
        {
          uint8_t pixel = device.gfx[i];
          pixels[i] = (0x00FFFFFF * pixel) | 0xFF000000;
        }
        SDL_UpdateTexture(sdlTexture, nullptr, pixels, 64 * sizeof(Uint32));
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, sdlTexture, nullptr, nullptr);
        SDL_RenderPresent(renderer);
      }
      std::this_thread::sleep_for(std::chrono::microseconds(delay));
      if (queueTitle < 1)
        SDL_SetWindowTitle(window, title.c_str());
      else
        queueTitle--;
    }
  }
}

void GFX::initialize()
{
  title = std::string("dent: " + device.currentGame);
  if (device.currentGame.find_last_of('\\') < 100)
    title = "dent: " + device.currentGame.substr(device.currentGame.find_last_of('\\') + 1);
  if (device.currentGame.find_last_of('/') < 100)
    title = "dent: " + device.currentGame.substr(device.currentGame.find_last_of('/') + 1);

  window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED,
                            SDL_WINDOWPOS_CENTERED, this->width, this->height,
                            SDL_WINDOW_SHOWN); // SDL_WINDOW_BORDERLESS looks nice too.
  renderer = SDL_CreateRenderer(window, -1, 0);
  SDL_RenderSetLogicalSize(renderer, this->width, this->height);
  sdlTexture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, 64, 32);
  title = SDL_GetWindowTitle(window);

}

void GFX::sendMsg(std::string msg)
{
  queueTitle = 800;
  SDL_SetWindowTitle(window, msg.c_str());
}
