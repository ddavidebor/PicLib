//Ads1015 library
//Heavly adapted to the pic architecture, less abstracted i2c library
//Partially ported from Adafruit arduino library https://github.com/adafruit/Adafruit_ADS1X15



#ifndef ADS1015_ROUTINES_H
#define	ADS1015_ROUTINES_H

//include configuration mask registers
#include "ads1015-config.h"

//Acquire differential voltage between ain0 and ain1
//you need to define the range inside the code

float Get_differential_1_2_voltage(void) {
    //    possible settings:
    //
    //    channel from 0 to 4
    //    RANGE_6_144V   +/-6.144V range = Gain 2/3
    //    RANGE_4_096V   +/-4.096V range = Gain 1
    //    RANGE_2_048V   +/-2.048V range = Gain 2 (default)
    //    RANGE_1_024V   +/-1.024V range = Gain 4
    //    RANGE_0_512V   +/-0.512V range = Gain 8
    //    RANGE_0_256V   +/-0.256V range = Gain 16


    //the measured voltage
    float voltage = 0;

    //16 bit config register
    unsigned int config;

    //Range is fixed
    const unsigned char range = RANGE_0_512V;  // DEFINE THE RANGE

    //calculate ads1015 configuration register
    config = ADS1015_REG_CONFIG_CQUE_NONE    | // Disable the comparator (default val)
             ADS1015_REG_CONFIG_CLAT_NONLAT  | // Non-latching (default val)
             ADS1015_REG_CONFIG_CPOL_ACTVLOW | // Alert/Rdy active low   (default val)
             ADS1015_REG_CONFIG_CMODE_TRAD   | // Traditional comparator (default val)
             ADS1015_REG_CONFIG_DR_128SPS    | // 128 samples per second
             ADS1015_REG_CONFIG_MODE_SINGLE  ; // Single-shot mode (default)

    //set gain
    switch (range) {
        case (RANGE_0_256V):
            config |= ADS1015_REG_CONFIG_PGA_0_256V;
            break;
        case (RANGE_0_512V):
            config |= ADS1015_REG_CONFIG_PGA_0_512V;
            break;
        case (RANGE_1_024V):
            config |= ADS1015_REG_CONFIG_PGA_1_024V;
            break;
        case (RANGE_2_048V):
            config |= ADS1015_REG_CONFIG_PGA_2_048V;
            break;
        case (RANGE_4_096V):
            config |= ADS1015_REG_CONFIG_PGA_4_096V;
            break;
        case (RANGE_6_144V):
            config |= ADS1015_REG_CONFIG_PGA_6_144V;
            break;
    }



    // Set differential acquisition channel
    config |= ADS1015_REG_CONFIG_MUX_DIFF_0_1;

    // Set 'start single-conversion' bit
    config |= ADS1015_REG_CONFIG_OS_SINGLE;


    //write config register
    I2CStart();
    I2CSend(0b10010000); //ads1015 address
    I2CSend(0b00000001);  //point to config register
    I2CSend((unsigned char) (config >> 8)); //msb of config register
    I2CSend((unsigned char) (config & 0xFF)); //lsb of config register
    I2CStop();

    __delay_ms(20);

    //Point to conversion register
    I2CStart();
    I2CSend(0b10010000); //ads1015 address
    I2CSend(0b00000000); //point to conversion register
    I2CStop();

    //Buffer for data
    unsigned int data = 0;

    //Read conversion result
    I2CStart();
    I2CSend(0b10010001); //ads1015 address
    data = ((unsigned int)I2CRead() << 8);
    I2CAck(); //acknowledge after each byte read
    data |=I2CRead();
    I2CAck();
    I2CStop();

    //Data/16 is equivalent to >>4 but keeps the sign
    //(int) force data to be read as int
    voltage = ((int) data)/16;


    //Adjust value for the chosen voltage
    switch (range) {
            //multiplier = range/4096;
        case (RANGE_0_256V):
            voltage *=1.25e-4;
            break;
        case (RANGE_0_512V):
            voltage *= 2.5e-4;
            break;
        case (RANGE_1_024V):
            voltage *= 5e-4;
            break;
        case (RANGE_2_048V):
            voltage *= 1.0e-3;
            break;
        case (RANGE_4_096V):
            voltage *= 2.00e-3;
            break;
        case (RANGE_6_144V):
            voltage *= 3e-3;
            break;
    }

    return voltage;

}


