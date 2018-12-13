################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/amqp/amqp_calc.c \
../src/amqp/amqp_client.c \
../src/amqp/amqp_parser.c \
../src/amqp/amqp_timers.c \
../src/amqp/header_factory.c \
../src/amqp/tlv_factory.c 

OBJS += \
./src/amqp/amqp_calc.o \
./src/amqp/amqp_client.o \
./src/amqp/amqp_parser.o \
./src/amqp/amqp_timers.o \
./src/amqp/header_factory.o \
./src/amqp/tlv_factory.o 

C_DEPS += \
./src/amqp/amqp_calc.d \
./src/amqp/amqp_client.d \
./src/amqp/amqp_parser.d \
./src/amqp/amqp_timers.d \
./src/amqp/header_factory.d \
./src/amqp/tlv_factory.d 


# Each subdirectory must supply rules for building sources it contributes
src/amqp/%.o: ../src/amqp/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I/usr/local/include -O2 -g -Wall -c -fmessage-length=0 -pthread -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


