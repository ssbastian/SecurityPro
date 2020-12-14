/*
    PIR Sensor Motion Interfacing with PIC18f4550
 */

/*!
\file   Controler.c
\date   2020-12-11
\author Alvaro Daniel Erazo Ramirez ( 104618011588 ) <danieleraso@unicauca.edu.co>
\author Braian Alexis Bastidas Diaz ( 104617011741 ) <brabadi@unicauca.edu.co>
\author Juan Sebastian Sanchez Pizo ( 104617012079 ) <spjuan@unicauca.edu.co>
\author Héctor Esteban Coral Ordoñez ( 104616011357 ) <hectorcoral@unicauca.edu.co>

\brief  Example Controler .

 ******************************************************************************
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "Configuration_Header_File.h"          /* Header File for Configuration bits */
#include "LCD_20x4_Header_File.h"                 /* Header File for LCD Functions */
#include "PIC18F4550_ADC_Header_File.h"


#include "I2C_Master_File_1.h"

//#include "teclado.h"
//#define LED PORTEbits.RE0
//#define Motion_detection PORTAbits.RA2      /* Read PIR sensor's data on this pin */
//#define PORT_Dir TRISAbits.RA2             /* define for setting direction */
//#define LED2 LATA5                           /* connect LED to the PORTD.0 pin */
//#define LED_Dir TRISAbits.RA5               /* define for setting direction */
#define _XTAL_FREQ  8000000L

#define device_id_write 0xD0
#define device_id_read 0xD1

int sec,min,hour;
int Day,Date,Month,Year;


unsigned char character1[8] = {0x01, 0x00, 0x1F, 0x04, 0x04, 0x04, 0x04, 0x04};
unsigned char character2[8] = {0x00, 0x04, 0x0E, 0x1F, 0x04, 0x04, 0x04, 0x00};
unsigned char character3[8] = {0x00, 0x04, 0x04, 0x04, 0x1F, 0x0E, 0x04, 0x00};
unsigned char character4[8] = {0x04, 0x15, 0x00, 0x0E, 0x0E, 0x0E, 0x00, 0x04};

//VARIABLES PARA EL TECLADO
char tecla = 0; //  GUARDAR LA TECLA PRESIONADA
char inclave[5] = {0}; //  GUARDAR LOS DATOS INGRESADOS EN UN ARREGLO
char countinclave = 0; //  CONTADOR PARA LA POSICION EN EL ARREGLO
int clave1check = 0; //  VERIFICACION DE LA CLAVE DIGITO1
int clave2check = 0; //  VERIFICACION DE LA CLAVE DIGITO2
int clave3check = 0; //  VERIFICACION DE LA CLAVE DIGITO3
int clavechecktotal = 0; //  VERIFICACION DE LA CLAVE TOTAL
char clave1 = '1'; //  CLAVE POR DEFECTO DIGITO1
char clave2 = '2'; //  CLAVE POR DEFECTO DIGITO2
char clave3 = '3'; //  CLAVE POR DEFECTO DIGITO3
char direccion1 = 0x01; //  DIRECCION DE LA MEMORIA EEPROM
char direccion2 = 0x02; //  DIRECCION DE LA MEMORIA EEPROM
char direccion3 = 0x03; //  DIRECCION DE LA MEMORIA EEPROM

//bandera alarma
int estado_alarma = 1;

void SensorMovimiento();

void desactivarAlarm();
void ActivateAlarm();

//protocolos
void protocoloFDS();
void protocoloEmergencia();
void protocoloNoche();
void protocoloProtocoloDia();

//palabra a enviar
char buffer_TX[] = "detecto un intruso \r";


  /**
   * Obtener la hora actual.
   * Un objeto time, con la hora actual.
   */
