/* 
 * File:   app_init.h
 * Author: M62849
 *
 * Created on May 5, 2021, 4:18 PM
 */

#ifndef APP_INIT_H
#define	APP_INIT_H

#ifdef	__cplusplus
extern "C" {
#endif
    
extern bool is_time_to_publish;
    
void wifi_init(void);

void application_init(void);

void set_new_uart_interrupt(void);

void set_MQTT_publish_handler(void);

#ifdef	__cplusplus
}
#endif

#endif	/* APP_INIT_H */

