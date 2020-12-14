
#include "PIC18F4550_ADC_Header_File.h"


void ADC_Init()
{    
    //TRISDbits.RD2 = 0;  /*set RD2 bit as output*/
//    TRISDbits.RD3 = 0;  /*set RD2 bit as output*/
//    TRISDbits.RD4 = 0;  /*set RD2 bit as output*/
//    TRISDbits.RD5 = 0;  /*set RD2 bit as output*/
    TRISA = 0xff;       /*set as input port*/
    ADCON0 = 0x85;
    ADCON1 = 0xC0;      /*ref vtg is VDD and Configure pin as analog pin*/    
    ADCON2 = 0x92;      /*Right Justified, 4Tad and Fosc/32. */
    ADRESH = 0;           /*Flush ADC output Register*/
    ADRESL = 0;   
}
/**
   * conversor ADC.
   * .
   */
int ADC_Read(int channel)
{
    int digital;
    ADCON0 =(ADCON0 & 0b11000011)|((channel<<2) & 0b00111100);      /*channel is selected i.e (CHS3:CHS0) 
                                                                      and ADC is disabled i.e ADON=0*/
    ADCON0 |= ((1<<ADON)|(1<<GO));                   /*Enable ADC and start conversion*/
    while(ADCON0bits.GO_nDONE==1);                  /*wait for End of conversion i.e. Go/done'=0 conversion completed*/        
    digital = (ADRESH*256) | (ADRESL);              /*Combine 8-bit LSB and 2-bit MSB*/
    return(digital);
}