void RTC_Read_Clock(char read_clock_address)
{
    I2C_Start(device_id_write);
    I2C_Write(read_clock_address);     /* address from where time needs to be read*/
    I2C_Repeated_Start(device_id_read);
    sec = I2C_Read(0);                 /*read data and send ack for continuous reading*/
    min = I2C_Read(0);                 /*read data and send ack for continuous reading*/
    hour= I2C_Read(1);                 /*read data and send nack for indicating stop reading*/
    
}

  /**
   * Obtener la hora actual.
   * Un objeto time, con la hora actual.
  */
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

/**
   * Funcion principal.
   * .fn funcion principal
   */
void main() {
    
    char secs[10],mins[10],hours[10];
    char date[10],month[10],year[10];
    char Clock_type = 0x06;
    char AM_PM = 0x05;
    char days[7] = {'S','M','T','W','t','F','s'};
    
    char i;
    OSCCON = 0x72; /* set internal Oscillator frequency to 8 MHz*/
    LCD_Init(); /* initialize 20x4 LCD*/
    ADC_Init(); /* initialize 10-bit ADC*/
 
    
    I2C_Init();    
     

   
    //Configura UART a 9600 baudios
    TRISCbits.RC6 = 0; //  Pin RC6 como salida digital para TX.
    TXSTAbits.TX9 = 0; //  Modo-8bits.
    TXSTAbits.TXEN = 1; //  Habilita Transmisión.
    TXSTAbits.SYNC = 0; //  Modo-Asíncrono.
    TXSTAbits.BRGH = 0; //  Modo-Baja Velocidad de Baudios.
    BAUDCONbits.BRG16 = 0; //  Baudios modo-8bits.
    RCSTAbits.SPEN = 1; //  Hbilita el Módulo SSP como UART.
    SPBRG = (unsigned char) (((_XTAL_FREQ / 9600) / 64) - 1); //baudios  = 9600
    
    
    //LCD_String_xy(1, 1, "INGRESAR CLAVE");

    //inicia la alarma
//    LCD_String_xy(0,12,"ALARMA:");
//    if(estado_alarma == 1){
//        LCD_String_xy(0,19,"ON");
//    }else{
//        LCD_String_xy(0,19,"OFF");
//    }
//    
         ADCON1bits.PCFG=0xff;
         TRISAbits.RA1 = 1;
         LATA=0X00;
         TRISA=0X11;
        
    int flag = PORTAbits.RA1;
    while (1) {
        
         ADCON1bits.PCFG=0xff;
         TRISAbits.RA1 = 1;
         LATA=0X00;
         TRISA=0X11;
        
        
        estado_alarma = PORTAbits.RA1;
                      
             if(estado_alarma == 1){ 
                 if(flag == 1)
                  ActivateAlarm();  
                  flag = 0;
             
             //SensorMovimiento();
            
           RTC_Read_Clock(0);  
        I2C_Stop();
        MSdelay(500);
        //I2C_Stop()
        
            hour = hour & (0x3f);
            sprintf(secs,"%x ",sec);   /*%x for reading BCD format from RTC DS1307*/
            sprintf(mins,"%x:",min);    
            sprintf(hours,"%x",hour);  
            
            
            //para leer el dia
            RTC_Read_Calendar(3);        /*gives day, date, month, year*/        
            I2C_Stop();
            
            /* find day */
        switch(days[Day])
        {
            case 'S':
                        //LCD_String("Sun");
                        protocoloFDS(); 
                       break;
            case 'M':
                //LCD_String("Tue");
                if(hour >= 0  && hour <= 19){
                   //LCD_String_xy(14,3,"dia"); 
                    //LCD_String_xy(3,0,hours);
                    protocoloProtocoloDia();
                }else{
                    //LCD_String_xy(14,3,"noche"); 
                    protocoloNoche();
                }
                break;
            case 'T':
                //LCD_String("Tue");
                if(hour >= 0  && hour <= 19){
                   //LCD_String_xy(14,3,"dia"); 
                    //LCD_String_xy(3,0,hours);
                    protocoloProtocoloDia();
                }else{
                    //LCD_String_xy(14,3,"noche"); 
                    protocoloNoche();
                }
                break;                
            case 'W':   
                //LCD_String("Tue");
                if(hour >= 0  && hour <= 19){
                   //LCD_String_xy(14,3,"dia"); 
                    //LCD_String_xy(3,0,hours);
                    protocoloProtocoloDia();
                }else{
                    //LCD_String_xy(14,3,"noche"); 
                    protocoloNoche();
                }
                break;
            case 't':
                //LCD_String("Tue");
                if(hour >= 0  && hour <= 19){
                   //LCD_String_xy(14,3,"dia"); 
                    //LCD_String_xy(3,0,hours);
                    protocoloProtocoloDia();
                }else{
                    //LCD_String_xy(14,3,"noche"); 
                    protocoloNoche();
                }
                break;
            case 'F':
                 //LCD_String("Tue");
                if(hour >= 0  && hour <= 19){
                   //LCD_String_xy(14,3,"dia"); 
                    //LCD_String_xy(3,0,hours);
                    protocoloProtocoloDia();
                }else{
                    //LCD_String_xy(14,3,"noche"); 
                    protocoloNoche();
                }
                break;             
            case 's':
                        //LCD_String("Sat");
                        protocoloFDS(); 
                        break;
            default: 
                        break;
                        
        }
            
        }else{
          desactivarAlarm();
                 
           LCD_Clear();
           flag = 1;
        }
            
    }
}

  /**
   * Muestra en la pantalla 'alarma desactivada'.
   * @return Un objeto time, con la hora actual.
   */
 void desactivarAlarm(){
     LCD_Clear();
     LCD_String_xy(2,1,"ALARMA DESACTIVADA");
     MSdelay(100);
      LED2 = 0; /* initially turned OFF LED */
      LED3 = 0;
      LED4 = 0;
      BUZZER = 0;
 }
  /**
   * Muestra en la pantalla 'alarma Activada'.
   * Un objeto time, con la hora actual.
   */
 void ActivateAlarm(){
     LCD_String_xy(2,1,"ALARMA ACTIVADA");
     MSdelay(100);
     LCD_Clear();
     LCD_String_xy(0,12,"ALARMA:");
     LCD_String_xy(0,19,"ON");
 }

 
  /**
   * Sensor movimiento'.
   * @fn activa o desactiva los tres sensores y con estos los leds y el buzer.
   */
