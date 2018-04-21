/*
 * cpp lf.cpp
 *
 * Created: 24.03.2018 16:01:40
 * Author : kamil
 */ 

#define  F_CPU    32000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <stddef.h>
#include "config.h"
#include "motors.h"
#include "pid.h"
#include "sensors.h"

Sensors sensors;
PID main_pid(0, 4, 9, 0);
Motors motors;
int16_t base_speed = 70;

int main(void)
{
	Osc32MHz();
	pmic_init();
	set_main_interrupts();
	
	motors.motorR.set_pid(2,2,0);
	motors.motorL.set_pid(2,2,0);
	motors.motorR.set_speed(base_speed);
	motors.motorL.set_speed(base_speed);

    while (1) {}
}

ISR(TCD1_OVF_vect)
{	
	motors.motorR.update();
	motors.motorL.update();
}

ISR(TCC1_OVF_vect)
{
	main_pid.update_process_value(sensors.calculate_position());
	int16_t corr = main_pid.get_correction();
	if(sensors.is_line_lost())
	{
		base_speed = 35;
	}
	else
	{
		base_speed = 70;
	}
	motors.motorR.set_speed((base_speed + corr));
	motors.motorL.set_speed((base_speed - corr));
}

ISR(ADCA_CH0_vect)
{
	sensors.ADC_read(ADCA.CH0, 0);
}
ISR(ADCA_CH1_vect)
{
	sensors.ADC_read(ADCA.CH1, 1);
}
ISR(ADCA_CH2_vect)
{
	sensors.ADC_read(ADCA.CH2, 2);
}
ISR(ADCA_CH3_vect)
{
	sensors.ADC_read(ADCA.CH3, 3);
}
ISR(ADCB_CH0_vect)
{
	sensors.ADC_read(ADCB.CH0, 4);
}

ISR(ADCB_CH1_vect)
{
	sensors.ADC_read(ADCB.CH1, 5);
}
ISR(ADCB_CH2_vect)
{
	sensors.ADC_read(ADCB.CH2, 6);
}
ISR(ADCB_CH3_vect)
{
	sensors.ADC_read(ADCB.CH3, 7);
}
ISR(OSC_OSCF_vect)// przerwanie w razie awarii oscylatora
{
	OSC.XOSCFAIL	|=	OSC_XOSCFDIF_bm;				// kasowanie flagi przerwania
}