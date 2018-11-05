# Makefile CANopen example using CAN FD

CFLAGS =  -c -Wall -Wmissing-prototypes -DDEBUG
CFLAGS = -g -c -Wall -Wmissing-prototypes
CC = gcc

# used driver, can4linux is default, else call make DRV=socketcan
DRV = can4linux

OBJ_DIR = obj
TARGET_NAME = canfd_$(shell $(CC) -dumpmachine)_$(DRV)

ifeq ($(DRV), socketcan)
DRVOBJ = codrv_socketcan.o
else
DRVOBJ = codrv_c4l.o
endif


CO_INC_DIRS = ../../colib/inc ../../colib/src .
C4L_INC_DIRS = /usr/src/can4linux

CO_OBJECTS = \
	main.o	\
	gen_objdict.o	\
	co_odaccess.o	\
	co_commtask.o	\
	co_sdoserv.o	\
	co_usdoserv.o	\
	co_usdo.o	\
	co_sdoclient.o	\
	co_pdo.o	\
	co_nmt.o	\
	co_errctrl.o	\
	co_event.o	\
	co_queue.o	\
	co_cobhandler.o	\
	co_store.o	\
	co_timer.o	\
	co_emcy.o	\
	co_stackinit.o	\
	co_led.o	\
	$(DRVOBJ)	\
	cpu_linux.o	\


VPATH += ../../colib/src
VPATH += ../../codrv/common
VPATH += ../../codrv/can4linux
VPATH += ../../codrv/socketcan

# add -I option
INC_DIRS = $(addprefix -I,$(CO_INC_DIRS) $(C4L_INC_DIRS))

# add path obj/
CO_OBJS         = $(addprefix $(OBJ_DIR)/,$(CO_OBJECTS))

# deps
CO_DEPS := $(CO_OBJS:.o=.d)

#---------------------------------------------------------------------------
.PHONY:$(TARGET_NAME)
$(TARGET_NAME): $(CO_OBJS) gen_define.h
	@echo " [LD] $(TARGET_NAME)"
	@$(CC) $(LDFLAGS) -o $(TARGET_NAME) $(CO_OBJS) -lrt

# ------------------------------------------------------------
# objects
#
#@echo " [CC] $<"
.SUFFIXES: o
$(OBJ_DIR)/%.o : %.c gen_define.h Makefile
	@mkdir -p $(@D)
	@echo " [CC] $<"
	$(CC) $(CFLAGS) $(INC_DIRS) -MMD -o$@ -c $<


all: $(TARGET_NAME)

-include $(CO_DEPS)

.PHONY:clean
clean:
	rm -f $(OBJ_DIR)/* $(TARGET_NAME)

help:
	@echo "make targets:"
	@echo "make                    - target app for can4linux"
	@echo "make DRV=socketcan      - target app for SocketCAN"
	@echo "make CANopenlibc        - libCANopenlibc.a  for can4linux"
	@echo "make CANopenlibs        - libCANopenlibc.a  for SocketCAN"
	@echo ""
	@echo ""
