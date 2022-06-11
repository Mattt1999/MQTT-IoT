/* 
 * File:   ping_cb.h
 * Author: M62849
 *
 * Created on April 9, 2021, 12:26 PM
 */

#ifndef PING_CB_H
#define	PING_CB_H

#ifdef	__cplusplus
extern "C" {
#endif


void ping_cb(uint32 u32IPAddr, uint32 u32RTT, uint8 u8ErrorCode);

void ping_callback(uint32 u32IPAddr, uint32 u32RTT, uint8 u8ErrorCode);

#ifdef	__cplusplus
}
#endif

#endif	/* PING_CB_H */

