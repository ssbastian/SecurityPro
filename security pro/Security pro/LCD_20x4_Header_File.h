
// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef XC_HEADER_TEMPLATE_H
#define	XC_HEADER_TEMPLATE_H

#include <xc.h> // include processor files - each processor file is guarded.  


//#define _XTAL_FREQ 4000000

#define LCD_Data_Dir TRISD				/* Define LCD data port direction */
#define LCD_Command_Dir TRISE			/* Define LCD command port direction register */
#define LCD_Data_Port LATD				/* Define LCD data port */
#define LCD_Command_Port LATE			/* Define LCD data port */
#define EN LATE2							/* Define Enable signal pin */
//#define RW LATD2							/* Define Read/Write signal pin */
#define RS LATE1	

#define BUZZER PORTEbits.RE0
//CONFIG SENSOR 1 (PIR))
#define MD_Pir1 PORTAbits.RA2      /* Read PIR sensor's data on this pin */
#define PORT_Dir TRISAbits.RA2             /* define for setting direction */
#define LED2 LATC0                           /* connect LED to the PORTD.0 pin */
#define LED_Dir TRISCbits.RC0              /* define for setting direction */
//CONFIG SENSOR 2 (PIR)
#define MD_Pir2 PORTAbits.RA3
#define PORT_Dir2 TRISAbits.RA3
#define LED3 LATC1
#define LED_Dir2 TRISCbits.RC1
//CONFIG SENSOR 3 (PIR)
#define MD_Pir3 PORTAbits.RA4
#define PORT_Dir3 TRISAbits.RA4
#define LED4 LATC2
#define LED_Dir3 TRISCbits.RC2

void eeprom_writex(int address, char data);
char eeprom_readx(int address);

void LCD_Init();
void LCD_Command(char );
void LCD_Char(char x);
void LCD_String(const char *);
void MSdelay(unsigned int );
void LCD_String_xy(char ,char ,const char*);
void LCD_Custom_Char(unsigned char loc,unsigned char *msg);
void LCD_Clear();

#endif	/* XC_HEADER_TEMPLATE_H */

