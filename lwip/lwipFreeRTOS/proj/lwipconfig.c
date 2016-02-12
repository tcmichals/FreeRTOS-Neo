
#include "lwip/init.h"

#include "lwip/debug.h"

#include "lwip/mem.h"
#include "lwip/memp.h"
#include "lwip/sys.h"
#include "lwip/timers.h"

#include "lwip/stats.h"

#include "lwip/ip.h"
#include "lwip/ip_frag.h"
#include "lwip/udp.h"
#include "lwip/tcp.h"
#include "lwip/tcpip.h"
#define DEBUG 
#ifdef DEBUG 
#pragma message "Compiling with DEBUG" __FILE__ "..."
#define DEBUG_PRNT printf
#else
#define DEBUG_PRNT(...) 
#endif

//#define LWIP_DEBUG

/* (manual) host IP configuration */
static ip4_addr_t ipaddr, netmask, gw;



void tcpip_init_done_ok(void * arg)
{
  sys_sem_t *sem;
  sem = arg;
  sys_sem_signal(sem);
  
}


//Need to configure rpmsg... 

int lwipConfig(void)
{
  ip4_addr_t ipaddr, netmask, gw;
  sys_sem_t sem;

  printf("TCP/IP initializing...\n");  
  sys_sem_new(&sem,0);
  tcpip_init(tcpip_init_done_ok, &sem);
  sys_sem_wait(&sem);
    printf("sys_sem_free... .\n");
  sys_sem_free(&sem);
  printf("TCP/IP initialized.\n");
  
  return 0;

    
}

//eof
