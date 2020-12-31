################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Middlewares/USBH_Library/Core/subdir.mk
-include Middlewares/USBH_Library/Config/subdir.mk
-include Middlewares/USBH_Library/Class/MSC/subdir.mk
-include Middlewares/STemWin/OS/subdir.mk
-include Middlewares/STemWin/Config/subdir.mk
-include Middlewares/FreeRTOS/Portable/subdir.mk
-include Middlewares/FreeRTOS/subdir.mk
-include Middlewares/FatFs/Options/subdir.mk
-include Middlewares/FatFs/Drivers/subdir.mk
-include Middlewares/FatFs/Core/subdir.mk
-include Drivers/STM32F4_HAL_Driver/subdir.mk
-include Drivers/CMSIS/subdir.mk
-include Drivers/BSP/STM32469I_DISCO/subdir.mk
-include Drivers/BSP/Component/subdir.mk
-include Application/User/Utilities/subdir.mk
-include Application/User/Core/subdir.mk
-include Application/SW4STM32/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
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
EXECUTABLES += \
STemWin.elf \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
STemWin.list \

OBJCOPY_HEX += \
STemWin.hex \


# All Target
all: STemWin.elf secondary-outputs

# Tool invocations
STemWin.elf: $(OBJS) $(USER_OBJS) ../STM32F469NIHx_FLASH.ld
	arm-none-eabi-gcc -o "STemWin.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m4 -T"../STM32F469NIHx_FLASH.ld" --specs=nosys.specs -Wl,-Map="STemWin.map" -Wl,--gc-sections -static -L../../../Middlewares/ST/STM32_Audio/Addons/PDM/Lib -L../../../Gui/STemWin_Addons -L../../../Middlewares/ST/STemWin/Lib --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

STemWin.list: $(EXECUTABLES)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "STemWin.list"
	@echo 'Finished building: $@'
	@echo ' '

STemWin.hex: $(EXECUTABLES)
	arm-none-eabi-objcopy  -O ihex $(EXECUTABLES) "STemWin.hex"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) *
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST) $(OBJCOPY_HEX)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
