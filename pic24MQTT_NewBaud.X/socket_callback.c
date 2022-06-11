#include "mcc_generated_files/system.h"
#include "mcc_generated_files/mcc.h"
#include "../winc/include/winc.h"
#include "mqtt_core/mqtt_core.h"
#include "mcc_generated_files/winc/socket/socket.h"
#include "socket_callback.h"
#include "state_control.h"

uint8_t recv_buffer[256];   

void socket_call_back(SOCKET soc, uint8_t msg, void *p_msg)
{
    switch(msg)
    {
        case(SOCKET_MSG_CONNECT):
        {
            tstrSocketConnectMsg *con_msg = (tstrSocketConnectMsg *) p_msg;
            mqttContext* mqtt_connect_info = MQTT_GetClientConnectionInfo();
            
            if(con_msg && con_msg->s8Error >= 0)
            {
                puts("\nSocketConnected\n");
                change_state_mqtt(CONNECT_STATE);
                recv(soc, recv_buffer, sizeof(recv_buffer), 0);
                
                //if a socket connection is established successfully, close the error led:
                LATBbits.LATB4 = 1;
            }
            else
            {
                puts("\r\nError at socket connect\r\n");
                change_state(ERROR_STATE);
                *mqtt_connect_info->tcpClientSocket = -1; //invalidate the socket
                close(soc);
                //if issue, close the green led
                LATCbits.LATC4 = 1;
            }
            
            break;
        }
       
        case(SOCKET_MSG_RECV):
        {
            tstrSocketRecvMsg *recv_msg = (tstrSocketRecvMsg *) p_msg;
            mqttContext* mqtt_connect_info = MQTT_GetClientConnectionInfo();
            
            //puts("\nSocketReceive\n");
            if (recv_msg && recv_msg->s16BufferSize > 0)
            {   
                MQTT_GetReceivedData(recv_msg->pu8Buffer, recv_msg->s16BufferSize);         
            }
            else
            {
                //puts("\nError at socket receive\n");
                change_state(ERROR_STATE);
                *mqtt_connect_info->tcpClientSocket = -1; //invalidate the socket
                close(soc);
                //if issue, close the green led
                LATCbits.LATC4 = 1;
            }
            
            break;
        }
        
        case(SOCKET_MSG_SEND):
        {
            recv(soc, recv_buffer, sizeof(recv_buffer), 0);
            break;
        }
        
        default:
            break;
    }
    
}
