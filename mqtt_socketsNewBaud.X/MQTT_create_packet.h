/* 
 * File:   MQTT_create_packet.h
 * Author: M62849
 *
 * Created on April 9, 2021, 2:48 PM
 */

#ifndef MQTT_CREATE_PACKET_H
#define	MQTT_CREATE_PACKET_H

#ifdef	__cplusplus
extern "C" {
#endif
    
#define TEMP_TOPIC_PUB "/topic1"
#define TEMP_TOPIC_SUB "/topic2"
    
#define PUBLISH_MSG "message"
    

void create_MQTT_connect_packet(void);

void create_MQTT_subscribe_packet(char *topic);

void create_MQTT_publish_packet(char *topic, char *message);


#ifdef	__cplusplus
}
#endif

#endif	/* MQTT_CREATE_PACKET_H */

