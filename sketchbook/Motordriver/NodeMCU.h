#ifndef NodeMCU_h
#define NodeMCU_h

/*
 * NodeMCU ESP12 Dev Kit V1.0 Pin Definition:  
 *  
 * see https://iotbytes.wordpress.com/nodemcu-pinout/
 * and https://learn.sparkfun.com/tutorials/esp8266-thing-hookup-guide/using-the-arduino-addon
 * 
 */

//
// Mapping of board pins to ESP8266 pins
// GPIOs
//
#define D0  16    /* only for GPIO, no PWM, no interrupt */
#define D1  5
#define D2  4
#define D3  0
#define D4  2
#define D5  14
#define D6  12
#define D7  13
#define D8  15

/*
 * The GPIO’s shown in the blue box (1, 3, 9, 10) are mostly not used for GPIO purpose on Dev Kit
 * ESP8266 is a System on Chip (SoC) design with components like the processor chip. The processor has around 16 GPIO lines, some of which are used internally to interface with other components of the SoC, like flash memory.
 * Since several lines are used internally within the ESP8266 SoC, we have about 11 GPIO pins remaining for GPIO purposes.
 * Now again 2 pins out of 11 are generally reserved for RX and TX in order to communicate with a host PC through which compiled object code is downloaded.
 * Hence finally, this leaves just 9 general-purpose I/O pins i.e. D0 to D8.
 * As shown in the above figure of NodeMCU Dev Kit. We can see RX, TX, SD2, SD3 pins are not mostly used as GPIOs since they are used for other internal processes. But we can try with SD3 (D12) pin which mostly likes to respond for GPIO/PWM/interrupt like functions.
 * Note that the D0/GPIO16 pin can be only used as GPIO read/write, no special functions are supported on it.
 * (from https://www.electronicwings.com/nodemcu/nodemcu-pwm-with-esplorer-ide)
 */
#define D9_RX   3
#define D10_TX  1

#define D11_SD2  9
#define D12_SD3  10


#endif
