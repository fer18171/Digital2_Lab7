
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
#include "driverlib/uart.h"

//**********************PROTOTIPOS DE FUNCIONES***********************
void TimerConfig(void);
void Timer0IntHandler(void);
void UARTconfig(void);
void UARTIntHandler(void);
void SendString(char* frase);
/**
 * main.c
 */

//*********************DECLARACION DE VARIABLES************************

bool state = false;
bool state2 = false;
char valor = ' ';
char valorP = ' ';
uint8_t n=0;
//****************************PROGRAMA**********************************
void main(void)
//*******************************SETUP*************************************
{   //Se configura reloj a 40MHz
    SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
    //Se activa el puerto F
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    //Se configuran como salida los LEDS
    GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
    IntMasterEnable();
    TimerConfig();
    UARTconfig();
    SendString("Coloca las letras r, g, b para encender el led con los 3 colores, coloca una letra de nuevo para apagar: ");
//******************************MAIN LOOP********************************
    while(1){
        //Si el usuario selecciona encender un led, dependiendo del color, se enciende el led corrspondientee
        if (state & state2){
            if (valor == 'r'){ //rojo
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 2);
            }
            if (valor == 'g'){ //verde
                        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 8);
                        }
            if (valor == 'b'){ //azul
                        GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 4);
                        }
        }
        else{  //En la parte cuando esta apagado del parpadeo, o cuando el usuario apaga el led, se queda apagado
            GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0x00);
        }
    }

}

//**********************************FUNCIONES*********************************************
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

void Timer0IntHandler(void){
    TimerIntClear(TIMER0_BASE, TIMER_A);
    // Simplemente se hace un toggle de la bandera state, para que el led se apague y encienda cada 500ms
    state= !state;
}

void UARTIntHandler(void){
    UARTIntClear(UART0_BASE, UART_INT_RT | UART_INT_RX);
    valorP = valor; //se guarda el valor anterior
    valor = UARTCharGetNonBlocking (UART0_BASE); //Se toma el nuevo valor recibido por UART
    if (valorP == valor){ //Si el valor anterior es igual al actual, es decir, si vuelve a apachar r,g o b, se hace un toggle de state2
        state2 = !state2;
    }
    else if (valor == 'r'|valor == 'g'|valor == 'b'){ //Si no se esta apachando de nuevo un valor y el valor presioando es r,g o b, entonces se enciende state2
        state2 = true;
    }
    else { //Se lo contrario, si se presiona una tecla que nada que ver, se dejara el valor que se tenia anteriormente.
        valor = valorP;
    }

}

void UARTconfig(void){
    //Aqui se configura el UART
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0); //Activar clock para UART0
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);//Activar clock para puerto A de la tiva
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0|GPIO_PIN_1); //Se activan los pines 0 y 1 del puerto A
    UARTConfigSetExpClk(UART0_BASE,SysCtlClockGet(), 115200, UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE);
    UARTIntEnable(UART0_BASE, UART_INT_RT | UART_INT_RX); //Se activa interrupcion cada vez que se reciba un dato
    UARTIntRegister(UART0_BASE, UARTIntHandler); //Se le coloca el nombre a la funcion del handler
}

void SendString(char* frase){
    //Funcion para poder enviar string mediante UART
    while (*frase){
        UARTCharPut(UART0_BASE, *frase++);
    }
}

