/*
 * motors.cpp
 *
 * Created: 20.04.2018 13:51:07
 *  Author: kamil
 */ 
#ifndef MOTORS_H
#define MOTORS_H

#define  F_CPU    32000000UL
#include "pid.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <stddef.h>

#define R_ENKODER TCF0.CNT //zwraca stan enkodera silnik R
#define L_ENKODER TCC0.CNT //zwraca stan enkodera silnik L


class MotorR
{
	PID pid;
	int16_t speed;
	
	void set_PWM(const int16_t &pulse_width);
	
	
	public:
	MotorR(const int16_t &_speed = 0);
	~MotorR() {};
	void set_speed(const int16_t &_speed);
	void set_pid(const uint8_t &Kp, const uint8_t &Kd, const uint8_t &Ki);
	void update();
	
		
};

class MotorL
{
	PID pid;
	int16_t speed;
	
	void set_PWM(const int16_t &pulse_width);
	
	
	public:
	MotorL(const int16_t &_speed = 0);
	~MotorL() {};
	void set_speed(const int16_t &_speed);
	void set_pid(const uint8_t &Kp, const uint8_t &Kd, const uint8_t &Ki);
	void update();
	
};


class Motors
{
	public:
	MotorR motorR;
	MotorL motorL;
	
	
	Motors();
	~Motors() {};
};

#endif