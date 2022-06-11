#include "mcc_generated_files/mcc.h"
#include "../winc/include/winc.h"
#include "../winc/include/winc_legacy.h"
#include "socket_callback.h"
#include "state_control.h"
#include "ping_callback.h"

void ping_cb(uint32 u32IPAddr, uint32 u32RTT, uint8 u8ErrorCode) 
{
    puts("Ping called\n");
    if (!u8ErrorCode) 
    {
        printf("REPLY RECEIVED WITH RTT= %lums\r\n",u32RTT);
        
        //if the ping works, then the red led needs to be closed:
        LATBbits.LATB4 = 1;
        
        change_state(SOCKET_INIT_STATE);
    } 
    else if (u8ErrorCode == 1) 
    {
        printf("DESTINATION UNREACH\r\n");
        change_state(ERROR_STATE);
    } 
    else if (u8ErrorCode == 2) 
    {
        printf("TIMEOUT ERROR\r\n");
        change_state(ERROR_STATE);
    }
}

//this is to be called in case the new ping is required from the "cmd"
//also no state transition here
void ping_callback(uint32 u32IPAddr, uint32 u32RTT, uint8 u8ErrorCode)
{
    puts("\nPing called\n");
    if (!u8ErrorCode) 
    {
        //printf("ping_cb: PING_SUCCESS\r\n");
        printf("ping_cb: Reply with RTT= %lums\r\n", u32RTT);
    } 
    else if (u8ErrorCode == 1) 
    {
        printf("ping_cb: PING_ERR_DEST_UNREACH\r\n");
    } 
    else if (u8ErrorCode == 2) 
    {
        printf("ping_cb: PING_ERR_TIMEOUT\r\n");
    }
}
