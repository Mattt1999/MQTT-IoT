#include "mcc_generated_files/mcc.h"
#include "../winc/include/winc.h"
#include "../winc/include/winc_legacy.h"
#include "../drivers/timeout.h"
#include "mqtt_core/mqtt_core.h"
#include "mcc_generated_files/mqtt/mqtt_packetTransfer_interface.h"
#include "mcc_generated_files/mqtt/mqtt_winc_adapter.h"
#include "mcc_generated_files/drivers/i2c_simple_master.h"
#include "application_init.h"
#include "command_line_interface.h"
#include "MQTT_pub_cb.h"
#include "wifi_callback.h"

//check timer
uint32_t check_timeout()
{
    is_time_to_publish = true;
    return 330;
}

timerStruct_t publish_timer  = {check_timeout,NULL};
timerStruct_t user_input_timer = {user_input, NULL};

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

void set_MQTT_publish_Handler(void)
{
    imqttHandlePublishDataFuncPtr handler = &MQTT_publish_handler_cb;
    
    MQTT_SetPublishReceptionCallback(handler);
}


void application_init(void)
{
    wifi_init();
    
    MQTT_ClientInitialise();
    MQTT_initialiseState();
    timeout_create(&user_input_timer, 50);
    timeout_create(&publish_timer,330);
    set_new_uart_interrupt();
    //set_MQTT_publish_Handler();
    
    //make pins output
    PORTD.DIR |= PIN0_bm;
    PORTD.DIR |= PIN1_bm;
    PORTD.DIR |= PIN2_bm;
    PORTD.DIR |= PIN3_bm;
    
    //LEDS are active 0, so close them:
    PORTD.OUT |= PIN0_bm;
    PORTD.OUT |= PIN1_bm;
    PORTD.OUT |= PIN2_bm;
    PORTD.OUT |= PIN3_bm;
}