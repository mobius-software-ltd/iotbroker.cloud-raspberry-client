################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/net/dtls_client.c \
../src/net/dyad.c \
../src/net/lws_net_client.c \
../src/net/tcp_client.c 

OBJS += \
./src/net/dtls_client.o \
./src/net/dyad.o \
./src/net/lws_net_client.o \
./src/net/tcp_client.o 

C_DEPS += \
./src/net/dtls_client.d \
./src/net/dyad.d \
./src/net/lws_net_client.d \
./src/net/tcp_client.d 


# Each subdirectory must supply rules for building sources it contributes
src/net/%.o: ../src/net/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I/usr/local/include -O2 -g -Wall -c -fmessage-length=0 -pthread -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


