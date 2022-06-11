/* 
 * File:   state_control.h
 * Author: M62849
 *
 * Created on April 9, 2021, 10:55 AM
 */

#ifndef STATE_CONTROL_H
#define	STATE_CONTROL_H

#ifdef	__cplusplus
extern "C" {
#endif

typedef enum{
    INIT_STATE,
    WIFI_WAIT_FOR_CREDENTIALS_STATE,
    WIFI_CONNECTING_STATE,
    WIFI_CONNECTED_STATE,
    WIFI_WAIT_FOR_PING,
    SOCKET_INIT_STATE,
    SOCKET_START_STATE,
    SOCKET_CONNECTING_STATE,
    SOCKET_CONNECTED_STATE,
    IDLE,
    ERROR_STATE,
    RESTART_STATE
}possible_state;

possible_state current_state;

typedef enum {
    IDLE_STATE,
    MQTT_CONNECT,
    MQTT_ERROR,
    PROCESS_LAST_PACKET,
    MQTT_PUBLISH,
    MQTT_SUBSCRIBE
}mqtt_state;

mqtt_state current_mqtt_state;

void change_state(possible_state new_state);

void change_mqtt_state(mqtt_state new_state);


#ifdef	__cplusplus
}
#endif

#endif	/* STATE_CONTROL_H */

