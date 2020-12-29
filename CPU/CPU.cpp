#include "CPU.h"

CPU::CPU() : I(0), OPCODE(0), PC(0x200), SP(0), DT(0), ST(0), stack{0}, V{0}, key{0}, mem{0}, gfx{0}, DF(false),
             paused(false)
{
  // Load font into memory.
  for (int i = 0; i < 0x50; ++i)
    mem[i] = font[i];

  srand(static_cast<unsigned int>(time(nullptr)));
}

bool CPU::readMem(const std::string &file)
{
  std::ifstream finput(file, std::ios::binary);
  currentGame = file;

  if (finput.good())
  {

    finput.peek();
    for (int i = 0; !finput.eof() && (i + 0x200 < 0xFFF); ++i)
      mem[i + 0x200] = finput.get();

    if (finput.peek() != EOF)
    {
      fprintf(stderr, "ROM uses too much memory.");
      return false;
    }
    finput.close();
    return true;
  } else
    fprintf(stderr, "Unable to read ROM");
  return false;
}

void CPU::runCycle() // Comments from https://en.wikipedia.org/wiki/CHIP-8
{
  OPCODE = mem[PC] << 8 | mem[PC + 1]; // 2 byte instructions
  // printf(":: 0x%X", OPCODE); print all OPCODE

  auto X = (OPCODE & 0x0F00) >> 8;
  auto Y = (OPCODE & 0x00F0) >> 4;
  auto NN = OPCODE & 0x00FF;

  switch (OPCODE & 0xF000)
  {
    case 0x0000:
      switch (OPCODE & 0x000F)
      {
        case 0x0000: // 0x00E0: Clears the screen
          for (unsigned char &i : gfx)
          {
            i = 0;
          }
          DF = true;
          nextInstruction();
          break;

        case 0x000E: // 0x00EE: return
          SP--;
          PC = stack[SP];
          nextInstruction();
          break;

        default:
          printf("UNKNOWN INSTRUCTION [0x0000]: 0x%X\n", OPCODE);
          exit(1);
      }
      break;

    case 0x1000: // 0x1NNN: Jumps to address.
      PC = OPCODE & 0x0FFF;
      break;

    case 0x2000: // 0x2NNN: Calls subroutine.
      stack[SP] = PC;
      SP++;
      PC = OPCODE & 0x0FFF;
      break;


    case 0x3000: // 0x3XNN: Skips the next instruction if VX equals NN.
      if (V[X] == NN)
        skipInstruction();
      else
        nextInstruction();
      break;

    case 0x4000: // 0x4XNN: Skips the next instruction if VX doesn't equal NN.
      if (V[X] != NN)
        skipInstruction();
      else
        nextInstruction();
      break;


    case 0x5000: // 5XY0: Skips the next instruction if VX equals VY.
      if (V[X] == V[Y])
        skipInstruction();
      else
        nextInstruction();
      break;

    case 0x6000: // 6XNN: Sets VX to NN.
      V[X] = NN;
      nextInstruction();
      break;

    case 0x7000: // 7XNN: Adds NN to VX. (Carry flag is not changed)
      V[X] += NN;
      nextInstruction();
      break;

    case 0x8000:
      switch (OPCODE & 0x000F)
      {
        case 0x0000: // 8XY0: Sets VX to the value of VY.
          V[X] = V[Y];
          nextInstruction();
          break;

        case 0x0001: // 8XY1: Sets VX to VX or VY. (Bitwise OR operation)
          V[X] |= V[Y];
          nextInstruction();
          break;

        case 0x0002: // 8XY2: Sets VX to VX and VY. (Bitwise AND operation)
          V[X] &= V[Y];
          nextInstruction();
          break;

        case 0x0003: // 8XY3: Sets VX to VX xor VY.
          V[X] ^= V[Y];
          nextInstruction();
          break;

        case 0x0004: // 8XY4: Adds VY to VX. VF is set to 1 when there's a carry, and to 0 when there isn't.
          V[X] += V[Y];
          V[0xF] = static_cast<uint8_t>(V[Y] > (0xFF - V[X]));
          nextInstruction();
          break;

        case 0x0005: // 8XY5: VY is subtracted from VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
          V[X] -= V[Y];
          V[0xF] = static_cast<uint8_t>(V[Y] < 0xFF - V[X]);
          nextInstruction();
          break;

        case 0x0006: // 8XY6: Stores the least significant bit of VX in VF and then shifts VX to the right by 1.
          V[0xF] = V[X] & 1;
          V[X] >>= 1;
          nextInstruction();
          break;

        case 0x0007: // 8XY7: Sets VX to VY minus VX. VF is set to 0 when there's a borrow, and 1 when there isn't.
          V[0xF] = static_cast<uint8_t>(V[X] < V[Y]);
          V[X] = V[Y] - V[X];
          nextInstruction();
          break;

        case 0x000E: // 8XYE: Stores the most significant bit of VX in VF and then shifts VX to the left by 1.
          V[0xF] = V[X] >> 7;
          V[X] <<= 1;
          nextInstruction();
          break;

        default:
          printf("UNKNOWN INSTRUCTION [0x0000]: 0x%X\n", OPCODE);
          exit(1);
      }
      break;

    case 0x9000: // 9XY0: Skips the next instruction if VX doesn't equal VY.
      if (V[X] != V[Y])
        skipInstruction();
      else
        nextInstruction();
      break;

    case 0xA000: // ANNN: Sets I to the address NNN.
      I = OPCODE & 0x0FFF;
      nextInstruction();
      break;

    case 0xB000: // BNNN: Jumps to the address NNN plus V0.
      PC = (OPCODE & 0xF000) + V[0];
      break;

    case 0xC000: // CXNN: Sets VX to the result of a bitwise and operation on a random number (Typically: 0 to 255) and NN.
      V[X] = (rand() % (0xFF + 1)) & NN;
      nextInstruction();
      break;

      /* DXYN :Draws a sprite at coordinate (VX, VY) that has a width of 8 pixels and a height of N+1 pixels.
       * Each row of 8 pixels is read as bit-coded starting from memory location I;
       * I value doesn’t change after the execution of this instruction. As described above,
       * VF is set to 1 if any screen pixels are flipped from set to unset when the sprite is drawn, and to 0 if that doesn’t happen.
       */
    case 0xD000:
    {
      uint16_t x = V[X];
      uint16_t y = V[Y];
      uint16_t z = OPCODE & 0x000F;
      uint16_t p;

      V[0xF] = 0;
      for (int yl = 0; yl < z; yl++)
      {
        p = mem[I + yl];
        for (int xl = 0; xl < 8; xl++)
        {
          if ((p & (0x80 >> xl)) != 0)
          {
            if (gfx[(x + xl + ((y + yl) * 0x40))] == 1)
            {
              V[0xF] = 1;
            }
            gfx[x + xl + ((y + yl) * 0x40)] ^= 1;
          }
        }
      }
      DF = true; // draw
      nextInstruction();
    }
      break;

    case 0xE000:
      switch (NN)
      {
        case 0x009E: // EX9E: Skips the next instruction if the key stored in VX is pressed.
          if (key[V[X]] != 0)
            skipInstruction();
          else
            nextInstruction();
          break;

        case 0x00A1: // EXA1: Skips the next instruction if the key stored in VX isn't pressed.
          if (key[V[X]] == 0)
            skipInstruction();
          else
            nextInstruction();
          break;

        default:
          printf("UNKNOWN INSTRUCTION [0x0000]: 0x%X\n", OPCODE);
          exit(1);
      }
      break;

    case 0xF000:
      switch (NN)
      {
        case 0x0007: // FX07: Sets VX to the value of the delay timer.
          V[X] = DT;
          nextInstruction();
          break;

        case 0x000A: // FX0A: A key press is awaited, and then stored in VX. (Blocking Operation. All instruction halted until next key event)
        {
          bool pressed = false;
          for (int i = 0; i < 0x10; ++i)
          {
            if (key[i] != 0)
            {
              V[X] = i;
              pressed = true;
            }
          }
          if (!pressed)
          {
            return;
          }
          nextInstruction();
        }
          break;

        case 0x0015: // FX15: Sets the delay timer to VX.
          DT = V[X];
          nextInstruction();
          break;

        case 0x0018: // FX18: Sets the sound timer to VX.
          ST = V[X];
          nextInstruction();
          break;

        case 0x001E: // FX1E: Adds VX to I. VF is not affected *(in most cases?)
          I += V[X];
          nextInstruction();
          break;

        case 0x0029: // FX29: Sets I to the location of the sprite for the character in VX. Characters 0-F (in hexadecimal) are represented by a 4x5 font.
          I = V[X] * 5;
          nextInstruction();
          break;

          /* FX33: Stores the binary-coded decimal representation of VX, with the most significant of three digits at the address in I,
           * the middle digit at I plus 1, and the least significant digit at I plus 2.
           */
        case 0x0033:
        {
          mem[I] = V[X] / 100;
          mem[I + 1] = (V[X] / 10) % 10;
          mem[I + 2] = (V[X] % 100) % 10;
          nextInstruction();
        }
          break;

        case 0x0055: // FX55: Stores V0 to VX (including VX) in memory starting at address I.
          // The offset from I is increased by 1 for each value written, but I itself is left unmodified
          for (int i = 0; i <= X; ++i)
          {
            mem[I + 1] = V[i];
          }
          I += X + 1;
          nextInstruction();
          break;

        case 0x0065: // FX65: Fills V0 to VX (including VX) with values from memory starting at address I.
          // The offset from I is increased by 1 for each value written, but I itself is left unmodified.
        {
          for (int i = 0; i <= X; ++i)
            V[i] = mem[I + i];

          I += X + 1;
          nextInstruction();

        }
          break;

        default:
          printf("UNKNOWN INSTRUCTION [0x0000]: 0x%X\n", OPCODE);
          exit(1);
      }
      break;

    default:
      printf("INSTRUCTION DOES NOT EXIST [0x0000]: 0x%X\n", OPCODE);
      exit(1);
  }

  if (DT > 0)
    DT--;
  if (ST > 0)

    ST--;
}

inline void CPU::nextInstruction()
{
  PC += 2;
}

inline void CPU::skipInstruction()
{
  PC += 4;
}

bool CPU::readDrawFlag() const
{
  return this->DF;
}

void CPU::setDrawFlag(bool value)
{
  this->DF = value;
}

void CPU::reload()
{
  this->PC = 0x200;

  for (int i = 0x50; i < 0x1000; i++)
    mem[i] = 0;

  for (unsigned char &i : gfx)
    i = 0;

  readMem(this->currentGame);
}



