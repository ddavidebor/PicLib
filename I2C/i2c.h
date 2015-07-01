#ifndef __I2C_DDAVIDEBOR__
#define __I2C_DDAVIDEBOR__


//***********************************************************
//Tested and modified by Davide Bortolami
//
//ddavidebor.it ddavidebor@gmail.com
//
//during year 2015
/*
 DESCRIPTION:
 i2c is a common protocol that allow MCUs (as pic, avr, arduino...) to
 comunicate easly with various digital IC.
 This library allow every pic16 to use i2c devices.

 COMPILER:
 * This library has been ported to XC8 for pic16 microcontroller
 * You need to define _XTAL_FREQ = [clock frequency] for the library to work correctly

 IMPLEMENTATION:
 This library is implemented in software only.
 It need only PORT and TRIS register.
 It does not use serial hardware on the pic.

 HARDWARE DESCRIPTION:
 i2c has two pin: SDA (Data) and SCL (clock)
 You need to use a pullup resistor (10Kohm is fine) on both SCL and SDA lines
//***********************************************************

/*

I2C bit-banged library
Written for midrange 8-bit PICs, (but should be easily portable)
2012, M. Eric Carr / Paleotechnologist.Net
eric@paleotechnologist.net

Free for all educational and noncommercial use:
Contact the author for commercial application requests


- Single-master mode only is supported (for now)

- Slave clock stretching is supported (with a spinloop;
  be aware that SCL shorted to ground could cause the
  code to hang. Comment out #define WAITFORSLAVES if
  this is not desirable.)

- 7-bit address mode only is supported.

- Code is configured for (PIC-like) TRIS=0 means output;
  comment out the relevant definition if otherwise (most other MCUs)

- Adjust the port and bit numbers for SCL and SDA as needed.
  Both can be on the same port or different ports.
  As written, SCL is on GPIO.4 and SDA is on GPIO.

*/

#ifndef __I2C_SEVERI_IO_CONFIG
#define __I2C_SEVERI_IO_CONFIG


    //Port and bit definitions. Adjust as needed.
    //-------------------------------------------
    #define TRISTATE_0_MEANS_OUTPUT		//Comment out if TRIS=1 means output
                                            //leave defined for pic

    #define SCL_PORT PORTA		//The port where the SCL pin is located
    #define SCL_TRIS TRISA		//The tristate register for the SCL bit
    #define SCL_ON_MASK  0b00000001	//This should OR-mask ON the SCL bit
    #define SCL_OFF_MASK 0b11111110	//This should AND-mask OFF the SCL bit

    #define SDA_PORT PORTA
    #define SDA_TRIS TRISA
    #define SDA_ON_MASK  0b00000010	//This should OR-mask ON the SDA bit
    #define SDA_OFF_MASK 0b11111101	//This should AND-mask OFF the SDA bit

#endif


//User functions implemented.
//(Try to use only these two, for simplicity)
//------------------------------------------------------------------
char I2C_ReadRegister(char deviceAddress, char registerAddress);
void I2C_WriteRegister(char deviceAddress, char registerAddress, char data);


//Support functions implemented:
//------------------------------------------------------------------
void I2C_Delay();				//Adjust as needed
void SCL_Low();					//Asserts SCL low
void SCL_High();				//Releases SCL to float high
void SDA_Low();					//Asserts SDA low
void SDA_High();				//Releases SDA to float high
void I2C_SendZero();				//Sends a "zero" bit
void I2C_SendOne();				//Sends a "one" bit
void I2C_Start();				//Sends the I2C start sequence
void I2C_Stop();				//Sends the I2C stop sequence
void I2C_WaitForSlave();			//Waits for SCL to be released


//useful function implemented
void I2C_WriteByte(char);			//Writes a byte to the I2C bus
char I2C_ReadResult();				//Clocks in a character
void I2C_WriteToAddress(char deviceAddress);	//Selects a device for writing
void I2C_ReadFromAddress(char deviceAddress);	//Selects a device for reading
void I2C_Test();                       //send from 0 to 255 in i2c for test






//Behavior definitions. Adjust as needed.
//---------------------------------------
//#define WAITFORSLAVES		//If defined, MCU will check for SCL high
				//after transactions. This supports
				//clock-stretching, but will cause the code
				//to hang if SCL is held low.
				//Comment this out to skip SCL checking.



//Function implementations
//----------------------------------------------------------------------------
void I2C_Delay(){
    
	__delay_us(10);		//A delay may or may not be needed here.
	}	


#ifdef TRISTATE_0_MEANS_OUTPUT
//Low and high definitions, if PIC-like (tristate 0 == output)
void SCL_Low()
        {
	SCL_PORT = SCL_PORT & SCL_OFF_MASK;
	SCL_TRIS = SCL_TRIS & SCL_OFF_MASK;
	}	

void SCL_High(){	//Really, just release it
	SCL_TRIS = SCL_TRIS | SCL_ON_MASK;
	}	

