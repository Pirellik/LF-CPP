/*
 * config.cpp
 *
 * Created: 20.04.2018 13:50:11
 *  Author: kamil
 */ 

#include "config.h"
#define F_CPU 32000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stddef.h>

void OscXtal(void) {
	// konfiguracja generatora kwarcowego
	OSC.XOSCCTRL	=	OSC_FRQRANGE_12TO16_gc |		// wyb�r kwarcu od 12 do 16 MHZ
	OSC_XOSCSEL_XTAL_16KCLK_gc;		// czas na uruchomienie generatora
	OSC.CTRL		=	OSC_XOSCEN_bm;					// uruchomienie generatora kwarcowego
	// czekanie na ustabilizowanie si� generatora
	for(uint8_t i=0; i<255; i++) {
		if(OSC.STATUS & OSC_XOSCRDY_bm) {
			_delay_ms(20);
			CPU_CCP			=	CCP_IOREG_gc;			// odblokowanie zmiany �r�d�a sygna�u zegarowego
			CLK.CTRL		=	CLK_SCLKSEL_XOSC_gc;	// wyb�r �r�d�a sygna�u zegarowego na XTAL 16MHz
			// uk�ad nadzoruj�cy kwarc
			CPU_CCP			=	CCP_IOREG_gc;			// odblokowanie modyfikacji wa�nych rejestr�w
			OSC.XOSCFAIL	=	OSC_XOSCFDEN_bm;		// w��czenie uk�adu detekcji b��du sygna�u zegarowego
			
			return;										// wyj�cie z funkcji je�li generator si� uruchomi�
		}
		_delay_us(10);
	}
}

void Osc32MHz(void) {
	OSC.CTRL     =    OSC_RC32MEN_bm;       // w��czenie oscylatora 32MHz
	while(!(OSC.STATUS & OSC_RC32MRDY_bm)); // czekanie na ustabilizowanie si� generatora
	CPU_CCP      =    CCP_IOREG_gc;         // odblokowanie zmiany �r�d�a sygna�u
	CLK.CTRL     =    CLK_SCLKSEL_RC32M_gc; // zmiana �r�d�a sygna�u na RC 32MHz
	_delay_ms(1000);
}

void set_main_interrupts()
{
	//==============================	timery		=========================================================
	TCD1.INTCTRLA     =    TC_OVFINTLVL_LO_gc;         // przepe�nienie ma generowa� przerwanie LO
	TCD1.CTRLB        =    TC_WGMODE_NORMAL_gc;        // tryb normalny
	TCD1.PER = 192;
	
	TCC1.INTCTRLA     =    TC_OVFINTLVL_MED_gc;         // przepe�nienie ma generowa� przerwanie LO
	TCC1.CTRLB        =    TC_WGMODE_NORMAL_gc;        // tryb normalny
	TCC1.PER = 192;
	
	TCD1.CTRLA        =    TC_CLKSEL_DIV1024_gc;
	TCC1.CTRLA        =    TC_CLKSEL_DIV1024_gc;
}

void pmic_init(void) 
{
	
	//////////////////////////////////////////////////////////////////////
	//PMIC.CTRL
	//     7       6       5       4       3        2         1         0
	// | RREN  | IVSEL |   -   |   -   |   -   | HILVLEN | MEDLVLEN | LOLVLEN |
	//     0       0       0       0       0        0         0         0
	// Enable all interrupt levels
	// Load Int vectors to application section of flash
	// Set interrupt priority to Static (lower address = higher priority)
	PMIC.CTRL = PMIC_HILVLEN_bm | PMIC_MEDLVLEN_bm | PMIC_LOLVLEN_bm; //0x07
	//////////////////////////////////////////////////////////////////////
	
}