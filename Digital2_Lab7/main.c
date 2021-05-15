
#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/systick.h"

//**********************PROTOTIPOS DE FUNCIONES***********************
void TimerConfig(void);
void Timer0IntHandler(void);


/**
 * main.c
 */

//*********************DECLARACION DE VARIABLES************************

bool state = true;


void Timer0IntHandler(void){
    TimerIntClear(TIMER0_BASE, TIMER_A);
    state=~state;
}
//****************************PROGRAMA**********************************
int main(void)
{   //Se configura reloj a 40MHz
    SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
    //Se activa el puerto F
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    //Se configuran como salida los LEDS
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
    IntMasterEnable();
    TimerConfig();

//******************************MAIN LOOP********************************
    while(1){
        if (state){
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0xFF);
        }
        else{
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0x00);
        }


    }
	return 0;
}


void TimerConfig(void){
    //Configuracion del Timer 0 segun parte 2 del lab
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0)) //Espera a que el modulo del T0 este listo
    { }
    TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC); //Se configura T0 como periodico de 32 bits
    TimerLoadSet(TIMER0_BASE, TIMER_A, 20000000);//Se asigna un valor de 20M de modo que se tarde 0.5s en hacer un timeout
    TimerIntRegister(TIMER0_BASE, TIMER_A, Timer0IntHandler);
    IntEnable(INT_TIMER0A); //Se activa interrupcion para el timer 0A
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT); //Se habilita interrupcion para que se haga en el timeout de T0A
    TimerEnable(TIMER0_BASE, TIMER_A);
}



