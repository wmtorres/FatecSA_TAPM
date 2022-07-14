#define	EEPROM_C
#include "I2C_SW.h"
#undef EEPROM_C

unsigned char	eeprom_timer;
static unsigned char eeprom_fail;

/******************************************************************************
*	- Defines
******************************************************************************/
#define EECSW								0xa0
#define EECSR								0xa1
#define MAX_ADDRESS							0x800

/******************************************************************************
*	- Functions Prototype
******************************************************************************/

void delay_1us(void);
void delay_2us(void);
void delay_5us(void);
void delay_10us(void);
void read_eeprom_i2c_byte(unsigned char *rx_target);
void init_eeprom_i2c(void);
void send_eeprom_i2c_start(void);
void send_eeprom_i2c_stop(void);
unsigned char send_eeprom_i2c_byte(unsigned char tx_byte);
unsigned char eeprom_present(void);
/******************************************************************************
*	- I2C module body
******************************************************************************/
void delay_1us(void)
{
	unsigned char i;

	__nop();
	__nop();
	__nop();
	__nop();
	__nop();
	__nop();
}

void delay_2us(void)
{
	unsigned char i;

	for (i=0; i<3;i++)
	{
		__nop();
		__nop();
		__nop();
		__nop();
		__nop();
		__nop();
	}
}
void delay_5us(void)
{
	unsigned char i;

	for (i=0; i<10;i++)
	{
		__nop();
		__nop();
		__nop();
		__nop();
		__nop();
		__nop();
	}
}

void delay_10us(void)
{
	delay_5us();
	delay_5us();
}

/******************************************************************************
*	- This function reads one byte using i2c protocol. and store the
*	  value read into the variable pointed by rx_target
*	- Interrupts are allowed during TX sequence
******************************************************************************/
void read_eeprom_i2c_byte(unsigned char *rx_target)
{
	unsigned char i;
	unsigned char Data;
	Data=0x00;// Ensure start with blank byte
	_SET_SDA_EEPROM_PORT;				//set RB0 high
	_CONFIGURE_SDA_EEPROM_INPUT_PORT;;	//set TRISB0 as input
	for (i=8; i!=0; i--)
	{
		delay_1us();
		_SET_SCL_EEPROM_PORT;
		Data<<=1;
		delay_1us();
		if (SDA_EEPROM_PORT)
			Data |= 1;
		delay_1us();
		_CLEAR_SCL_EEPROM_PORT;
		delay_1us();
	}
	_SET_SDA_EEPROM_PORT;				//|Send NoACK to EEPROM
	_CONFIGURE_SDA_EEPROM_OUTPUT_PORT;	//|
	delay_1us();						//|
	_SET_SCL_EEPROM_PORT;				//|
	delay_2us();						//|
	_CLEAR_SCL_EEPROM_PORT;				//|
	delay_1us();						//|
	*rx_target = Data;
}

/******************************************************************************
*	- This function initilize i2c signals
*	- Verifiy if SDA line is released before send start+stop sequence
******************************************************************************/
void init_eeprom_i2c(void)
{
	char i;
	_CONFIGURE_SCL_EEPROM_OUTPUT_PORT;
	_SET_SCL_EEPROM_PORT;
	_CONFIGURE_SDA_EEPROM_INPUT_PORT;
	delay_2us();
	// Verify if SDA line is released
	for(i=0;i<10;i++)
	{
		if(SDA_EEPROM_PORT)
			break;
		_CLEAR_SCL_EEPROM_PORT;
		delay_2us();
		_SET_SCL_EEPROM_PORT;
		delay_2us();
	}	
	_CONFIGURE_SDA_EEPROM_OUTPUT_PORT;
	// start sequence
	_CLEAR_SDA_EEPROM_PORT;
	delay_2us();
	_CLEAR_SCL_EEPROM_PORT;
	delay_2us();
	// stop sequence
	_SET_SCL_EEPROM_PORT;
	delay_2us();
	_SET_SDA_EEPROM_PORT;
	delay_2us();
}
/******************************************************************************
*	- This function sends the i2c start condition
*	- Interrupts are allowed during TX sequence
******************************************************************************/
void send_eeprom_i2c_start(void)
{
	_SET_SCL_EEPROM_PORT;				//set RB1 high |
	_CONFIGURE_SCL_EEPROM_OUTPUT_PORT;	//set TRISB1 as output |
	_CONFIGURE_SDA_EEPROM_INPUT_PORT;	//set TRISB0 as output |
	delay_1us();						//					|Preset Start EEPROM IIC (like a stop sequence)
	if(!SDA_EEPROM_PORT)
		init_eeprom_i2c();
	delay_2us();
	_CONFIGURE_SDA_EEPROM_OUTPUT_PORT;	//set RB0 as output|
	_CLEAR_SDA_EEPROM_PORT;	//|Start EEPROM IIC
	delay_2us();			//|
	_CLEAR_SCL_EEPROM_PORT;	//|
	delay_1us();			//|
}

