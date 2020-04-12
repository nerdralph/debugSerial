// 9.2M/(7+2) = 1.022Mbps
#define DTXWAIT 2

#include <debugSerial.h>

debugSerial dSerial;

void setup() {}

void loop()
{
    dSerial.println( F("\nHi ") );
    dSerial.println(42);
    dSerial.println(42, HEX);
    delay(1000);
}
