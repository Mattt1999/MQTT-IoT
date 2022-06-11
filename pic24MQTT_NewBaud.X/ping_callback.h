/* 
 * File:   ping_callback.h
 * Author: M62849
 *
 * Created on May 6, 2021, 12:18 PM
 */

#ifndef PING_CALLBACK_H
#define	PING_CALLBACK_H

#ifdef	__cplusplus
extern "C" {
#endif
  
void ping_cb(uint32 u32IPAddr, uint32 u32RTT, uint8 u8ErrorCode);

void ping_callback(uint32 u32IPAddr, uint32 u32RTT, uint8 u8ErrorCode);


#ifdef	__cplusplus
}
#endif

#endif	/* PING_CALLBACK_H */

