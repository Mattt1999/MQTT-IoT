/* 
 * File:   socket_cb.h
 * Author: M62849
 *
 * Created on April 9, 2021, 12:39 PM
 */

#ifndef SOCKET_CB_H
#define	SOCKET_CB_H

#ifdef	__cplusplus
extern "C" {
#endif

    
uint8_t recv_buffer[256];
void socket_call_back(SOCKET soc, uint8_t msg, void *p_msg);


#ifdef	__cplusplus
}
#endif

#endif	/* SOCKET_CB_H */