/******************************************************************************
*	- This function sends the i2c stop condition
*	- Interrupts are allowed during TX sequence
******************************************************************************/
void send_eeprom_i2c_stop(void)
{
	_CLEAR_SCL_EEPROM_PORT;	//|Ensure IIC lines start low
	delay_1us();			//|
	_CLEAR_SDA_EEPROM_PORT;	//|
	delay_1us();
	_SET_SCL_EEPROM_PORT;	//|Stop EEPROM IIC
	delay_2us();			//|
	_SET_SDA_EEPROM_PORT;	//|
	delay_10us();			//|
}

/******************************************************************************
*	- This function sends one byte using i2c protocol
*	- Interrupts are allowed during TX sequence
*	- MSB is sent first
******************************************************************************/
unsigned char send_eeprom_i2c_byte(unsigned char tx_byte)
{
	unsigned char i;
	for (i=8; i!=0; i--)
	{
		if (tx_byte&0x80)
			_SET_SDA_EEPROM_PORT;
		else
			_CLEAR_SDA_EEPROM_PORT;
		tx_byte<<=1;
		delay_1us();
		_SET_SCL_EEPROM_PORT;//Pulse SCL on middle of SDA
		delay_2us();
		_CLEAR_SCL_EEPROM_PORT;
		delay_1us();
	}
	_SET_SDA_EEPROM_PORT;				//|set RB0 high
	_CONFIGURE_SDA_EEPROM_INPUT_PORT;;	//|set TRISB0 as input
	delay_1us();						//|More one SCL pulse for ACK
	_SET_SCL_EEPROM_PORT;				//|
	if(SDA_EEPROM_PORT)
		i=0;
	else
		i=1;
	delay_2us();						//|
	_CLEAR_SCL_EEPROM_PORT;				//|
	delay_1us();						//|
	_SET_SDA_EEPROM_PORT;				//|set RB0 high
	_CONFIGURE_SDA_EEPROM_OUTPUT_PORT;	//|set TRIsB0 as output
	return i;
}

/****************************************************************************
*	- If address is invalid (max = 0x4f) do nothing.
*	- If EEPROM finishing write cycle (10ms), wait for cycle to complete
*	  before writing a new data.
*	- Send one byte to EEPROM.
****************************************************************************/
unsigned char write_eeprom_byte(unsigned int address, unsigned char byte)
{
	unsigned char ee_block_address;
	unsigned char ee_final_address;
	if (address < MAX_ADDRESS)					/* Max valid address = 0x7ff) */
	{
		ee_final_address = address & 0x00FF;
		address = address >> 7;
		ee_block_address = address & 0x000E;
		ee_block_address = ee_block_address + EECSW;
		eeprom_timer = 30; 			/* Time-out for EEPROM to be ready */
		while(1)
		{
			if(!eeprom_timer)
				return 0xFF;					/* Abort trying, that can be a hardware problem */
			send_eeprom_i2c_start();
			if(!send_eeprom_i2c_byte(ee_block_address))
			{
				send_eeprom_i2c_stop();
				continue;
			}
			send_eeprom_i2c_byte(ee_final_address);
			send_eeprom_i2c_byte(byte);
			send_eeprom_i2c_stop();
			break;
		}
	}
	return 0;
}

