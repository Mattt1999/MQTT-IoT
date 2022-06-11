#include "mcc_generated_files/mcc.h"
#include "mcc_generated_files/drivers/i2c_simple_master.h"
#include "mcc_generated_files/include/adc0.h"
#include "sensors_handle.h"

float get_light(void)
{
    float lux;
    uint16_t res = ADC0_GetConversion(ADC_MUXPOS_AIN5_gc);
    float vin = (res*3.3)/1023.0;
    lux = vin * 100.0 * 2.0;
    return lux;
}

float get_temperature(void)
{  
    //float temp;
    float temp;
    int16_t res;
    int16_t up_byte, low_byte;
    
    res = i2c_read2ByteRegister(TEMP_SENSOR_ADDR, TEMP_SENSOR_TA_ADDR);
    
    up_byte = res & UPPER_BYTE_MASK;
    low_byte = res & LOWER_BYTE_MASK;
    
    up_byte = up_byte & 0x1F00; // clear flag bits
    up_byte = up_byte >> 8;
    
    if((up_byte & 0x10) == 0x10) // if temp is negative
    {
        temp = 256 - (up_byte * 16.0) + (low_byte / 16.0);
    }
    else //if temp is positive
    { 
        temp = (up_byte * 16.0) + (low_byte / 16.0);
    }
    
    return temp;
}
