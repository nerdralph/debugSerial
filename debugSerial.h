/* debug(transmit-only) uart
 * Ralph Doncaster 2020 open source MIT license
 * 20200412 v0.5
 * 20200414 v0.6 - update printu8b16 to use r18
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
#define bit(io)     BIT(io)
#define port(io)    PORT(io)


extern "C" void write_r20();

// transmit character in r20, clobbers __tmp_reg__, leaves T set
__attribute((weak, naked))
void write_r20()
{
    register char c asm("r20");
    asm volatile (
    "cbi %[tx_port], %[tx_bit]\n"       // disable pullup
    "cli\n"
    "sbi %[tx_port]-1, %[tx_bit]\n"     // start bit 
    "in __tmp_reg__, %[tx_port]\n"      // save port state
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
    "bld __tmp_reg__, %[tx_bit]\n"
    "ror %[c]\n"                        // shift for next bit
    "clc\n"
    "out %[tx_port], __tmp_reg__\n"
    "brne 1b\n"
    "cbi %[tx_port]-1, %[tx_bit]\n"     // set to input pullup mode
    "reti\n"
    : [c] "+r" (c)
    : [tx_port] "I" (_SFR_IO_ADDR(port(DBG_TX))),
      [tx_bit] "I" (bit(DBG_TX))
    );
}

inline void dwrite(char ch)
{
    register char c asm("r20") = ch;
    asm volatile ("%~call %x1" : "+r"(c) : "i"(write_r20));
}


// asm function in print.S - print string in flash
extern "C" void printsp_z();
inline void dprints_p(const __FlashStringHelper* s)
{
    asm volatile (
    "%~call %x1"
    : "+z" (s)
    : "i" (printsp_z)
    : "r20"
    );
}


// asm function in print.S - print u8(r20) Base16 (hex)
extern "C" void printu8b16_r20();
inline void dprintu8b16(uint8_t val)
{
    register char c asm("r20") = val;
    asm volatile (
    "%~call %x1"
    : "+r" (c) 
    : "i" (printu8b16_r20)
    );
}

// asm function in print.S - print u16(r23:22) Base10 (decimal)
extern "C" void printu16b10_r22();
inline void dprintu16b10(uint16_t val)
{
    register uint16_t i asm("r22") = val;
    asm volatile (
    "%~call %x1"
    : "+r" (i) 
    : "i" (printu16b10_r22)
    : "r20"
    );
}


class debugSerial
{
public:
    enum _base {Base10 = 10, Base16 = 16};

    void write(char c) { dwrite(c); }
    void print(uint16_t val, _base base = Base10)
    {
        if (base == Base10) dprintu16b10(val);
        else dprintu8b16(val);
    }

    void print(const __FlashStringHelper* str) { dprints_p(str); }
    /* badArg requires picoCore
    void print(const char*) 
    {
        badArg("printed strings must be in flash");
    }
    */

    template<typename T>
    void println(const T& val)
    {
        print(val);
        write('\n');
    }

    template<typename T1, typename T2>
    void println(const T1& val, const T2& base)
    {
        print(val, base);
        write('\n');
    }
};

