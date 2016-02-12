

BSP_DIR:=/neo/neo_m4

BSP_base_SRC:= \
	board.c\
	clock_freq.c \
	gpio_pins.c \
	pin_mux.c
	
	
BSP_INCLUDE:= $(BSP_DIR)
BSP_SRC:= $(addprefix $(BSP_DIR)/, $(BSP_base_SRC)) 
