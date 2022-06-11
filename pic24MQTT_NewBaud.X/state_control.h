/* 
 * File:   state_control.h
 * Author: M62849
 *
 * Created on May 6, 2021, 3:36 PM
 */

#ifndef STATE_CONTROL_H
#define	STATE_CONTROL_H

#ifdef	__cplusplus
extern "C" {
#endif

typedef enum 
{
    INIT_STATE,
    IDLE,
    WIFI_WAIT_FOR_CREDENTIALS_STATE,
    WIFI_CONNECTING_STATE,
    WIFI_CONNECTED_STATE,
    WIFI_WAIT_FOR_PING,
    SOCKET_INIT_STATE,
    SOCKET_START_STATE,
    SOCKET_CONNECTING_STATE,
    ERROR_STATE
} POSSIBLE_STATE_CONNECTION;

extern POSSIBLE_STATE_CONNECTION current_state;

typedef enum 
{
    IDLE_STATE,
    CONNECT_STATE,
    MQTT_ERROR,
    PROCESS_LAST_PACKET,
    PUBLISH_STATE,
    SUBSCRIBE_STATE
}MQTT_STATE;

extern MQTT_STATE mqtt_current_state;

void change_state(POSSIBLE_STATE_CONNECTION new_state);

void change_state_mqtt(MQTT_STATE new_state);


#ifdef	__cplusplus
}
#endif

#endif	/* STATE_CONTROL_H */

