#include "../winc/include/winc.h"
#include "../winc/include/winc_legacy.h"
#include "state_control.h"


void wifi_call_back(uint8_t msg_type, void *p_msg)
{   
    tstrM2mWifiStateChanged *pstrWifiState = (tstrM2mWifiStateChanged *)p_msg;
    switch(msg_type)
    {
        case M2M_WIFI_RESP_CON_STATE_CHANGED:
        {
            if (pstrWifiState->u8CurrState == M2M_WIFI_CONNECTED) 
            {
                puts("\r\nConnection successful\r\n");
                change_state(WIFI_CONNECTED_STATE);
            }
            if(pstrWifiState->u8CurrState == M2M_WIFI_DISCONNECTED)
            {
                puts("\r\nDisconnected with success\r\n");
                change_state(WIFI_WAIT_FOR_CREDENTIALS_STATE); //after disconnecting, wait for new credentials
            }        
            break;
        }
        

        default:
        {
            break;
        } 
    }
}
