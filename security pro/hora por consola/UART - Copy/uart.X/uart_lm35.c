/*!
\file   uart.c
\date   2020-11-23
\author Fulvio Vivas <fulvio.vivas@unicauca.edu.co>
\brief  Example uart.

\par Copyright
Information contained herein is proprietary to and constitutes valuable
confidential trade secrets of unicauca, and
is subject to restrictions on use and disclosure.

\par
Copyright (c) unicauca 2020. All rights reserved.

\par
The copyright notices above do not evidence any actual or
intended publication of this material.
******************************************************************************
*/
#include <stdio.h>
#include <xc.h>
#include <pic18f4550.h>  /*Header file PIC18f4550 definitions*/
#include "fuses.h"
#include "I2C_Master_File.h"



#define _XTAL_FREQ  8000000L

  char buffer_TX[] = "------    ::    \r";
//char buffer_TX[] = "Temp: xxx grados\r";
int value_adc;
double temp;
char lm35;


#define device_id_write 0xD0
#define device_id_read 0xD1

int sec,min,hour;
int Day,Date,Month,Year;

void RTC_Read_Clock(char read_clock_address)
{
    I2C_Start(device_id_write);
    I2C_Write(read_clock_address);     /* address from where time needs to be read*/
    I2C_Repeated_Start(device_id_read);
    sec = I2C_Read(0);                 /*read data and send ack for continuous reading*/
    min = I2C_Read(0);                 /*read data and send ack for continuous reading*/
    hour= I2C_Read(1);                 /*read data and send nack for indicating stop reading*/
    
}

void RTC_Read_Calendar(char read_calendar_address)
{   
    I2C_Start(device_id_write);
    I2C_Write(read_calendar_address); /* address from where time needs to be read*/
    I2C_Repeated_Start(device_id_read);
    Day = I2C_Read(0);                /*read data and send ack for continuous reading*/
    Date = I2C_Read(0);               /*read data and send ack for continuous reading*/
    Month = I2C_Read(0);              /*read data and send ack for continuous reading*/
    Year = I2C_Read(1);               /*read data and send nack for indicating stop reading*/
    I2C_Stop();
}


void main(void) {
     I2C_Init();
     char date[10],month[10],year[10];
     char mins[10],hours[10];
     char Clock_type = 0x06;
    char AM_PM = 0x05;
    //Configura Fosc = 8Mhz interno
    OSCCONbits.IRCF = 0b111; //Configura oscilador interno (FOSC = 8Mhz)
    OSCCONbits.SCS = 0b10; // Fuente de Fosc del sistema = interno

    //Configura UART a 9600 baudios
    TRISCbits.RC6 = 0; //  Pin RC6 como salida digital para TX.
    TXSTAbits.TX9 = 0; //  Modo-8bits.
    TXSTAbits.TXEN = 1; //  Habilita Transmisión.
    TXSTAbits.SYNC = 0; //  Modo-Asíncrono.
    TXSTAbits.BRGH = 0; //  Modo-Baja Velocidad de Baudios.
    BAUDCONbits.BRG16 = 0; //  Baudios modo-8bits.
    RCSTAbits.SPEN = 1; //  Hbilita el Módulo SSP como UART.
    SPBRG = (unsigned char) (((_XTAL_FREQ / 9600) / 64) - 1); //baudios  = 9600

     TRISAbits.TRISA1 = 1; //COMO ENTRADA
     TRISBbits.TRISB1 = 1;
     TRISBbits.TRISB1 = 1;
    

    //ADC = 10bits, Tad = 1us, Tacq = 4us, Vref = 5v-0v, RA=ANALOG
    ADCON1bits.PCFG = 0b1110; //  Configura el Puerto como Entrada Analógica.
    ADCON1bits.VCFG = 0b00; //  Selecciona Voltajes de Referencia (5v-0v).
    ADCON0bits.CHS = 0b0000; //  Selecciona el Canal Analógico.
    ADCON2bits.ACQT = 0b010; //  Tiempo de Adquisición 4Tad.
    ADCON2bits.ADCS = 0b001; //  Tiempo de Conversión Fosc/8.
    ADCON2bits.ADFM = 1; //  Justificación derecha (modo-10bits).
    ADCON0bits.ADON = 1; //  Habilita el Módulo AD.
    
        RTC_Read_Clock(0);              /*gives second,minute and hour*/
        I2C_Stop();       
            hour = hour & (0x3f);
            sprintf(mins,"%x",min);    
            sprintf(hours,"%x",hour);  
            buffer_TX[10] = hours[0];
            buffer_TX[11] = hours[1];
             buffer_TX[12] = ':';
             buffer_TX[13] = mins[0];
            buffer_TX[14] = mins[1]; 
        
        
        RTC_Read_Calendar(3);        /*gives day, date, month, year*/        
        I2C_Stop();
        sprintf(date,"%x-",Date);
         sprintf(month,"%x-",Month);
        sprintf(year,"%x ",Year);
        buffer_TX[0] = date[0];
         buffer_TX[1] = date[1];              
        buffer_TX[3] = month[0];
        buffer_TX[4] = month[1];
        buffer_TX[6] = year[0];
        buffer_TX[7] = year[1];
              
    while (1) {
        
//        //Inicia el proceso de conversión ADC.
//        ADCON0bits.GO_DONE = 1; //Inicia la COnversió AD.
//        while (ADCON0bits.GO_DONE); //  Espera a que termine la conversión AD.
//        value_adc = ADRESH; //  Lectura de valor AD.
//        value_adc = (value_adc << 8) + ADRESL;
//        temp = value_adc;
//        temp = (temp * 500.0) / 1023.0;
//        lm35 = (char) temp;
      
      
        
            if (PORTAbits.RA1) {
        //Inicia el proceso de transmisión TX.
        for (int i = 0; i < 17; i++) {
            //  espera a que el registro de transmisión este disponible o vacio.
            while (!TXSTAbits.TRMT) {
            }
            //  escribe el dato que se enviará a través de TX.
            TXREG = buffer_TX[i];
            
        }
        __delay_ms(100);
            }
    }
}
