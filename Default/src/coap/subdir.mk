################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/coap/coap_client.c \
../src/coap/coap_parser.c \
../src/coap/coap_timers.c 

OBJS += \
./src/coap/coap_client.o \
./src/coap/coap_parser.o \
./src/coap/coap_timers.o 

C_DEPS += \
./src/coap/coap_client.d \
./src/coap/coap_parser.d \
./src/coap/coap_timers.d 


# Each subdirectory must supply rules for building sources it contributes
src/coap/%.o: ../src/coap/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I/usr/local/include -O2 -g -Wall -c -fmessage-length=0 -pthread -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


