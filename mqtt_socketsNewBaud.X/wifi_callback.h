/* 
 * File:   wifi_callback.h
 * Author: M62849
 *
 * Created on April 9, 2021, 10:49 AM
 */

#ifndef WIFI_CALLBACK_H
#define	WIFI_CALLBACK_H

#ifdef	__cplusplus
extern "C" {
#endif


//callback function which handles the connection with the ATWINC1510 device
void wifi_call_back(uint8_t msg_type, void *p_msg);

#ifdef	__cplusplus
}
#endif

#endif	/* WIFI_CALLBACK_H */

