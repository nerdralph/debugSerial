/* debugSerial example
 * Ralph Doncaster [nerdralph] 2020 MIT Licence open source
 */

// default extra wait = 1 cycle (8 cycles/bit total)
// valid range = 0-3 cycles, for a total of 7-10 cycles/bit 
// #define DTXWAIT 1

// default TX GPIO = PB0
// #define DBG_TX B,0 

#include <debugSerial.h>

debugSerial dSerial;

void setup() {}

void loop()
{
    uint8_t count = 0;
    while (1) {
        dSerial.print( F("count: ") );
        dSerial.print(count);
        dSerial.print( F(" 0x") );
        dSerial.println(count++, dSerial.Base16);
        delay(1000);
    }
}
