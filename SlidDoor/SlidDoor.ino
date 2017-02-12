/*
	Statemachine SlidDoor

	modified 04 Feb 2017
	by Frank Bruhn
*/

#include <Button.h>
#include "Motor.h"
#include "Pins_SlidDoor.h"

enum STATES
{
	STATE_UNKNOWN,
	STATE_CLOSED,
	STATE_CLOSING,
	STATE_OPEN,
	STATE_OPENING,
};

double position = 0; //rising = clockwise
byte currentState;
byte nextState;
Motor SlidDoor = Motor(&position, PIN_MOTOR_PWM, PIN_MOTOR_DIR, PIN_MOTOR_CURR, 0.3, 0, 0);
Button ButtonOpen = Button(PIN_BUTTON_OPEN, PULLUP);
Button RadarIndoor = Button(PIN_RADAR_INDOOR, PULLUP);
Button RadarOutdoor = Button(PIN_RADAR_OUTDOOR, PULLUP);
Button LockState = Button(PIN_LOCK_STATE, PULLUP);
Button ButtonLock = Button(PIN_BUTTON_LOCK, PULLUP);

//Timings
unsigned long enterStateTime;

bool debug = true;

void setup() {
	Serial.begin(14400);
	pinMode(PIN_MOTOR_ENC1, INPUT);
	pinMode(PIN_MOTOR_ENC2, INPUT);
	attachInterrupt(digitalPinToInterrupt(PIN_MOTOR_ENC1), Encoder, RISING);

	SlidDoor.Setup();
	if (!SlidDoor.Learn()) {
		Serial.println("Lernen nicht erfolgreich!");
		while (true);
	};
	enterStateTime = millis();
	currentState = STATE_CLOSED;
	nextState = STATE_CLOSED;
}


void loop() {
	switch (currentState)
	{
	case STATE_OPEN: {
		SlidDoor.Stop();
		if (millis() - enterStateTime > 1000)
		{
			nextState = STATE_CLOSING;
		}
	} break;
	case STATE_OPENING: {
		if (position <= SlidDoor.OpenPosition)
		{
			SlidDoor.Open(SlidDoor.OpenPosition);
		}
		else
		{
			nextState = STATE_OPEN;
		}
	} break;
	case STATE_CLOSED: {
		SlidDoor.Stop();
		if (millis() - enterStateTime > 500 && ButtonOpen.isPressed())
		{
			nextState = STATE_OPENING;
		}
	} break;
	case STATE_CLOSING: {
		if (position >= 0)
		{
			SlidDoor.Close();
		}
		else
		{
			nextState = STATE_CLOSED;
		}
	} break;
	default:
		break;
	}

	if (currentState != nextState) {	
		currentState = nextState;
		enterStateTime = millis();
	}
	//if (debug) Serial.println();
	SlidDoor.Compute();
}

void Encoder() {
	if (digitalRead(PIN_MOTOR_ENC2)) position += 1.0; //CW
	else position -= 1.0; //CCW
}