/*
 * motors.cpp
 *
 * Created: 20.04.2018 13:50:52
 *  Author: kamil
 */ 


#define  F_CPU    32000000UL
#include "motors.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <stddef.h>

Motors::Motors()
{
	//------------ustawienia silnika 1
	PORTD.DIRSET	=	PIN5_bm|//inpu2
	PIN4_bm|//input1
	PIN3_bm;//pwm
	PORTD.OUTCLR	=	PIN5_bm;//input 1->0
	PORTD.OUTCLR	=	PIN4_bm;//input 2->0
	//------------ustawienia silnika 2
	PORTD.DIRSET	=	PIN2_bm|//inpu2
	PIN1_bm|//input1
	PIN0_bm;//pwm
	PORTD.OUTCLR	=	PIN1_bm;//input 1->0
	PORTD.OUTCLR	=	PIN2_bm;//input 2->0
	
	TCD0.CTRLB		=	TC_WGMODE_SINGLESLOPE_gc|		// pwm singleslope
	TC0_CCDEN_bm|
	TC0_CCAEN_bm;
	TCD0.PER		=	500;
	TCD0.CCD		=	0;
	TCD0.CCA		=	0;
	TCD0.CTRLA		=	TC_CLKSEL_DIV1_gc;
	
	//-------------------------------	enkodera R	------------------------------------------------------
	PORTC.PIN0CTRL		=	PORT_ISC_LEVEL_gc | PORT_OPC_PULLUP_gc;
	PORTC.PIN1CTRL		=	PORT_ISC_LEVEL_gc | PORT_OPC_PULLUP_gc;
	EVSYS.CH0MUX		=   EVSYS_CHMUX_PORTC_PIN0_gc;        // pin C0 wywo³uje zdarzenie
	EVSYS.CH0CTRL		=   EVSYS_QDEN_bm|                    // w³¹czenie dekodera w systemie zdarzeñ
	EVSYS_DIGFILT_8SAMPLES_gc;        // filtr cyfrowy
	TCC0.CTRLA			=   TC_CLKSEL_EVCH0_gc;               // taktowanie systemem zdarzeñ
	TCC0.CTRLD			=   TC_EVACT_QDEC_gc |                // w³¹czenie dekodera kwadraturowego
	TC_EVSEL_CH0_gc;                  // dekoder zlicza impulsy z kana³u 0*/
	//-------------------------------	enkodera L	------------------------------------------------------
	PORTF.PIN0CTRL		=	PORT_ISC_LEVEL_gc | PORT_OPC_PULLUP_gc;
	PORTF.PIN1CTRL		=	PORT_ISC_LEVEL_gc | PORT_OPC_PULLUP_gc;
	EVSYS.CH2MUX		=   EVSYS_CHMUX_PORTF_PIN0_gc;        // pin E0 wywo³uje zdarzenie
	EVSYS.CH2CTRL		=   EVSYS_QDEN_bm|                    // w³¹czenie dekodera w systemie zdarzeñ
	EVSYS_DIGFILT_8SAMPLES_gc;        // filtr cyfrowy
	TCF0.CTRLA			=   TC_CLKSEL_EVCH2_gc;               // taktowanie systemem zdarzeñ
	TCF0.CTRLD			=   TC_EVACT_QDEC_gc |                // w³¹czenie dekodera kwadraturowego
	TC_EVSEL_CH2_gc;                  // dekoder zlicza impulsy z kana³u 0*/

	L_ENKODER=0;
	R_ENKODER=0;
}

MotorR::MotorR(const int16_t &_speed)
{
	speed = _speed;
}

MotorL::MotorL(const int16_t &_speed)
{
	speed = _speed;
}

void MotorR::set_speed(const int16_t &_speed)
{
	speed = _speed;
	pid.new_set_point(speed);
}

void MotorL::set_speed(const int16_t &_speed)
{
	speed = _speed;
	pid.new_set_point(speed);
}

void MotorR::set_pid(const uint8_t &Kp, const uint8_t &Kd, const uint8_t &Ki)
{
	pid.set_gains(Kp, Kd, Ki);
}

void MotorL::set_pid(const uint8_t &Kp, const uint8_t &Kd, const uint8_t &Ki)
{
	pid.set_gains(Kp, Kd, Ki);
}

void MotorR::update()
{
	pid.update_process_value(13 * R_ENKODER);
	set_PWM((speed + pid.get_correction()));
	R_ENKODER = 0;
}

void MotorL::update()
{
	pid.update_process_value((- 13 * L_ENKODER));
	set_PWM((speed + pid.get_correction()));
	L_ENKODER = 0;
}

void MotorR::set_PWM(const int16_t &pulse_width)
{
	if(pulse_width < 0)
		TCD0.CCD		=	- pulse_width;
	else
		TCD0.CCD		=	pulse_width;
	
	if(pulse_width == 0)
	{
		PORTD.OUTCLR	=	PIN5_bm;//input 1->0
		PORTD.OUTCLR	=	PIN4_bm;//input 2->0
	}
	else if(pulse_width > 0)
	{
		PORTD.OUTCLR	=	PIN5_bm;//input 1->0
		PORTD.OUTSET	=	PIN4_bm;//input 2->1
	}
	else
	{
		PORTD.OUTSET	=	PIN5_bm;//input 1->1
		PORTD.OUTCLR	=	PIN4_bm;//input 2->0
	}
}

void MotorL::set_PWM(const int16_t &pulse_width)
{
	if(pulse_width < 0)
		TCD0.CCA		=	- pulse_width;
	else
		TCD0.CCA		=	pulse_width;
	
	if(pulse_width == 0)
	{
		PORTD.OUTCLR	=	PIN1_bm;//input 1->0
		PORTD.OUTCLR	=	PIN2_bm;//input 2->0
	}
	else if(pulse_width < 0)
	{
		PORTD.OUTCLR	=	PIN1_bm;//input 1->0
		PORTD.OUTSET	=	PIN2_bm;//input 2->1
	}
	else
	{
		PORTD.OUTSET	=	PIN1_bm;//input 1->1
		PORTD.OUTCLR	=	PIN2_bm;//input 2->0
	}
}

