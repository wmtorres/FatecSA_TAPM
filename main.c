/* Changed by Weslley Torres in order to fit into hardware developed on FATEC Santo André	*/
/* Author: Weslley Torres																	*/
/* Date: 22/May/2022																		*/


#include "Config.h"
#include "Hardware.h"
#include "displayLCD.h"
#include "I2C_SW.h"

/************************************************************************************************/
/* 											Variables		 									*/
/************************************************************************************************/
/*Store function pointers of Task.*/
void (*ScheduledTasks[NUMBER_OF_TASKS])(void);  

/*Store task´s times (time period to execute)*/
int TimeScheduledTasks[NUMBER_OF_TASKS];       
 
/*Stores recent task´s times ("time to execute" each task) */
int RecentTasksTimeToExecute[NUMBER_OF_TASKS];  

/*Tells if TImer0 interrupt has been generated*/
char Timer0IntGeneraed = 0;                        
 
/* Tells if a task in executing (used to check task timeout validation)*/
volatile char TaskIsExecuting;                  

int TaskTimeout;

uint8_t TimerBreathingLight = 100;
uint8_t Tecla_lida = 0;

uint8_t FlagDeb = 0;


/************************************************************************************************/


/************************************************************************************************/
/* 											Function Prototipe 									*/
/************************************************************************************************/

void ConfigTimer0(void);
void InitTasks(void);
void ExecuteTask(void);
void Init_Ports(void);

void Task1(void);
void Task2(void);
void Task3(void);
void Task4(void);

/*************************************************************************************************/

/**
 *  \brief Initialize and schedule tasks
 *  
 *  \return none
 *  
 *  \details Initialize the scheduller
 */
void InitTasks(void)
{
	//init function pointers of tasks
	ScheduledTasks[INDEX_TAREFA_1] = Task1;
	ScheduledTasks[INDEX_TAREFA_2] = Task2;
	ScheduledTasks[INDEX_TAREFA_3] = Task3;
    ScheduledTasks[INDEX_TAREFA_4] = Task4;
	
	/*init temporization values of each task. These values do no 
	change during execution*/
	TimeScheduledTasks[INDEX_TAREFA_1] = TIME_TO_EXECUTE_TASK1;
	TimeScheduledTasks[INDEX_TAREFA_2] = TIME_TO_EXECUTE_TASK2;
	TimeScheduledTasks[INDEX_TAREFA_3] = TIME_TO_EXECUTE_TASK3;
    TimeScheduledTasks[INDEX_TAREFA_4] = TIME_TO_EXECUTE_TASK4;
	
	/*init recent temporization values of each task. These values will 
	change during execution (they´re used to decide which task must be executed)*/
	
	RecentTasksTimeToExecute[INDEX_TAREFA_1] = TIME_TO_EXECUTE_TASK1;
	RecentTasksTimeToExecute[INDEX_TAREFA_2] = TIME_TO_EXECUTE_TASK2;
	RecentTasksTimeToExecute[INDEX_TAREFA_3] = TIME_TO_EXECUTE_TASK3;	
    RecentTasksTimeToExecute[INDEX_TAREFA_4] = TIME_TO_EXECUTE_TASK4;	
    
    //It indicates that there´s no task executing
    TaskIsExecuting = NO;
}


/**
 *  \brief ExecuteTask
 *  
 *  \return none
 *  
 *  \details Execute tasks assigned to the system
 */
void ExecuteTask(void)
{
    char i;
	for (i=0; i<NUMBER_OF_TASKS; i++)
	{
		//Check if it´s time to execute a task
		if ((ScheduledTasks[i] != 0) && (RecentTasksTimeToExecute[i] == 0))
        {
			TaskIsExecuting = YES;
            TaskTimeout = TASK_TIMEOUT;
            ScheduledTasks[i]();  //executes the task
            TaskIsExecuting = NO;
			RecentTasksTimeToExecute[i] = TimeScheduledTasks[i];  //reagendamento da tarefa
		}
	}
}


/**
 *  \brief Task1
 *  
 *  \return none
 *  
 *  \details Executes task 1
 */
void Task1(void)
{
	static uint8_t Contagem = 0;
    if(CHAVE_B1 == 0)
	{
		if(FlagDeb == 0)
		{
			Contagem++;
			if(Contagem == 10)
			{
				FlagDeb = 1;
				Contagem = 0;
                LimpaDisplay();
			}
		}
	}
	LATBbits.LATB0 = ~LATBbits.LATB0;
}


/**
 *  \brief Task2 -> Escrita EEPROM
 *  
 *  \return none
 *  
 *  \details Executes task 2
 */