/****************************************************************************
*	- Read one byte at the position specified and return it.
*	- If address is not valid (max = 0x4f) data returned will be undefined.
*	- write -> bit 0 = 0; read -> bit0 = 1
****************************************************************************/
unsigned char read_eeprom_byte(unsigned int address)
{
	unsigned char  ee_block_address;
	unsigned char  ee_final_address;
	unsigned char  data;
	if (address < MAX_ADDRESS)					/* Max valid address = 0x7ff) */
	{
		ee_final_address = address & 0x00FF;
		address = address >> 7;
		ee_block_address = address & 0x000E;
		ee_block_address = ee_block_address + EECSW;
		eeprom_timer = 30;			/* Time-out for EEPROM to be ready */
		while(1)
		{
			if(!eeprom_timer)
				return(0xFF);			/* Abort trying, that can be a hardware problem */
			send_eeprom_i2c_start();
			if(!send_eeprom_i2c_byte(ee_block_address))
			{
				send_eeprom_i2c_stop();
				continue;
			}
			if(!send_eeprom_i2c_byte(ee_final_address))
			{
				send_eeprom_i2c_stop();
				continue;
			}
			break;
		}
		send_eeprom_i2c_stop();
		eeprom_timer = 30;			/* Time-out for EEPROM to be ready */
		while(1)
		{
			if(!eeprom_timer)
				return(0xFF);			/* Abort trying, that can be a hardware problem */
			send_eeprom_i2c_start();							/* Start read sequence */
			if(!send_eeprom_i2c_byte(ee_block_address + 1))
			{
				send_eeprom_i2c_stop();
				continue;
			}
			break;
		}
		read_eeprom_i2c_byte(&data);						/* Read eeprom */
		send_eeprom_i2c_stop();
		return data;										/* Return value read */
	}
}

#ifdef EEPROM_MANIPULATE_WORD
void write_eeprom_word(unsigned int address, unsigned int dado)
{
	unsigned char ee_block_address;
	unsigned char ee_final_address;
	if (address < MAX_ADDRESS)					/* Max valid address = 0x7ff) */
	{
		ee_final_address = address & 0x00FF;
		address = address >> 7;
		ee_block_address = address & 0x000E;
		ee_block_address = ee_block_address + EECSW;
		eeprom_timer = T_30_MS;			/* Time-out for EEPROM to be ready */
		while(1)
		{
			if(!eeprom_timer)
				return;					/* Abort trying, that can be a hardware problem */
			watchDog_reset();			/* Reset Watchdog *///???????????????????????
			send_eeprom_i2c_start();
			if(!send_eeprom_i2c_byte(ee_block_address))
			{
				send_eeprom_i2c_stop();
				continue;
			}
			send_eeprom_i2c_byte(ee_final_address);
			send_eeprom_i2c_byte((unsigned char)dado);
			send_eeprom_i2c_byte((unsigned char)(dado>>8));
			send_eeprom_i2c_stop();
			break;
		}
	}
}

unsigned int read_eeprom_word(unsigned int address)
{
	unsigned char  ee_block_address;
	unsigned char  ee_final_address;
	unsigned char  data;
	unsigned int  data16;
	if (address < MAX_ADDRESS)					/* Max valid address = 0x7ff) */
	{
		ee_final_address = address & 0x00FF;
		address = address >> 7;
		ee_block_address = address & 0x000E;
		ee_block_address = ee_block_address + EECSW;
		eeprom_timer = T_30_MS;			/* Time-out for EEPROM to be ready */
		while(1)
		{
			if(!eeprom_timer)
				return(0xFF);			/* Abort trying, that can be a hardware problem */
			watchDog_reset();			/* Reset Watchdog *///???????????????????????
			send_eeprom_i2c_start();
			if(!send_eeprom_i2c_byte(ee_block_address))
			{
				send_eeprom_i2c_stop();
				continue;
			}
			if(!send_eeprom_i2c_byte(ee_final_address))
			{
				send_eeprom_i2c_stop();
				continue;
			}
			break;
		}
		send_eeprom_i2c_stop();
		eeprom_timer = T_30_MS;			/* Time-out for EEPROM to be ready */
		while(1)
		{
			if(!eeprom_timer)
				return(0xFF);			/* Abort trying, that can be a hardware problem */
			watchDog_reset();			/* Reset Watchdog *///???????????????????????
			send_eeprom_i2c_start();							/* Start read sequence */
			if(!send_eeprom_i2c_byte(ee_block_address + 1))
			{
				send_eeprom_i2c_stop();
				continue;
			}
			break;
		}
		read_block_eeprom_i2c_byte(&data);						/* Read eeprom */
		data16 = data;
		read_eeprom_i2c_byte(&data);						/* Read eeprom */
		data16 += ((unsigned int)data)<<8;
		send_eeprom_i2c_stop();
		return data16;										/* Return value read */
	}
}
#endif

