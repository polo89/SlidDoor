#include "Motor.h"

Motor::Motor(double *position,
	int pin_pwm, int pin_dir,
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
	_pin_currSens = pin_currSens;
}

void Motor::Setup() {
	_currSens.Calibrate();
	_pid.SetMode(AUTOMATIC);
	_setpoint = 0;
	_pid.SetOutputLimits(50.0, 255.0);
	pinMode(_pin_pwm, OUTPUT);
	pinMode(_pin_dir, OUTPUT);
}

byte Motor::GetState() {

}

void Motor::Compute() {
	//Serial.println(*_position);
}

bool Motor::Learn() {
	Serial.println("Starte Lernmodus ...");
	unsigned long startTime = millis();
	while (true)
	{
		clockwise(60);
		if ((_currSens.Read() > 1.00) && (startTime + 100 < millis()))
		{
			Stop();
			OpenPosition = *_position;
			Serial.println(*_position);
			break;
		}
	}
	delay(1000);
	startTime = millis();
	while (true)
	{
		counterClockwise(60);
		if ((_currSens.Read() > 1.00) && (startTime + 100 < millis()))
		{
			Stop();
			if (*_position > 10) return false;
			OpenPosition = abs(OpenPosition - *_position);
			Serial.println(*_position);
			break;
		}
	}
	*_position = 0;
	Serial.print("Öffnungsweg: ");
	Serial.println(OpenPosition);
	return true;
}

void Motor::Open(int setpoint) {
	_pid.SetMode(AUTOMATIC);
	_pid.SetControllerDirection(DIRECT);
	_pid.Compute();
	OpenPosition;
	_setpoint = setpoint;
	_currSens.Read();
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

