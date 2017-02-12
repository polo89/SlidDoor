#ifndef _MOTOR_h
#define _MOTOR_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include <PID_v1.h>
#include "CurrSens.h"

#define DOOR_STATE_UNKNOWN 0
#define DOOR_STATE_OPEN 1
#define DOOR_STATE_CLOSE 2
#define DOOR_STATE_BETWEEN 3
#define DOOR_STATE_ISLEARNING 3

class Motor
{
private:
	double *_position;
	int _pin_pwm;
	int _pin_dir;
	int _pin_currSens;
	double _output;
	double _setpoint;
	CurrSens _currSens;
	PID _pid;
	void clockwise(int);
	void counterClockwise(int);

public:
	Motor(double*, int, int, int, double, double, double);
	double OpenPosition;
	void Setup();
	void Open(int);
	void Close();
	void Stop();
	bool Learn();
	void Compute();
	byte GetState();
};

#endif