#ifdef EEPROM_MANIPULATE_BLOCK
unsigned char write_eeprom_block(unsigned int address, unsigned int size, unsigned char *buffer)
{
	unsigned char ee_block_address;
	unsigned char ee_final_address;
	unsigned int i,addr,j,last_start;
	unsigned char erro=0;

	for(i=0;i<size;i++)
	{
		watchDog_reset();			/* Reset Watchdog *///???????????????????????
		addr = address+i;
		if (addr < MAX_ADDRESS)					/* Max valid address = 0x7ff) */
		{
			if(((addr&0x00f)==0) || (i==0) || erro)  // begin of new page or first byte
			{
				if(erro)
				{
					i=last_start;
					addr = address+i;
					erro=0;
				}
				else
					last_start=i;
				if(i!=0)
					send_eeprom_i2c_stop();
				ee_block_address = (addr>>7) & 0x000E;
				ee_block_address = ee_block_address + EECSW;
				ee_final_address = addr & 0x00FF;
				eeprom_timer = T_30_MS;			/* Time-out for EEPROM to be ready */
				while(1)
				{
					if(!eeprom_timer)
						return 0xFF;					/* Abort trying, that can be a hardware problem */
					watchDog_reset();			/* Reset Watchdog *///???????????????????????
					send_eeprom_i2c_start();
					if(!send_eeprom_i2c_byte(ee_block_address))
					{
						send_eeprom_i2c_stop();
						continue;
					}
					if(!send_eeprom_i2c_byte(ee_final_address))
					{
						send_eeprom_i2c_stop();
						continue;
					}
					break;
				}
			}
			if(!send_eeprom_i2c_byte(buffer[i]))
				erro=1;
		}
	}
	send_eeprom_i2c_stop();
	return 0;
}

unsigned char read_eeprom_block(unsigned int address, unsigned int size, unsigned char *buffer)
{
	unsigned char  ee_block_address;
	unsigned char  ee_final_address;
	unsigned char  data;
	unsigned int i,addr;

	for(i=0;i<size;i++)
	{
		watchDog_reset();			/* Reset Watchdog *///???????????????????????
		addr = address+i;
		if (addr < MAX_ADDRESS)					/* Max valid address = 0x7ff) */
		{
			if(((addr&0x0ff)==0) || (i==0))
			{
				ee_block_address = (addr>>7) & 0x000E;
				ee_block_address = ee_block_address + EECSW;
				ee_final_address = addr & 0x00FF;
				eeprom_timer = T_30_MS;			/* Time-out for EEPROM to be ready */
				while(1)
				{
					if(!eeprom_timer)
					{
						for(i=0;i<size;i++)
							buffer[i]=0xFF;		/* Fill the rest of the buffer with invalid data */
						return 0xFF;			/* Abort trying, that can be a hardware problem */
					}
					watchDog_reset();			/* Reset Watchdog *///???????????????????????
					send_eeprom_i2c_start();
					if(!send_eeprom_i2c_byte(ee_block_address))
					{
						send_eeprom_i2c_stop();
						continue;
					}
					if(!send_eeprom_i2c_byte(ee_final_address))
					{
						send_eeprom_i2c_stop();
						continue;
					}
					break;
				}
				send_eeprom_i2c_stop();
			}
			eeprom_timer = T_30_MS;			/* Time-out for EEPROM to be ready */
			while(1)
			{
				if(!eeprom_timer)
				{
					for(i=0;i<size;i++)
						buffer[i]=0xFF;		/* Fill the rest of the buffer with invalid data */
					return 0xFF;			/* Abort trying, that can be a hardware problem */
				}
				watchDog_reset();			/* Reset Watchdog *///???????????????????????
				send_eeprom_i2c_start();							/* Start read sequence */
				if(!send_eeprom_i2c_byte(ee_block_address + 1))
				{
					send_eeprom_i2c_stop();
					continue;
				}
				break;
			}
			read_eeprom_i2c_byte(&buffer[i]);						/* Read eeprom */
			send_eeprom_i2c_stop();
		}
	}
	return 0;

}
#endif

unsigned char eeprom_present(void)
{
	if(eeprom_fail)
		return 0;
	return 1;
}

/****************************************************************************
*	Used during development to init blank EEPROM
*	The defines are enabled inside definition
****************************************************************************/
void eeprom_init(void)
{

#ifdef CF_USE_EEPROM
	// Write a byte just to verify the communication
	eeprom_fail = write_eeprom_byte(EE_PRESENT_ADDR,0xAA);
	if(eeprom_fail)
		return;
#else
	eeprom_fail = 0xFF;
	return;
#endif

}


void Write_FF_On_Eeprom(unsigned char mode)
{
	unsigned int j;
	j = 0;

	for (j=0; j<0x800 ;j++)						/* Fill EEPROM with FF's */
	{
		write_eeprom_byte(j,0xff);				/* Write it to EEPROM */
	}
}


/* END */
