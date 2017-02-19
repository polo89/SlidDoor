/*
	Statemachine SlidDoor

	modified 19 Feb 2017
	by Frank Bruhn
*/

//#include "Buzzer.h"
#include "Selector.h"
#include <EEPROM.h>
#include "DoorLock.h"
#include <Button.h>
#include "Motor.h"
#include "Pins_SlidDoor.h"

#define DEBUG

enum STATES
{
	STATE_UNKNOWN,
	STATE_CLOSED,
	STATE_CLOSING,
	STATE_OPEN,
	STATE_OPENING,
	STATE_OBSTACLE,
	STATE_LOCKED,
	STATE_UNLOCK,
	STATE_LEARN,
	STATE_MANUAL
};

double position = 0; //rising = clockwise
double obstaclePosition = -1;
byte currentState;
byte nextState;
byte ActivMode;
Motor SlidDoor = Motor(&position, PIN_MOTOR_PWM, PIN_MOTOR_DIR, PIN_MOTOR_CURR, 0.3, 0, 0);
Button ButtonOpen = Button(PIN_BUTTON_OPEN, PULLUP);
Button RadarIndoor = Button(PIN_RADAR_INDOOR, PULLUP);
Button RadarOutdoor = Button(PIN_RADAR_OUTDOOR, PULLUP);
Button LockState = Button(PIN_LOCK_STATE, PULLUP);
Button ButtonLock = Button(PIN_BUTTON_LOCK, PULLUP);
DoorLock Lock = DoorLock(PIN_LOCK_STATE, PIN_LOCK_TRIGGER, PIN_LOCK_DIR);
Selector Remote = Selector(PIN_MODE1, PIN_MODE2, PIN_MODE3, PIN_LED);

//Timings
unsigned long enterStateTime;
unsigned long lastUnlockTime;

//Request
bool lockStateRequest = false;

bool enterState = true;
bool learn = false;

bool debug = true;
bool printLine = false;

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

#ifdef DEBUG
	pinMode(PIN_LED_OPENING, OUTPUT);
	pinMode(PIN_LED_CLOSING, OUTPUT);
#endif // DEBUG
}


