
#include <stdlib.h>


#include "lwip/debug.h"

#include "lwip/opt.h"
#include "lwip/def.h"
#include "lwip/mem.h"
#include "lwip/pbuf.h"
#include "lwip/stats.h"
#include "lwip/sys.h"

#include "netif/etharp.h"
#include "lwip/mld6.h"
#include <stdbool.h>

#include "rpmsg/rpmsg_rtos.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "board.h"
#include "mu_imx.h"
#include "debug_console_imx.h"



/* Define those to better describe your network interface. */
#define IFNAME0 'm'
#define IFNAME1 'c'

//The MAX RPMSG from Linux is 512-16 = 496  for now try 496
#define MTU_RPMSG 496

#define MAC0_ADDR 0x2
#define MAC1_ADDR 0x2
#define MAC2_ADDR 0x0
#define MAC3_ADDR 0x0
#define MAC4_ADDR 0x0
#define MAC5_ADDR 0x0

#define MIN(X,Y) ((X) < (Y) ? (X) : (Y))

#define MCCETH_DEBUG           LWIP_DBG_ON

//From Linux...


#define ETHERNET_ENDPT 125

static sys_thread_t _rpmsgReadThread = NULL;
static uint8_t pmsgMsg[2048];
struct rpmsg_channel *_app_chnl;
struct rpmsg_endpoint *_eptEthernet;


extern bool getRPMSGChannel( struct rpmsg_channel **app_chnl);
/**
 * Helper struct to hold private data used to operate your ethernet interface.
 * Keeping the ethernet address of the MAC in this struct is not necessary
 * as it is already kept in the struct netif.
 * But this is only an example, anyway...
 */
struct ethernetif
{
    struct eth_addr *ethaddr;
    /* Add whatever per-interface state that is needed here. */
};

/* Forward declarations. */

static struct pbuf *
low_level_input(struct netif *netif, uint8_t *pBuff, int len);


/**
 * In this function, the hardware should be initialized.
 * Called from ethernetif_init().
 *
 * @param netif the already initialized lwip network interface structure
 *        for this ethernetif
 */

void dumpPkt(char *pInfo, uint8_t *pPkt, int lenPkt)
{
    PRINTF("%s ", pInfo);
    
    for(int _it=0; _it < lenPkt; _it++)
    {
        int _val = pPkt[_it];
        PRINTF("%X ", _val);
    }
    PRINTF("\n\r");
    
}



static void rpmsgRead( void *pParams)
{
    struct netif * rpmsgNetIf = ( struct netif *)pParams;
    int rcMsg;
    int bytesRx;

   PRINTF("INFO:%s %d +\n",__FUNCTION__ , __LINE__);

    while (true)
    {
        bytesRx = 0;

        rcMsg = rpmsg_rtos_recv(_eptEthernet, &pmsgMsg, &bytesRx, sizeof(pmsgMsg), NULL, 0xFFFFFFFF);
        
        dumpPkt("rpmsg", pmsgMsg, bytesRx);
  
        
        if(rcMsg == 0)
        {
            struct pbuf * pPkt = low_level_input(rpmsgNetIf,pmsgMsg, bytesRx);
            struct eth_hdr *ethhdr;

            if (pPkt == NULL)
                continue;
           
            ethhdr = pPkt->payload;
   PRINTF("INFO:%s %d  %X+\n",__FUNCTION__ , __LINE__, htons(ethhdr->type));

            switch (htons(ethhdr->type))
            {
            /* IP or ARP packet? */
            case ETHTYPE_IP:
            case ETHTYPE_IPV6:
            case ETHTYPE_ARP:
#if PPPOE_SUPPORT
            /* PPPoE packet? */
            case ETHTYPE_PPPOEDISC:
            case ETHTYPE_PPPOE:
#endif /* PPPOE_SUPPORT */
                
             PRINTF("INFO:%s %d %x\n\r", __FUNCTION__ , __LINE__,htons(ethhdr->type));

                /* full packet send to tcpip_thread to process */
                if (rpmsgNetIf->input(pPkt, rpmsgNetIf)!=ERR_OK)
                {
                    LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error \n\r"));
                    pbuf_free(pPkt);
                    pPkt = NULL;
                }
                else
                {
                        LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP OK \n\r"));

                }
                break;

            default:
            LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_input: IP input error \n\r"));

                pbuf_free(pPkt);
                pPkt = NULL;
                break;
            }
        }

    }
}


