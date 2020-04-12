# debugSerial
AVR Arduino bit-bang serial output library

Under development as of April 2020

This library is for very high-speed (7-10 cycles/bit) serial output.  The default speed is 8 cycles/bit, or 1mbps at 8Mhz.  Bit-bang serial implementations must disable interrupts during transmit, so a high-speed implementation minimizes the impact on interrupt latency.  Most of the code is written in assembler for minimal code size.
