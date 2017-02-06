#include "Motor.h"

Motor::Motor(double *position,
	int pin_pwm, int pin_dir,
	int pin_currSens,
	double kp,
	double ki,
	double kd
) :
	_currSens(pin_currSens),
	_pid(position, &_output, &_setpoint, kp, ki, kd, DIRECT)
{
	_position = position;
	_pin_pwm = pin_pwm;
	_pin_dir = pin_dir;
	_pin_currSens = pin_currSens;
}

void Motor::Setup() {
	_pid.SetMode(AUTOMATIC);
	_setpoint = 0;
	_pid.SetOutputLimits(50.0, 255.0);
	pinMode(_pin_pwm, OUTPUT);
	pinMode(_pin_dir, OUTPUT);
}

void Motor::Compute() {
	_pid.Compute();
	Serial.print("PID X:");
	Serial.print(*_position);
	Serial.print(" W:");
	Serial.print(_setpoint);
	Serial.print(" Y:");
	Serial.print(_output); 
}

byte Motor::GetState() {

}

void Motor::Learn() {

}

void Motor::Open(int setpoint) {
	_pid.SetMode(AUTOMATIC);
	_pid.SetControllerDirection(DIRECT);
	_pid.Compute();
	_setpoint = setpoint;
	clockwise(_output);
}

void Motor::Close() {
	_pid.SetMode(AUTOMATIC);
	_pid.SetControllerDirection(REVERSE);
	_pid.Compute();
	_setpoint = 0;
	counterClockwise(_output);
}

void Motor::Stop() {
	_pid.SetMode(MANUAL);
	_output = 0;
	digitalWrite(_pin_dir, LOW);
	analogWrite(_pin_pwm, 0);
}

void Motor::counterClockwise(int pwm) {
	digitalWrite(_pin_dir, LOW);
	analogWrite(_pin_pwm, pwm);
}

void Motor::clockwise(int pwm) {
	digitalWrite(_pin_dir, HIGH);
	pwm = abs(pwm - 255);
	analogWrite(_pin_pwm, pwm);
}

