

FreeRTOS_BASE_DIR:=/FreeRTOS/$(FreeRTOS_VERSION)
FreeRTOS_PROTABLE_INCLUDE:= $(FreeRTOS_BASE_DIR)/FreeRTOS/Source/portable/GCC/ARM_CM4F
FreeRTOS_INCLUDE:= \
	$(FreeRTOS_BASE_DIR)/FreeRTOS/Source/include \
	$(FreeRTOS_PROTABLE_INCLUDE)
	
FreeRTOS_DIR:= $(FreeRTOS_BASE_DIR)/FreeRTOS
	

FreeRTOS_BASE_SRC:= \
	croutine.c \
	event_groups.c \
	list.c \
	queue.c \
	tasks.c \
	timers.c
	
FreeRTOS_Processor_SRC:= \
	portable/GCC/ARM_CM4F/port.c \
	portable/MemMang/heap_2.c
	
FreeRTOS_SRC:= \
	$(addprefix $(FreeRTOS_DIR)/Source/, $(FreeRTOS_BASE_SRC)) \
	$(addprefix $(FreeRTOS_DIR)/Source/, $(FreeRTOS_Processor_SRC))

