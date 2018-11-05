################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../app/t_unit/simple_type.c \
../app/t_unit/t_data_obj.c \
../app/t_unit/t_data_obj_const.c \
../app/t_unit/t_data_obj_flowstep.c \
../app/t_unit/t_data_obj_measdata.c \
../app/t_unit/t_data_obj_pid.c \
../app/t_unit/t_data_obj_simple.c \
../app/t_unit/t_data_obj_stepinfo.c \
../app/t_unit/t_data_obj_test.c \
../app/t_unit/t_data_obj_time.c \
../app/t_unit/t_dataclass.c \
../app/t_unit/t_unit.c 

OBJS += \
./app/t_unit/simple_type.o \
./app/t_unit/t_data_obj.o \
./app/t_unit/t_data_obj_const.o \
./app/t_unit/t_data_obj_flowstep.o \
./app/t_unit/t_data_obj_measdata.o \
./app/t_unit/t_data_obj_pid.o \
./app/t_unit/t_data_obj_simple.o \
./app/t_unit/t_data_obj_stepinfo.o \
./app/t_unit/t_data_obj_test.o \
./app/t_unit/t_data_obj_time.o \
./app/t_unit/t_dataclass.o \
./app/t_unit/t_unit.o 

C_DEPS += \
./app/t_unit/simple_type.d \
./app/t_unit/t_data_obj.d \
./app/t_unit/t_data_obj_const.d \
./app/t_unit/t_data_obj_flowstep.d \
./app/t_unit/t_data_obj_measdata.d \
./app/t_unit/t_data_obj_pid.d \
./app/t_unit/t_data_obj_simple.d \
./app/t_unit/t_data_obj_stepinfo.d \
./app/t_unit/t_data_obj_test.d \
./app/t_unit/t_data_obj_time.d \
./app/t_unit/t_dataclass.d \
./app/t_unit/t_unit.d 


# Each subdirectory must supply rules for building sources it contributes
app/t_unit/%.o: ../app/t_unit/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m3 -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections  -g3 -DDEBUG -DUSE_FULL_ASSERT -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