//Acquire voltage in single ended mode
float VoltageReadSingleEnded(unsigned char channel, unsigned char range) {
    //    possible settings:
    //
    //    channel from 0 to 4
    //    RANGE_6_144V   +/-6.144V range = Gain 2/3
    //    RANGE_4_096V   +/-4.096V range = Gain 1
    //    RANGE_2_048V   +/-2.048V range = Gain 2 (default)
    //    RANGE_1_024V   +/-1.024V range = Gain 4
    //    RANGE_0_512V   +/-0.512V range = Gain 8
    //    RANGE_0_256V   +/-0.256V range = Gain 16


    float voltage = 0;
    unsigned int data = 0;
    unsigned int config;

    //Calculate ads1015 configuration register
    config = ADS1015_REG_CONFIG_CQUE_NONE | // Disable the comparator (default val)
            ADS1015_REG_CONFIG_CLAT_NONLAT | // Non-latching (default val)
            ADS1015_REG_CONFIG_CPOL_ACTVLOW | // Alert/Rdy active low   (default val)
            ADS1015_REG_CONFIG_CMODE_TRAD | // Traditional comparator (default val)
            ADS1015_REG_CONFIG_DR_128SPS | // 128 samples per second
            ADS1015_REG_CONFIG_MODE_SINGLE; // Single-shot mode (default)

    //Set gain
    switch (range) {
        case (RANGE_0_256V):
            config |= ADS1015_REG_CONFIG_PGA_0_256V;
            break;
        case (RANGE_0_512V):
            config |= ADS1015_REG_CONFIG_PGA_0_512V;
            break;
        case (RANGE_1_024V):
            config |= ADS1015_REG_CONFIG_PGA_1_024V;
            break;
        case (RANGE_2_048V):
            config |= ADS1015_REG_CONFIG_PGA_2_048V;
            break;
        case (RANGE_4_096V):
            config |= ADS1015_REG_CONFIG_PGA_4_096V;
            break;
        case (RANGE_6_144V):
            config |= ADS1015_REG_CONFIG_PGA_6_144V;
            break;
    }

    //Set acquisition channel
    switch (channel) {
        case (0):
            config |= ADS1015_REG_CONFIG_MUX_SINGLE_0;
            break;
        case (1):
            config |= ADS1015_REG_CONFIG_MUX_SINGLE_1;
            break;
        case (2):
            config |= ADS1015_REG_CONFIG_MUX_SINGLE_2;
            break;
        case (3):
            config |= ADS1015_REG_CONFIG_MUX_SINGLE_3;
            break;
    }

    // Set 'start single-conversion' bit
    config |= ADS1015_REG_CONFIG_OS_SINGLE;


    //write config register
    I2CStart();
    I2CSend(0b10010000); //ads1015 address
    I2CSend(0b00000001);  //point to config register
    I2CSend((unsigned char) (config >> 8)); //msb of config register
    I2CSend((unsigned char) (config & 0xFF)); //lsb of config register



    I2CStop();


    __delay_ms(20);

    //point to conversion register
    I2CStart();
    I2CSend(0b10010000); //ads1015 address
    I2CSend(0b00000000); //point to conversion register
    I2CStop();


    //read conversion result
    I2CStart();
    I2CSend(0b10010001); //ads1015 address
    data = ((unsigned int)I2CRead() << 8);
    I2CAck();
    data |=I2CRead();
    I2CAck();
    I2CStop();

    data = data >> 4; //From 12 bit to 16 bit



    voltage = (int) data;

    //Adjust value for voltage
    switch (range) {
            //multiplier = range/4096;
        case (RANGE_0_256V):
            voltage *=1.25e-4;
            break;
        case (RANGE_0_512V):
            voltage *= 2.5e-4;
            break;
        case (RANGE_1_024V):
            voltage *= 5e-4;
            break;
        case (RANGE_2_048V):
            voltage *= 1.0e-3;
            break;
        case (RANGE_4_096V):
            voltage *= 2.00e-3;
            break;
        case (RANGE_6_144V):
            voltage *= 3e-3;
            break;
    }


    return (voltage);

}

#endif	/* ADS1015_ROUTINES_H */

