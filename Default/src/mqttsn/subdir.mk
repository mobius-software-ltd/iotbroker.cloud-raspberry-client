################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/mqttsn/mqtt_sn_client.c \
../src/mqttsn/mqtt_sn_timers.c \
../src/mqttsn/sn_parser.c 

OBJS += \
./src/mqttsn/mqtt_sn_client.o \
./src/mqttsn/mqtt_sn_timers.o \
./src/mqttsn/sn_parser.o 

C_DEPS += \
./src/mqttsn/mqtt_sn_client.d \
./src/mqttsn/mqtt_sn_timers.d \
./src/mqttsn/sn_parser.d 


# Each subdirectory must supply rules for building sources it contributes
src/mqttsn/%.o: ../src/mqttsn/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I/usr/local/include -O2 -g -Wall -c -fmessage-length=0 -pthread -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


