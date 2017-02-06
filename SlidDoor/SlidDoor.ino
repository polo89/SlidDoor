/*
	Statemachine SlidDoor

	modified 04 Feb 2017
	by Frank Bruhn
*/

#define STATE_UNKNOWN 0
#define STATE_CLOSED 1;
#define STATE_OPEN 2;

#include "Motor.h"
#include "Pins_SlidDoor.h"

double position = 0; //rising = clockwise
double out = 0;
double set = 100;
byte currentState = 0;
byte nextState = 0;
byte lastState = 0;
Motor SlidDoor = Motor(&position, MOTOR_PWM, MOTOR_DIR, MOTOR_CURR, 0.1, 0.02, 0);

void setup() {
	Serial.begin(14400);
	SlidDoor.Setup();
	pinMode(MOTOR_ENC1, INPUT);
	pinMode(MOTOR_ENC2, INPUT);
	attachInterrupt(digitalPinToInterrupt(MOTOR_ENC1), Encoder, RISING);
}


void loop() {
	SlidDoor.Compute();
	//SlidDoor.Open();
	Serial.println();

	switch (switch_on)
	{
	default:
		break;
	}
}

void Encoder() {
	if (digitalRead(MOTOR_ENC2)) position += 1.0; //CW
	else position -= 1.0; //CCW
}