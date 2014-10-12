#if not defined(__AVR_ATtiny84__)
    #error "This one is for attiny84."
#endif


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
      int16_t rh_comp;
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
  payload.rh_comp = 3;
  rf12_sendNow(0, &payload, sizeof payload);
  rf12_sendWait(2);
//  rf12_sleep(RF12_SLEEP);

//  i2c_init();
  sensor.init();
  
  sensor.startTempConv();
  sensor.waitEndConversion();

  payload.temp = sensor.getConversionValue();

  sensor.startRHConv();
  sensor.waitEndConversion();

  payload.rh = sensor.getConversionValue();
  payload.rh_comp = sensor.getConpensatedRH(true);

//  payload.three = 0xF0;
//  rf12_sleep(RF12_WAKEUP);
//  rf12_sendNow(0, &payload, sizeof payload);
//  rf12_sendWait(2);
//  rf12_sleep(RF12_SLEEP);

//  payload.one = i2c_start( 0x80 );
//  payload.two = i2c_write( 17 );
//  i2c_rep_start( 0x81 );
//  payload.three = i2c_read(true) >> 4;
//  i2c_stop();

//  rf12_sleep(RF12_WAKEUP);
  rf12_sendNow(0, &payload, sizeof payload);
  rf12_sendWait(2);
//  rf12_sleep(RF12_SLEEP);
  
//  i2c_start( 0x80 );
//  payload.two = i2c_write( 0 );
//  i2c_rep_start( 0x81 );
//  payload.three = i2c_read(true);
//  i2c_stop();
//  rf12_sendNow(0, &payload, sizeof payload);
//  rf12_sendWait(2);
//  
//  payload.one = i2c_start( 0x80 );
//  payload.two = i2c_write( 3 );
//  i2c_rep_start( 0x81 );
//  payload.three = i2c_read(true);
//  i2c_stop();
//  rf12_sendNow(0, &payload, sizeof payload);
//  rf12_sendWait(2);

}

void loop()
{
  delay(20000);
}
