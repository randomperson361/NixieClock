#include "Arduino.h"
#include <math.h>
#include "Adafruit_TLC5947.h"
#include "SparkFunDS3234RTC.h"

// Define Arduino pins
const uint8_t PIN_BCD[] = {2, 3, 4, 5};
#define PIN_BUZZER 			 6
#define PIN_PWM_SS 			 9
#define PIN_RTC_SS 			10
#define PIN_SPI_MOSI 		11
#define PIN_SPI_MISO 		12
#define PIN_SPI_CLK 		13
#define PIN_BRIGHT_KNOB 	A5
#define PIN_SET_ALM_TIME 	A4
#define PIN_DOWN_BUTTON 	A3
#define PIN_ADJ_BUTTON 		A2
#define PIN_UP_BUTTON 		A1
#define PIN_ALM_ON_OFF 		A0
#define BIT_MAX_10			1023

// Define PWM outputs
const uint8_t PWM_TUBE[] = {37, 33, 29, 13, 9, 5};
const uint8_t PWM_DOT[]  = {35, 31, 11, 7};
const uint8_t PWM_LED[]  = {13, 14, 15, 5, 6, 7};

// Define constants
#define NUM_TLC5974 		 2

// Initialize globals
Adafruit_TLC5947 pwm = Adafruit_TLC5947(NUM_TLC5974, PIN_SPI_CLK, PIN_SPI_MOSI, PIN_PWM_SS);
uint8_t loopCounter = 0;
bool buttonStateAdj = 0;
bool buttonStateUp = 0;
bool buttonStateDown = 0;
bool switchAlarmOn = 0;
int knobBright = 0;
bool switchSetAlm = 0;
bool switchSetTime = 0;

void readInputs()
{

	buttonStateUp = !digitalRead(PIN_UP_BUTTON);
	buttonStateAdj = !digitalRead(PIN_ADJ_BUTTON);
	buttonStateDown = !digitalRead(PIN_DOWN_BUTTON);
	switchAlarmOn = !digitalRead(PIN_ALM_ON_OFF);
	knobBright = 4.2*max((1018.0 - analogRead(PIN_BRIGHT_KNOB)),0);
	uint16_t slider = analogRead(PIN_SET_ALM_TIME);
	switchSetAlm = 0;
	switchSetTime = 0;
	if (slider > (2*BIT_MAX_10)/3)
	{
		switchSetTime = 1;
	}
	else if (slider > BIT_MAX_10/3)
	{
		switchSetAlm = 1;
	}
}

void printInputs()
{
	Serial.print("Button Up: ");
	Serial.print(buttonStateUp);
	Serial.print("\tButton Adj: \t");
	Serial.print(buttonStateAdj);
	Serial.print("\tButton Down: \t");
	Serial.print(buttonStateDown);
	Serial.print("\tSwitch Alm: \t");
	Serial.print(switchAlarmOn);
	Serial.print("\tKnob Bright: \t");
	Serial.print(knobBright);
	Serial.print("\tSwitch Alm: \t");
	Serial.print(switchSetAlm);
	Serial.print("\tSwitch Time: \t");
	Serial.print(switchSetTime);
	Serial.print("\n");
}

void setTubeDigit(uint8_t value)
{
	for (int i = 0; i<4; i++)
	{
		digitalWrite(PIN_BCD[i], (value>>i) % 2);
	}
}

void blankAll()
{
	for (int i=0; i<(24*NUM_TLC5974); i++)
	{
		pwm.setPWM(i, 0);
	}
	pwm.writeFaster();
}

//The setup function is called once at startup of the program
void setup()
{
	Serial.begin(9600);

	// Setup pins
	for (int i = 0; i<4; i++)
	{
		pinMode(PIN_BCD[i], OUTPUT);
		digitalWrite(PIN_BCD[i], LOW);
	}
	pinMode(PIN_UP_BUTTON, INPUT_PULLUP);
	pinMode(PIN_ADJ_BUTTON, INPUT_PULLUP);
	pinMode(PIN_DOWN_BUTTON, INPUT_PULLUP);
	pinMode(PIN_ALM_ON_OFF, INPUT);
	pinMode(PIN_BRIGHT_KNOB, INPUT);
	pinMode(PIN_ALM_ON_OFF, INPUT);
	readInputs();

	// Initialize PWM board to all off
	pwm.begin();
	blankAll();

	// Initialize RTC
	rtc.begin(PIN_RTC_SS);
	// Uncomment to manually set time rtc.setTime(s, m, h, day, date, month, year)
	//rtc.setTime(0, 49, 18, 4, 30, 4, 20);
	rtc.update();
}

// The loop function is called in an endless loop
void loop()
{
	loopCounter++;
	loopCounter = loopCounter % 10;

	blankAll();
	delayMicroseconds(100); // blanking interval

	readInputs();
	//printInputs();
	rtc.update();

	// Setup digit value
	switch (loopCounter)
	{
	case 0:
		setTubeDigit(rtc.hour()/10);
		break;
	case 1:
		setTubeDigit(rtc.hour()%10);
		break;
	case 2:
		setTubeDigit(rtc.minute()/10);
		break;
	case 3:
		setTubeDigit(rtc.minute()%10);
		break;
	case 4:
		setTubeDigit(rtc.second()/10);
		break;
	case 5:
		setTubeDigit(rtc.second()%10);
		break;
	default:
		break;
	}

	// Loop through tubes lighting one at a time
	if (loopCounter <= 5)
	{
		pwm.setPWM(PWM_TUBE[loopCounter], (uint16_t)knobBright);
	}
	else
	{
		pwm.setPWM(PWM_DOT[loopCounter-6], (uint16_t)knobBright);
	}

	// RGB pattern on LEDs for test
	pwm.setLED(PWM_LED[0],knobBright/10,0,0);
	pwm.setLED(PWM_LED[1],0,knobBright/10,0);
	pwm.setLED(PWM_LED[2],0,0,knobBright/10);
	pwm.setLED(PWM_LED[3],knobBright/10,0,0);
	pwm.setLED(PWM_LED[4],0,knobBright/10,0);
	pwm.setLED(PWM_LED[5],0,0,knobBright/10);

	// Write to tubes
	pwm.writeFaster();
	delayMicroseconds(1000);
}
