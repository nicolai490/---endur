################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/code.cpp \
../src/main.cpp \
../src/parser.cpp \
../src/scanner.cpp \
../src/sourceline.cpp \
../src/symtab.cpp \
../src/token.cpp 

OBJS += \
./src/code.o \
./src/main.o \
./src/parser.o \
./src/scanner.o \
./src/sourceline.o \
./src/symtab.o \
./src/token.o 

CPP_DEPS += \
./src/code.d \
./src/main.d \
./src/parser.d \
./src/scanner.d \
./src/sourceline.d \
./src/symtab.d \
./src/token.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


