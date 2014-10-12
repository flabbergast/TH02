#if not defined(__AVR_ATtiny84__)
    #error "This one is for attiny84. Sorry."
#endif

/* 
 * Sample code for JeeNode Micro + TH02 (software I2C)
 * (c) 2014 flabbergast
 *
 * Comments:
 *  - the current code is running out of memory (I think), so can't compute compensated
 *    humidity on the attiny, the compensation needs to be done at the receiving end.
 *  - couldn't use Sleepy for sleep (same reason, memory)
 *  - if you want to use this for a real sensor, you should also power the TH02 sensor
 *    a pin and power it up/down as needed, instead of having it powered continuously.
 *    According to the datasheet, the device doesn't have a sleep mode, and consumes
 *    150uA of power in standby (too much). Also, note that it can guzzle up to 40mA
 *    of power for 5ms on each powerup and shutdown...
 *  - I would guess that one can save memory by getting rid of the C++ Object stuff and
 *    coding the I2C transactions directly here. That's the next step.
 */

#define RF69_COMPAT 1
#define RF_NODE_ID 22
#define RF_GROUP 212
#define RF_FREQ RF12_868MHZ

#include <JeeLib.h>

//#define SCL_PIN 3
//#define SCL_PORT PORTA
//#define SDA_PIN 2
//#define SDA_PORT PORTA
//#define I2C_TIMEOUT 1000
//#define I2C_SLOWMODE 1
//#include <SoftI2CMaster.h>

#include <TH02soft.h>
TH02 sensor;

//##### radio packet structure #####
typedef struct {                            // RFM12B RF payload datastructure
      int16_t temp;
      int16_t rh;
} Payload;
Payload payload;

// this must be added since we're using the watchdog for low-power waiting
ISR(WDT_vect) { Sleepy::watchdogEvent(); }

void setup() {
//    cli();
//    CLKPR = bit(CLKPCE);
//    CLKPR = 0; // div 1, i.e. speed up to 8 MHz
//    sei();

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
//  payload.rh_comp = sensor.getConpensatedRH(false);

  // send the reading
  rf12_sendNow(0, &payload, sizeof payload);
  rf12_sendWait(2);
  
  // running out of memory again: can't use Sleepy
//  Sleepy::loseSomeTime(20000); // JeeLabs power save function: enter low power mode for x seconds (valid range 16-65000 ms)

  delay(20000);
}
