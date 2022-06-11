#include "mcc_generated_files/system.h"
#include "../winc/include/winc.h"
#include "../winc/include/winc_legacy.h"
#include "state_control.h"


void wifi_call_back(uint8_t msg_type, void *p_msg)
{   
    switch(msg_type)
    {
        case M2M_WIFI_RESP_CON_STATE_CHANGED:
        {
            tstrM2mWifiStateChanged *pstrWifiState = (tstrM2mWifiStateChanged *)p_msg;
            if(pstrWifiState->u8CurrState == M2M_WIFI_CONNECTED) 
            {
                puts("\r\nConnection successful\r\n");
                change_state(WIFI_CONNECTED_STATE);
            }
            if(pstrWifiState->u8CurrState != M2M_WIFI_CONNECTED && pstrWifiState->u8CurrState != M2M_WIFI_DISCONNECTED)
            {
                 puts("\r\nCouldn't connect to AP\r\n");
                 change_state(ERROR_STATE);
            }
            if(pstrWifiState->u8CurrState == M2M_WIFI_DISCONNECTED)
            {
                LATCbits.LATC5 = 1;//close the wifi led
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