void loop() {
	byte mode = Remote.GetMode();

	switch (currentState)
	{
	case STATE_OPEN: {
		SlidDoor.Stop();
		if (RadarIndoor.isPressed() || ButtonOpen.isPressed()) enterStateTime = millis();
		if (millis() - enterStateTime > 1000 && mode != MODE_OPEN)
		{
			nextState = STATE_CLOSING;
		}
		if (mode == MODE_MANUAL) nextState = STATE_MANUAL;
	} break;
	case STATE_OPENING: {
		if (position <= SlidDoor.OpenPosition && ActivMode != MODE_WINTER)
		{
			SlidDoor.Open(SlidDoor.OpenPosition);
		}
		else if (position <= SlidDoor.OpenPosition / 2) {
			SlidDoor.Open(SlidDoor.OpenPosition / 2);
		}
		else
		{
			nextState = STATE_OPEN;
		}
	} break;
	case STATE_CLOSED: {
		SlidDoor.Stop();
		if (millis() - enterStateTime > 500 && (ButtonOpen.isPressed() || RadarIndoor.isPressed() || mode == MODE_OPEN))
		{
			nextState = STATE_OPENING;
		}
		if (mode == MODE_MANUAL) nextState = STATE_MANUAL;
		if (mode == MODE_LEARN && learn) nextState = STATE_LEARN;
		if (lockStateRequest || ButtonLock.isPressed()) nextState = STATE_LOCKED;
	} break;
	case STATE_CLOSING: {
		if (obstaclePosition != -1) {
			SlidDoor.Close(obstaclePosition - 100);
			if (position <= (obstaclePosition - 200)) {
				obstaclePosition = -1;
				enterStateTime = millis();
			}
		}
		else if (position >= 0) SlidDoor.Close();
		else nextState = STATE_CLOSED;
		if (RadarIndoor.isPressed() || ButtonOpen.isPressed()) nextState = STATE_OBSTACLE;
		if (SlidDoor.CheckForObstacle(1.0) && (millis() - enterStateTime) > 200) {
			obstaclePosition = position;
			nextState = STATE_OBSTACLE;
		}
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
			nextState = STATE_CLOSED;
			Lock.Unlock();
			while (ButtonLock.isPressed());
			delay(500);
			lastUnlockTime = millis();
		};
	} break;
	case STATE_LEARN: {
		SlidDoor.Stop();
		if (millis() - enterStateTime > 10000 && learn) {
			if (!SlidDoor.Learn()) {
#ifdef DEBUG
				Serial.println("Lernen nicht erfolgreich!");
#endif // DEBUG
				while (true) {
					SlidDoor.Stop();
				};
			};
			learn = false;
			EEPROM.put(0, SlidDoor.OpenPosition);
		}
		else
		{
			if (mode != MODE_LEARN) nextState = STATE_CLOSED;
		}
		if (!learn) {
			if (mode != MODE_LEARN) nextState = STATE_CLOSED;
		}
	} break;
	case STATE_MANUAL: {
		SlidDoor.Stop();
		if (mode == MODE_LEARN) nextState = STATE_LEARN;
		if (mode == MODE_LOCK) nextState = STATE_OPEN;
		if (mode == MODE_ONEWAY) nextState = STATE_OPEN;
		if (mode == MODE_OPEN) nextState = STATE_OPENING;
		if (mode == MODE_SUMMER) nextState = STATE_OPEN;
		if (mode == MODE_WINTER) nextState = STATE_OPEN;
		if (mode == MODE_UNKNOWN) nextState = STATE_MANUAL;
	} break;
	default:
		nextState = STATE_MANUAL;
		break;
	}

	if (ButtonLock.isPressed() && (currentState != STATE_LOCKED || currentState != STATE_UNLOCK || currentState != STATE_CLOSED)) {
		if (millis() - lastUnlockTime > 2000) lockStateRequest = true;
		else lockStateRequest = false;
	}

	if (currentState == STATE_CLOSED)	Remote.SetLedState(LEDSTATE_NORMAL);
	if (currentState == STATE_CLOSING)	Remote.SetLedState(LEDSTATE_NORMAL);
	if (currentState == STATE_LOCKED)	Remote.SetLedState(LEDSTATE_NORMAL);
	if (currentState == STATE_OBSTACLE) Remote.SetLedState(LEDSTATE_NORMAL);
	if (currentState == STATE_OPEN)		Remote.SetLedState(LEDSTATE_NORMAL);
	if (currentState == STATE_OPENING)	Remote.SetLedState(LEDSTATE_NORMAL);
	if (currentState == STATE_UNLOCK)	Remote.SetLedState(LEDSTATE_NORMAL_LOCKED);
	if (currentState == STATE_MANUAL)	Remote.SetLedState(LEDSTATE_MANUAL_LEARN);
	if (currentState == STATE_LEARN)	Remote.SetLedState(LEDSTATE_MANUAL_LEARN);

	if (currentState != nextState) {

#ifdef DEBUG
		if (currentState == STATE_CLOSED)	Serial.print("STATE_CLOSED  ");
		if (currentState == STATE_CLOSING)	Serial.print("STATE_CLOSING ");
		if (currentState == STATE_LOCKED)	Serial.print("STATE_LOCKED  ");
		if (currentState == STATE_OBSTACLE) Serial.print("STATE_OBSTACLE");
		if (currentState == STATE_OPEN)		Serial.print("STATE_OPEN    ");
		if (currentState == STATE_OPENING)	Serial.print("STATE_OPENING ");
		if (currentState == STATE_UNLOCK)	Serial.print("STATE_UNLOCK  ");
		if (currentState == STATE_UNKNOWN)	Serial.print("STATE_UNKNOWN ");
		if (currentState == STATE_MANUAL)	Serial.print("STATE_MANUAL  ");
		if (currentState == STATE_LEARN)	Serial.print("STATE_LEARN   ");
		Serial.print(" => ");
		if (nextState == STATE_CLOSED)		Serial.print("STATE_CLOSED  ");
		if (nextState == STATE_CLOSING)		Serial.print("STATE_CLOSING ");
		if (nextState == STATE_LOCKED)		Serial.print("STATE_LOCKED  ");
		if (nextState == STATE_OBSTACLE)	Serial.print("STATE_OBSTACLE");
		if (nextState == STATE_OPEN)		Serial.print("STATE_OPEN    ");
		if (nextState == STATE_OPENING)		Serial.print("STATE_OPENING ");
		if (nextState == STATE_UNLOCK)		Serial.print("STATE_UNLOCK  ");
		if (nextState == STATE_UNKNOWN)		Serial.print("STATE_UNKNOWN ");
		if (nextState == STATE_MANUAL)		Serial.print("STATE_MANUAL  ");
		if (nextState == STATE_LEARN)		Serial.print("STATE_LEARN   ");
		Serial.print(" | ");
		if (ActivMode == MODE_UNKNOWN)	Serial.print("MODE_UNKOWN");
		if (ActivMode == MODE_MANUAL)	Serial.print("MODE_MANUAL");
		if (ActivMode == MODE_OPEN)		Serial.print("MODE_OPEN  ");
		if (ActivMode == MODE_SUMMER)	Serial.print("MODE_SUMMER");
		if (ActivMode == MODE_WINTER)	Serial.print("MODE_WINTER");
		if (ActivMode == MODE_ONEWAY)	Serial.print("MODE_ONEWAY");
		if (ActivMode == MODE_LOCK)		Serial.print("MODE_LOCK  ");
		if (ActivMode == MODE_LEARN)	Serial.print("MODE_LEARN ");
		Serial.print(" | ");
		Serial.print(position);
		Serial.print(" | ");
		Serial.print(SlidDoor.calcSpeed());
		Serial.print("cm/s ");
		Serial.println();
#endif // DEBUG

		currentState = nextState;
		enterStateTime = millis();
		enterState = true;
		ActivMode = mode;
	}
	else enterState = false;

