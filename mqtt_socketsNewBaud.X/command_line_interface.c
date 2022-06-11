#include "mcc_generated_files/mcc.h"
#include "../winc/include/winc.h"
#include "../winc/include/winc_legacy.h"
#include "mcc_generated_files/winc/socket/socket.h"
#include "mqtt_core/mqtt_core.h"
#include "ping_cb.h"
#include "command_line_interface.h"
#include "state_control.h"
#include "MQTT_create_packet.h"
#include "socket_cb.h"
#include "sensors_handle.h"

#define WIFI_SERVER_PORT 1883
#define MAIN_WLAN_AUTH M2M_WIFI_SEC_WPA_PSK // the authentification security type

uint8_t poz = 0;
volatile bool is_command_finished = false;
char buffer[BUFFER_SIZE]; // global variable that holds the user input.

////a vector containing the names of all available commands
char *builtins_functions[] = {
    "get_mac",
    "connect_to",
    "broker_disconnect",
    "wifi_disconnect",
    "new_socket",
    "subscribe_to",
    "set_resolution",
    "publish_to",
    "ping"
};

//returns 1 in case of success, 0 otherwise
//TO DO: Move the declaration from the line 45 into the file where all the commands will be declared
void get_mac(char *args[])
{
    uint8_t mac_addr[6];
    uint8_t *is_valid;
    //print the mac address for connection with the module testing
    if(M2M_SUCCESS != m2m_wifi_get_otp_mac_address(mac_addr, is_valid))
    {
        puts("MAC retrieval failed!\r\n");
    }
    else
    {
        printf("%02X:%02X:%02X:%02X:%02X:%02X\r\n", mac_addr[0], 
                mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
    }
}

void connect_to(char *args[])
{
    uint8_t connection_result;
    //used when the default connection is not working
    if(strcmp("-n", *(args + 1)) == 0)
    {
        puts("\r\nNewCon\r\n");
        //How to call it: connect_to -n "NameOfTheNetwork" , "Password"
        connection_result = m2m_wifi_connect(*(args+2), strlen(*(args+2)), MAIN_WLAN_AUTH, (void *)(*(args+3)), M2M_WIFI_CH_ALL);
        if(connection_result != 0)
        {
            puts("\r\nErrorConn\r\n");
            //change_state(ERROR_STATE); //could not connect, try again
        }
        else
        {
            //if connection successful, then wait for it. 
            change_state(WIFI_CONNECTING_STATE);
        }
    }
    else
    {
        puts("\r\nWRONG arguments\r\n");
    }
}

//void scan(char *args[])
//{
//    puts("\r\nScanning...\r\n");
//    m2m_wifi_request_scan(M2M_WIFI_CH_ALL);
//}

//void get_connection_info(char *args[])
//{
//    if(m2m_wifi_get_connection_info() != M2M_SUCCESS)
//    {
//        puts("\r\nError getting connection info\r\n");
//    }
//}

void subscribe_to(char *args[])
{
    //add error checking --> we want the command to be used properly!
    create_MQTT_subscribe_packet(*(args+1));
    
    mqttContext* mqtt_connect_info = MQTT_GetClientConnectionInfo();
    
    MQTT_TransmissionHandler(mqtt_connect_info);
    MQTT_ReceptionHandler(mqtt_connect_info);
                
    change_state(PROCESS_LAST_PACKET);
}

void publish_to(char *args[])
{
    // add error checking
    create_MQTT_publish_packet(*(args + 1), *(args + 2));
    
    mqttContext* mqtt_connect_info = MQTT_GetClientConnectionInfo();
    
    MQTT_TransmissionHandler(mqtt_connect_info);
    MQTT_ReceptionHandler(mqtt_connect_info);
}

void ping(char *args[])
{
    printf("\r\nPING %s\r\n", *(args + 1));
    
    uint32 dest_ip = inet_addr(*(args + 1));
                
    uint8 u8TTL = 250;

    printf("Pinging [%s] with TTL=%d\r\n", *(args + 1),u8TTL);
                                
    m2m_ping_req(dest_ip, u8TTL, ping_callback);
}

void broker_disconnect(char *args[])
{
    puts("\nDisconnect from broker\n");
    mqttContext* mqtt_connect_info = MQTT_GetClientConnectionInfo();
    
    MQTT_Disconnect(mqtt_connect_info);
    
    close(*mqtt_connect_info->tcpClientSocket);
}

void wifi_disconnect(char *args[])
{
    puts("\nDisconnect from network\n");
    m2m_wifi_disconnect();
}

void new_socket(char *args[])
{
    printf("\nNew socket ip: %s\n", *(args + 1));
    
    uint32 ip_addr = inet_addr(*(args + 1));
    
    struct sockaddr_in addr;
    
    //initialization of the new socket
    socketInit();
    registerSocketCallback(socket_call_back, NULL);

    //create new socket
    mqttContext* mqtt_connect_info = MQTT_GetClientConnectionInfo();

    *mqtt_connect_info->tcpClientSocket = socket(AF_INET, SOCK_STREAM, 0);

    //this means that a new socket couldn't be created
    if (*mqtt_connect_info->tcpClientSocket < 0) 
    {
        puts("\nError creating socket\n");
    }
    //initialize the socket details and connect
    addr.sin_family = AF_INET;
    addr.sin_port = _htons(WIFI_SERVER_PORT);
    addr.sin_addr.s_addr = ip_addr;
    
    if(connect(*mqtt_connect_info->tcpClientSocket, (struct sockaddr *)&addr, sizeof(struct sockaddr_in)) < 0)
    {
        close(*mqtt_connect_info->tcpClientSocket);
        *mqtt_connect_info->tcpClientSocket = -1;
        puts("\nError at socket connection\n");
    }
}

void set_resolution(char *args[])
{
    bool cmd_valid = false;
    
    if(strcmp(*(args+1), "0") == 0)
    {
        i2c_write1ByteRegister(TEMP_SENSOR_ADDR, TEMP_SENSOR_RES_ADDR, 0x00);
        cmd_valid = true;
    }
    
    if(strcmp(*(args+1), "1") == 0)
    {
        i2c_write1ByteRegister(TEMP_SENSOR_ADDR, TEMP_SENSOR_RES_ADDR, 0x01);
        cmd_valid = true;
    }
    
    if(strcmp(*(args+1), "2") == 0)
    {
        i2c_write1ByteRegister(TEMP_SENSOR_ADDR, TEMP_SENSOR_RES_ADDR, 0x02);
        cmd_valid = true;
    }
    
    if(strcmp(*(args+1), "3") == 0)
    {
        i2c_write1ByteRegister(TEMP_SENSOR_ADDR, TEMP_SENSOR_RES_ADDR, 0x03);
        cmd_valid = true;
    }
    
    if(!cmd_valid)
    {
        puts("Wrong arguments\n");
    }
}

void (*function[])(char *args[]) = {
    &get_mac,
    &connect_to,
    &broker_disconnect,
    &wifi_disconnect,
    &new_socket,
    &subscribe_to,
    &set_resolution,
    &publish_to,
    &ping
};

//returns the dimension of the builtins_functions vector
int number_of_commands(void)
{
    return sizeof(builtins_functions)/sizeof(char *);
}

//the new interrupt function
//this is where the buffer will be populated.
void USART2_NewRxIsrCb(void)
{
    char data;
    data = USART2.RXDATAL;
    
    if(data != '\n' && data != '\r') // there are still characters to come
    {
        buffer[poz] = data;
        is_command_finished = false;
        poz = poz + 1;
    }
    else // the command has finished
    {
        buffer[poz] = '\0';
        is_command_finished = true;
    }
}

uint32_t user_input(void)
{
    
    if(is_command_finished)
    {
          //Print the content of the buffer for testing purposes
//        printf("%d\r\n", poz);
//        for(int i = 0; buffer[i] != '\0'; i++)
//            printf("%c", buffer[i]);
//        puts("\r\n");
      
        //Here we will parse the buffer to extract the command and it's parameters
        
        char *token;
        char *command[10]; 
        //char **command; --> WRONG!!! because it is only a pointer, in order to make it work, we have to allocate memory for it, to make it a vector of pointers!!!
        uint8_t position = 0, i;
        bool cmd_found = false; 
        int no_of_coms;
        token = strtok(buffer, DELIMS);
        while(token != NULL)
        {
            //printf("\r\n%s\r\n", token); --> for testing
            command[position] = token;
            //printf("\r\n%s\r\n", command[position]); --> for testing
            position = position + 1;
            token = strtok(NULL, DELIMS);
        }
        //Then the function that is to execute the command will be called.
        no_of_coms = number_of_commands();//for efficiency, make it global
        for(i = 0; i<no_of_coms; i++)
        {
            if(strcmp(builtins_functions[i], *(command)) == 0)
            {
                cmd_found = true;
                function[i](command);
                puts("\r\nCallFunction\r\n");
            }
        }
        if(!cmd_found)
        {
            puts("\r\nNoSuchFunctionFound\r\n");
        }
        
        //clear the buffer to accept a new command
        memset(buffer, 0, sizeof(buffer));
        //start writing the new commands at the begining of the buffer
        poz = 0;
        //clear the command flag to accept new commands
        is_command_finished = false;
    }
    return 50;
}

void set_new_uart_interrupt(void)
{
    //set the interrupt function to be USART2_NewRxIsrCb function.
    usart_callback u = &USART2_NewRxIsrCb;
    USART2_SetRXISRCb(u);
}