void SensorMovimiento() {
    ADCON1 = 0x0F; /* @param this makes all pins as a digital I/O pins */
    PORT_Dir = 1; /* @param set as input port */
    PORT_Dir2 = 1;
    PORT_Dir3 = 1;
    LED_Dir = 0; /* @param set as output port */
    LED_Dir2 = 0;
    LED_Dir3 = 0;
    LED2 = 0; /* @param initially turned OFF LED */
    LED3 = 0;
    LED4 = 0;
    
    LCD_String_xy(2,0,"SI: ");
    if (MD_Pir1) {
        LED2 = 1;
        LCD_String_xy(6,0,"ON ");
        BUZZER = 1;
    } else {
        LED2 = 0;
        BUZZER = 0;
        LCD_String_xy(6,0,"OFF");
        char buffer_TX[] = "";
    }
    
    LCD_String_xy(3,0,"ST: ");
    if(MD_Pir2){
        LED3 = 1;
        BUZZER = 1;
        LCD_String_xy(6,0,"ON ");
    }else{
        LED3 = 0;
        BUZZER = 0;
        LCD_String_xy(6,0,"OFF");
        char buffer_TX[] = "";
    }
    
    LCD_String_xy(4,0,"SF: ");
    if(MD_Pir3){
        LED4 = 1;
        BUZZER = 1;
        LCD_String_xy(6,0,"ON ");
    }else{
        LED4 = 0;
        LCD_String_xy(6,0,"OFF");
        BUZZER = 0;
    }
    
                if(MD_Pir1 || MD_Pir2 || MD_Pir3){
                    
                //imprimir por puerto serial
                for (int i = 0; i < 39; i++) {
                     //  espera a que el registro de transmisión este disponible o vacio.
                     while (!TXSTAbits.TRMT) {
                     }
                     //  escribe el dato que se enviará a través de TX.
                     TXREG = buffer_TX[i];
                 }
                
            }
}

  /**
   * Sensor protocoloEmergencia'.
   * @fn .protocolo espepecial manda un mensaje y encienden las luces activa alarma 
   */
