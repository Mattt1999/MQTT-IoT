/* 
 * File:   application_init.h
 * Author: M62849
 *
 * Created on April 9, 2021, 11:19 AM
 */

#ifndef APPLICATION_INIT_H
#define	APPLICATION_INIT_H

#ifdef	__cplusplus
extern "C" {
#endif

#define SEC 200

bool is_time_to_publish;

timerStruct_t publish_timer;
timerStruct_t user_input_timer;

void wifi_init(void);
void application_init(void);
void set_MQTT_publish_Handler(void);

#ifdef	__cplusplus
}
#endif

#endif	/* APPLICATION_INIT_H */