static void
low_level_init(struct netif *netif)
{
    //TCM struct ethernetif *ethernetif = netif->state;

    /* set MAC hardware address length */
    netif->hwaddr_len = ETHARP_HWADDR_LEN;

    /* set MAC hardware address */
    netif->hwaddr[0] = MAC0_ADDR;
    netif->hwaddr[1] = MAC1_ADDR;
    netif->hwaddr[2] = MAC2_ADDR;
    netif->hwaddr[3] = MAC3_ADDR;
    netif->hwaddr[4] = MAC4_ADDR;
    netif->hwaddr[5] = MAC5_ADDR;

    /* maximum transfer unit */
    netif->mtu = MTU_RPMSG;

    /* device capabilities */
    /* don't set NETIF_FLAG_ETHARP if this device is not an ethernet one */
    netif->flags = NETIF_FLAG_BROADCAST | NETIF_FLAG_ETHARP | NETIF_FLAG_LINK_UP;

#if LWIP_IPV6 && LWIP_IPV6_MLD
    /*
     * For hardware/netifs that implement MAC filtering.
     * All-nodes link-local is handled by default, so we must let the hardware know
     * to allow multicast packets in.
     * Should set mld_mac_filter previously. */
    if (netif->mld_mac_filter != NULL)
    {
        ip6_addr_t ip6_allnodes_ll;
        ip6_addr_set_allnodes_linklocal(&ip6_allnodes_ll);
        netif->mld_mac_filter(netif, &ip6_allnodes_ll, MLD6_ADD_MAC_FILTER);
    }
#endif /* LWIP_IPV6 && LWIP_IPV6_MLD */

   LWIP_DEBUGF(MCCETH_DEBUG,("INFO:%s %d +\n\r", __FUNCTION__ , __LINE__));
   
   if (getRPMSGChannel(&_app_chnl))
   {
       _eptEthernet = rpmsg_rtos_create_ept(_app_chnl, ETHERNET_ENDPT);
       
       if(_eptEthernet == NULL)
       {
            PRINTF("OK endp failed created\n\r"); 
       }
       else
       {
           PRINTF("OK endp created\n\r");
       }
        if (_eptEthernet)
               _rpmsgReadThread = sys_thread_new("rpmsgThread",
                                   rpmsgRead,
                                   (void *)netif,
                                   TCPIP_THREAD_STACKSIZE,
                                   TCPIP_THREAD_PRIO); 
     
     
   }

    /* Do whatever else is needed to initialize interface. */
}

/**
 * This function should do the actual transmission of the packet. The packet is
 * contained in the pbuf that is passed to the function. This pbuf
 * might be chained.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @param p the MAC packet to send (e.g. IP packet including MAC addresses and type)
 * @return ERR_OK if the packet could be sent
 *         an err_t value if the packet couldn't be sent
 *
 * @note Returning ERR_MEM here if a DMA queue of your MAC is full can lead to
 *       strange results. You might consider waiting for space in the DMA queue
 *       to become available since the stack doesn't retry to send a packet
 *       dropped because of memory failure (except for the TCP timers).
 */

static err_t
low_level_output(struct netif *netif, struct pbuf *p)
{
    //TCM struct ethernetif *ethernetif = netif->state;
    struct pbuf *q;

    //TCM initiate transfer();

#if ETH_PAD_SIZE
    pbuf_header(p, -ETH_PAD_SIZE); /* drop the padding word */
#endif

    for (q = p; q != NULL; q = q->next)
    {

        int rcMsg = 0;
        /* Send the data from the pbuf to the interface, one pbuf at a
           time. The size of the data in each pbuf is kept in the ->len
           variable. */
        PRINTF("INFO:%s %d \n\r",   __FUNCTION__ , __LINE__);
            rcMsg = rpmsg_rtos_send(_eptEthernet, q->payload, q->len, ETHERNET_ENDPT);
        PRINTF("INFO:%s %d rcMsg=%d\n\r",   __FUNCTION__ , __LINE__, rcMsg);

        if (rcMsg!=0)
            return ERR_MEM;
    }

#if ETH_PAD_SIZE
    pbuf_header(p, ETH_PAD_SIZE); /* reclaim the padding word */
#endif

    LINK_STATS_INC(link.xmit);

    return ERR_OK;
}

