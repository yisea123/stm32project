#!/bin/sh
#
#
# create webexamples
#
# script sollte irgendwo im /tmp/slave ausgeführt werden
# mit 
# /z/0/1004/software/trunk/example/slave1 <targetname>
#

TARGET_NAME=$1

WEB_DIR=/z/0/1018/build
CO_DIR="/z/0/1004/software/trunk"
CO_LIB_DIR="$CO_DIR/colib"
CO_EXAMPLE="$CO_DIR/example/slave1"
DST_INC_DIR=co_inc

LIB_NAME=libslave1.a


# alle Dateien kopieren
cp ${CO_EXAMPLE}/*.[ch] ${CO_EXAMPLE}/Makefile .
cp ${CO_EXAMPLE}/slave1.eds .
cp ${CO_EXAMPLE}/slave1.cddp .

# übersetzen
make DRV=socketcan	USER_DEFINES="-DCO_EVAL_MODE -DCO_EVAL_MODE_ENABLED"	\
		CO_LIB_DIR=$CO_DIR/colib	\
		CO_DRV_DIR=$CO_DIR/codrv

# lösche alle obj-Dateien, die wir im Quelltext haben
for i in *.c
do
	echo $i	;\
	rm obj/`basename $i .c`.*	;\
done

# Erzeuge Libs von Lib und Treiber
ar rv $LIB_NAME `ls obj/*.o`
strip --strip-debug $LIB_NAME

# lösche all objects
make DRV=socketcan distclean
rmdir obj

# kopiere includes hierher
if [ ! -d $DST_INC_DIR ]
then 
	mkdir $DST_INC_DIR
fi
cp $CO_LIB_DIR/inc/*.h $DST_INC_DIR
for i in co_flyingmaster.h co_gfc.h co_manager.h co_network.h co_srd.h co_srdo.h
do
	rm $DST_INC_DIR/$i
done


# Erzeuge neues Makefile
cat > Makefile <<EOM
# Makefile for CANopen example

CFLAGS = -Wall -Wmissing-prototypes
CC = gcc

TARGETNAME = slave1-$TARGET_NAME

INC_DIRS = -I. -Ico_inc

#---------------------------------------------------------------------------
.PHONY:\$(TARGETNAME)
\$(TARGETNAME):
	\$(CC) \$(CFLAGS) -o \$(TARGETNAME) \$(INC_DIRS) -MMD *.c $LIB_NAME -lrt


all: \$(TARGETNAME)

EOM

# erzeuge binary
make

# erzeuge zipfile
rm $WEB_DIR/co_$TARGET_NAME.zip
zip -r $WEB_DIR/co_$TARGET_NAME.zip *

