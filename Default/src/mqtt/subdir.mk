################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/mqtt/mqtt_client.c \
../src/mqtt/mqtt_timers.c \
../src/mqtt/parser.c 

OBJS += \
./src/mqtt/mqtt_client.o \
./src/mqtt/mqtt_timers.o \
./src/mqtt/parser.o 

C_DEPS += \
./src/mqtt/mqtt_client.d \
./src/mqtt/mqtt_timers.d \
./src/mqtt/parser.d 


# Each subdirectory must supply rules for building sources it contributes
src/mqtt/%.o: ../src/mqtt/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I/usr/local/include -O2 -g -Wall -c -fmessage-length=0 -pthread -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


