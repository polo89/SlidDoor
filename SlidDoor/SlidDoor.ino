/*
	Statemachine SlidDoor

	modified 19 Feb 2017
	by Frank Bruhn
*/

//#include "Buzzer.h"
#include <Button.h>
#include "Selector.h"
#include <EEPROM.h>
#include "DoorLock.h"
#include <Button\Button.h>
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
	STATE_UNLOCK,
	STATE_LEARN,
	STATE_MANUAL,
	STATE_LOCKERROR
};

double position = 0; //rising = clockwise
double obstaclePosition = -1;
byte currentState;
byte nextState;
byte lastState;
byte ActivMode;
byte mode;
Motor SlidDoor = Motor(&position, PIN_MOTOR_PWM, PIN_MOTOR_DIR, PIN_MOTOR_EN, PIN_MOTOR_CURR, 1.2, 0, 0); //TimerOne 1.2
Button RadarIndoor = Button(PIN_RADAR_INDOOR);
Button RadarOutdoor = Button(PIN_RADAR_OUTDOOR);
Button Thrubeam = Button(PIN_THRUBEAM);
Button LockState = Button(PIN_LOCK_STATE);
Button ButtonLock = Button(PIN_BUTTON_LOCK);
Button EmergencyStop = Button(PIN_EMERGENCYSTOP);
Button Reed_Close = Button(PIN_REED_CLOSE);
DoorLock Lock = DoorLock(PIN_LOCK_STATE, PIN_LOCK_TRIGGER, PIN_LOCK_DIR);
Selector Remote = Selector(PIN_MODE1, PIN_MODE2, PIN_MODE3, PIN_LED);

//Timings
unsigned long enterStateTime;
unsigned long lastUnlockTime;

//Request
bool lockStateRequest = false;

bool enterState = true;
bool forceLearn = false;
bool finishedLearn = false;

bool debug = true;
bool printLine = false;

void setup() {
//#ifndef __AVR_ATmega328P__
//	#define DEBUG
//	Serial.begin(115200);
//#endif // __AVR_ATmega328P__

	#define DEBUG
	Serial.begin(9600);

	pinMode(PIN_MOTOR_ENC1, INPUT);
	pinMode(PIN_MOTOR_ENC2, INPUT);
	attachInterrupt(digitalPinToInterrupt(PIN_MOTOR_ENC2), Encoder, RISING);

	SlidDoor.Setup();

	currentState = STATE_LOCKED;
	nextState = STATE_LOCKED;

	SlidDoor.Stop();
	double eeOpenPosition;
	EEPROM.get(0, eeOpenPosition);
	if (Reed_Close.isPressed() || eeOpenPosition == 0)
	{
		currentState = STATE_LEARN;
		nextState = STATE_LEARN;
		forceLearn = true;
	}
	else
	{
		SlidDoor.OpenPosition = eeOpenPosition;
		if (Lock.GetState() == LOCKED)
		{
			currentState = STATE_LOCKED;
			nextState = STATE_LOCKED;
		}
		else
		{
			currentState = STATE_CLOSED;
			nextState = STATE_CLOSED;
		}
	}
	SlidDoor.Stop();
	enterStateTime = millis();
#ifdef DEBUG
	Serial.println("Setup Finished!");
	if (currentState == STATE_CLOSED)	Serial.print("STATE_CLOSED   ");
	if (currentState == STATE_CLOSING)	Serial.print("STATE_CLOSING  ");
	if (currentState == STATE_LOCKED)	Serial.print("STATE_LOCKED   ");
	if (currentState == STATE_OBSTACLE) Serial.print("STATE_OBSTACLE ");
	if (currentState == STATE_OPEN)		Serial.print("STATE_OPEN     ");
	if (currentState == STATE_OPENING)	Serial.print("STATE_OPENING  ");
	if (currentState == STATE_UNLOCK)	Serial.print("STATE_UNLOCK   ");
	if (currentState == STATE_UNKNOWN)	Serial.print("STATE_UNKNOWN  ");
	if (currentState == STATE_MANUAL)	Serial.print("STATE_MANUAL   ");
	if (currentState == STATE_LEARN)	Serial.print("STATE_LEARN    ");
	if (currentState == STATE_LOCKERROR)Serial.print("STATE_LOCKERROR");
#endif // DEBUG

#pragma region TestSensors
	//while (1) {
	//	Serial.print("Position: ");
	//	Serial.print(position);
	//	Serial.print("\t");
	//	Serial.print("MotorCurr: ");
	//	Serial.print(analogRead(PIN_MOTOR_CURR));
	//	Serial.print("\t");
	//	Serial.print("RadarIndoor: ");
	//	Serial.print(digitalRead(PIN_RADAR_INDOOR));
	//	Serial.print("\t");
	//	Serial.print("RadarOutdoor: ");
	//	Serial.print(digitalRead(PIN_RADAR_OUTDOOR));
	//	Serial.print("\t");
	//	Serial.print("Thrubeam: ");
	//	Serial.print(digitalRead(PIN_THRUBEAM));
	//	Serial.print("\t");
	//	Serial.print("LockState: ");
	//	Serial.print(digitalRead(PIN_LOCK_STATE));
	//	Serial.print("\t");
	//	Serial.print("ButtonLock: ");
	//	Serial.print(digitalRead(PIN_BUTTON_LOCK));
	//	Serial.print("\t");
	//	Serial.print("EStop: ");
	//	Serial.print(digitalRead(PIN_EMERGENCYSTOP));
	//	Serial.print("\t");
	//	Serial.print("ReedClose: ");
	//	Serial.print(digitalRead(PIN_REED_CLOSE));
	//	Serial.print("\t");
	//	Serial.print("Remote: ");
	//	Serial.print(Remote.GetMode());
	//	Serial.println();
	//}
#pragma endregion

}


