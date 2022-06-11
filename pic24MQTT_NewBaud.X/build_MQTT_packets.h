/* 
 * File:   build_MQTT_packets.h
 * Author: M62849
 *
 * Created on May 7, 2021, 4:16 PM
 */

#ifndef BUILD_MQTT_PACKETS_H
#define	BUILD_MQTT_PACKETS_H

#ifdef	__cplusplus
extern "C" {
#endif

void create_MQTT_connect_packet(void);

void create_MQTT_subscribe_packet(char *topic);

void create_MQTT_publish_packet(char *topic, char *message);


#ifdef	__cplusplus
}
#endif

#endif	/* BUILD_MQTT_PACKETS_H */

