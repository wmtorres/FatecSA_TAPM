/******************************************************************************
 * Nome do Arquivo:	Hardware.c
 * Dependencias:	
 * Processador:		PIC18F4550
 * Opção de Clock:	HS 20MHz (externo) - CPU: 48MHz
 * Compilador:		XC8 
 *
 * Autor:			Weslley M. Torres
 *
 * Data:			v1 - 05/May/2019 - Initial revision 
 *
 * Descrição:       Realiza a configuração dos perifericos internos
 * do microcontrolador PIC18F4550
 * 
 *	Notas:
 *	
 *****************************************************************************/
#include "Config.h"
#include "Hardware.h"

/**
 *  \brief ConfigTimer0
 *  
 *  \return none
 *  
 *  \details Configure timer0 registers
 */
void ConfigTimer0(void)
{
    T0CON   = 0b11000101;
    INTCON  = 0b11100000;
    INTCON2 = 0b10000000;
    INTCON3 = 0b00000000;
    SPPCON  = 0b00000000;
    SPPCFG  = 0b00000000;
    ADCON0  = 0b00000000;
    ADCON1  = 0b00001111;
    TMR0    = TIMER0_INIT_VALUE;  //Set initial value of TMR0 register (for counting 1ms, as we need)

}

/**
 *  \brief Init_Ports
 *  
 *  \return none
 *  
 *  \details Initialize ports
 */
void Init_Ports(void)
{
    TRISB = 0x00;            //configure all portb as output
	LATB  = 0xFF;

    /* Inicializa portas conectadas ao CI 74HC595 */
    TRISE = 0x07;
    
    TRISA = 0x00;
    
}

void InitHardware(void)
{
    /* Initialize ports */
	Init_Ports();	
    
	/* Configure Timer0 */
	ConfigTimer0();

}