#ifndef __PWM_SEVERI__
#define __PWM_SEVERI__

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

//define logic state applied after pwm stop
#define PWM1_DEFAULT_STATE 0
#define PWM2_DEFAULT_STATE 0


//initialize the pwm. input value is frequency. look at datasheet for more info
void PWM_Init(long);

//calculate max duty cycle. only for internal use
int PWM_Max_Duty(void);

//set the Duty cycle from 0 to 1023
void PWM1_Duty(unsigned int);
void PWM2_Duty(unsigned int);

//start the pwm and apply the duty cycle if previously set
void PWM1_Start(void);
void PWM2_Start(void);

//stop the pwm. Please remember that output will be set low
void PWM1_Stop(void);
void PWM2_Stop(void);


#define TMR2PRESCALE 4
long freq;




void PWM_Init(long fre)
{
  PR2 = (_XTAL_FREQ/(freq*4*TMR2PRESCALE)) - 1;
  freq = fre;
}

int PWM_Max_Duty(void)
{
   return(_XTAL_FREQ/(freq*TMR2PRESCALE));
}




void PWM1_Duty(unsigned int duty)
{
  if(duty<1024)
  {  
    duty = ((float)duty/1023)*PWM_Max_Duty();
    CCP1X = duty & 2;
    CCP1Y = duty & 1;
    CCPR1L = duty>>2;
  }
}


void PWM2_Duty(unsigned int duty)
{
  if(duty<1024)
  {  
    duty = ((float)duty/1023)*PWM_Max_Duty();
    CCP2X = duty & 2;
    CCP2Y = duty & 1;
    CCPR2L = duty>>2;
  } 
}

void PWM1_Start(void)
{
  CCP1M3 = 1;
  CCP1M2 = 1;
  #if TMR2PRESCALE == 1
  T2CKPS0 = 0;
  T2CKPS1 = 0;
  #elif TMR2PRESCALE == 4
  T2CKPS0 = 1;
  T2CKPS1 = 0;
  #elif TMR2PRESCALE == 16
  T2CKPS0 = 1;
  T2CKPS1 = 1;
  #endif
  TMR2ON = 1;
  TRISCbits.TRISC2 = 0;
}

void PWM1_Stop(void)
{
  CCP1M3 = 0;
  CCP1M2 = 0;
  PORTCbits.RC2 = PWM1_DEFAULT_STATE;

}

void PWM2_Start(void)
{
  CCP2M3 = 1;
  CCP2M2 = 1;
  #if TMR2PRESCALE == 1
  T2CKPS0 = 0;
  T2CKPS1 = 0;
  #elif TMR2PRESCALE == 4
  T2CKPS0 = 1;
  T2CKPS1 = 0;
  #elif TMR2PRESCALE == 16
  T2CKPS0 = 1;
  T2CKPS1 = 1;
  #endif
  TMR2ON = 1;
  TRISCbits.TRISC1 = 0;
}

void PWM2_Stop(void)
{
  CCP2M3 = 0;
  CCP2M2 = 0;
  PORTCbits.RC1 = PWM2_DEFAULT_STATE;
}




#endif