void protocoloEmergencia(){
    ADCON1 = 0x0F; /* @param this makes all pins as a digital I/O pins */
    PORT_Dir = 1; /* @param set as input port */
    PORT_Dir2 = 1;
    PORT_Dir3 = 1;
    LED_Dir = 0; /* @param set as output port */
    LED_Dir2 = 0;
    LED_Dir3 = 0;
    LED2 = 0; /* @param initially turned OFF LED */
    LED3 = 0;
    LED4 = 0;
    
    LCD_String_xy(2,0,"SI: ");
    if (MD_Pir1) {
        LED2 = 1;
        LCD_String_xy(6,0,"ON ");
        BUZZER = 1;
    } else {
        LED2 = 0;
        BUZZER = 0;
        LCD_String_xy(6,0,"OFF");
        char buffer_TX[] = "";
    }
    
    LCD_String_xy(3,0,"ST: ");
    if(MD_Pir2){
        LED3 = 1;
        BUZZER = 1;
        LCD_String_xy(6,0,"ON ");
    }else{
        LED3 = 0;
        BUZZER = 0;
        LCD_String_xy(6,0,"OFF");
        char buffer_TX[] = "";
    }
    
    LCD_String_xy(4,0,"SF: ");
    if(MD_Pir3){
        LED4 = 1;
        BUZZER = 1;
        LCD_String_xy(6,0,"ON ");
    }else{
        LED4 = 0;
        LCD_String_xy(6,0,"OFF");
        BUZZER = 0;
    }
    
                if(MD_Pir1 || MD_Pir2 || MD_Pir3){
                    
                //imprimir por puerto serial
                for (int i = 0; i < 39; i++) {
                     //  espera a que el registro de transmisión este disponible o vacio.
                     while (!TXSTAbits.TRMT) {
                     }
                     //  escribe el dato que se enviará a través de TX.
                     TXREG = buffer_TX[i];
                 }
                
            }
}

  /**
   * Sensor protocolo Fin de Semana (Sunday,saturday)'.
   * @fn Enciende luces y alarma. 
   */
void protocoloFDS(){
    ADCON1 = 0x0F; /* @param this makes all pins as a digital I/O pins */
    PORT_Dir = 1; /* @param  set as input port */
    PORT_Dir2 = 1;
    PORT_Dir3 = 1;
    LED_Dir = 0; /*@param set as output port */
    LED_Dir2 = 0;
    LED_Dir3 = 0;
    LED2 = 0; /* @param initially turned OFF LED */
    LED3 = 0;
    LED4 = 0;
   
    
    LCD_String_xy(2,0,"SI: ");
    if (MD_Pir1) {
        LED2 = 1;
        LCD_String_xy(6,0,"ON ");
        BUZZER = 1;
    } else {
        LED2 = 0;
        BUZZER = 0;
        LCD_String_xy(6,0,"OFF");
        char buffer_TX[] = "";
    }
    
    LCD_String_xy(3,0,"ST: ");
    if(MD_Pir2){
        LED3 = 1;
        BUZZER = 1;
        LCD_String_xy(6,0,"ON ");
    }else{
        LED3 = 0;
        BUZZER = 0;
        LCD_String_xy(6,0,"OFF");
        char buffer_TX[] = "";
    }
    
    LCD_String_xy(4,0,"SF: ");
    if(MD_Pir3){
        LED4 = 1;
        BUZZER = 1;
        LCD_String_xy(6,0,"ON ");
    }else{
        LED4 = 0;
        LCD_String_xy(6,0,"OFF");
        BUZZER = 0;
    }
    
                if(MD_Pir1 || MD_Pir2 || MD_Pir3){
                    
                //imprimir por puerto serial
                for (int i = 0; i < 39; i++) {
                     //  espera a que el registro de transmisión este disponible o vacio.
                     while (!TXSTAbits.TRMT) {
                     }
                     //  escribe el dato que se enviará a través de TX.
                     TXREG = buffer_TX[i];
                 }
                
            }
}

  /**
   * Sensor protocolo dia 6 a 18'.
   * @fn Enciende alarma. 
   */
