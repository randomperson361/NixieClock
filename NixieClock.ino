#include "Arduino.h"
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
#define PIN_PREV_BUTTON 	A3
#define PIN_ADJ_BUTTON 		A2
#define PIN_NEXT_BUTTON 	A1
#define PIN_ALM_ON_OFF 		A0

// Define PWM outputs
const uint8_t PWM_TUBE[] = {37, 33, 29, 13, 9, 5};
const uint8_t PWM_DOT[]  = {35, 31, 11, 7};
const uint8_t PWM_LED[]  = {13, 14, 15, 5, 6, 7};

// Define constants
#define NUM_TLC5974 		 2

// Initialize globals
Adafruit_TLC5947 pwm = Adafruit_TLC5947(NUM_TLC5974, PIN_SPI_CLK, PIN_SPI_MOSI, PIN_PWM_SS);
uint8_t loopCounter = 0;

void printTime()
{
  Serial.print(String(rtc.hour()) + ":"); // Print hour
  if (rtc.minute() < 10)
    Serial.print('0'); // Print leading '0' for minute
  Serial.print(String(rtc.minute()) + ":"); // Print minute
  if (rtc.second() < 10)
    Serial.print('0'); // Print leading '0' for second
  Serial.print(String(rtc.second())); // Print second

  if (rtc.is12Hour()) // If we're in 12-hour mode
  {
    // Use rtc.pm() to read the AM/PM state of the hour
    if (rtc.pm()) Serial.print(" PM"); // Returns true if PM
    else Serial.print(" AM");
  }

  Serial.print(" | ");

  // Few options for printing the day, pick one:
  Serial.print(rtc.dayStr()); // Print day string
  //Serial.print(rtc.dayC()); // Print day character
  //Serial.print(rtc.day()); // Print day integer (1-7, Sun-Sat)
  Serial.print(" - ");
  Serial.print(String(rtc.month()) + "/" +   // Print month
                 String(rtc.date()) + "/");  // Print date
  Serial.println(String(rtc.year()));        // Print year
}

//The setup function is called once at startup of the program
void setup()
{
	// Setup pins
	for (int i = 0; i<4; i++)
	{
		pinMode(PIN_BCD[i], OUTPUT);
		digitalWrite(PIN_BCD[i], LOW);
	}
	pinMode(PIN_PREV_BUTTON, INPUT_PULLUP);
	pinMode(PIN_ADJ_BUTTON, INPUT_PULLUP);
	pinMode(PIN_NEXT_BUTTON, INPUT_PULLUP);

	// Initialize PWM board to all off
	pwm.begin();
	for (int i=0; i<(24*NUM_TLC5974); i++)
	{
		pwm.setPWM(i, 0);
	}
	pwm.writeFaster();

	// Initialize RTC
	rtc.begin(PIN_RTC_SS);
	// Uncomment to manually set time rtc.setTime(s, m, h, day, date, month, year)
	// rtc.setTime(0, 4, 20, 2, 28, 4, 20);
	rtc.update();

	// Use the serial monitor to view time/date output
	Serial.begin(9600);
}

// The loop function is called in an endless loop
void loop()
{
	// RTC testing
	static int8_t lastSecond = -1;
	rtc.update();
	if (rtc.second() != lastSecond) // If the second has changed
	{
	  printTime(); // Print the new time
	  lastSecond = rtc.second(); // Update lastSecond value
	}

	// Clear all PWM outputs
	for (int i=0; i<(24*NUM_TLC5974); i++)
	{
		pwm.setPWM(i, 0);
	}

	// Loop through tubes lighting one at a time
	loopCounter++;
	loopCounter = loopCounter % 10;
	if (loopCounter <= 5)
	{
		pwm.setPWM(PWM_TUBE[loopCounter], 4095);
	}
	else
	{
		pwm.setPWM(PWM_DOT[loopCounter-6], 4095);
	}

	// RGB pattern on LEDs for test
	pwm.setLED(PWM_LED[0],49,0,0);
	pwm.setLED(PWM_LED[1],0,40,0);
	pwm.setLED(PWM_LED[2],0,0,40);
	pwm.setLED(PWM_LED[3],40,0,0);
	pwm.setLED(PWM_LED[4],0,40,0);
	pwm.setLED(PWM_LED[5],0,0,40);

	pwm.writeFaster();
}
