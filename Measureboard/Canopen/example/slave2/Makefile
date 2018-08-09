# Makefile for CANopen example

CFLAGS = -g -c -Wall -Wmissing-prototypes
CC = gcc

TARGET_NAME = slave2_$(shell $(CC) -dumpmachine)_$(DRV)

# used driver, can4linux is default, else call make DRV=socketcan
DRV = socketcan
DRV = can4linux
OBJ_DIR = obj
CO_LIB_DIR = ../../colib
CO_DRV_DIR = ../../codrv


CO_LIB_OBJECTS = \
	co_cobhandler.o	\
	co_commtask.o	\
	co_emcy.o	\
	co_errctrl.o	\
	co_event.o	\
	co_led.o	\
	co_nmt.o	\
	co_odaccess.o	\
	co_pdo.o	\
	co_queue.o	\
	co_sdoserv.o	\
	co_stackinit.o	\
	co_store.o	\
	co_time.o	\
	co_timer.o	\

ifeq ($(DRV), socketcan)
CO_DRV_OBJECTS = cpu_linux.o codrv_socketcan.o
VPATH += $(CO_DRV_DIR)/socketcan
else
CO_DRV_OBJECTS = cpu_linux.o codrv_c4l.o
VPATH += $(CO_DRV_DIR)/can4linux
endif

CO_OBJECTS = \
	main.o		\
	gen_objdict.o		\
	$(CO_LIB_OBJECTS)	\
	$(CO_DRV_OBJECTS)


VPATH += $(CO_LIB_DIR)/src
VPATH += $(CO_DRV_DIR)/common

CO_INC_DIRS = $(CO_LIB_DIR)/inc $(CO_LIB_DIR)/src .
C4l_INC_DIR = /usr/src/can4linux

# add -I option
INC_DIRS = $(addprefix -I,$(CO_INC_DIRS) $(C4l_INC_DIR))

# add path obj/
CO_OBJS	= $(addprefix $(OBJ_DIR)/,$(CO_OBJECTS))

# deps
CO_DEPS := $(CO_OBJS:.o=.d)

#---------------------------------------------------------------------------
.PHONY:$(TARGET_NAME)
$(TARGET_NAME): $(CO_OBJS)
	@echo " [LD] $(TARGET_NAME)"
	@$(CC) $(LDFLAGS) -o $(TARGET_NAME) $(CO_OBJS) -lrt

# ------------------------------------------------------------
# objects
#
.SUFFIXES: o
$(OBJ_DIR)/%.o : %.c Makefile
	@mkdir -p $(@D)
	@echo " [CC] $<"
	@$(CC) $(CFLAGS) $(INC_DIRS) -MMD -o$@ -c $<


all: $(TARGET_NAME)

-include $(CO_DEPS)

.PHONY:clean
clean:
	@echo "I do not delete the target \"$(TARGET_NAME)\""
	rm -f $(OBJ_DIR)/* 
	
.PHONY:distclean
distclean:
	rm -f $(OBJ_DIR)/* $(TARGET_NAME)

.PHONY:strip
	# strip all debug information from library
strip:
	strip --strip-debug lib$(LIBNAME)_$(shell $(CC) -dumpmachine).a 

help:
	@echo "make targets:"
	@echo "make                    - target app for can4linux"
	@echo "make DRV=socketcan      - target app for SocketCAN"



