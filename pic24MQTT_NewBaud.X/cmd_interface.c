#include "mcc_generated_files/mcc.h"
#include "../winc/include/winc.h"
#include "../winc/include/winc_legacy.h"
#include "mcc_generated_files/drivers/uart.h"
#include "mcc_generated_files/mqtt/mqtt_core/mqtt_core.h"
#include "mcc_generated_files/mqtt/mqtt_packetTransfer_interface.h"
#include "build_MQTT_packets.h"
#include "cmd_interface.h"
#include "ping_callback.h"
#include "state_control.h"
#include "socket_callback.h"

#define MAIN_WLAN_AUTH M2M_WIFI_SEC_WPA_PSK // the authentification security type

uint8_t poz = 0;
bool is_command_finished = false;
char buffer[BUFFER_SIZE]; // global variable that holds the user input.
//char dest_ip[];

////a vector containing the names of all available commands
char *builtins_functions[] = {
    "get_mac",
    "connect_to",
    "broker_disconnect",
    "wifi_disconnect",
    "subscribe_to",
    "publish_to",
    "new_socket",
    "ping"
};

//returns 1 in case of success, 0 otherwise
//TO DO: Move the declaration from the line 45 into the file where all the commands will be declared
void get_mac(char *args[])
{
    uint8_t mac_addr[6];
    //print the mac address for connection with the module testing
    if(M2M_SUCCESS != m2m_wifi_get_mac_address(mac_addr))
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
        }
        else
        {
            puts("\r\nConnected with success\r\n");
        }
    }
    else
    {
        puts("\r\nWRONG arguments\r\n");
    }
}


void subscribe_to(char *args[])
{
    //add error checking --> we want the command to be used properly!
    create_MQTT_subscribe_packet(*(args+1));
    
    mqttContext* mqtt_connect_info = MQTT_GetClientConnectionInfo();
    
    MQTT_TransmissionHandler(mqtt_connect_info);
    MQTT_ReceptionHandler(mqtt_connect_info);
                
    change_state_mqtt(PROCESS_LAST_PACKET);
    puts("\r\nSubscribe\r\n");
}

void publish_to(char *args[])
{
//    // add error checking
    create_MQTT_publish_packet(*(args + 1), *(args + 2));
    
    mqttContext* mqtt_connect_info = MQTT_GetClientConnectionInfo();
    
    MQTT_TransmissionHandler(mqtt_connect_info);
    MQTT_ReceptionHandler(mqtt_connect_info);
    puts("\r\npublish\r\n");
}

void ping(char *args[])
{
    printf("\r\nPING %s\r\n", *(args + 1));
    
    uint32 dest_ip = inet_addr(*(args + 1));
                
    uint8 u8TTL = 250;

    printf("Pinging [%s] with TTL=%d\r\n", *(args + 1),u8TTL);
    
    //call the ping with other callback function
    m2m_ping_req(dest_ip, u8TTL, ping_callback);
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

void (*function[])(char *args[]) = {
    &get_mac,
    &connect_to,
    &broker_disconnect,
    &wifi_disconnect,
    &subscribe_to,
    &publish_to,
    &new_socket,
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
    //puts("\r\nIN\r\n");
    char data;
    data = U1RXREG; // extract the data from the receive register
    
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
        uint8_t i;
//        printf("%d\r\n", poz);
//        for(i = 0; buffer[i] != '\0'; i++)
//            printf("%c", buffer[i]);
//        puts("\r\n");
      
        //Here we will parse the buffer to extract the command and it's parameters
        
        char *token;
        char *command[10]; 
        //char **command; --> WRONG!!! because it is only a pointer, in order to make it work, we have to allocate memory for it, to make it a vector of pointers!!!
        uint8_t position = 0;
        bool cmd_found = false; 
        int no_of_coms;
        token = strtok(buffer, DELIMS);
        while(token != NULL)
        {
            //printf("\n%s\n", token); //--> for testing
            command[position] = token;
            //printf("\n%s\n", command[position]); //--> for testing
            position = position + 1;
            token = strtok(NULL, DELIMS);
        }
        //Then the function that is to execute the command will be called.
        no_of_coms = number_of_commands();//for efficiency, make it global
        for(i = 0; i<no_of_coms; i++)
        {
            if(strcmp(builtins_functions[i], *(command)) == 0)
            {
                puts("\r\nCallFunction\r\n");
                cmd_found = true;
                function[i](command);
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
    UART1_SetRxInterruptHandler(&USART2_NewRxIsrCb);
}
