#include "mcc_generated_files/mcc.h"
#include "../winc/include/winc.h"
#include "mqtt_core/mqtt_core.h"
#include "build_MQTT_packets.h"

void create_MQTT_connect_packet(void) {
    //this packet must be transmitted only once
    mqttConnectPacket connect_packet;

    //initially fill with 0
    memset(&connect_packet, 0, sizeof (mqttConnectPacket));

    //initialize the fields
    connect_packet.connectVariableHeader.connectFlagsByte.All = 0x02;

    //initialize the keep alive time
    connect_packet.connectVariableHeader.keepAliveTimer = CFG_MQTT_CONN_TIMEOUT;

    //initialize the client ID, it has to be different from all the other clients IDs.
    connect_packet.clientID = "clientKODe123";

    MQTT_CreateConnectPacket(&connect_packet);
}

void create_MQTT_subscribe_packet(char *topic) {
    mqttSubscribePacket subscribe_packet;

    //fill the whole packet with 0:
    memset(&subscribe_packet, 0, sizeof (mqttSubscribePacket));

    subscribe_packet.subscribePayload->topic = topic;

    subscribe_packet.subscribePayload->topicLength = strlen(subscribe_packet.subscribePayload->topic);

    subscribe_packet.subscribePayload->requestedQoS = 0;

    subscribe_packet.packetIdentifierLSB = 1;
    subscribe_packet.packetIdentifierMSB = 0;

    MQTT_CreateSubscribePacket(&subscribe_packet);
}

void create_MQTT_publish_packet(char *topic, char *message) {
    mqttPublishPacket appPublishPacket;

    memset(&appPublishPacket, 0, sizeof (mqttPublishPacket));

    appPublishPacket.topic = topic;
    appPublishPacket.payload = message;

    // Fixed header
    appPublishPacket.publishHeaderFlags.duplicate = 0;
    appPublishPacket.publishHeaderFlags.qos = 0;
    appPublishPacket.publishHeaderFlags.retain = 0;

    // Variable header
    appPublishPacket.packetIdentifierLSB = 0;
    appPublishPacket.packetIdentifierMSB = 0;

    appPublishPacket.payloadLength = strlen(appPublishPacket.payload);

    MQTT_CreatePublishPacket(&appPublishPacket);
}
