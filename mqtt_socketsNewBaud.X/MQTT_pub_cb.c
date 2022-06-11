#include "mcc_generated_files/mcc.h"
#include "mcc_generated_files/mqtt/mqtt_packetTransfer_interface.h"

#include "MQTT_pub_cb.h"

void MQTT_publish_handler_cb(uint8_t *topic, uint8_t *payload)
{
    puts("\r\nMqtt publish callback\r\n");
}
