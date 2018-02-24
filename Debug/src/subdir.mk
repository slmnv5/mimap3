################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/AlsaMapper.cpp \
../src/MidiEvent.cpp \
../src/MidiFilter.cpp \
../src/RuleMapper.cpp \
../src/mimap3.cpp 

OBJS += \
./src/AlsaMapper.o \
./src/MidiEvent.o \
./src/MidiFilter.o \
./src/RuleMapper.o \
./src/mimap3.o 

CPP_DEPS += \
./src/AlsaMapper.d \
./src/MidiEvent.d \
./src/MidiFilter.d \
./src/RuleMapper.d \
./src/mimap3.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -std=c++0x -DDEBUG -O0 -g3 -Wall -c   -lasound -Winvalid-pch -v -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


