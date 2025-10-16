#pragma once

#include "io.hpp"

class Pic
{
  public:
    void remap()
    {
        IO::out<uint8_t>(0x20, 0x11);
        IO::out<uint8_t>(0xA0, 0x11);
        IO::wait();
        IO::out<uint8_t>(0x21, 0x20);
        IO::out<uint8_t>(0xA1, 0x28);
        IO::wait();
        IO::out<uint8_t>(0x21, 0x04);
        IO::out<uint8_t>(0xA1, 0x02);
        IO::wait();
        IO::out<uint8_t>(0x21, 0x01);
        IO::out<uint8_t>(0xA1, 0x01);
        IO::wait();
        IO::out<uint8_t>(0x21, 0x00);
        IO::out<uint8_t>(0xA1, 0x00);
    }

    void disable()
    {
        IO::out<uint8_t>(0xA1, 0xFF);
        IO::out<uint8_t>(0x21, 0xFF);
    }

    void eoi(int isr)
    {
        if (isr > 40)
            IO::out<uint8_t>(0xA0, 0x20);

        IO::out<uint8_t>(0x20, 0x20);
    }
};

EXPOSE_SINGLETON(Pic, pic);