void protocoloProtocoloDia(){
    ADCON1 = 0x0F; /* this makes all pins as a digital I/O pins */
    PORT_Dir = 1; /* set as input port */
    PORT_Dir2 = 1;
    PORT_Dir3 = 1;
    LED_Dir = 0; /* set as output port */
    LED_Dir2 = 0;
    LED_Dir3 = 0;
    LED2 = 0; /* initially turned OFF LED */
    LED3 = 0;
    LED4 = 0;
    
    LCD_String_xy(2,0,"SI: ");
    if (MD_Pir1) {
        LED2 = 1;
        LCD_String_xy(6,0,"ON ");
    } else {
        LED2 = 0;
        LCD_String_xy(6,0,"OFF");
    }
    
    LCD_String_xy(3,0,"ST: ");
    if(MD_Pir2){
        LED3 = 1;
        LCD_String_xy(6,0,"ON ");
    }else{
        LED3 = 0;
        LCD_String_xy(6,0,"OFF");
    }
    
    LCD_String_xy(4,0,"SF: ");
    if(MD_Pir3){
        LED4 = 1;
        LCD_String_xy(6,0,"ON ");
    }else{
        LED4 = 0;
        LCD_String_xy(6,0,"OFF");
        BUZZER = 0;
    }
}

/**
   * Sensor protocolo noche 18 a 6'.
   * @fn Enciende alarma. 
   */
void protocoloNoche(){
    ADCON1 = 0x0F; /* @param this makes all pins as a digital I/O pins */
    PORT_Dir = 1; /* @param set as input port */
    PORT_Dir2 = 1;
    PORT_Dir3 = 1;
    LED_Dir = 0; /* @param set as output port */
    LED_Dir2 = 0;
    LED_Dir3 = 0;
    LED2 = 0; /* @param initially turned OFF LED */
    LED3 = 0;
    LED4 = 0;
    
    LCD_String_xy(2,0,"SI: ");
    if (MD_Pir1) {
        LED2 = 1;
        LCD_String_xy(6,0,"ON ");
        BUZZER = 1;
    } else {
        LED2 = 0;
        BUZZER = 0;
        LCD_String_xy(6,0,"OFF");
        char buffer_TX[] = "";
    }
    
    LCD_String_xy(3,0,"ST: ");
    if(MD_Pir2){
        LED3 = 1;
        BUZZER = 1;
        LCD_String_xy(6,0,"ON ");
    }else{
        LED3 = 0;
        BUZZER = 0;
        LCD_String_xy(6,0,"OFF");
        char buffer_TX[] = "";
    }
    
    LCD_String_xy(4,0,"SF: ");
    if(MD_Pir3){
        LED4 = 1;
        BUZZER = 1;
        LCD_String_xy(6,0,"ON ");
    }else{
        LED4 = 0;
        LCD_String_xy(6,0,"OFF");
        BUZZER = 0;
    }
    
                if(MD_Pir1 || MD_Pir2 || MD_Pir3){
                    
                //imprimir por puerto serial
                for (int i = 0; i < 39; i++) {
                     //  espera a que el registro de transmisión este disponible o vacio.
                     while (!TXSTAbits.TRMT) {
                     }
                     //  escribe el dato que se enviará a través de TX.
                     TXREG = buffer_TX[i];
                 }
                
            }
}







