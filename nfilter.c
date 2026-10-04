#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/skbuff.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#include <linux/icmp.h>
#include <linux/in.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#include <linux/string.h>
#include <linux/kfifo.h>
#include <linux/mutex.h>

#include "filter.h"

// sudo cat /proc/nfa
// sudo sh -c 'echo "data" > /proc/nfa'

#define pr_fmt(fmt) "Netfilter: " fmt

#define BUFFER_LEN 1024

// Hook ops
static struct nf_hook_ops hook_ops;

// Proc FS
#define PROC_NAME "nfa"
static struct proc_dir_entry *proc_file_entry;

static ssize_t proc_read(struct file *, char *, size_t, loff_t *);
static ssize_t proc_write(struct file *, const char *, size_t, loff_t *);

static const struct proc_ops proc_file_fops = {
	.proc_read = proc_read,
	.proc_write = proc_write,
};

// FIFO for reading by user
#define FIFO_SIZE 1024 * 5
struct kfifo_rec_ptr_2 read_fifo;
static DEFINE_MUTEX(fifo_mutex);

static ssize_t proc_read(struct file *filp, char *buffer, size_t length, loff_t *offset)
{
	if (mutex_lock_interruptible(&fifo_mutex) != 0)
		return -ERESTARTSYS;

	unsigned int copied = 0;
	int ret = kfifo_to_user(&read_fifo, buffer, length, &copied);
	mutex_unlock(&fifo_mutex);
	if (ret != 0)
	{
		pr_err("Proc [%s] read error\n", PROC_NAME);
		return -EFAULT;
	}

	return copied;
}

static ssize_t proc_write(struct file *filp, const char *buffer, size_t length, loff_t *offset)
{
	char command[BUFFER_LEN] = {0};
	length = min(BUFFER_LEN, length);
	if (copy_from_user(command, buffer, length) > 0)
	{
		pr_err("Proc [%s] write error\n", PROC_NAME);
		return -EFAULT;
	}

	command[length - 1] = 0;
	filter_update_lists(command);
	pr_info("Proc [%s] write: %s\n", PROC_NAME, command);
	return length;
}

static void print_data(unsigned char *data, unsigned int len)
{
	char buffer[len * 2 + 1];
	for (int i = 0; i < len; i++)
		sprintf(&buffer[i * 2], "%02x", data[i]);
	buffer[len * 2] = '\0';
	pr_info("Hex data: %s\n", buffer);
}

static BOOL process_skb_data(struct sk_buff *skb)
{
	if (skb_is_nonlinear(skb))
	{
		// Do smth
	}

	struct iphdr *ip_header = NULL;
	struct tcphdr *tcp_header = NULL;
	struct udphdr *udp_header = NULL;
	unsigned char *payload_data = NULL;
	unsigned int payload_len = 0;

	ip_header = ip_hdr(skb);
	if (ip_header->protocol == IPPROTO_TCP)
	{
		tcp_header = tcp_hdr(skb);
		payload_data = (unsigned char *)tcp_header + (tcp_header->doff * 4);
		payload_len = ntohs(ip_header->tot_len) - (ip_header->ihl * 4) - (tcp_header->doff * 4);
	}
	else if (ip_header->protocol == IPPROTO_UDP)
	{
		udp_header = udp_hdr(skb);
		payload_data = (unsigned char *)(udp_header + 1);
		payload_len = ntohs(udp_header->len) - sizeof(struct udphdr);
	}
	else
	{
		return TRUE;
	}

	char* word = NULL;
	if (!filter_process_data((const char*)payload_data, &word))
	{
		if (word == NULL)
			return FALSE;

		pr_info("Block word [%s]\n", word);
		char buffer[strlen(word) + 9];
		sprintf(buffer, "Word [%s]\n", word);
		kfree(word);

		mutex_lock(&fifo_mutex);
		kfifo_in(&read_fifo, buffer, strlen(buffer));
		mutex_unlock(&fifo_mutex);
		return FALSE;
	}

	return TRUE;
}

static BOOL process_ip(struct sk_buff *skb)
{
	struct iphdr *ip_header = ip_hdr(skb);

	// printk(KERN_INFO "Netfilter Hook: Proto %u, SRC IP %pI4, DST IP %pI4\n",
	// 	ip_header->protocol, &ip_header->saddr, &ip_header->daddr);

	unsigned int src_ip = ip_header->saddr;
	unsigned int dest_ip = ip_header->daddr;
	unsigned int detected_ip = filter_process_ip(src_ip, dest_ip);
	if (detected_ip)
	{
		char str_ip[16] = {0};
		sprintf(str_ip, "%u.%u.%u.%u"
			, (detected_ip) & 0xff
			, (detected_ip >> 8) & 0xff
			, (detected_ip >> 16) & 0xff
			, (detected_ip >> 24) & 0xff);
		pr_info("Block IP [%s]", str_ip);

		char buffer[23] = {0};
		sprintf(buffer, "IP [%s]\n", str_ip);

		mutex_lock(&fifo_mutex);
		kfifo_in(&read_fifo, buffer, strlen(buffer));
		mutex_unlock(&fifo_mutex);
		return FALSE;
	}

	return TRUE;
}

unsigned int hook_func(void *priv, struct sk_buff *skb, const struct nf_hook_state *state)
{
	if (!skb)
		return NF_ACCEPT;

	if (!process_ip(skb))
		return NF_DROP;

	if (!process_skb_data(skb))
		return NF_DROP;

	return NF_ACCEPT;
}

int __init init_mod(void)
{
	// Netfilter hook ops
	hook_ops.hook = hook_func;
	hook_ops.hooknum = NF_INET_PRE_ROUTING;
	hook_ops.pf = NFPROTO_IPV4;
	hook_ops.priority = NF_IP_PRI_FIRST;

	// Register hook
	int ret = nf_register_net_hook(&init_net, &hook_ops);
	if (ret < 0)
	{
		pr_err("Register failed: %d\n", ret);
		return ret;
	}

	// Create proc file
	proc_file_entry = proc_create(PROC_NAME, 0666, NULL, &proc_file_fops);
	if (proc_file_entry == NULL)
	{
		pr_err("Proc create error\n");
		return -ENOMEM;
	}

	// Alloc fifo buffer
	ret = kfifo_alloc(&read_fifo, FIFO_SIZE, GFP_KERNEL);
	if (ret != 0)
	{
		pr_err("kfifo alloc error\n");
		return ret;
	}

	filter_init();
	pr_info("Module initialized\n");
	return 0;
}

void __exit exit_mod(void)
{
	filter_clear();
	nf_unregister_net_hook(&init_net, &hook_ops);
	remove_proc_entry(PROC_NAME, NULL);
	kfifo_free(&read_fifo);
	pr_info("Module unloaded\n");
}

module_init(init_mod);
module_exit(exit_mod);

MODULE_AUTHOR("xalex");
MODULE_DESCRIPTION("A simple netfilter kernel module");
MODULE_LICENSE("GPL");
