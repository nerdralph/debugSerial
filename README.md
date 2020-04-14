# debugSerial
AVR Arduino tiny and fast bit-bang serial output library


This library is for very high-speed (7-10 cycles/bit) serial output.  The default speed is 8 cycles/bit, or 1mbps at 8Mhz.  Bit-bang serial implementations must have interrupts disabled during transmit, so a high-speed implementation minimizes the impact on interrupt latency.  At the default 8 cycles per bit, the code to transmit a full frame including start and stop bits takes 84 cycles.  Most of the code is written in assembler to achieve the minimum possible code size.

A subset of the standard Serial print methods are supported: flash strings, single characters, and bytes as HEX or DEC can be printed.  Methods not used will not consume any flash.  Flash size requirements are as follows:
* print single characters: 28 bytes
* print strings: 10 extra bytes
* print u8 as HEX: 20 extra bytes
* print u8 as DEC: 34 extra bytes
 
## Configuration

To change the default baudrate, define DTXWAIT before including debugSerial.h.  Valid values of DTXWAIT are between 0 and 3, resulting in the range of 7-10 clock cycles per bit. 

To set the transmit GPIO, define DBG_TX before including debugSerial.h.  The default is B,0 for output on PB0.

