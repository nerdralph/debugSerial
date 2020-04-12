/* debug(transmit-only) uart
 * Ralph Doncaster 2020 open source MIT license
 * 20200412 v0.5
 */

#pragma once

#include <avr/io.h>

#ifndef DBG_TX
#define DBG_TX B,0
#endif

// default to 8-cycles/bit (1mbps@8Mhz)
#ifndef DTXWAIT
#define DTXWAIT 1 
#endif

// I/O register macros
#define BIT(r,b)    (b)
#define PORT(r,b)   (PORT ## r)
#define DDR(r,b)    (DDR ## r)
#define bit(io)     BIT(io)
#define port(io)    PORT(io)
#define ddr(io)     DDR(io)


extern "C" void write_r18();

// transmit character in r18, clobbers r0, leaves T set
__attribute((weak, naked))
void write_r18()
{
    register char c asm("r18");
    asm volatile (
    "cbi %[tx_port], %[tx_bit]\n"       // disable pullup
    "cli\n"
    "sbi %[tx_port]-1, %[tx_bit]\n"     // start bit 
    "in r0, %[tx_port]\n"               // save port state
    "sec\n"                             // hold stop bit in C
    "1:\n"                              // tx bit loop
#if DTXWAIT & 1
    "nop\n"                             // wait 1 cycle
#endif
#if DTXWAIT & 2
    "rjmp .\n"                          // wait 2 cycles
#endif
    // 7 cycle loop
    "bst %[c], 0\n"                     // store lsb in T
    "bld r0, %[tx_bit]\n"
    "ror %[c]\n"                        // shift for next bit
    "clc\n"
    "out %[tx_port], r0\n"
    "brne 1b\n"
    "cbi %[tx_port]-1, %[tx_bit]\n"     // set to input pullup mode
    "reti\n"
    : [c] "+r" (c)
    : [tx_port] "I" (_SFR_IO_ADDR(port(DBG_TX))),
      [tx_bit] "I" (bit(DBG_TX))
    );
}

extern inline void dwrite(char ch)
{
    register char c asm("r18") = ch;
    asm volatile ("%~call %x1" : "+r"(c) : "i"(write_r18));
}


// asm function in print.S
extern "C" void printsp_z();

// print string in flash
extern inline void dprints_p(const __FlashStringHelper* s)
{
    asm volatile (
    "%~call %x1"
    : "+z" (s)
    : "i" (printsp_z)
    : "r18"
    );
}


// asm function in print.S - print u8(r19) base16 (HEX)
extern "C" void printu8b16_r19();

extern inline void dprintu8b16(uint8_t val)
{
    register char c asm("r19") = val;
    asm volatile (
    "%~call %x1"
    : "+r" (c) 
    : "i" (printu8b16_r19)
    : "r18"
    );
}

// asm function in print.S - print u8(r19) base10 (DEC)
extern "C" void printu8b10_r19();

extern inline void dprintu8b10(uint8_t val)
{
    register char c asm("r19") = val;
    asm volatile (
    "%~call %x1"
    : "+r" (c) 
    : "i" (printu8b10_r19)
    : "r18"
    );
}


enum _base {DEC = 10, HEX = 16};

class debugSerial
{
public:
    //void begin(int) {}
    void print(char c) { dwrite(c); }
    void print(int val, _base base = DEC)
    {
        uint8_t u8 = val;
        if (base == DEC) dprintu8b10(u8);
        else dprintu8b16(u8);
    }
    void print(const __FlashStringHelper* str) { dprints_p(str); }
    void print(const char* str)
    {
        badArg("printed strings must be in flash");
    }

    template<typename T>
    void println(const T& val)
    {
        print(val);
        print('\n');
    }

    template<typename T1, typename T2>
    void println(const T1& val, const T2& base)
    {
        print(val, base);
        print('\n');
    }
};

