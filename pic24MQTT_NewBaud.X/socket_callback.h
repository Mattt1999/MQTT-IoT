/* 
 * File:   socket_callback.h
 * Author: M62849
 *
 * Created on May 6, 2021, 12:32 PM
 */

#ifndef SOCKET_CALLBACK_H
#define	SOCKET_CALLBACK_H

#ifdef	__cplusplus
extern "C" {
#endif

#define WIFI_SERVER_PORT 1883     

void socket_call_back(SOCKET soc, uint8_t msg, void *p_msg);


#ifdef	__cplusplus
}
#endif

#endif	/* SOCKET_CALLBACK_H */

