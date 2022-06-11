/* 
 * File:   MQTT_pub_cb.h
 * Author: M62849
 *
 * Created on May 9, 2021, 11:23 AM
 */

#ifndef MQTT_PUB_CB_H
#define	MQTT_PUB_CB_H

#ifdef	__cplusplus
extern "C" {
#endif

void MQTT_publish_handler_cb(uint8_t *topic, uint8_t *payload);


#ifdef	__cplusplus
}
#endif

#endif	/* MQTT_PUB_CB_H */

