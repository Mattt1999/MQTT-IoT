#include "../winc/include/winc.h"
#include "../winc/include/winc_legacy.h"
#include "state_control.h"


void change_state(POSSIBLE_STATE_CONNECTION new_state)
{
    if(new_state != current_state)
        current_state = new_state;
}

void change_state_mqtt(MQTT_STATE new_state)
{
    if(new_state != mqtt_current_state)
        mqtt_current_state = new_state;
}