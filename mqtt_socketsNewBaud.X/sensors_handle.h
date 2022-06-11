/* 
 * File:   sensors_handle.h
 * Author: M62849
 *
 * Created on May 1, 2021, 10:20 PM
 */

#ifndef SENSORS_HANDLE_H
#define	SENSORS_HANDLE_H

#ifdef	__cplusplus
extern "C" {
#endif

#define UPPER_BYTE_MASK 0xFF00
#define LOWER_BYTE_MASK 0x00FF

#define TEMP_SENSOR_ADDR 0x18
#define TEMP_SENSOR_TA_ADDR 0x05   
#define TEMP_SENSOR_RES_ADDR 0x08

float get_light(void);

float get_temperature(void);


#ifdef	__cplusplus
}
#endif

#endif	/* SENSORS_HANDLE_H */