void Task2(void)
{
	uint8_t data1Error, data2Error;
	static uint8_t ValorA = 0;
    static uint8_t ValorB = 10;
    if(FlagDeb == 1)
    {

        data1Error = write_eeprom_byte(0x10, ValorA);
    //                                  |     |----> Dado a ser escrito na EEPROM
    //                                  |----------> Endereco na EEPROM
        data2Error = write_eeprom_byte(0x11, ValorB);
    //                                  |     |----> Dado a ser escrito na EEPROM
    //                                  |----------> Endereco na EEPROM    
        FlagDeb = 0; // somente uma escrita na EEPROM
        ValorA++;
        ValorB++;
    }
	LATBbits.LATB1 = ~LATBbits.LATB1;
}


/**
 *  \brief Task3
 *  
 *  \return none
 *  
 *  \details Executes task 3
 */
void Task3(void)
{
    static uint8_t Escrever = 0;
    uint8_t MsgPrimLinhaTask3[17] = "End. 0x10 = ";
	uint8_t MsgSegLinhaTask3[17]  = "End. 0x11 = ";
    
    uint8_t DadoLido1, DadoLido2;

  
    /* Write welcome screen on LCD line 1 */
	PosicaoCursorLCD(1,1);
	EscreveFraseRamLCD(MsgPrimLinhaTask3);
	
    /* Write welcome screen on LCD line 2 */
	PosicaoCursorLCD(2,1);
	EscreveFraseRamLCD(MsgSegLinhaTask3);
    
    DadoLido1 = read_eeprom_byte(0x10);
//                                  |----------> Endereco na EEPROM
    DadoLido2 = read_eeprom_byte(0x11);
//                                  |----------> Endereco na EEPROM

	
    PosicaoCursorLCD(1,13);
    EscreveInteiroLCD(DadoLido1);
    
    PosicaoCursorLCD(2,13);
    EscreveInteiroLCD(DadoLido2);
    
    //A linha abaixo funciona somente como um aviso que a task esta "rodando"
    LATBbits.LATB2 = ~LATBbits.LATB2;  
}

/**
 *  \brief Task4
 *  
 *  \return none
 *  
 *  \details Executes task 4
 */
void Task4(void)
{
    LATBbits.LATB3 = ~LATBbits.LATB3;
}

/**
 *  \brief ISR
 *  
 *  \return none
 *  
 *  \details Interrupt service routine (be careful snce PIC has only one interrupt vector, based on that, the way you check the 	interrupt flag will be the priority)
 */
void interrupt isr(void)
{
    char  i;
    	
    //check if Timer0 interupt was triggered
	if (TMR0IF && T0IF)   
    {
        T0IF = 0;                 //set trigger for Timer0 interrupt (so it can be generated again)
        INTCONbits.TMR0IF = 0;   
        TMR0 = TIMER0_INIT_VALUE; //Set initial value of TMR0 register (for counting 1ms, as we need)	

        if(TimerBreathingLight)     TimerBreathingLight--;
        if(TimerBreathingLight <= 0)
        {
             LATBbits.LATB4 = ~LATBbits.LATB4;
             TimerBreathingLight = 100;
        }
        Timer0IntGeneraed = YES;
		
		//Refresh "time to execute" of each task        
		for (i=0; i<NUMBER_OF_TASKS; i++)
		{
			if (RecentTasksTimeToExecute[i] > 0)
				RecentTasksTimeToExecute[i]--;			
		}
    }   
}


 /**
 *  \brief main
 *  
 *  \return none
 *  
 *  \details Main function of the system
 */
void main(void) 
{


	unsigned char MsgPrimLinha[17] = "TAPM Com. I2C   ";
	unsigned char MsgSegLinha[17] =  "23/05/22 ver 1.0";
        
	InitHardware();
    
    /* Initialize tasks */ 
    InitTasks();   

    ConfiguraLCD();         //configure LCD display
 
    eeprom_init();
    
	/* Write welcome screen on LCD line 1 */
	PosicaoCursorLCD(1,0);
	EscreveFraseRamLCD(MsgPrimLinha);
	
    /* Write welcome screen on LCD line 2 */
	PosicaoCursorLCD(2,0);
	EscreveFraseRamLCD(MsgSegLinha);

    __delay_ms(2000);
    LimpaDisplay();
    //main loop 
    while(1)                      
    {
        //Verification: check if there´s a task to be executed
        if ((Timer0IntGeneraed == YES)  && (NUMBER_OF_TASKS)) 
        {
            Timer0IntGeneraed = NO;  
            ExecuteTask();			
        }
    }
}


