################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../LCDTFT_MOD/MiniGFX.c \
../LCDTFT_MOD/TFT.c \
../LCDTFT_MOD/regValues.c 

C_DEPS += \
./LCDTFT_MOD/MiniGFX.d \
./LCDTFT_MOD/TFT.d \
./LCDTFT_MOD/regValues.d 

OBJS += \
./LCDTFT_MOD/MiniGFX.o \
./LCDTFT_MOD/TFT.o \
./LCDTFT_MOD/regValues.o 


# Each subdirectory must supply rules for building sources it contributes
LCDTFT_MOD/%.o: ../LCDTFT_MOD/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: AVR Compiler'
	avr-gcc -Wall -g2 -gstabs -Os -fpack-struct -fshort-enums -ffunction-sections -fdata-sections -std=gnu99 -funsigned-char -funsigned-bitfields -mmcu=atmega32 -DF_CPU=1600000UL -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


