
//copy the two library files from piclib.ddavidebor.it into the main project folder
#include "Severi.h"
#include "pwm.h


/*
 HOW TO USE:
 *
 * Usage is very simple:
 * -- be sure to define _XTAL_FREQ
 * -- in your setup routine (the one that run once) launch PWM_Init(desired frequency). frequency will be aproximated
 * -- change your pwm with  PWM1_Duty(); or PWM2_Duty();  [values from 0 to 1023]
 * -- start your pwm with PWM1_Start(); or PWM2_Start();
 * -- change your pwm sgsin if you want with  PWM1_Duty(); or PWM2_Duty();  [values from 0 to 1023]
 * -- stop your pwm with PWM1_Stop(); or PWM2_Stop();
 *
 * PIN CONFIGURATION:
 * On PIC16F876A PWM1 is on RC2 and PWM2 is on RC1

 */
 
 /* *******PWM.H FUNCTIONS**********
initialize the pwm. input value is frequency. look at datasheet for more info
PWM_Init(frequency);

set the Duty cycle from 0 to 1023
PWM1_Duty(duty cycle);
PWM2_Duty(duty cycle);

start the pwm and apply the duty cycle if previously set
PWM1_Start();
PWM2_Start();

stop the pwm. Please remember that output will be set low
PWM1_Stop();
PWM2_Stop();
*/

void main(void)
{
	//initialize portc
	PORTC = 0x00;
	TRISC = 0x00;
	
	//initialize pwm
	PWM_Init(5000); //initialize pwm at 5khz
	PWM1_Duty(0); //set initial duty cycle at 0 on pin RC2
	PWM2_Duty(0); 
	
	//start pwm
	PWM1_Start();
    PWM2_Start();
	
	
	while (1)
	{
	
		for (int i=0; i<1023; i+=4)
		{
			PWM1_duty();
			PWM2_Duty(1023-i);
			__delay_ms(2);
		}
	
	
	}


}