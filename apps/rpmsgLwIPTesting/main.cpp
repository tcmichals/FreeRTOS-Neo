
#ifdef __cplusplus
extern "C" {
#endif
#include "FreeRTOS.h"
#include "task.h"
#include "board.h"
#include "mu_imx.h"
#include "debug_console_imx.h"
#include "rpmsg/rpmsg_rtos.h"

#include "rpmsgEthernet.h"  

extern void hardware_init();

#ifdef __cplusplus
}
#endif

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
  
  #include "lwip/opt.h"
#include "lwip/sys.h"
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
#include "netif/etharp.h"
#include "lwip/sockets.h"

#include <string.h>

#define USR_ENDPT 127
#define TTY_ENDPT 126
#define ETHERNET_ENDPT 125
////////////////////////////////////////////////////////////////////////////////
// Definitions
////////////////////////////////////////////////////////////////////////////////
#define APP_TASK_STACK_SIZE 1024

/*
 * APP decided interrupt priority
 */
#define APP_MU_IRQ_PRIORITY 3
#define TICKS_TO_MS(A)  ((portMAX_DELAY == (A)) ? portMAX_DELAY : (A) / portTICK_PERIOD_MS)

////////////////////////////////////////////////////////////////////////////////
// Code
////////////////////////////////////////////////////////////////////////////////

struct remote_device *rdev = NULL;
struct rpmsg_channel *app_chnl = NULL;


//IP Stack 
/* (manual) host IP configuration */
static ip4_addr_t ipaddr, netmask, gw;
 struct netif netif;

void tcpip_init_done_ok(void * arg)
{
  sys_sem_t *sem;
  sem = (sys_sem_t *)arg;
  sys_sem_signal(sem);
  
}

//Need to configure rpmsg... 

int lwipConfig(void)
{
  sys_sem_t sem;

  vTaskSuspendAll();
    PRINTF("TCP/IP initializing...\n\r");
  xTaskResumeAll();
 

  sys_sem_new(&sem,0);
  tcpip_init(tcpip_init_done_ok, &sem);
  sys_sem_wait(&sem);
  sys_sem_free(&sem);
  
  
  vTaskSuspendAll();
    PRINTF("TCP/IP initialized.\n\r");
  xTaskResumeAll();
  
  return 0;
}

static void ImAlive(void *param)
{
    int _cnt =0;
    while(1)
    {
        vTaskSuspendAll();
        PRINTF("Task running... %d\r\n", ++_cnt);
        xTaskResumeAll();
        vTaskDelay(5000U / portTICK_PERIOD_MS);
    }
}

char msgPingPong[512];
static void PingPongTask (void* param)
{
    int result;
    int len;
    struct rpmsg_endpoint *ept;

    /* Print the initial banner */
    vTaskSuspendAll();
    PRINTF("\r\n%s Task running... \r\n", __FUNCTION__);
    xTaskResumeAll();

    ept = rpmsg_rtos_create_ept(app_chnl, TTY_ENDPT);

    if(ept == NULL)
    {
        while(true)
        {
            vTaskSuspendAll();
            PRINTF("ERROR: cannot create endpoint %s %d\r\n", __FUNCTION__, __LINE__);
            xTaskResumeAll ();
        }
    }
    else
    {
        vTaskSuspendAll();
        PRINTF("created endpnt %d\r\n", TTY_ENDPT);
        xTaskResumeAll ();
    }

    vTaskSuspendAll();
    PRINTF("%s %d\r\n", __FUNCTION__, __LINE__);
    xTaskResumeAll ();

    while (true)
    {
        /* receive/send data to channel default ept */
        memset(msgPingPong, 0, sizeof(msgPingPong));
        result = rpmsg_rtos_recv(ept, msgPingPong, &len, sizeof(msgPingPong), NULL, 0xFFFFFFFF);
        assert(0 == result);
        result = rpmsg_rtos_send(ept, msgPingPong, len, TTY_ENDPT);
        assert(0 == result);
    }

    /* If destruction required */
    /*
    PRINTF("\r\nMessage pingpong finished\r\n");

    rpmsg_rtos_deinit(rdev);
    */
}

