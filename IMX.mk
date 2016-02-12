
IMX_VERSION:=1.0.0
IMX_DIR:=/NXP/FreeRTOS_BSP_$(IMX_VERSION)_iMX6SX



RPMSG_SRC_INCLUDE:= \
	middleware/multicore/open-amp/rpmsg \
	middleware/multicore/open-amp/common/hil \
	middleware/multicore/open-amp/common/llist \
	middleware/multicore/open-amp/common/shm \
	middleware/multicore/open-amp/porting/config \
	middleware/multicore/open-amp/virio \
	middleware/multicore/open-amp \
	middleware/multicore/open-amp/porting/env/freertos \
	middleware/multicore/open-amp/porting/env/imx6sx_m4 \
	middleware/multicore/open-amp/porting/imx6sx_m4 
 
RPMSG_SRC:= \
	middleware/multicore/open-amp/common/hil/hil.c \
	middleware/multicore/open-amp/common/llist/llist.c \
	middleware/multicore/open-amp/common/shm/sh_mem.c \
	middleware/multicore/open-amp/porting/config/config.c \
	middleware/multicore/open-amp/porting/env/freertos/rpmsg_porting.c \
	middleware/multicore/open-amp/porting/imx6sx_m4/platform.c \
	middleware/multicore/open-amp/porting/imx6sx_m4/platform_info.c \
	middleware/multicore/open-amp/rpmsg/rpmsg.c \
	middleware/multicore/open-amp/rpmsg/rpmsg_core.c \
	middleware/multicore/open-amp/rpmsg/rpmsg_ext.c \
	middleware/multicore/open-amp/rpmsg/rpmsg_rtos.c \
	middleware/multicore/open-amp/virtio/virtio.c \
	middleware/multicore/open-amp/virtio/virtqueue.c \
	middleware/multicore/open-amp/rpmsg/remote_device.c 

CMSIS_SRC_INCLUDE:= \
	platform/CMSIS/Include

CMSIS_SRC:= 

MCIMX6X_SRC_INCLUDE:= \
	/platform/devices \
	platform/devices/MCIMX6X \
	platform/devices/MCIMX6X/include \
	platform/devices/MCIMX6X/startup \
	platform/devices/MCIMX6X/startup/gcc \
	platform/drivers/inc \

MCIMX6X_SRC:= \
	platform/devices/MCIMX6X/startup/system_MCIMX6X_M4.c \
	platform/devices/MCIMX6X/startup/gcc/startup_MCIMX6X_M4.S \
	platform/drivers/src/adc_imx6sx.c  \
	platform/drivers/src/ccm_analog_imx6sx.c  \
	platform/drivers/src/ccm_imx6sx.c  \
	platform/drivers/src/ecspi.c  \
	platform/drivers/src/epit.c  \
	platform/drivers/src/flexcan.c \
	platform/drivers/src/gpio_imx.c  \
	platform/drivers/src/i2c_imx.c  \
	platform/drivers/src/lmem.c  \
	platform/drivers/src/mu_imx.c \
	platform/drivers/src/rdc.c  \
	platform/drivers/src/rdc_semaphore.c  \
	platform/drivers/src/sema4.c \
	platform/drivers/src/uart_imx.c \
	platform/drivers/src/wdog_imx.c 

MCIMX6X_UTIL_SRC:= \
	platform/utilities/src/debug_console_imx.c \
	platform/utilities/src/print_scan.c

MCIMX6X_UTIL_INCLUDE:= \
	platform/utilities/inc \
	platform/utilities/src

IMX_SRC := \
	$(addprefix $(IMX_DIR)/, $(MCIMX6X_SRC)) \
	$(addprefix $(IMX_DIR)/, $(RPMSG_SRC)) \
	$(addprefix $(IMX_DIR)/, $(MCIMX6X_UTIL_SRC)) 	

IMX_INCLUDE:= 	\
	$(addprefix $(IMX_DIR)/, $(RPMSG_SRC_INCLUDE)) \
	$(addprefix $(IMX_DIR)/, $(CMSIS_SRC_INCLUDE)) \
	$(addprefix $(IMX_DIR)/, $(MCIMX6X_SRC_INCLUDE)) \
	$(addprefix $(IMX_DIR)/, $(MCIMX6X_UTIL_INCLUDE)) 




 
