



TOP:= $(abspath ${CURDIR}/../../)

PROJECT_NAME:=rpmsgTesting



#FreeRTOS_VERSION:=NXP
FreeRTOS_VERSION:=FreeRTOSV8.2.3


include ../../FreeRTOS.mk
include ../../IMX.mk
include ../../BSP.mk

ROOT = ${CURDIR}
TARGET=rpmsg
OBJECT_DIR	 = ./obj


ifdef _DEBUG
EXTRA_FLAGS= -Og -ggdb3
else
EXTRA_FLAGS= -Os
endif

# Tool names
CC	= arm-none-eabi-gcc
OBJCOPY	= arm-none-eabi-objcopy
CXX	= arm-none-eabi-g++
ASM	= arm-none-eabi-gcc


IMX_CPU:= CPU_MCIMX6X_M4

GEN_FLAGS_COMPILE= \
	-fno-strict-aliasing  -mcpu=cortex-m4 \
	-mfloat-abi=hard \
	-mfpu=fpv4-sp-d16 \
	-mthumb -Wall \
	-fno-common \
	-ffunction-sections \
	-fdata-sections \
	-ffreestanding \
	-fno-builtin \
	-mapcs \
	$(EXTRA_FLAGS)

PROJECT_SRC:= \
	hardware_init.c \
	main.cpp
	
#$(FreeRTOS_SRC) $(IMX_SRC) $(BSP_SRC)
SRC_FILES:= $(BSP_SRC) $(FreeRTOS_SRC) $(IMX_SRC) 
INCLUDE_DIRS= $(IMX_INCLUDE) $(FreeRTOS_INCLUDE) $(BSP_INCLUDE)

REALSRC_PATH = $(addprefix $(TOP)/, $(SRC_FILES))
TARGET_OBJS = $(addsuffix .o,$(addprefix $(OBJECT_DIR),$(basename $(SRC_FILES)))) \
	$(addsuffix .o,$(addprefix $(OBJECT_DIR)/,$(basename $(PROJECT_SRC))))
	
CFLAGS = \
	$(GEN_FLAGS_COMPILE) \
	-D$(IMX_CPU) \
	-std=gnu99 -MMD \
	$(addprefix -I $(TOP)/, $(INCLUDE_DIRS)) \
	-I ./
	
LDFLAGS= -T "${TOP}/ddr.ld" \
	$(GEN_FLAGS_COMPILE) \
	 --specs=nano.specs -Wl,--gc-sections,-static -Xlinker -z -Xlinker muldefs \
	  -Wl,--start-group -lgcc  -lc -lm -lnosys -Wl,--end-group
	 
	 
ASFLAGS = $(GEN_FLAGS_COMPILE) \
	$(GEN_FLAGS_COMPILE) \
	-D$(IMX_CPU) \
	$(addprefix -I $(TOP)/, $(INCLUDE_DIRS)) \
	-I ./
	
CXXFLAGS = \
	$(GEN_FLAGS_COMPILE) \
	-D$(IMX_CPU) -MMD \
	$(addprefix -I $(TOP)/, $(INCLUDE_DIRS)) \
	-I ./
	
MKDIR_OBJDIR = @mkdir -p $(dir $@)

$(OBJECT_DIR)/%.o: $(addprefix $(TOP)/, %.c)
	$(MKDIR_OBJDIR)
	@echo %% $(notdir $<)
	@$(CC) -c -o $@ $(CFLAGS) $<

$(OBJECT_DIR)/%.o: %.c
	$(MKDIR_OBJDIR)
	@echo %% $(notdir $<)
	@$(CC) -c -o $@ $(CFLAGS) $<
	
$(OBJECT_DIR)/%.o: %.cpp
	$(MKDIR_OBJDIR)
	@echo %% $(notdir $<)
	@echo %% $<
	@$(CXX) -c -o $@ $(CXXFLAGS) $<	
	
$(OBJECT_DIR)/%.o: $(addprefix $(TOP)/, %.S)
	$(MKDIR_OBJDIR)
	@echo %% $(notdir $<)
	@$(CC) -c -o $@ $(ASFLAGS) $< 


$(PROJECT_NAME).bin:  $(PROJECT_NAME).elf $(PROJECT_NAME).hex
	$(OBJCOPY) -O binary $< $@

$(PROJECT_NAME).hex:  $(PROJECT_NAME).elf
	$(OBJCOPY) -O ihex $< $@
	
$(PROJECT_NAME).elf:  $(TARGET_OBJS)
	$(CC) -o $@ $^ $(LDFLAGS) -Xlinker -Map=${PROJECT_NAME}.map
	
clean: 
	find -name \*~ | xargs rm -f
	rm -rf $(OBJECT_DIR)
	rm -rf *.elf *.bin *.hex *.map


-include $(TARGET_OBJS:%.o=%.d)




