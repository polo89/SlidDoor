/* 
	Pin Mapping for SlidDoor 

	modified 01 Apr 2017
	by Frank Bruhn
*/

#ifndef Pins_SlidDoor_h
#define Pins_SlidDoor_h

#ifdef __AVR_ATmega328P__

#define PIN_EMERGENCYSTOP 0
#define PIN_MOTOR_EN 1
#define PIN_BRAKE 2
#define PIN_MOTOR_ENC2 3
#define PIN_MOTOR_ENC1 4
#define PIN_LOCK_DIR 5
#define PIN_LOCK_TRIGGER 6
#define PIN_RADAR_OUTDOOR 7
#define PIN_MOTOR_DIR 8
#define PIN_MOTOR_PWM 9
#define PIN_LED 10
#define PIN_RADAR_INDOOR 11
#define PIN_THRUBEAM 12
#define PIN_LOCK_STATE 13

#define PIN_MODE1 A0
#define PIN_MODE2 A1
#define PIN_MODE3 A2
#define PIN_REED_CLOSE A3
#define PIN_BUTTON_LOCK A4
#define PIN_MOTOR_CURR A5

#else

#define PIN_EMERGENCYSTOP	21 //PC5
#define PIN_MOTOR_EN		22 //PC6
#define PIN_BRAKE			23 //PC7
#define PIN_MOTOR_ENC1		1  //PB1
#define PIN_MOTOR_ENC2		2  //PB2	
#define PIN_LOCK_DIR		3  //PB3
#define PIN_LOCK_TRIGGER	4  //PB4
#define PIN_RADAR_OUTDOOR	10 //PD2
#define PIN_MOTOR_DIR		11 //PD3
#define PIN_MOTOR_PWM		13 //PD5
#define PIN_LED				12 //PD4
#define PIN_RADAR_INDOOR	14 //PD6
#define PIN_THRUBEAM		15 //PD7
#define PIN_LOCK_STATE		16 //PC0

#define PIN_MODE1			17 //PC1
#define PIN_MODE2			18 //PC2
#define PIN_MODE3			19 //PC3
#define PIN_REED_CLOSE		20 //PC4
#define PIN_BUTTON_LOCK		A0 //PA0
#define PIN_MOTOR_CURR		A1 //PA1

#endif

#endif