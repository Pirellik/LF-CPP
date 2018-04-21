/*
 * sensors.cpp
 *
 * Created: 20.04.2018 15:24:54
 *  Author: kamil
 */ 

#ifndef SENSORS_H
#define SENSORS_H

#define F_CPU 32000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stddef.h>

class Sensors
{
	int8_t voltage_treshold; //próg bieli
	//uint8_t adcread[8];
	uint8_t line[8]; //1<==>linia pod czujnikiem, 0<==>brak linii
	int8_t weights[8]; //wagi czujników	
	int8_t last_position; //poprzednia pozycja wzglêdem linii
	int8_t position; //aktualna pozycja wzglêdem linii
	bool line_lost;
	
	uint8_t ReadCalibrationByte(uint8_t index);
	void ADC_CH_Init(ADC_CH_t &adcch, register8_t muxpos);
	
	
	public:
	
	Sensors();
	const bool &is_line_lost(){ return line_lost; }
	void ADC_read(ADC_CH_t &adcch, const uint8_t &i);
	const int16_t &calculate_position();
};

#endif