/**
 * Should allocate a pbuf and transfer the bytes of the incoming
 * packet from the interface into the pbuf.
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return a pbuf filled with the received packet (including MAC header)
 *         NULL on memory error
 */
static struct pbuf *
low_level_input(struct netif *netif, uint8_t *pBuff, int pktLen)
{
    //TCM struct ethernetif *ethernetif = netif->state;
    struct pbuf *p=NULL, *q=NULL;
    u16_t len;

    /* Obtain the size of the packet and put it into the "len"
       variable. */
    len = pktLen;
   

#if ETH_PAD_SIZE
    len += ETH_PAD_SIZE; /* allow room for Ethernet padding */
#endif

    /* We allocate a pbuf chain of pbufs from the pool. */
    p = pbuf_alloc(PBUF_RAW, len, PBUF_POOL);
    
    if (p != NULL)
    {

#if ETH_PAD_SIZE
        kkkk
        pbuf_header(p, -ETH_PAD_SIZE); /* drop the padding word */
#endif
        uint32_t offset = 0;

        /* We iterate over the pbuf chain until we have read the entire
         * packet into the pbuf. */
        for (q = p; q != NULL; q = q->next)
        {
            /* Read enough bytes to fill this pbuf in the chain. The
             * available data in the pbuf is given by the q->len
             * variable.
             * This does not necessarily have to be a memcpy, you can also preallocate
             * pbufs for a DMA-enabled MAC and after receiving truncate it to the
             * actually received size. In this case, ensure the tot_len member of the
             * pbuf is the sum of the chained pbuf len members.
             */

            memcpy(q->payload, pBuff+ offset, MIN((q->len), (len - offset)) );
            offset += q->len;
            if( offset == len)
                break;
        }

#if ETH_PAD_SIZE
        pbuf_header(p, ETH_PAD_SIZE); /* reclaim the padding word */
#endif
        LINK_STATS_INC(link.recv);
    }
    else
    {
//TCM   drop packet();
        LINK_STATS_INC(link.memerr);
        LINK_STATS_INC(link.drop);
    }

    return p;
}


/**
 * Should be called at the beginning of the program to set up the
 * network interface. It calls the function low_level_init() to do the
 * actual setup of the hardware.
 *
 * This function should be passed as a parameter to netif_add().
 *
 * @param netif the lwip network interface structure for this ethernetif
 * @return ERR_OK if the loopif is initialized
 *         ERR_MEM if private data couldn't be allocated
 *         any other err_t on error
 */
err_t
rpmsg_init_drv(struct netif *netif)
{
    struct ethernetif *ethernetif;

    LWIP_ASSERT("netif != NULL", (netif != NULL));
    LWIP_DEBUGF(MCCETH_DEBUG,("INFO: %s %s %d \n\r",  __FILE__, __FUNCTION__ , __LINE__));
    
    ethernetif = mem_malloc(sizeof(struct ethernetif));
    if (ethernetif == NULL)
    {
        LWIP_DEBUGF(NETIF_DEBUG, ("ethernetif_init: out of memory\n"));
        return ERR_MEM;
    }

#if LWIP_NETIF_HOSTNAME
    /* Initialize interface hostname */
    netif->hostname = "lwip";
#endif /* LWIP_NETIF_HOSTNAME */

    /*
     * Initialize the snmp variables and counters inside the struct netif.
     * The last argument should be replaced with your link speed, in units
     * of bits per second.
     */
//tcm  MIB2_INIT_NETIF(netif, snmp_ifType_ethernet_csmacd,100000);

    netif->state = ethernetif;
    netif->name[0] = IFNAME0;
    netif->name[1] = IFNAME1;
    /* We directly use etharp_output() here to save a function call.
     * You can instead declare your own function an call etharp_output()
     * from it if you have to do some checks before sending (e.g. if link
     * is available...) */
    netif->output = etharp_output;
#if LWIP_IPV6
//TCM  netif->output_ip6 = ethip6_output;
#endif /* LWIP_IPV6 */
    netif->linkoutput = low_level_output;

    ethernetif->ethaddr = (struct eth_addr *)&(netif->hwaddr[0]);

    /* initialize the hardware */
    low_level_init(netif);

    return ERR_OK;
}

