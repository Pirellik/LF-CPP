/*
 * pid.cpp
 *
 * Created: 20.04.2018 14:13:18
 *  Author: kamil
 */ 

#ifndef PID_H
#define PID_H

#define  F_CPU    32000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <stddef.h>

class PID
{
	private:
	int16_t set_point,
			process_value,
			error,
			last_error,
			integrated_error;
			
	uint8_t Kp,
			Kd,
			Ki;
			
	public:
	PID(int16_t = 0, uint8_t = 0, uint8_t = 0, uint8_t = 0);
	~PID() {};
	void set_gains(const uint8_t &_Kp, const uint8_t &_Kd, const uint8_t &_Ki);
	void update_process_value(const int16_t &_process_value);
	void new_set_point(const int16_t &);
	int16_t get_correction();
				
				
};

#endif