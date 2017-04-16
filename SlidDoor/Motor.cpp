#include "Motor.h"
#define DEBUG 1

Motor::Motor(double *position,
	int pin_pwm, int pin_dir, int pin_en,
	int pin_currSens,
	double kp,
	double ki,
	double kd
) :
	_currSens(pin_currSens, 100),
	_pid(position, &_output, &_setpoint, kp, ki, kd, DIRECT)
{
	_position = position;
	_pin_pwm = pin_pwm;
	_pin_dir = pin_dir;
	_pin_en = pin_en;
	_pin_currSens = pin_currSens;
	_lastCalcSpeedPosition = *_position;
	_state = MOTOR_STOPPED;
}

void Motor::Setup() {
	_currSens.Calibrate();
	_pid.SetMode(AUTOMATIC);
	_setpoint = 0;
	_pid.SetOutputLimits(50.0, 255.0);
	pinMode(_pin_en, OUTPUT);
	pinMode(_pin_pwm, OUTPUT);
	pinMode(_pin_dir, OUTPUT);
}

MOTOR_STATES Motor::GetState() {
	return _state;
}

bool Motor::Learn() {
	
	unsigned long startTime = millis();
	while (true)
	{
		clockwise(60);
		if ((_currSens.Read() > 0.6) && (startTime + 100 < millis()))
		{
			Stop();
			OpenPosition = *_position;
			break;
		}
	}
	delay(1000);
	startTime = millis();
	while (true)
	{
		counterClockwise(255);
		if ((_currSens.Read() > 1.2) && (startTime + 100 < millis()))
		{
			Stop();
			if (*_position > 10) return false;
			OpenPosition = abs(OpenPosition - *_position);
			break;
		}
	}
	*_position = 0;
#ifdef DEBUG
	Serial.print("Oeffnungsweg: ");
	Serial.println(OpenPosition);
#endif // DEBUG
	return true;
}

void Motor::Open(int setpoint) {
	_setpoint = setpoint;
	_pid.SetMode(AUTOMATIC);
	_pid.SetControllerDirection(DIRECT);
	_pid.Compute();
	OpenPosition;
	_currSens.Read();
	clockwise(_output);
}

void Motor::Close() {
	Close(0);
}

void Motor::Close(int setpoint) {
	_setpoint = setpoint;
	_pid.SetMode(AUTOMATIC);
	_pid.SetControllerDirection(REVERSE);
	_pid.Compute();
	counterClockwise(_output);
}

void Motor::Stop() {
	_state = MOTOR_STOPPED;
	_pid.SetMode(MANUAL);
	_output = 0;
	digitalWrite(_pin_en, LOW);
	digitalWrite(_pin_dir, LOW);
	analogWrite(_pin_pwm, 0);
}

void Motor::clockwise(int pwm) {
	_state = MOTOR_CW;
	digitalWrite(_pin_en, HIGH);
	digitalWrite(_pin_dir, LOW);
	analogWrite(_pin_pwm, pwm);
}

void Motor::counterClockwise(int pwm) {
	_state = MOTOR_CCW;
	digitalWrite(_pin_en, HIGH);
	digitalWrite(_pin_dir, HIGH);
	pwm = abs(pwm - 255);
	analogWrite(_pin_pwm, pwm);
}

bool Motor::CheckForObstacle(double maxCurrent) {
	if (_currSens.Read() > maxCurrent) return true;
	else return false;
}

double Motor::calcSpeed() {
	if (abs(*_position - _lastCalcSpeedPosition) > 50)
	{
		double _speed = ((*_position - _lastCalcSpeedPosition) / 1909.0) / ((millis() - _lastCalcSpeedTime) / 100000.0);
		_lastCalcSpeedTime = millis();
		_lastCalcSpeedPosition = *_position;
		return _speed;
	}
}

