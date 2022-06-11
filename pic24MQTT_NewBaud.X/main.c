/**
  Generated main.c file from MPLAB Code Configurator

  @Company
    Microchip Technology Inc.

  @File Name
    main.c

  @Summary
    This is the generated main.c using PIC24 / dsPIC33 / PIC32MM MCUs.

  @Description
    This source file provides main entry point for system initialization and application code development.
    Generation Information :
        Product Revision  :  PIC24 / dsPIC33 / PIC32MM MCUs - 1.170.0
        Device            :  PIC24FJ128GA705
    The generated drivers are tested against the following:
        Compiler          :  XC16 v1.61
        MPLAB 	          :  MPLAB X v5.45
 */

/*
    (c) 2020 Microchip Technology Inc. and its subsidiaries. You may use this
    software and any derivatives exclusively with Microchip products.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION
    WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.

    MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
    TERMS.
 */

/**
  Section: Included Files
 */
#include "mcc_generated_files/mcc.h"
#include "mcc_generated_files/system.h"
#include "mcc_generated_files/drivers/timeout.h"
#include "mcc_generated_files/winc/include/winc.h"
#include "../winc/include/winc_legacy.h"
#include "mcc_generated_files/mqtt/mqtt_core/mqtt_core.h"
#include "mcc_generated_files/mqtt/mqtt_packetTransfer_interface.h"
#include "mcc_generated_files/winc/socket/socket.h"
#include "mcc_generated_files/mqtt/mqtt_comm_bsd/mqtt_comm_layer.h"
#include "app_init.h"
#include "build_MQTT_packets.h"
#include "ping_callback.h"
#include "socket_callback.h"
#include "state_control.h"

#define FCY 4000000
#define WIFI_SERVER_PORT 1883// for now, port that will be used with the online MQTT broker, test mosquito

#include <libpic30.h> //for delay

//global variables
char dstIPaddr[] = "192.168.1.142";
mqttHeaderFlags last_received_header;
struct sockaddr_in addr;
bool is_time_to_publish = false;

POSSIBLE_STATE_CONNECTION current_state = INIT_STATE;

MQTT_STATE mqtt_current_state = IDLE_STATE;



int main(void) 
{
    // initialize the device
    SYSTEM_Initialize();

    //necessary for a good print after programming the device
    __delay_ms(1000);

    application_init();
    
    OC1_SecondaryValueSet(0x3A97);//set the 30ms period

    while (1) 
    {

        mqttContext* mqtt_connect_info = MQTT_GetClientConnectionInfo();

        switch (current_state) 
        {
            case(INIT_STATE):
            {
                if (m2m_wifi_default_connect() != M2M_SUCCESS) 
                {
                    puts("\nCouldn't connect to default access point");
                    puts("\nPlease type new wifi credentials");
                    //default connection failed, change the state to one that waits for new connection
                    change_state(WIFI_WAIT_FOR_CREDENTIALS_STATE);
                } 
                else 
                {
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
                //wifi connected, ligh the BLUE LED
                LATCbits.LATC5 = 0;
                
                //here will do a ping request to test the connection
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
            }

            case(SOCKET_START_STATE):
            {
                //if the socket is invalid 
                if (*mqtt_connect_info->tcpClientSocket == -1) 
                {
                    close(*mqtt_connect_info->tcpClientSocket);
                }

                //create new socket
                *mqtt_connect_info->tcpClientSocket = socket(AF_INET, SOCK_STREAM, 0);

                //this means that a new socket couldn't be created
                if (*mqtt_connect_info->tcpClientSocket < 0) 
                {
                    change_state(ERROR_STATE);
                    break;
                }
                //initialize the socket details and connect
                addr.sin_family = AF_INET;
                addr.sin_port = _htons(WIFI_SERVER_PORT);
                addr.sin_addr.s_addr = inet_addr(dstIPaddr);

                //connection failed
                if (connect(*mqtt_connect_info->tcpClientSocket, (struct sockaddr *) &addr, sizeof (struct sockaddr_in)) < 0) 
                {
                    close(*mqtt_connect_info->tcpClientSocket);
                    *mqtt_connect_info->tcpClientSocket = -1;
                    puts("\r\nError at socket connection\r\n");
                    change_state(ERROR_STATE);
                }
                else 
                {
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
                LATBbits.LATB4 = 0;
                break;
            }
            case(IDLE):
            {
                break;
            }          
        }
        
        switch(mqtt_current_state)
        {
            case(IDLE_STATE):
            {
//                MQTT_ReceptionHandler(mqtt_connect_info);
//                
//                MQTT_TransmissionHandler(mqtt_connect_info);
               
//                mqttContext* mqtt_connect_info = MQTT_GetClientConnectionInfo();
//                MQTT_ReceptionHandler(mqtt_connect_info);
//                
//                MQTT_TransmissionHandler(mqtt_connect_info);
                
                break;
            }
            case(CONNECT_STATE):
            {
                mqttContext* mqtt_connect_info = MQTT_GetClientConnectionInfo();
       
                create_MQTT_connect_packet();
                
                MQTT_TransmissionHandler(mqtt_connect_info);
                
                MQTT_ReceptionHandler(mqtt_connect_info);
                
                change_state_mqtt(PROCESS_LAST_PACKET);
                
                change_state(IDLE);
                
                break;
            }
            case(PROCESS_LAST_PACKET):
            { 
                //puts("\nProcessLastPacket\n");
                MQTT_ReceptionHandler(mqtt_connect_info);
                last_received_header = MQTT_GetLastReceivedPacketHeader();
                
                if(last_received_header.All == 0x20) // conack received
                {
                    change_state_mqtt(SUBSCRIBE_STATE);   
                    //if connack received light the GREEN LED:
                    LATCbits.LATC4 = 0;
                }
                
                if(last_received_header.All == 0x90) //suback received
                {
                    puts("\r\nSuback received\r\n");
                    change_state_mqtt(PUBLISH_STATE);
                }
                
                break;
            }
            
            case(SUBSCRIBE_STATE):
            {
                //will handle subscription to the automated topic "commands"
                //puts("\nSubscribe State\n");
                char t[] = "commands";
                create_MQTT_subscribe_packet(t);
                
                MQTT_TransmissionHandler(mqtt_connect_info);
                
                change_state_mqtt(PROCESS_LAST_PACKET);
                
                break;
            }
            
            case(PUBLISH_STATE):
            {
                if(is_time_to_publish)
                {
                    LATCbits.LATC3 ^= 1;//toggle the LED while publishing
                    create_MQTT_publish_packet("dummy", "d");
                    MQTT_TransmissionHandler(mqtt_connect_info);
                    is_time_to_publish = false;
                }
                
                MQTT_TransmissionHandler(mqtt_connect_info);
                
                mqttContext* mqtt_connect_info = MQTT_GetClientConnectionInfo();
                MQTT_ReceptionHandler(mqtt_connect_info);
                
                break;
            }
            
            case(MQTT_ERROR):
            {
                puts("\nMQTT error");
                change_state_mqtt(IDLE_STATE);
                break;
            }        
        }

        timeout_callNextCallback();

        m2m_wifi_handle_events(NULL);
    }
}

