################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/amqp/wrappers/unwrapper.c \
../src/amqp/wrappers/wrapper.c 

OBJS += \
./src/amqp/wrappers/unwrapper.o \
./src/amqp/wrappers/wrapper.o 

C_DEPS += \
./src/amqp/wrappers/unwrapper.d \
./src/amqp/wrappers/wrapper.d 


# Each subdirectory must supply rules for building sources it contributes
src/amqp/wrappers/%.o: ../src/amqp/wrappers/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I/usr/local/include -O2 -g -Wall -c -fmessage-length=0 -pthread -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