void SDA_Low(){
	SDA_PORT = SDA_PORT & SDA_OFF_MASK;
	SDA_TRIS = SDA_TRIS & SDA_OFF_MASK;
	}	

void SDA_High(){	//Really, just release it
	SDA_TRIS = SDA_TRIS | SDA_ON_MASK;
	}	

#else
//Low and high definitions, if tristate 1 == output
void SCL_High(){	//Really, just release it
	SCL_TRIS = SCL_TRIS & SCL_OFF_MASK;
	}	

void SCL_Low(){	
	SCL_PORT = SCL_PORT & SCL_OFF_MASK;
	SCL_TRIS = SCL_TRIS | SCL_ON_MASK;
	}	

void SDA_High(){	//Really, just release it
	SDA_TRIS = SDA_TRIS & SDA_OFF_MASK;
	}	

void SDA_Low(){	
	SDA_PORT = SDA_PORT & SDA_OFF_MASK;
	SDA_TRIS = SDA_TRIS | SDA_ON_MASK;
	}	

#endif




void I2C_SendZero(){
	SCL_Low();			//Should already be low
	SDA_Low();			//zero
	SCL_High();			//clock it out
	I2C_Delay();
	SCL_Low();			//clock idle
	}	

void I2C_SendOne(){
	SCL_Low();			//Should already be low
	SDA_High();			//one
	SCL_High();			//clock it out
	I2C_Delay();
	SCL_Low();			//clock idle
	}	

void I2C_Start(){
	SDA_High();			//prepare
	SCL_High();			//prepare
	I2C_Delay();
	SDA_Low();			//Start signal
	I2C_Delay();
	SCL_Low();			//Clock low to prepare for transmission
	I2C_Delay();
	}	


void I2C_Stop(){
	SCL_Low();			//prepare
	I2C_Delay();
	SDA_Low();			//prepare
	I2C_Delay();
	SCL_High();			//Stop signal
	I2C_Delay();
	SDA_High();			//Raise SDA to prepare for next Start signal
	I2C_Delay();
	}	


void I2C_WaitForSlave(){
	#ifdef WAITFORSLAVES
	while(SCL_PORT & SCL_ON_MASK == 0x00);	//Loop while the SCL line is low
	#endif
	}	


void I2C_WriteByte(char theByte){
	char temp = theByte;
	//Send the bits, MSB first (mask them off with 0x80)
	for(int x=0;x<8;x++){
		if((temp & 0x80) == 0x80){I2C_SendOne();}
			else{I2C_SendZero();}
		temp = temp << 1;
		}//for x
	//Send an ack clock bit (and ignore the result)
	I2C_SendOne();		
	//**DO NOT SEND A STOP HERE; NOT NECESSARILY DONE WITH THE TRANSACTION**
	}	


char I2C_ReadResult(){
	//Device already addressed in Read mode; just clock in the data...
	char readback = 0x00;
	//Send eight clock pulses, reading the data bus after each one
	SDA_High();	//Just to be sure
	for(int x=0;x<8;x++){
		readback = (readback << 1) & 0xFE;	//Mask off the new LSB
		SCL_High();
		I2C_WaitForSlave();			//Check if line is high
		if(SDA_PORT & SDA_ON_MASK){readback++;}
		I2C_Delay();
		SCL_Low();
		I2C_Delay();
		}	//for x
	SDA_Low();			//To prepare for stop condition
	I2C_Delay();
	SCL_High();
	I2C_WaitForSlave();		//Check if line really did go high
	I2C_Delay();
	SDA_High();			//Stop condition
	return(readback);
	}	


void I2C_WriteToAddress(char deviceAddress){
	char temp;
	temp = deviceAddress << 1;	//Shift it over one bit
	temp = temp & 0xFE;		//Ensure LSB == 0
	I2C_WriteByte(temp);
	}

void I2C_ReadFromAddress(char deviceAddress){
	I2C_WriteByte((deviceAddress << 1) | 0x01); // 7-bit address, then a 1
	}


char I2C_ReadRegister(char deviceAddress, char registerAddress){
	char result;
	//Write to the device, giving it the memory address to access
	I2C_Start();
	I2C_WriteToAddress(deviceAddress);
	I2C_WriteByte(registerAddress);

	//Read from the device at the memory address given above
	I2C_Start();
	I2C_ReadFromAddress(deviceAddress);
	result = I2C_ReadResult();
	return(result);
	}

void I2C_WriteRegister(char deviceAddress, char registerAddress, char data){
	char result;
	//Write to the device, giving it the memory address to access
	I2C_Start();
	I2C_WriteToAddress(deviceAddress);
	I2C_WriteByte(registerAddress);
	I2C_WriteByte(data);
	I2C_Stop();
	}

void I2C_Test(void){

    I2C_Start();
    for (int i = 0; i != 256; i++) {
        I2C_WriteByte(i); //address
    }
    I2C_Stop();
}

#endif