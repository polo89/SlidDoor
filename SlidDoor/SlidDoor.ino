/*
	Statemachine SlidDoor

	modified 12 Feb 2017
	by Frank Bruhn
*/

#include <EEPROM.h>
#include "DoorLock.h"
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
	STATE_OBSTACLE,
	STATE_LOCKED,
	STATE_UNLOCK
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
DoorLock Lock = DoorLock(PIN_LOCK_STATE, PIN_LOCK_TRIGGER, PIN_LOCK_DIR);

//Timings
unsigned long enterStateTime;

//Request
bool lockStateRequest = false;

bool debug = true;

void setup() {
	Serial.begin(14400);
	pinMode(PIN_MOTOR_ENC1, INPUT);
	pinMode(PIN_MOTOR_ENC2, INPUT);
	attachInterrupt(digitalPinToInterrupt(PIN_MOTOR_ENC1), Encoder, RISING);

	SlidDoor.Setup();

	currentState = STATE_LOCKED;
	nextState = STATE_LOCKED;

	double eeOpenPosition;
	EEPROM.get(0, eeOpenPosition);
	if (Lock.GetState() != LOCKED || eeOpenPosition == 0)
	{
		if (!SlidDoor.Learn()) {
			Serial.println("Lernen nicht erfolgreich!");
			while (true);
		};
		EEPROM.put(0, SlidDoor.OpenPosition);
		currentState = STATE_CLOSED;
		nextState = STATE_CLOSED;
	}
	else
	{
		SlidDoor.OpenPosition = eeOpenPosition;
	}
	enterStateTime = millis();

}


void loop() {
	switch (currentState)
	{
	case STATE_OPEN: {
		SlidDoor.Stop();
		if (RadarIndoor.isPressed() || ButtonOpen.isPressed()) enterStateTime = millis();
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
		if (millis() - enterStateTime > 500 && (ButtonOpen.isPressed() || RadarIndoor.isPressed()))
		{
			nextState = STATE_OPENING;
		}
		if (lockStateRequest || ButtonLock.isPressed()) nextState = STATE_LOCKED;
	} break;
	case STATE_CLOSING: {
		if (position >= 0) SlidDoor.Close();
		else nextState = STATE_CLOSED;
		if (RadarIndoor.isPressed() || ButtonOpen.isPressed()) nextState = STATE_OBSTACLE;
	} break;
	case STATE_OBSTACLE: {
		SlidDoor.Stop();
		if (millis() - enterStateTime > 400) nextState = STATE_OPENING;
	} break;
	case STATE_LOCKED: {
		SlidDoor.Stop();
		Lock.Lock();
		lockStateRequest = false;
		while (ButtonLock.isPressed());
		if (millis() - enterStateTime > 400) {
			nextState = STATE_UNLOCK;
		};
	} break;
	case STATE_UNLOCK: {
		SlidDoor.Stop();
		if (millis() - enterStateTime > 400 && ButtonLock.isPressed()) {
			nextState = STATE_OPENING;
			Lock.Unlock();
			while (ButtonLock.isPressed());
		};
	} break;
	default:
		break;
	}

	if (ButtonLock.isPressed() && (currentState != STATE_LOCKED || currentState != STATE_UNLOCK || currentState != STATE_CLOSED)) lockStateRequest = true;

	if (currentState != nextState) {

		if (currentState == STATE_CLOSED)	Serial.print("STATE_CLOSED  ");
		if (currentState == STATE_CLOSING)	Serial.print("STATE_CLOSING ");
		if (currentState == STATE_LOCKED)	Serial.print("STATE_LOCKED  ");
		if (currentState == STATE_OBSTACLE) Serial.print("STATE_OBSTACLE");
		if (currentState == STATE_OPEN)		Serial.print("STATE_OPEN    ");
		if (currentState == STATE_OPENING)	Serial.print("STATE_OPENING ");
		if (currentState == STATE_UNLOCK)	Serial.print("STATE_UNLOCK  ");
		if (currentState == STATE_UNKNOWN)	Serial.print("STATE_UNKNOWN ");

		Serial.print(" => ");

		if (nextState == STATE_CLOSED)		Serial.print("STATE_CLOSED  ");
		if (nextState == STATE_CLOSING)		Serial.print("STATE_CLOSING ");
		if (nextState == STATE_LOCKED)		Serial.print("STATE_LOCKED  ");
		if (nextState == STATE_OBSTACLE)	Serial.print("STATE_OBSTACLE");
		if (nextState == STATE_OPEN)		Serial.print("STATE_OPEN    ");
		if (nextState == STATE_OPENING)		Serial.print("STATE_OPENING ");
		if (nextState == STATE_UNLOCK)		Serial.print("STATE_UNLOCK  ");
		if (nextState == STATE_UNKNOWN)		Serial.print("STATE_UNKNOWN ");

		Serial.println();

		currentState = nextState;
		enterStateTime = millis();
	}
}

void Encoder() {
	if (digitalRead(PIN_MOTOR_ENC2)) position += 1.0; //CW
	else position -= 1.0; //CCW
}