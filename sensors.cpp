/*
 * sensors.cpp
 *
 * Created: 20.04.2018 15:24:30
 *  Author: kamil
 */ 

#include "sensors.h"

void Sensors::ADC_CH_Init(ADC_CH_t &adcch, register8_t muxpos)// konfiguracja kana³u
{ 
	adcch.CTRL=ADC_CH_INPUTMODE_DIFFWGAIN_gc|ADC_CH_GAIN_DIV2_gc; // Ró¿nicowy sygna³ wejœciowy z wzmocnieniem 1/2
	adcch.MUXCTRL=muxpos|ADC_CH_MUXNEG_GND_MODE3_gc;										   // wybór MUX dla dodatniego wejœcia ADC
	adcch.INTCTRL=ADC_CH_INTLVL1_bm | ADC_CH_INTLVL0_bm;				   // Compare result above threshold s.363
}

uint8_t Sensors::ReadCalibrationByte(uint8_t index) 
{
	uint8_t result;
	NVM_CMD = NVM_CMD_READ_CALIB_ROW_gc;
	result = pgm_read_byte(index);
	
	NVM_CMD = NVM_CMD_NO_OPERATION_gc;
	
	return(result);
}

void Sensors::ADC_read(ADC_CH_t &adcch, const uint8_t &i)
{
	int8_t res = adcch.RES;
	//adcread[i] = res;
	if(res > voltage_treshold)
	{
		line[i] = 1;
	}
	else
	{
		line[i] = 0;
	}
	adcch.CTRL |= ADC_CH_START_bm;
}

