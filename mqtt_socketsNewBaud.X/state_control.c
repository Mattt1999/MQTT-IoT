#include "state_control.h"

void change_state(possible_state new_state)
{
    if(new_state != current_state)
        current_state = new_state;
}

void change_mqtt_state(mqtt_state new_state)
{
    if(new_state != current_mqtt_state)
        current_mqtt_state = new_state;
}