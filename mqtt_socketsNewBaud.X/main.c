/*
    (c) 2018 Microchip Technology Inc. and its subsidiaries. 
    
    Subject to your compliance with these terms, you may use Microchip software and any 
    derivatives exclusively with Microchip products. It is your responsibility to comply with third party 
    license terms applicable to your use of third party software (including open source software) that 
    may accompany Microchip software.
    
    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER 
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY 
    IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS 
    FOR A PARTICULAR PURPOSE.
    
    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, 
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND 
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP 
    HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO 
    THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL 
    CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT 
    OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS 
    SOFTWARE.
 */

#include "mcc_generated_files/mcc.h"
#include "mcc_generated_files/examples/uart_example.h"
#include "mcc_generated_files/examples/mqtt_example.h"
#include "../winc/include/winc.h"
#include "../winc/include/winc_legacy.h"
#include "mcc_generated_files/winc/socket/socket.h"
#include <util/delay.h>
#include "../drivers/timeout.h"
#include "mqtt_core/mqtt_core.h"
#include "mcc_generated_files/mqtt/mqtt_packetTransfer_interface.h"
#include "wifi_callback.h"
#include "state_control.h"
#include "application_init.h"
#include "ping_cb.h"
#include "socket_cb.h"
#include "MQTT_create_packet.h"
#include "sensors_handle.h"

//The purpose of this project is to test the mqtt functionality.
//Connect to AP, then use MQTT/Sockets

//at the final code, these will be read from the serial terminal
//#define MAIN_WLAN_SSID "BOTO-NET" // the name of the network, also called SSID
//#define MAIN_WLAN_AUTH M2M_WIFI_SEC_WPA_PSK // the authentification security type
//#define MAIN_WLAN_PSK  "HELIADE69" // password for destination network

#define WIFI_SERVER_PORT 1883// for now, port that will be used with the online MQTT broker, test mosquito

//global variables
char dstIPaddr[] = "192.168.1.142";
bool is_time_to_publish = false;
mqttHeaderFlags last_received_header;
possible_state current_state = INIT_STATE;
mqtt_state current_mqtt_state = IDLE_STATE;

struct sockaddr_in addr;

char r[11];
float l;
float temp;

/*
 * To do: problem with the error led... Needs to be closed after the error is solved.
 * Solution: close the led in the callback functions if they are successful.
 */