#ifdef DEBUG
	if (Remote.ModeChange())
	{
		learn = true;

		Serial.print("Mode changed to ");

		if (mode == MODE_UNKNOWN)	Serial.print("MODE_UNKOWN");
		if (mode == MODE_MANUAL)	Serial.print("MODE_MANUAL");
		if (mode == MODE_OPEN)		Serial.print("MODE_OPEN  ");
		if (mode == MODE_SUMMER)	Serial.print("MODE_SUMMER");
		if (mode == MODE_WINTER)	Serial.print("MODE_WINTER");
		if (mode == MODE_ONEWAY)	Serial.print("MODE_ONEWAY");
		if (mode == MODE_LOCK)		Serial.print("MODE_LOCK  ");
		if (mode == MODE_LEARN)		Serial.print("MODE_LEARN ");

		Serial.println();
	}

	if (currentState == STATE_CLOSED || currentState == STATE_CLOSING)
	{
		if (currentState == STATE_CLOSING) Blink(200, PIN_LED_CLOSING);
		else digitalWrite(PIN_LED_CLOSING, HIGH);
	}
	else digitalWrite(PIN_LED_CLOSING, LOW);

	if (currentState == STATE_OPEN || currentState == STATE_OPENING)
	{
		if (currentState == STATE_OPENING) Blink(200, PIN_LED_OPENING);
		else digitalWrite(PIN_LED_OPENING, HIGH);
	}
	else digitalWrite(PIN_LED_OPENING, LOW);
#endif // DEBUG
}

void Encoder() {
	if (digitalRead(PIN_MOTOR_ENC2)) position += 1.0; //CW
	else position -= 1.0; //CCW
}

#ifdef DEBUG
bool _blink = 1;
unsigned long _lastBlinkTime = 0;

void Blink(int intervall, int pin_LED) {
	if ((millis() - _lastBlinkTime) > intervall)
	{
		_lastBlinkTime = millis();
		if (_blink)
		{
			_blink = false;
			digitalWrite(pin_LED, HIGH);
		}
		else {
			_blink = true;
			digitalWrite(pin_LED, LOW);
		}
	}
}
#endif // DEBUG