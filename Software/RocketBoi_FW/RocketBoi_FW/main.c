#include <atmel_start.h>
#include <util/delay.h>

#include "I2C.h"

// Let's calculate all the thresholds at compile time so we avoid doing float-math during runtime. Our poor ATTiny does not have enough memory for that.
#define T1 18
#define T2 20
#define T3 21
#define T4 22
#define T5 24

#define H1 30
#define H2 40
#define H3 50
#define H4 60
#define H5 70

const uint16_t temp_thrs1 = ((T1+46.85)*65536)/175.72;
const uint16_t temp_thrs2 = ((T2+46.85)*65536)/175.72;
const uint16_t temp_thrs3 = ((T3+46.85)*65536)/175.72;
const uint16_t temp_thrs4 = ((T4+46.85)*65536)/175.72;
const uint16_t temp_thrs5 = ((T5+46.85)*65536)/175.72;

const uint16_t hum_thrs1 = ((H1+6)*65536)/125;
const uint16_t hum_thrs2 = ((H2+6)*65536)/125;
const uint16_t hum_thrs3 = ((H3+6)*65536)/125;
const uint16_t hum_thrs4 = ((H4+6)*65536)/125;
const uint16_t hum_thrs5 = ((H5+6)*65536)/125;

// Function prototypes
void setLed(uint8_t led);
void ledAnimation();
uint16_t readTemperature();
uint16_t readHumidity();
uint16_t readTemperatureFromPrevious();

//  --= MAIN 
int main(void)
{
	atmel_start_init(); // Initialize MCU, drivers and middleware 
	I2C_init();			// Initialize custom I2C implementation
	ledAnimation();		// Blink all LEDs for testing purposes
	
	uint8_t temperatureLED = 0;
	uint8_t humidityLED = 0;
	uint8_t displayCounter = 0;
	
	while (1) {
		// Time to measure
		uint16_t value = readHumidity();
		if(value < hum_thrs1)		humidityLED=0;
		else if(value < hum_thrs2)	humidityLED=1;
		else if(value < hum_thrs3)	humidityLED=2;
		else if(value < hum_thrs4)	humidityLED=3;
		else if(value < hum_thrs5)	humidityLED=4;
		else						humidityLED=5;		
		value = readTemperatureFromPrevious();
		if(value < temp_thrs1)		temperatureLED = 6;
		else if(value < temp_thrs2) temperatureLED = 7;
		else if(value < temp_thrs3) temperatureLED = 8;
		else if(value < temp_thrs4) temperatureLED = 9;
		else if(value < temp_thrs5) temperatureLED = 10;
		else						temperatureLED = 11;
		
		// Time to show the LEDs for a while instead of measuring.
		while(++displayCounter){
			setLed(temperatureLED);
			_delay_ms(8);
			setLed(humidityLED);
			_delay_ms(8);
		}
	}
}

void setLed(uint8_t led){
	PORTA.OUT &= 0x06;  // Leave the i²C pins alone
	PORTA.DIR &= 0x06;
	switch(led){
		case 0:
		PORTA.DIR |= PIN4_bm | PIN6_bm;
		PORTA.OUT |= PIN4_bm;
		break;
		case 1:
		PORTA.DIR |= PIN4_bm | PIN6_bm;
		PORTA.OUT |= PIN6_bm;
		break;
		case 2:
		PORTA.DIR |= PIN3_bm | PIN4_bm;
		PORTA.OUT |= PIN3_bm;
		break;
		case 3:
		PORTA.DIR |= PIN3_bm | PIN4_bm;
		PORTA.OUT |= PIN4_bm;
		break;
		case 4:
		PORTA.DIR |= PIN4_bm | PIN7_bm;
		PORTA.OUT |= PIN7_bm;
		break;
		case 5:
		PORTA.DIR |= PIN4_bm | PIN7_bm;
		PORTA.OUT |= PIN4_bm;
		break;
		
		case 6:
		PORTA.DIR |= PIN3_bm | PIN6_bm;
		PORTA.OUT |= PIN3_bm;
		break;
		case 7:
		PORTA.DIR |= PIN3_bm | PIN6_bm;
		PORTA.OUT |= PIN6_bm;
		break;
		case 8:
		PORTA.DIR |= PIN3_bm | PIN7_bm;
		PORTA.OUT |= PIN3_bm;
		break;
		case 9:
		PORTA.DIR |= PIN3_bm | PIN7_bm;
		PORTA.OUT |= PIN7_bm;
		break;
		case 10:
		PORTA.DIR |= PIN6_bm | PIN7_bm;
		PORTA.OUT |= PIN6_bm;
		break;
		case 11:
		PORTA.DIR |= PIN6_bm | PIN7_bm;
		PORTA.OUT |= PIN7_bm;
		break;
	}
}

void ledAnimation(){
	for(uint8_t led = 0; led<12;led++){
		setLed(led);
		_delay_ms(100);
	}
}


uint16_t readHumidity(){
	uint16_t humidity = 0;

	uint8_t data[2] = {0,0};
	I2C_read_bytes_from_reg(0x80, data, 0xE5, 2);

	humidity = (data[0] << 8) + data[1];
	return humidity;
}

// Whole new conversion, so very slow.
uint16_t readTemperature(){
	uint16_t temperature = 0;

	uint8_t data[2] = {0,0};
	I2C_read_bytes_from_reg(0x80, data, 0xE3, 2);

	temperature = (data[0] << 8) + data[1];
	return temperature;
}

// Much faster
uint16_t readTemperatureFromPrevious(){
	uint16_t temperature = 0;

	uint8_t data[2] = {0,0};
	I2C_read_bytes_from_reg(0x80, data, 0xE0, 2);

	temperature = (data[0] << 8) + data[1];
	return temperature;
}