int main(void) {
    /* Initializes MCU, drivers and middleware */
    SYSTEM_Initialize();
    puts("\r\n");
    puts("START\r\n");

    application_init();

    while (1) {
        // extract info about the socket_id
        mqttContext* mqtt_connect_info = MQTT_GetClientConnectionInfo();

        switch (current_state) {
            case(INIT_STATE):
            {
                //New approach, in the init state the device will be idle, in order to accept the wifi connection data from the command_line
                //These lines may be uncommented, since they do not affect the problem...
                //the problem is the order in which the timers were declared...

                if (m2m_wifi_default_connect() != M2M_SUCCESS) {
                    puts("\r\nCouldn't connect to default access point");
                    puts("\r\nPlease type new wifi credentials\r\n");
                    //defaulut connection failed, change the state to one that waits for new connection
                    change_state(WIFI_WAIT_FOR_CREDENTIALS_STATE);
                } else {
                    puts("\r\nConnecting to the last AP\r\n");
                    change_state(WIFI_CONNECTING_STATE);
                }

                break;
            }
            case(WIFI_WAIT_FOR_CREDENTIALS_STATE):
            {
                //this state is just waiting for new credentials
                puts("\nPlease type new connection credentials\n");
                change_state(IDLE);
                break;
            }

            case(WIFI_CONNECTING_STATE):
            {
                break;
            }

            case(WIFI_CONNECTED_STATE):
            {
                //light the BLUE LED to mark a wifi connection
                PORTD.OUT &= ~(PIN3_bm);

                //here will do a ping request to test the connection
                //the device we are pinging must be on the same network
                uint32 dest_ip = inet_addr(dstIPaddr);

                uint8 u8TTL = 250;

                printf("Pinging [%s] with TTL=%d\r\n", dstIPaddr, u8TTL);

                m2m_ping_req(dest_ip, u8TTL, ping_cb);

                change_state(WIFI_WAIT_FOR_PING);

                break;
            }

            case(WIFI_WAIT_FOR_PING):
            {
                break;
            }

            case(SOCKET_INIT_STATE):
            {
                //if the ping was successful, then it is ok to proceed with the sockets
                socketInit();
                registerSocketCallback(socket_call_back, NULL);
                change_state(SOCKET_START_STATE);
                break;
                                //if the socket is invalid 
//                if (*mqtt_connect_info->tcpClientSocket == -1) {
//                    close(*mqtt_connect_info->tcpClientSocket);
//                }
            }

            case(SOCKET_START_STATE):
            {
                //create new socket
                *mqtt_connect_info->tcpClientSocket = socket(AF_INET, SOCK_STREAM, 0);

                //this means that a new socket couldn't be created
                if (*mqtt_connect_info->tcpClientSocket < 0) {
                    change_state(ERROR_STATE);
                    break;
                }
                //initialize the socket details and connect
                addr.sin_family = AF_INET;
                addr.sin_port = _htons(WIFI_SERVER_PORT);
                addr.sin_addr.s_addr = inet_addr(dstIPaddr);

                //connection failed
                if (connect(*mqtt_connect_info->tcpClientSocket, (struct sockaddr *) &addr, sizeof (struct sockaddr_in)) < 0) {
                    close(*mqtt_connect_info->tcpClientSocket);
                    *mqtt_connect_info->tcpClientSocket = -1;
                    puts("\r\nError at socket connection\r\n");
                    change_state(ERROR_STATE);
                }
                else {
                    change_state(SOCKET_CONNECTING_STATE);
                }

                break;
            }

            case(SOCKET_CONNECTING_STATE):
            {
                //puts("\r\nSOCKET_CONNECTING_STATE\r\n");
                break;
            }

            case(ERROR_STATE):
            {
                //puts("\r\nAn error has occurred\r\n");

                //delete all and reinitialize
                //m2m_wifi_deinit(NULL);
                //delete the timers if an error has occured
                //timeout_delete(&publish_timer);
                //timeout_delete(&user_input_timer);
                //change_state(RESTART_STATE);

                //in the error state light the RED LED
                PORTD.OUT &= ~(PIN0_bm);
                break;
            }

            case(RESTART_STATE):
            {
                //if an error has occurred, try again from the beginning
                //for now, for testing purposes, do not delete the timers

                puts("\r\nRestarting...\r\n");
                application_init();
                change_state(INIT_STATE);

                break;
            }

            case(IDLE):
                break;

            default:
                break;
        }

        switch (current_mqtt_state) {
            case(IDLE_STATE):
                break;
            case(MQTT_CONNECT):
            {
                create_MQTT_connect_packet();

                MQTT_TransmissionHandler(mqtt_connect_info);
                mqttContext* mqtt_connect_info = MQTT_GetClientConnectionInfo();

                MQTT_ReceptionHandler(mqtt_connect_info);

                change_mqtt_state(PROCESS_LAST_PACKET);

                change_state(IDLE);

                break;
            }

            case(PROCESS_LAST_PACKET):
            {
                mqttContext* mqtt_connect_info = MQTT_GetClientConnectionInfo();

                puts("\nLastPacket\n");

                MQTT_ReceptionHandler(mqtt_connect_info);
                //MQTT_TransmissionHandler(mqtt_connect_info);
                last_received_header = MQTT_GetLastReceivedPacketHeader();

                if (last_received_header.All == 0x20) // conack received
                {
                    puts("\r\nConack received\r\n");
                    change_mqtt_state(MQTT_SUBSCRIBE);

                    //if CONNACK received, means that there is connection with broker
                    //light the GREEN LED:
                    PORTD.OUT &= ~(PIN2_bm);
                }

                if (last_received_header.All == 0x90) //suback received
                {
                    puts("\r\nSuback received\r\n");
                    change_mqtt_state(MQTT_PUBLISH);
                }

                break;
            }

            case(MQTT_SUBSCRIBE):
            {
                puts("\nSubscribe State\n");
                char t[] = "commands";
                create_MQTT_subscribe_packet(t);

                MQTT_TransmissionHandler(mqtt_connect_info);

                change_mqtt_state(PROCESS_LAST_PACKET);

                break;
            }

            case(MQTT_PUBLISH):
            {
                if (is_time_to_publish) {
                    l = get_light();
                    sprintf(r, "%.3f", l);

                    create_MQTT_publish_packet("light", r);
                    MQTT_TransmissionHandler(mqtt_connect_info);

                    temp = get_temperature();

                    sprintf(r, "%.3f", temp);

                    create_MQTT_publish_packet("temperature", r);
                    is_time_to_publish = false;

                    PORTD.OUT ^= PIN1_bm;
                }

                MQTT_TransmissionHandler(mqtt_connect_info);

                break;
            }

            case(MQTT_ERROR):
            {
                //puts("\r\nAn error has occurred\r\n");

                //delete all and reinitialize
                //m2m_wifi_deinit(NULL);
                //delete the timers if an error has occured
                //timeout_delete(&publish_timer);
                //timeout_delete(&user_input_timer);
                //change_state(RESTART_STATE);
                break;
            }

            default:
            {
                break;
            }
        }

        timeout_callNextCallback();

        m2m_wifi_handle_events(NULL);
    }
}


