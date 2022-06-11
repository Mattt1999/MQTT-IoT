#include "mcc_generated_files/mcc.h"
#include "mcc_generated_files/winc/include/winc.h"
#include "mcc_generated_files/winc/include/winc_legacy.h"
#include "mqtt_core/mqtt_core.h"
#include "mcc_generated_files/mqtt/mqtt_packetTransfer_interface.h"
#include "mcc_generated_files/mqtt/mqtt_winc_adapter.h"
#include "../drivers/timeout.h"
#include "wifi_callback.h"
#include "app_init.h"
#include "cmd_interface.h"
#include "MQTT_pub_cb.h"

uint32_t publish_handler(void)
{            
    is_time_to_publish = true;
    return 3875; // task is executed after 3875 ticks
}

timerStruct_t user_input_timer = {user_input, NULL};
timerStruct_t publish_timer = {publish_handler, NULL};

void wifi_init(void)
{
    tstrWifiInitParam param;
    
    winc_adapter_init();
    
    m2m_memset((uint8*)&param, 0, sizeof(param));
    
    param.pfAppWifiCb = wifi_call_back;
    
    uint8_t ret = m2m_wifi_init(&param);
    if (M2M_SUCCESS != ret) 
    {
        puts("\r\nUnsuccessful initialization of wi-fi module!!!\r\n");
	}
    else
    {
        puts("Driver initialization successful\r\n");
    }
}

void set_MQTT_publish_handler(void)
{
    imqttHandlePublishDataFuncPtr handler = &MQTT_publish_handler_cb;
    
    MQTT_SetPublishReceptionCallback(handler);
}

void application_init(void)
{
    wifi_init();
    
    MQTT_ClientInitialise();
    MQTT_initialiseState();
    
    set_new_uart_interrupt();
    set_MQTT_publish_handler();
    
    timeout_create(&user_input_timer, 50);
    timeout_create(&publish_timer, 3875);
    
    //initialize the pins corresponding to the LEDs
    //they must be output.
    TRISBbits.TRISB4 = 0;
    TRISCbits.TRISC3 = 0;
    TRISCbits.TRISC4 = 0;
    TRISCbits.TRISC5 = 0;
    
    //the LEDs are closed, by default
    //they are active low, so write 1 in LATx register to close them
    LATBbits.LATB4 = 1;
    LATCbits.LATC3 = 1;
    LATCbits.LATC4 = 1;
    LATCbits.LATC5 = 1;
}