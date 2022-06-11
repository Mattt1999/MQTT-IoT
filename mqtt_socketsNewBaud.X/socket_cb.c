#include "mcc_generated_files/mcc.h"
#include "../winc/include/winc.h"
#include "mqtt_core/mqtt_core.h"
#include "mcc_generated_files/winc/socket/socket.h"
#include "state_control.h"
#include "socket_cb.h"


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
                change_mqtt_state(MQTT_CONNECT);
                recv(soc, recv_buffer, sizeof(recv_buffer), 0);
                
                //if a successful connection is established
                PORTD.OUT |= PIN0_bm;
            }
            else
            {
                puts("\r\nError at socket connect\r\n");
                change_state(ERROR_STATE);
                *mqtt_connect_info->tcpClientSocket = -1; //invalidate the socket
                close(soc);
            }
            
            break;
        }
        
        case(SOCKET_MSG_RECV):
        {
            tstrSocketRecvMsg *recv_msg = (tstrSocketRecvMsg *) p_msg;
            mqttContext* mqtt_connect_info = MQTT_GetClientConnectionInfo();
            
            if (recv_msg && recv_msg->s16BufferSize > 0)
            {   
                //puts("\nSocket receive\n");
                MQTT_GetReceivedData(recv_msg->pu8Buffer, recv_msg->s16BufferSize);         
            }
            else
            {
                puts("\r\nError at socket receive\r\n");
                change_state(ERROR_STATE);
                *mqtt_connect_info->tcpClientSocket = -1; //invalidate the socket
                close(soc);
            }
            
            break;
        }
        
        case(SOCKET_MSG_SEND):
        {
            //puts("\r\nSocket transmit\r\n");
            recv(soc, recv_buffer, sizeof(recv_buffer), 0);
            
            break;
        }
        
        default:
            break;
    }
}
