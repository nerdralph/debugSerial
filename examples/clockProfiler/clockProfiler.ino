/* debugSerial example clockProfiler
 * Ralph Doncaster [nerdralph] 2020 MIT Licence open source
 * profiles OSCCAL timing using an external 1kHz clock reference
 */

// default extra wait = 1 cycle (8 cycles/bit total)
// valid range = 0-3 cycles, for a total of 7-10 cycles/bit 
// #define DTXWAIT 1

// default TX GPIO = PB0
// #define DBG_TX B,0 

#include <debugSerial.h>

// default 1kHz clock reference on PB3
#define CLK_BIT 3
#define CLK_PINREG PINB

debugSerial dSerial;

void setup() {}

void loop()
{
    //OSCCAL -= 2;                        // adjust timing for serial

    uint8_t bootOsc= OSCCAL;

    CLK_PINREG = 1<<CLK_BIT;            // enable pullup
    delay(500);                         // wait for serial monitor

    uint16_t count = 20000;
    // check for external clock signal
    while (--count) {
        if ( !(CLK_PINREG & 1<<CLK_BIT) ) break;
    }

    bool extclk = (count != 0);
    CLK_PINREG = 1<<CLK_BIT;            // disable pullup

    dSerial.print( F("\nbootup OSCCAL: 0x") );
    dSerial.println(bootOsc, HEX);

    if (!extclk) dSerial.print( F("no ") );
    dSerial.print( F("external clock detected\n") );

    if (extclk) {
        for (uint8_t osctry = OSCCAL -4; osctry <= bootOsc+4; osctry++ )
            profile(osctry);
    }

    while (1);
}

// profile given OSCCAL value
// serial output may get garbled if timing is off more than +/-5% 
void profile(uint8_t osctry)
{
    OSCCAL = osctry;
    dSerial.print( F("testing OSCCAL 0x") );
    dSerial.print(osctry, HEX);
    uint16_t high = 0, low = -1;
    __uint24 avg = 0;
    // optimize calculations for 6-cycle loop
    int i = 256 * 6;
    do {
        if ((i & 0xFF) == 0) dSerial.write('.');
        uint16_t count = 0;
        // wait for clock low
        while (CLK_PINREG & 1<<CLK_BIT);
        // disable interrupts during timing loops
        asm ("cli");
        // wait for clock high
        while ( !(CLK_PINREG & 1<<CLK_BIT) );
        // time high phase
        while (CLK_PINREG & 1<<CLK_BIT) {
            count++;
        }
        // time low phase 
        while ( !(CLK_PINREG & 1<<CLK_BIT) ) {
            count++;
        }
        // accumulate samples for average
        avg += count;
        // 6 cycles per counting loop
        uint16_t cycles = count * 4 + count * 2;
        if (cycles < low) low = cycles;
        if (cycles > high) high = cycles;
    } while (--i);
    dSerial.print( F(" avg cycles/ms: ") );
    dSerial.print(avg >> 8);
    dSerial.print( F(", high-low: ") );
    dSerial.print(high);
    dSerial.write('-');
    dSerial.println(low);
}

