################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/OpenCVUtils.cpp \
../src/main.cpp \
../src/virtualSheep.cpp 

OBJS += \
./src/OpenCVUtils.o \
./src/main.o \
./src/virtualSheep.o 

CPP_DEPS += \
./src/OpenCVUtils.d \
./src/main.d \
./src/virtualSheep.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/usr/local/include/opencv -I/usr/local/include/opencv2 -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


