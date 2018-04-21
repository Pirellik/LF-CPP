/*
 * pid.cpp
 *
 * Created: 20.04.2018 14:13:06
 *  Author: kamil
 */ 

#define  F_CPU    32000000UL
#include "pid.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <stddef.h>

PID::PID(int16_t _set_point, uint8_t _Kp, uint8_t _Kd, uint8_t _Ki)
{
	set_point = _set_point;
	process_value = 0;
	error = 0;
	last_error = 0,
	integrated_error = 0;
	
	Kp = _Kp;
	Kd = _Kd;
	Ki = _Ki;
}

void PID::set_gains(const uint8_t &_Kp, const uint8_t &_Kd, const uint8_t &_Ki)
{
	Kp = _Kp;
	Kd = _Kd;
	Ki = _Ki;
}

void PID::new_set_point(const int16_t &_set_point)
{
	set_point = _set_point;
}

int16_t PID::get_correction()
{
	int16_t correction;
	
	error = set_point - process_value;
	if(error < 25)
	{
		if(integrated_error < 30000)
		integrated_error += error;
	}
	else
	{
		integrated_error = 0;
	}
	correction = Kp * error + Kd * (error - last_error) + Ki * integrated_error;
	last_error = error;
	return correction;
}

void PID::update_process_value(const int16_t &_process_value)
{
	process_value = _process_value;
}