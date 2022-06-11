#include "mcc_generated_files/mcc.h"
#include "mcc_generated_files/mqtt/mqtt_packetTransfer_interface.h"
#include "state_control.h"
#include "MQTT_pub_cb.h"

bool automate_motor = true;//default the motor is controlled automatically

int32_t map(int16_t value, int32_t input_min, int32_t input_max, int32_t output_min, int32_t output_max)
{
    int32_t r = output_min + ((value - input_min)*(output_max-output_min))/(input_max-input_min);
    return r;
}

void MQTT_publish_handler_cb(uint8_t *topic, uint8_t *payload)
{
    //transform the payload string into a number, up to the "." character.
    if(strcmp(topic, "light") == 0 && automate_motor == true)
    {
        int16_t light_val = atoi(payload);
        int32_t result = map(light_val, 0, 660, 250, 1499);
        printf("%ld\n\r", result);
        uint16_t d = (uint16_t)result;
        
        OC1_PrimaryValueSet(d);
    }
    //if the topic is commands, perform a command related action
    if(strcmp(topic, "commands") == 0)
    {
        //48 is the ASCII code for the digit 0
        if(*payload == 48)
        {
            puts("\nToo cold");
        }

        if(*payload == 49)
        {
            puts("\nToo hot");
        }
        
        if(*payload == 50)
        {
            puts("\nToo much light");
        }

        if(*payload == 51)
        {
            puts("\nToo little light");
        }
        //user wants to control the motor by himself
        if(*payload == 52)
        {
            puts("Motor not Automated\n");
            automate_motor = false;
        }
        //user want to control let the motor be controlled automatically
        if(*payload == 53)
        {
            puts("Motor Automated\n");
            automate_motor = true;
        }
    }
    
    if(strcmp(topic, "motorVal")==0)
    {
        if(automate_motor == false)
        {
            int16_t motor_val = atoi(payload);
            OC1_PrimaryValueSet(motor_val);
        }

    }
}
