//**************************************************************************************************************
// Katherine Caceros // 18307
//**************************************************************************************************************

// PROYECTO #4 ESTACIONAMIENTOS

//**************************************************************************************************************
// Librerias :v
//**************************************************************************************************************
#include <stdint.h> //se agregaron las librerias
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "inc/hw_gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/uart.h"
#include "driverlib/pin_map.h"

#define XTAL 16000000

//**************************************************************************************************************
// Variables
//**************************************************************************************************************
    uint32_t i = 0;
    uint32_t Parqueos = 0; //display
    uint32_t Boton1 = 0; // Sensores
    uint32_t Boton2 = 0;
    uint32_t Boton3 = 0;
    uint32_t Boton4 = 0;
    uint8_t Sensores = 0;
//**************************************************************************************************************
// Funciones
//**************************************************************************************************************
    void delay(uint32_t msec); // declaramos las funciones a utilizar
    void delay1ms(void);
    void Display7(uint32_t numero);

//**************************************************************************************************************
// Funcion Principal
//**************************************************************************************************************
int main(void)
 {
    // Se setea oscilador externo de 16MHz
    SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);

    // Se asigno el reloj a los puerto
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOE);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);


    //Configuramos los puertos como entradas y salidas
    GPIOPinTypeGPIOOutput(GPIO_PORTB_BASE, GPIO_PIN_6|GPIO_PIN_5|GPIO_PIN_4|GPIO_PIN_3|GPIO_PIN_2|GPIO_PIN_1|GPIO_PIN_0);// puerto B displays

    GPIOPinTypeGPIOInput(GPIO_PORTC_BASE, GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7);//son los botones(sensores)
    GPIOPadConfigSet(GPIO_PORTC_BASE, GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7, GPIO_STRENGTH_8MA, GPIO_PIN_TYPE_STD_WPD);

    GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_3|GPIO_PIN_2|GPIO_PIN_1|GPIO_PIN_0);// puertos de leds
    HWREG(GPIO_PORTD_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
    HWREG(GPIO_PORTD_BASE + GPIO_O_CR) |= GPIO_PIN_7;
    GPIOPinConfigure(GPIO_PD7_U2TX);// Uart 2

    GPIOPinTypeGPIOOutput(GPIO_PORTE_BASE, GPIO_PIN_3|GPIO_PIN_2|GPIO_PIN_1|GPIO_PIN_0);//puertos de leds


    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART2); // enable uart2
    GPIOPinTypeUART(GPIO_PORTD_BASE, GPIO_PIN_6 | GPIO_PIN_7); // pines de control del UART
    //Habilitamos el UART
    UARTConfigSetExpClk(UART2_BASE, SysCtlClockGet(), 115200, (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
    UARTIntClear(UART2_BASE, UART_INT_RX | UART_INT_RT | UART_INT_TX | UART_INT_FE | UART_INT_PE | UART_INT_BE | UART_INT_OE | UART_INT_RI | UART_INT_CTS | UART_INT_DCD | UART_INT_DSR);


//**********************************************************************************************************
// Loop Principal
//**********************************************************************************************************

    while (1)
    {
        Boton1 = (GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_4) && GPIO_PIN_4); // lee el pin y se compara para saber si esta
        Boton2 = (GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_5) && GPIO_PIN_5); // encendido o apagado AND
        Boton3 = (GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_6) && GPIO_PIN_6); // 0= vacio  1= ocupado
        Boton4 = (GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_7) && GPIO_PIN_7);

        Sensores = GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_4|GPIO_PIN_5|GPIO_PIN_6|GPIO_PIN_7) >> 4; // lee el puerto y corre  4 leds

        Parqueos = ((((4 - Boton4) - Boton3) - Boton2) - Boton1) ; // resta para saber la cantidad de parqueos disponbles
        Display7(Parqueos); // envia los datos al display

        GPIOPinWrite(GPIO_PORTE_BASE, GPIO_PIN_3|GPIO_PIN_2|GPIO_PIN_1|GPIO_PIN_0, Sensores); //leds rojas
        GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_3|GPIO_PIN_2|GPIO_PIN_1|GPIO_PIN_0, ~Sensores); // leds azules  esta negada para que al presionar enciendan las otras

        UARTCharPut(UART2_BASE, Boton1); // Envio el estado de los botones al UART
        UARTCharPut(UART2_BASE, Boton2);
        UARTCharPut(UART2_BASE, Boton3);
        UARTCharPut(UART2_BASE, Boton4);
        delay(100); //delay
    }

}

//**************************************************************************************************************
// Funcion display
//**************************************************************************************************************

void Display7(uint32_t numero) //
{
    switch(numero) // puerto b, para diplay, depende de la cantidad
    {
    case 0: GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_6|GPIO_PIN_5|GPIO_PIN_4|GPIO_PIN_3|GPIO_PIN_2|GPIO_PIN_1|GPIO_PIN_0, 0x3F); break;
    case 1: GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_6|GPIO_PIN_5|GPIO_PIN_4|GPIO_PIN_3|GPIO_PIN_2|GPIO_PIN_1|GPIO_PIN_0, 0x06); break;
    case 2: GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_6|GPIO_PIN_5|GPIO_PIN_4|GPIO_PIN_3|GPIO_PIN_2|GPIO_PIN_1|GPIO_PIN_0, 0x5B); break;
    case 3: GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_6|GPIO_PIN_5|GPIO_PIN_4|GPIO_PIN_3|GPIO_PIN_2|GPIO_PIN_1|GPIO_PIN_0, 0x4F); break;
    case 4: GPIOPinWrite(GPIO_PORTB_BASE, GPIO_PIN_6|GPIO_PIN_5|GPIO_PIN_4|GPIO_PIN_3|GPIO_PIN_2|GPIO_PIN_1|GPIO_PIN_0, 0x66); break;
    default: break;
    }
}


//**************************************************************************************************************
// Funcion delay ->  milisegundos
//**************************************************************************************************************
void delay(uint32_t msec) //funcion de delay milisegundos
{
    for (i = 0; i < msec; i++)
    {
        delay1ms();
    }

}
//**************************************************************************************************************
// Funcion delay -> 1 milisegundos
//**************************************************************************************************************
void delay1ms(void)
{
    SysTickDisable();
    SysTickPeriodSet(16000);
    SysTickEnable();

    while ((NVIC_ST_CTRL_R & NVIC_ST_CTRL_COUNT) == 0); //Pag. 138
}


