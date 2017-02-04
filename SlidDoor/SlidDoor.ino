/*
	Statemachine SlidDoor

	modified 04 Feb 2017
	by Frank Bruhn
*/

#include "Motor.h"
#include "Pins_SlidDoor.h"

double position = 0; //rising = clockwise
Motor SlidDoor = Motor(&position, MOTOR_PWM, MOTOR_DIR, MOTOR_CURR, 0.01, 0.1, 0);

void setup() {
	Serial.begin(9600);
	SlidDoor.Setup();
	pinMode(MOTOR_ENC1, INPUT);
	pinMode(MOTOR_ENC2, INPUT);
	attachInterrupt(digitalPinToInterrupt(MOTOR_ENC1), Encoder, RISING);
}


void loop() {
	if (position < 2000)
	{
		//SlidDoor.Open();
	}	
}

void Encoder() {
	if (digitalRead(MOTOR_ENC2)) position++; //CW
	else position--; //CCW
}