Sensors::Sensors()
{
	line[0] = 0;
	line[1] = 0;
	line[2] = 0;
	line[3] = 0;
	line[4] = 0;
	line[5] = 0;
	line[6] = 0;
	line[7] = 0;
	weights[0] = -16;
	weights[1] = -9;
	weights[2] = -4;
	weights[3] = -1;
	weights[4] = 1;
	weights[5] = 4;
	weights[6] = 9;
	weights[7] = 16;
	
	voltage_treshold = 230;
	
	PORTA.DIRCLR =PIN3_bm|PIN4_bm|PIN5_bm|PIN6_bm;		// piny 3-6 PORTu A wejscia - transoptory
	PORTB.DIRCLR =PIN4_bm|PIN5_bm|PIN6_bm|PIN7_bm;      // piny 4-7 PORTu B wejscia - transoptory
	
	// Wy³aczrnie bufora cyfrowego na wszystkich pinach ADC
	PORTA.PIN3CTRL = PORT_ISC_INPUT_DISABLE_gc;
	PORTA.PIN4CTRL = PORT_ISC_INPUT_DISABLE_gc;
	PORTA.PIN5CTRL = PORT_ISC_INPUT_DISABLE_gc;
	PORTA.PIN6CTRL = PORT_ISC_INPUT_DISABLE_gc;
	
	PORTB.PIN4CTRL = PORT_ISC_INPUT_DISABLE_gc;
	PORTB.PIN5CTRL = PORT_ISC_INPUT_DISABLE_gc;
	PORTB.PIN6CTRL = PORT_ISC_INPUT_DISABLE_gc;
	PORTB.PIN7CTRL = PORT_ISC_INPUT_DISABLE_gc;
	
	ADCA.CTRLB = ADC_IMPMODE_bm|ADC_CURRLIMIT_NO_gc|ADC_RESOLUTION_8BIT_gc; // s.352	Tryb impedancji trybu wzmocnienia | No limit | 8-bitowa rozdzielczosc
	ADCB.CTRLB = ADC_IMPMODE_bm|ADC_CURRLIMIT_NO_gc|ADC_RESOLUTION_8BIT_gc; // s.352	Tryb impedancji trybu wzmocnienia | No limit | 8-bitowa rozdzielczosc
	
	ADCA.REFCTRL = ADC_REFSEL_INTVCC_gc; // napecie odniesienia VCC/1.6
	_delay_us(3);
	ADCB.REFCTRL = ADC_REFSEL_INTVCC_gc; // napecie odniesienia VCC/1.6
	_delay_us(3);
	
	ADCA.PRESCALER = ADC_PRESCALER_DIV128_gc; // prescaler 250 000 Hz
	ADCB.PRESCALER = ADC_PRESCALER_DIV128_gc; // prescaler 250 000 Hz
	
	ADCA.CALL = ReadCalibrationByte( offsetof(NVM_PROD_SIGNATURES_t, ADCACAL0) );
	ADCA.CALH = ReadCalibrationByte( offsetof(NVM_PROD_SIGNATURES_t, ADCACAL1) );
	ADCA.CALL = ReadCalibrationByte( offsetof(NVM_PROD_SIGNATURES_t, ADCACAL0) );
	ADCA.CALH = ReadCalibrationByte( offsetof(NVM_PROD_SIGNATURES_t, ADCACAL1) );
	
	ADCB.CALL = ReadCalibrationByte( offsetof(NVM_PROD_SIGNATURES_t, ADCBCAL0) );
	ADCB.CALH = ReadCalibrationByte( offsetof(NVM_PROD_SIGNATURES_t, ADCBCAL1) );
	ADCB.CALL = ReadCalibrationByte( offsetof(NVM_PROD_SIGNATURES_t, ADCBCAL0) );
	ADCB.CALH = ReadCalibrationByte( offsetof(NVM_PROD_SIGNATURES_t, ADCBCAL1) );
	

	ADCA.CTRLA = ADC_DMASEL_OFF_gc; // No combined DMA req s.352
	ADCB.CTRLA = ADC_DMASEL_OFF_gc; // No combined DMA req s.352
	
	//ADCB.CH0.CTRL = ADC_CH_INPUTMODE_DIFFWGAIN_gc|ADC_CH_GAIN_DIV2_gc;	// Ró¿nicowy sygna³ wejœciowy z wzmocnieniem 1/2
	//ADCB.CH0.MUXCTRL = ADC_CH_MUXPOS_PIN4_gc|ADC_CH_MUXNEG_GND_MODE3_gc;	// wybór MUX dla dodatniego wejœcia ADC | masa PAD GROUND
	ADC_CH_Init(ADCA.CH0, ADC_CH_MUXPOS_PIN3_gc);
	ADC_CH_Init(ADCA.CH1, ADC_CH_MUXPOS_PIN4_gc);
	ADC_CH_Init(ADCA.CH2, ADC_CH_MUXPOS_PIN5_gc);
	ADC_CH_Init(ADCA.CH3, ADC_CH_MUXPOS_PIN6_gc);
	
	ADC_CH_Init(ADCB.CH0, ADC_CH_MUXPOS_PIN4_gc);
	ADC_CH_Init(ADCB.CH1, ADC_CH_MUXPOS_PIN5_gc);
	ADC_CH_Init(ADCB.CH2, ADC_CH_MUXPOS_PIN6_gc);
	ADC_CH_Init(ADCB.CH3, ADC_CH_MUXPOS_PIN7_gc);
	
	
	ADCB.INTFLAGS = ADC_CH0IF_bm|ADC_CH1IF_bm|ADC_CH2IF_bm|ADC_CH3IF_bm; // ustawienie flagi
	ADCA.INTFLAGS = ADC_CH0IF_bm|ADC_CH1IF_bm|ADC_CH2IF_bm|ADC_CH3IF_bm;
	//ADCB.CH0.INTCTRL = ADC_CH_INTLVL1_bm | ADC_CH_INTLVL0_bm; // Compare result above threshold s.363
	ADCA.CTRLA = ADC_ENABLE_bm;	// zezwolenie na ADC
	ADCB.CTRLA = ADC_ENABLE_bm;	// zezwolenie na ADC
	
	ADCA.CH0.CTRL |= ADC_CH_START_bm;
	ADCA.CH1.CTRL |= ADC_CH_START_bm;
	ADCA.CH2.CTRL |= ADC_CH_START_bm;
	ADCA.CH3.CTRL |= ADC_CH_START_bm;
	ADCB.CH0.CTRL |= ADC_CH_START_bm;
	ADCB.CH1.CTRL |= ADC_CH_START_bm;
	ADCB.CH2.CTRL |= ADC_CH_START_bm;
	ADCB.CH3.CTRL |= ADC_CH_START_bm;
}

const int16_t &Sensors::calculate_position()
{
	last_position = position;
	position = 0;
	uint8_t number = 0;
	for(uint8_t i = 0; i < 8; i++)
	{
		number += line[i];
		position += weights[i]*line[i];
	}
	if(number != 0)
	{
		position = position / number;
		line_lost = 0;
	}
	if((last_position >= weights[6] || last_position <= weights[1]) && number == 0)
	{
		position = last_position;
		line_lost = 1;
	}
	return position;
}
