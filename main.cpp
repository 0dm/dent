#include "GFX/GFX.h"

int main(int argc, char **argv)
{
  if (argc != 2)
  {
    printf("Usage: dent <ROM>");
    return 1;
  }

  CPU chip8;
  if (chip8.readMem(argv[1]))
  {
    GFX GFX(640, 320, chip8);
    GFX.initialize();
    GFX.update();
  }
  return 1;
}