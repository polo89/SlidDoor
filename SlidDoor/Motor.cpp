#include "Motor.h"

Motor::Motor(double *position,
	int pin_pwm, int pin_dir,
	int pin_currSens,
	double kp,
	double ki,
	double kd
) :
	_currSens(pin_currSens),
	_pid(_position, &_output, &_setpoint, kp, ki, kd, DIRECT)
{
	_position = position;
	_pin_pwm = pin_pwm;
	_pin_dir = pin_dir;
	_pin_currSens = pin_currSens;
}

void Motor::Setup() {
	pinMode(_pin_pwm, OUTPUT);
	pinMode(_pin_dir, OUTPUT);
}

byte Motor::GetState() {

}

void Motor::Learn() {

}

void Motor::Open() {
	_setpoint = 2000;
	_pid.Compute();
	counterClockwise(_output);
}

void Motor::Close() {
	_setpoint = 2000;
	_pid.Compute();
	counterClockwise(_output);
}

void Motor::clockwise(int pwm) {
	digitalWrite(_pin_dir, HIGH);
	analogWrite(_pin_pwm,pwm);
}

void Motor::counterClockwise(int pwm) {
	digitalWrite(_pin_dir, LOW);
	pwm = abs(pwm - 255);
	analogWrite(_pin_pwm, pwm);
}

