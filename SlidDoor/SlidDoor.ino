/*
	Statemachine SlidDoor

	modified 04 Feb 2017
	by Frank Bruhn
*/

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
Motor SlidDoor = Motor(&position, MOTOR_PWM, MOTOR_DIR, MOTOR_CURR, 0.5, 0.1, 0);

//Timings
unsigned long lastStateChangeTime;

bool debug = true;

void setup() {
	Serial.begin(14400);
	SlidDoor.Setup();
	pinMode(MOTOR_ENC1, INPUT);
	pinMode(MOTOR_ENC2, INPUT);
	attachInterrupt(digitalPinToInterrupt(MOTOR_ENC1), Encoder, RISING);

	lastStateChangeTime = millis();
	currentState = STATE_CLOSED;
	nextState = STATE_CLOSED;
}


void loop() {
	//SlidDoor.Compute();

	switch (currentState)
	{
	case STATE_OPEN: {
		SlidDoor.Stop();
		if (millis() - lastStateChangeTime > 1000)
		{
			nextState = STATE_CLOSING;
		}
	} break;
	case STATE_OPENING: {
		if (position <= 1000)
		{
			SlidDoor.Open(1000);
		}
		else
		{
			nextState = STATE_OPEN;
		}
	} break;
	case STATE_CLOSED: {
		SlidDoor.Stop();
		if (!digitalRead(BUTTON_OPEN))
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
		lastStateChangeTime = millis();
	}
	if (debug) Serial.println();
}

void Encoder() {
	if (digitalRead(MOTOR_ENC2)) position += 1.0; //CW
	else position -= 1.0; //CCW
}