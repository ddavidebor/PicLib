#ifndef _ADC_DDAVIDEBOR_
#define _ADC_DDAVIDEBOR_

//this library is made for 8 bit pics and XC8 compiler
//tested on pic16f876a

//FOSC MODE
//two MSB select the clock speed, the LSB power up the adc module
#define ADC_FOSC_2  0b00000001
#define ADC_FOSC_8  0b01000001
#define ADC_FOSC_32 0b10000001
#define ADC_FOSC_RC 0b11000001

//FOSC DIVISION SELECTION
#define ADC_FOSC_NORMAL         0b00000000
#define ADC_FOSC_DIVIDED_BY_TWO 0b01000000

//PIN CONFIGURATION
//four MSB are PCFG3-PCFG0 (look at datasheet for table
#define ADC_PORT_CONFIG 0b00000000


void ADC_Init(unsigned char FoscMode, unsigned char FoscDivision)
{

   ADCON0 = FoscMode;

   //change mode in the defines uptop
   ADCON1 = 0b10000000 | FoscDivision | ADC_PORT_CONFIG ;
}

void ADC_Power_On(void)
{
    ADCON0 |= 0b00000001;
}

void ADC_Power_Off(void)
{
    ADCON0 &= 0b11111110;
}

void ADC_De_init(unsigned char FoscMode)
{

   ADCON0 = 0x00;
   ADCON1 = 0x00;
}

unsigned int ADC_Read(unsigned char channel)
{
   if(channel > 7)
     return 0; 

   ADCON0 &= 0xC5;
   ADCON0 |= channel<<3;
   __delay_ms(2);
   GO_nDONE = 1;
   while(GO_nDONE);
   return ((ADRESH<<8)+ADRESL);
}

#endif