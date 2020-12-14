/*@class LCD_iNIT
  *
 */
#include<xc.h>
#include "LCD_20x4_Header_File.h"

/****************************Functions********************************/

/**
   * Iniciar display.
   * Inicializa componentes del displayl.
   */
void LCD_Init()
{
   	LCD_Command_Dir = 0;		/* @param Make LCD command port direction as o/p */
	LCD_Data_Dir = 0;							/* @param Make LCD data port direction as o/p */
	
	MSdelay(20);									/* @param LCD power up time to get things ready, it should always >15ms */
	LCD_Command (0x38);								/* @param Initialize 16X2 LCD in 8bit mode */
	LCD_Command (0x0C);								/* @param Display ON, Cursor OFF command */
	LCD_Command (0x06);								/* @param Auto Increment cursor */
	LCD_Command (0x01);								/* @param Clear LCD command */
	MSdelay(3);
	LCD_Command (0x80);
}

/**
   * clean display.
   * .
   */
void LCD_Clear()
{
    	LCD_Command (0x01);								/* @param Clear LCD command */
        MSdelay(3);
//        LCD_Command (0x80);								/* @param 8 is for first line and 0 is for 0th position */
//        MSdelay(1);
}

/**
   * Iniciar display.
   * Inicializa componentes del displayl.
   */
void LCD_Command(char cmd )
{
	LCD_Data_Port = cmd;							/* @param Write command data to LCD data port */
	RS = 0;                                			/* @param Make RS LOW (command reg.), RW LOW (Write) */
	//RW = 0;
    EN = 1;					/* @param High to Low transition on EN (Enable) */
	NOP();
	EN = 0;
	MSdelay(3);	
}
/**
   * Iniciar display.
   * Inicializa componentes del displayl.
   */
void LCD_Char(char char_data)
{
	LCD_Data_Port = char_data;						/* @param Write data to LCD data port */
	RS = 1;                                			/* @param Make RS LOW (command reg.), RW LOW (Write) */
	//RW = 0;
    EN = 1;					/* @param High to Low transition on EN (Enable) */
	NOP();
	EN = 0;
	MSdelay(3);
}

/**
   * String lcd.
   * .
   */
void LCD_String(const char *msg)
{
	while((*msg)!=0)
	{		
	  LCD_Char(*msg);
	  msg++;	
    }
		
}
/**
   * Escribir en display.
   * Selecciona la posicion en la que se va a escribir.
   */
void LCD_String_xy(char row,char pos,const char *str)
{
    if (row == 1)
		LCD_Command((pos & 0x0F)|0x80);				/* @param Command of first row and required position<16 */
	else if (row == 2)
		LCD_Command((pos & 0x0F)|0xC0);				/* @param Command of Second row and required position<16 */
	else if (row == 3)
		LCD_Command((pos & 0x0F)|0x94);				/* @param Command of Third row and required position<16 */
	else if (row == 4)
		LCD_Command((pos & 0x0F)|0xD4);				/* @param Command of Fourth row and required position<16 */
	LCD_String(str);
}

/**
   * Escribir en display.
   * 
   */
void LCD_Custom_Char(unsigned char loc,unsigned char *msg)
{
    unsigned char i;
    if(loc<8)
    {
     LCD_Command(0x40+(loc*8));     /* @param Command 0x40 and onwards forces the device to point CGRAM address */
       for(i=0;i<8;i++)             /* @param Write 8 byte for generation of 1 character */
           LCD_Char(msg[i]);
        
    }   
}

/*********************************Delay Function********************************/
/**
   * tiempo de retardo.
   * .
   */
void MSdelay(unsigned int val)
{
     unsigned int i,j;
        for(i=0;i<=val;i++)
            for(j=0;j<165;j++);      /*@param This count Provide delay of 1 ms for 8MHz Frequency */
 }


//void eeprom_writex(int address, char data)
//{
//    EEADR = address;		//  1.Escribir la dirección de la memoria en el registro EEADR
//    EEDATA = data;          //  2.	Escribir el dato a guardar en el registro EEDAT
//    EECON1bits.EEPGD = 0;	//  3.	Acceder a la memoria de datos de la EEPROM
//    EECON1bits.CFGS = 0;	//  4.	Acceder a la EEPROM
//    EECON1bits.WREN = 1;	//  5.	Habilitar escritura
//    INTCONbits.GIE = 0;     //  6.	Deshabilitar interrupciones
//    
//    //  7.  Enviar secuencia EECON2 = 0x55 y EECON2 = 0xAA y 
//    //      empezar la escritura en la memoria EEPROM
//    EECON2 = 0x55;
//    EECON2 = 0xaa;
//    EECON1bits.WR = 1;          // Bit que empieza la escritura en la memoria
//    
//    INTCONbits.GIE = 1;         //  8.	Habilitar interrupciones
//    while (EECON1bits.WR);      // 9.	Esperar hasta que la escritura se complete 
//}
//
//char eeprom_readx(int address)
//{
//    EEADR = address;        //  1.	Escribir la dirección de la memoria a leer en el registro EEADR  
//    EECON1bits.EEPGD = 0;	//  2.	Acceder a la memoria de datos de la EEPROM
//    EECON1bits.CFGS = 0;	//  3.	Acceder a la EEPROM
//    EECON1bits.RD = 1;      //  4.	Leer datos de la EEPROM
//    return(EEDATA);
//}



