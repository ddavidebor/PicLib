#ifndef __UART_DDAVIDEBOR__
#define __UART_DDAVIDEBOR__

//Original library by electrosome.com
//modification and repackaging by Davide Bortolami


char UART_Init(const long int );             //initialize uart at specified baudrate
char UART_TX_Empty(void);
char UART_Data_Ready(void);
char UART_Read(void);                        //read a char
void UART_Read_Text(char *, unsigned int );  //acquire text on specified string of specified lenght
void UART_Write_Char(char );                 //send a char
void UART_Write_String(char *);              //dend a string
void UART_printf(char *, ...);               //near complete printf function
void UART_Read_Line(char*);                  //read a line terminated witl CR or LF characters


//Initialize uart
//Suggested baudrate 9600
char UART_Init(const long int baudrate)
{
	unsigned int x;
	x = (_XTAL_FREQ - baudrate*64)/(baudrate*64);
	if(x>255)
	{
		x = (_XTAL_FREQ - baudrate*16)/(baudrate*16);
		BRGH = 1;
	}
	if(x<256)
	{
	  SPBRG = x;
	  SYNC = 0;
	  SPEN = 1;
          TRISC7 = 1;
          TRISC6 = 1;
          CREN = 1;
          TXEN = 1;
	  return 1;
	}
	return 0;
}

//check transmit register
char UART_TX_Empty(void)
{
  return TRMT;
}

//Return true if new data is available to be read
char UART_Data_Ready(void)
{
   return RCIF;
}

//wait until a character is received and return it
char UART_Read(void)
{
 
  while(!RCIF);
  return RCREG;
}


//wait until N caracter are received and write the value in the passed array
void UART_Read_Text(char *Output, unsigned int length)
{
	for(int i=0;i<length;i++)
		Output[i] = UART_Read();
}

//wait until N caracter are received and write the value in the passed array
void UART_Read_Line(char *Output)
{
    char temp = 0;
    char i;
    for(i=0; temp != '\n' && temp != '\r' ; i++)
    {
        while(!RCIF);
        temp = UART_Read();
        Output[i] = temp;
    }
    Output[i-1] = '\0';

}


//write a char
void UART_Write_Char(char data)
{
  while(!TRMT);
  TXREG = data;
}

//write a string
void UART_Write_String(char *text)
{
  int i;
  for(i=0;text[i]!='\0';i++)
	  UART_Write_Char(text[i]);
}

//custom printf with small footprint
//remove if you do not want to use the printf function
#include "UART_printf.h"

#endif