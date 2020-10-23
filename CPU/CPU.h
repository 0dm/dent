#ifndef DENT_CPU_H
#define DENT_CPU_H

#include <fstream>
#include <cstdio>
#include <ctime>
#include <cstdint>
#include <random>

static uint8_t font[0x50] = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};


class CPU
{
private:
    uint16_t OPCODE; // Current Instruction
    uint16_t I; // Index
    uint16_t PC; // Instruction Pointer
    uint16_t stack[0x10];
    uint16_t SP; // Stack PTR

    uint8_t mem[0x1000]; // 4KB
    uint8_t V[0x10]; // Registers
    uint8_t DT; // Delay timer
    uint8_t ST; // Sound Timer
    uint8_t key[0x10]; // Keys

    bool DF; // Draw Flag


    void nextInstruction();
    void skipInstruction();

public:

    CPU();

    void runCycle();

    bool readMem(const std::string &file);

    bool readDrawFlag() const;

    void setDrawFlag(bool value);

    void setKey(uint8_t key, uint8_t value);

    void reload();

    bool paused;
    std::string currentGame;
    uint8_t gfx[64 * 32]; // Original CHIP-8 Display resolution is 64×32 pixels.
};


#endif //DENT_CPU_H
