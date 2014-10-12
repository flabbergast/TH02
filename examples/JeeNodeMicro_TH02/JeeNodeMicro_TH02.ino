#if not defined(__AVR_ATtiny84__)
    #error "This one is for attiny84. Sorry."
#endif

/* 
 * Sample code for JeeNode Micro + TH02 (software I2C)
 * (c) 2014 flabbergast
 *
 * Comments:
 *  - if you want to use this for a real sensor, you should also power the TH02 sensor
 *    a pin and power it up/down as needed, instead of having it powered continuously.
 *    According to the datasheet, the device doesn't have a sleep mode, and consumes
 *    150uA of power in standby (too much). Also, note that it can guzzle up to 40mA
 *    of power for 5ms on each powerup and shutdown...
 *
 * Warning: You'll need to edit the TH02soft.cpp to change the pins!
 * Warning: the resulting size of this sketch is about 6400 bytes. So with Arduino 1.0.5,
 *          you might get an error with something like "relocation truncated to fit:
 *          R_AVR_13_PCREL against symbol `exit' defined". This is apparently due to
 *          having old avr-gcc: https://code.google.com/p/arduino-tiny/issues/detail?id=58
 *          [which is triggered if the code is >= 4kB.]
 *          So either update the avr-gcc in Arduino, or fiddle with the code (use _nomath,
 *          don't compute compensatedRH, don't include math.h, and like.) I managed to get
 *          it under 4kB with tricks like that, see commit
 *          ced130d978200841f12d2a394d2591cfc6580288 .
 *
 */

#define RF69_COMPAT 1
#define RF_NODE_ID 22
#define RF_GROUP 212
#define RF_FREQ RF12_868MHZ

#include <JeeLib.h>

#include <TH02soft.h>
TH02 sensor;

//##### radio packet structure #####
typedef struct {                            // RFM12B RF payload datastructure
      int16_t temp;
      int16_t rh;
      int16_t rh_comp;
} Payload;
Payload payload;

// this must be added since we're using the watchdog for low-power waiting
ISR(WDT_vect) { Sleepy::watchdogEvent(); }

void setup() {

  // power up the radio on JMv3
  bitSet(DDRB, 0);
  bitClear(PORTB, 0);

  rf12_initialize(RF_NODE_ID, RF_FREQ, RF_GROUP);

  // send an echo that we're starting  
  payload.temp = 1;
  payload.rh = 2;
  rf12_sendNow(0, &payload, sizeof payload);
  rf12_sendWait(2);

  // initialise i2c
  sensor.init();
}

void loop()
{
  // do a temp conversion
  sensor.startTempConv();
  sensor.waitEndConversion();

  // read the result
  payload.temp = sensor.getConversionValue_nomath();

  // do a humidity conversion
  sensor.startRHConv();
  sensor.waitEndConversion();

  // read the result
  payload.rh = sensor.getConversionValue_nomath();
  
  // couldn't get the math code working on attiny84 (I think a problem with memory size)
  payload.rh_comp = sensor.getCompensatedRH(false);

  // send the reading
  rf12_sendNow(0, &payload, sizeof payload);
  rf12_sendWait(2);
  
  // running out of memory again: can't use Sleepy
  Sleepy::loseSomeTime(20000); // JeeLabs power save function: enter low power mode for x seconds (valid range 16-65000 ms)
}
