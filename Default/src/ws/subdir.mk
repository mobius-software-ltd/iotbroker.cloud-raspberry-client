################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/ws/base64_parser.c \
../src/ws/ws_parser.c 

OBJS += \
./src/ws/base64_parser.o \
./src/ws/ws_parser.o 

C_DEPS += \
./src/ws/base64_parser.d \
./src/ws/ws_parser.d 


# Each subdirectory must supply rules for building sources it contributes
src/ws/%.o: ../src/ws/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I/usr/local/include -O2 -g -Wall -c -fmessage-length=0 -pthread -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


