#ifndef _MOTOR_h
#define _MOTOR_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include <PID_v1.h>
#include "CurrSens.h"
#include <TimerOne.h>

enum MOTOR_STATES
{
	MOTOR_STOPPED,
	MOTOR_RUNNING,
	MOTOR_CW,
	MOTOR_CCW,
};

class Motor
{
private:
	double *_position;
	int _pin_pwm;
	int _pin_dir;
	int _pin_en;
	int _pin_currSens;
	double _output;
	double _setpoint;
	unsigned long _lastCalcSpeedTime;
	double _lastCalcSpeedPosition;
	MOTOR_STATES _state;
	CurrSens _currSens;
	PID _pid;
	void clockwise(int);
	void counterClockwise(int);

public:
	Motor(double* pos, int pin_pwm, int pin_dir, int pin_en, int pin_currSens, double kp, double ki, double kd);
	double OpenPosition;
	double WinterPosition = OpenPosition / 2;
	void Setup();
	void Open(int);
	void Close();
	void Close(int);
	void Stop();
	bool Learn();
	bool CheckForObstacle(double maxCurrent);
	MOTOR_STATES GetState();
	double calcSpeed();
};

#endif