#define VERSION "1.00"
char msgUsr[512];

extern "C" bool getRPMSGChannel( struct rpmsg_channel **_app_chnl)
{
  *_app_chnl = app_chnl;
  return true;
  
}
static void usrTask (void* param)
{
    struct rpmsg_endpoint *ept;
    int len;
    int result;

    vTaskSuspendAll();
    PRINTF("\r\n" VERSION " %s Task running... \r\n", __FUNCTION__);
    PRINTF("RPMSG Init as Remote\r\n");
    xTaskResumeAll ();

    result = rpmsg_rtos_init(0 /*REMOTE_CPU_ID*/, &rdev, RPMSG_MASTER, &app_chnl);
    assert(0 == result);
    vTaskSuspendAll();
    PRINTF("Name service handshake is done, M4 has setup a rpmsg channel [%d ---> %d]\r\n", app_chnl->src, app_chnl->dst);
    xTaskResumeAll ();
    
    
     lwipConfig();
     IP4_ADDR(&gw, 192,168,0,1);
     IP4_ADDR(&ipaddr, 192,168,0,2);
     IP4_ADDR(&netmask, 255,255,255,0);
     
    netif_add(&netif, &ipaddr, &netmask, &gw, NULL, rpmsg_init_drv, ethernet_input);
    PRINTF("INFO: %s %s %d \n",__FILE__,__FUNCTION__, __LINE__);

   netif_set_default(&netif);
   PRINTF("INFO: %s %s %d \n",__FILE__,__FUNCTION__, __LINE__);

   netif_set_up(&netif);
    PRINTF("INFO: %s %s %d \n",__FILE__,__FUNCTION__, __LINE__);

    ept = rpmsg_rtos_create_ept(app_chnl,USR_ENDPT);

    if(ept == NULL)
    {
        while(true)
        {
            vTaskSuspendAll();
            PRINTF("ERROR: cannot create endpoint %s %d\r\n", __FUNCTION__, __LINE__);
            xTaskResumeAll ();
        }
    }
    else
    {
        vTaskSuspendAll();
        PRINTF("created endpnt %d\r\n", USR_ENDPT);
        xTaskResumeAll ();
    }

    vTaskSuspendAll();
    xTaskCreate(PingPongTask, "PingPongTask", APP_TASK_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
    xTaskResumeAll ();

    while (true)
    {
        /* receive/send data to channel default ept */
        memset(msgUsr, 0, sizeof(msgUsr));

        result = rpmsg_rtos_recv(ept, msgUsr, &len, sizeof(msgUsr), NULL, 0xFFFFFFFF);
        if (result < 0)
        {
            vTaskSuspendAll();
            PRINTF("time out\n\r");
            xTaskResumeAll();
            continue;
        }
        assert(0 == result);

        result = rpmsg_rtos_send(ept, msgUsr, len, USR_ENDPT);
        assert(0 == result);
    }
}

/*
 * MU Interrrupt ISR
 */
extern "C" void BOARD_MU_HANDLER(void)
{
    /*
     * calls into rpmsg_handler provided by middleware
     */
    rpmsg_handler();
 
}

/*!
 * @brief Main function
 */
int main (void)
{
    int result;

    hardware_init();

    /*
     * Prepare for the MU Interrupt
     *  MU must be initialized before rpmsg init is called
     */
    MU_Init(BOARD_MU_BASE_ADDR);
    NVIC_SetPriority(BOARD_MU_IRQ_NUM, APP_MU_IRQ_PRIORITY);
    NVIC_EnableIRQ(BOARD_MU_IRQ_NUM);

    /* Create a demo task. */
  
    xTaskCreate(ImAlive, "", APP_TASK_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
    xTaskCreate(usrTask, "ttyTask", APP_TASK_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);

    /* Start FreeRTOS scheduler. */
    vTaskStartScheduler();

    /* Should never reach this point. */
    while(true);
}

/*******************************************************************************
 * EOF
 ******************************************************************************/

