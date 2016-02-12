


LWIPDIR:=/lwip/lwip/src
include $(TOP)/lwip/lwip/src/Filelists.mk

lwipFreeRTOS_RPMSG_DIR:=/lwip/lwipFreeRTOS


lwipFreeRTOS_RPMSG_SRC_INCLUDE:= \
	$(lwipFreeRTOS_RPMSG_DIR)/include \
	$(lwipFreeRTOS_RPMSG_DIR)/proj \
	$(lwipFreeRTOS_RPMSG_DIR)/netif \
	$(lwipFreeRTOS_RPMSG_DIR) 
	
lwipFreeRTOS_RPMSG_SRC:= \
	$(lwipFreeRTOS_RPMSG_DIR)/sys_arch.c \
	$(lwipFreeRTOS_RPMSG_DIR)/netif/rpmsgEthernet.c 	
	
#	$(lwipFreeRTOS_RPMSG_DIR)/proj/lwipconfig.c \	
	
LWIP_SRC_INCLUDE:= $(LWIPDIR)/include \
	$(lwipFreeRTOS_RPMSG_SRC_INCLUDE)


LWIP_SRC:= \
	$(lwipFreeRTOS_RPMSG_SRC) \
	$(LWIPNOAPPSFILES) \
	$(LWIPERFFILES)