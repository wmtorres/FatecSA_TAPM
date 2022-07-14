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

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef HARDWARE_H
#define	HARDWARE_H

void Init_Ports(void);

void ConfigTimer0(void);

void InitHardware(void);



#define	SDA_EEPROM_PORT									PORTAbits.RA4
#define	SDA_EEPROM_PORT_MODE							TRISAbits.RA4
#define	_SET_SDA_EEPROM_PORT							SDA_EEPROM_PORT = 1
#define	_CLEAR_SDA_EEPROM_PORT							SDA_EEPROM_PORT = 0
#define	_CONFIGURE_SDA_EEPROM_OUTPUT_PORT				SDA_EEPROM_PORT_MODE = 0
#define	_CONFIGURE_SDA_EEPROM_INPUT_PORT				SDA_EEPROM_PORT_MODE = 1

#define	SCL_EEPROM_PORT									PORTAbits.RA3
#define	SCL_EEPROM_PORT_MODE							TRISAbits.RA3
#define	_SET_SCL_EEPROM_PORT							SCL_EEPROM_PORT = 1
#define	_CLEAR_SCL_EEPROM_PORT							SCL_EEPROM_PORT = 0
#define	_CONFIGURE_SCL_EEPROM_OUTPUT_PORT				SCL_EEPROM_PORT_MODE = 0
#define	_CONFIGURE_SCL_EEPROM_INPUT_PORT				SCL_EEPROM_PORT_MODE = 1


#define CHAVE_B1                                        PORTEbits.RE0

#endif	/* HARDWARE_H */

