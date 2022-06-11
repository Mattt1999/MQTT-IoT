#include "mcc_generated_files/mcc.h"
#include "../winc/include/winc.h"
#include "../winc/include/winc_legacy.h"
#include "state_control.h"


void ping_cb(uint32 u32IPAddr, uint32 u32RTT, uint8 u8ErrorCode)
{
    puts("\nPing called\n");
    if (!u8ErrorCode)
    {
        printf("REPLY RECEIVED WITH RTT= %lums\r\n",u32RTT);
        
        //if ping successful, close the error led
        PORTD.OUT |= PIN0_bm;
        
        change_state(SOCKET_INIT_STATE);
    }
    else if (u8ErrorCode==1)
    {
        printf("DESTINATION UNREACH\r\n");
        change_state(ERROR_STATE);
    }
    else if (u8ErrorCode==2)
    {
        printf("TIMEOUT ERROR\r\n");
        change_state(ERROR_STATE);
    }
}

//to check if it works without this
void ping_callback(uint32 u32IPAddr, uint32 u32RTT, uint8 u8ErrorCode)
{
    puts("\nPing called\n");
    if (!u8ErrorCode) 
    {
        printf("REPLY RECEIVED WITH RTT= %lums\r\n",u32RTT);
    } 
    else if (u8ErrorCode == 1) 
    {
        printf("DESTINATION UNREACH\r\n");
    } 
    else if (u8ErrorCode == 2) 
    {
        printf("TIMEOUT ERROR\r\n");
    }
}