################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
-include LCDTFT_MOD/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(I_DEPS)),)
-include $(I_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(M_DEPS)),)
-include $(M_DEPS)
endif
ifneq ($(strip $(MI_DEPS)),)
-include $(MI_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
LSS += \
OmniTFT.lss \

FLASH_IMAGE += \
OmniTFT.hex \

EEPROM_IMAGE += \
OmniTFT.eep \

SIZEDUMMY += \
sizedummy \


# All Target
all: OmniTFT.elf secondary-outputs

# Tool invocations
OmniTFT.elf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: AVR C Linker'
	avr-gcc -Wl,-Map,OmniTFT.map -mmcu=atmega32 -o "OmniTFT.elf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

OmniTFT.lss: OmniTFT.elf
	@echo 'Invoking: AVR Create Extended Listing'
	-avr-objdump -h -S OmniTFT.elf  >"OmniTFT.lss"
	@echo 'Finished building: $@'
	@echo ' '

OmniTFT.hex: OmniTFT.elf
	@echo 'Create Flash image (ihex format)'
	-avr-objcopy -R .eeprom -R .fuse -R .lock -R .signature -O ihex OmniTFT.elf  "OmniTFT.hex"
	@echo 'Finished building: $@'
	@echo ' '

OmniTFT.eep: OmniTFT.elf
	@echo 'Create eeprom image (ihex format)'
	-avr-objcopy -j .eeprom --no-change-warnings --change-section-lma .eeprom=0 -O ihex OmniTFT.elf  "OmniTFT.eep"
	@echo 'Finished building: $@'
	@echo ' '

sizedummy: OmniTFT.elf
	@echo 'Invoking: Print Size'
	-avr-size --format=avr --mcu=atmega32 OmniTFT.elf
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(M_DEPS)$(MI_DEPS)$(S_DEPS)$(LSS)$(C_DEPS)$(FLASH_IMAGE)$(ELFS)$(I_DEPS)$(OBJS)$(ASM_DEPS)$(EEPROM_IMAGE)$(SIZEDUMMY)$(S_UPPER_DEPS) OmniTFT.elf
	-@echo ' '

secondary-outputs: $(LSS) $(FLASH_IMAGE) $(EEPROM_IMAGE) $(SIZEDUMMY)

.PHONY: all clean dependents

-include ../makefile.targets