void loop() {
	mode = Remote.GetMode();

#pragma region EmergencyStop
	while (EmergencyStop.isPressed())
	{
		SlidDoor.Stop();
		Remote.SetLedState(LEDSTATE_MOTOR_MCB);
	}
#pragma endregion                             

	switch (currentState)
	{
	case STATE_OPEN: {
		Remote.SetLedState(LEDSTATE_NORMAL);
		SlidDoor.Stop();
		if (!RadarIndoor.isPressed() || !Thrubeam.isPressed()) enterStateTime = millis();
		if ((millis() - enterStateTime) > 1000 && (mode != MODE_OPEN || lockStateRequest))
		{
			nextState = STATE_CLOSING;
		}
		if (mode == MODE_MANUAL) nextState = STATE_MANUAL;
	} break;
	case STATE_OPENING: {
		Remote.SetLedState(LEDSTATE_NORMAL);
		if (position <= SlidDoor.OpenPosition-10 && ActivMode != MODE_WINTER)
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
		Remote.SetLedState(LEDSTATE_NORMAL);
		SlidDoor.Stop();
		if (millis() - enterStateTime > 500 && ((!RadarIndoor.isPressed() && mode != MODE_ONEWAY) || mode == MODE_OPEN))
		{
			nextState = STATE_OPENING;
		}
		if (mode == MODE_MANUAL) nextState = STATE_MANUAL;
		if (mode == MODE_LEARN) nextState = STATE_LEARN;
		if (lockStateRequest || ButtonLock.isPressed() || mode == MODE_LOCK) nextState = STATE_LOCKED;
	} break;
	case STATE_CLOSING: {
		Remote.SetLedState(LEDSTATE_NORMAL);
		if (obstaclePosition != -1 && false) {
			SlidDoor.Close(obstaclePosition - 100);
			if (position <= (obstaclePosition - 200)) {
				obstaclePosition = -1;
				enterStateTime = millis();
			}
		}
		else if (position >= 5) SlidDoor.Close();
		else nextState = STATE_CLOSED;
		if (!RadarIndoor.isPressed() || !Thrubeam.isPressed()) nextState = STATE_OBSTACLE;
		if (SlidDoor.CheckForObstacle(1.5) && (millis() - enterStateTime) > 200) {
			obstaclePosition = position;
			nextState = STATE_OBSTACLE;
		}
	} break;
	case STATE_OBSTACLE: {
		Remote.SetLedState(LEDSTATE_NORMAL);
		SlidDoor.Stop();
		if (millis() - enterStateTime > 400) nextState = STATE_OPENING;
	} break;
	case STATE_LOCKED: {
		Remote.SetLedState(LEDSTATE_NORMAL_LOCKED);
		SlidDoor.Stop();
		Lock.Lock();
		lockStateRequest = false;
		if (ButtonLock.isPressed()) {
			Serial.println("Button pressed!");
			break;
		}
		if (millis() - enterStateTime > 400) {
			nextState = STATE_UNLOCK;
		};
	} break;
	case STATE_UNLOCK: {
		Remote.SetLedState(LEDSTATE_NORMAL_LOCKED);
		SlidDoor.Stop();
		if (millis() - enterStateTime > 400 && (ButtonLock.isPressed() || (Remote.ModeChange() && mode != MODE_LOCK))) {
			Lock.Unlock();
			delay(500);
			nextState = STATE_CLOSED;
			lastUnlockTime = millis();
		};
	} break;
	case STATE_LEARN: {
		if (finishedLearn) Remote.SetLedState(LEDSTATE_NORMAL);
		else Remote.SetLedState(LEDSTATE_MANUAL_LEARN);
		
		SlidDoor.Stop();
		if ((millis() - enterStateTime > 3000 || forceLearn) && !finishedLearn) {
			if (!SlidDoor.Learn()) {
#ifdef DEBUG
				Serial.println("Lernen nicht erfolgreich!");
#endif // DEBUG
				while (true) {
					SlidDoor.Stop();
				};
			};
			forceLearn = false;
			finishedLearn = true;
			EEPROM.put(0, SlidDoor.OpenPosition);
		}
		else if (mode != MODE_LEARN && finishedLearn)
		{
			nextState = STATE_CLOSED;
		}
	} break;
	case STATE_MANUAL: {
		Remote.SetLedState(LEDSTATE_MANUAL_LEARN);
		SlidDoor.Stop();
		if (mode == MODE_LEARN) nextState = STATE_LEARN;
		if (mode == MODE_LOCK) nextState = STATE_OPEN;
		if (mode == MODE_ONEWAY) nextState = STATE_OPEN;
		if (mode == MODE_OPEN) nextState = STATE_OPENING;
		if (mode == MODE_SUMMER) nextState = STATE_OPEN;
		if (mode == MODE_WINTER) nextState = STATE_OPEN;
		if (mode == MODE_UNKNOWN) nextState = STATE_MANUAL;
	} break;
	case STATE_LOCKERROR: {
		Remote.SetLedState(LEDSTATE_LOCK_ERROR);
		SlidDoor.Stop();
		if (Lock.GetState() != LOCKED) {
			if (millis() - lastUnlockTime > 2000)
			{
				nextState = lastState;
			}
		}
		else lastUnlockTime = millis();
	} break;
	default:
		nextState = lastState;
		break;
	}

#pragma region LockStateRequest
	if (ButtonLock.isPressed() && (currentState != STATE_LOCKED || currentState != STATE_UNLOCK || currentState != STATE_CLOSED)) {
		if (millis() - lastUnlockTime > 2000) lockStateRequest = true;
		else lockStateRequest = false;
	}
#pragma endregion

#pragma region LockErrorDetection
	if (currentState != STATE_LOCKED && currentState != STATE_UNLOCK && Lock.GetState() == LOCKED) nextState = STATE_LOCKERROR;
#pragma endregion

#pragma region StateChange
	if (currentState != nextState) {

#ifdef DEBUG
		if (currentState == STATE_CLOSED)	Serial.print("STATE_CLOSED   ");
		if (currentState == STATE_CLOSING)	Serial.print("STATE_CLOSING  ");
		if (currentState == STATE_LOCKED)	Serial.print("STATE_LOCKED   ");
		if (currentState == STATE_OBSTACLE) Serial.print("STATE_OBSTACLE ");
		if (currentState == STATE_OPEN)		Serial.print("STATE_OPEN     ");
		if (currentState == STATE_OPENING)	Serial.print("STATE_OPENING  ");
		if (currentState == STATE_UNLOCK)	Serial.print("STATE_UNLOCK   ");
		if (currentState == STATE_UNKNOWN)	Serial.print("STATE_UNKNOWN  ");
		if (currentState == STATE_MANUAL)	Serial.print("STATE_MANUAL   ");
		if (currentState == STATE_LEARN)	Serial.print("STATE_LEARN    ");
		if (currentState == STATE_LOCKERROR)Serial.print("STATE_LOCKERROR");
		Serial.print(" => ");
		if (nextState == STATE_CLOSED)		Serial.print("STATE_CLOSED   ");
		if (nextState == STATE_CLOSING)		Serial.print("STATE_CLOSING  ");
		if (nextState == STATE_LOCKED)		Serial.print("STATE_LOCKED   ");
		if (nextState == STATE_OBSTACLE)	Serial.print("STATE_OBSTACLE ");
		if (nextState == STATE_OPEN)		Serial.print("STATE_OPEN     ");
		if (nextState == STATE_OPENING)		Serial.print("STATE_OPENING  ");
		if (nextState == STATE_UNLOCK)		Serial.print("STATE_UNLOCK   ");
		if (nextState == STATE_UNKNOWN)		Serial.print("STATE_UNKNOWN  ");
		if (nextState == STATE_MANUAL)		Serial.print("STATE_MANUAL   ");
		if (nextState == STATE_LEARN)		Serial.print("STATE_LEARN    ");
		if (nextState == STATE_LOCKERROR)	Serial.print("STATE_LOCKERROR");
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
		lastState = currentState;
		currentState = nextState;
		enterStateTime = millis();
		enterState = true;
		ActivMode = mode;
	}
	else enterState = false;
#pragma endregion

#pragma region ModeChangeDebugPrint
#ifdef DEBUG
	if (Remote.ModeChange())
	{

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
#endif // DEBUG
#pragma endregion
}

void Encoder() {
	if (digitalRead(PIN_MOTOR_ENC1)) position -= 1.0; //CW
	else position += 1.0; //CCW
}