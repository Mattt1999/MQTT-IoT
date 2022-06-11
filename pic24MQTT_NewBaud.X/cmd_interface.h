/* 
 * File:   cmd_interface.h
 * Author: M62849
 *
 * Created on May 5, 2021, 4:58 PM
 */

#ifndef CMD_INTERFACE_H
#define	CMD_INTERFACE_H

#ifdef	__cplusplus
extern "C" {
#endif

#define DELIMS " \n\t\r\a"
#define BUFFER_SIZE 64 // size of the input buffer

int number_of_commands(void);

uint32_t user_input(void);

void set_new_uart_interrupt(void);


#ifdef	__cplusplus
}
#endif

#endif	/* CMD_INTERFACE_H */

