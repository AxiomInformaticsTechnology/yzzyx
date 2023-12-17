################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/x24.cpp \
../src/x32.cpp \
../src/x32i.cpp \
../src/yzzyx.cpp 

OBJS += \
./src/x24.o \
./src/x32.o \
./src/x32i.o \
./src/yzzyx.o 

CPP_DEPS += \
./src/x24.d \
./src/x32.d \
./src/x32i.d \
./src/yzzyx.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/caliope/usr/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


