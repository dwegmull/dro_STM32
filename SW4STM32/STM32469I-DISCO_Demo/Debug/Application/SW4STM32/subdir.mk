################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
/home/dwegmull/STM32CubeIDE/workspace_1.4.0/STemWin/SW4STM32/startup_stm32f469xx.s 

OBJS += \
./Application/SW4STM32/startup_stm32f469xx.o 

S_DEPS += \
./Application/SW4STM32/startup_stm32f469xx.d 


# Each subdirectory must supply rules for building sources it contributes
Application/SW4STM32/startup_stm32f469xx.o: /home/dwegmull/STM32CubeIDE/workspace_1.4.0/STemWin/SW4STM32/startup_stm32f469xx.s
	arm-none-eabi-gcc -mcpu=cortex-m4 -g3 -c -I../../../Config -x assembler-with-cpp -MMD -MP -MF"Application/SW4STM32/startup_stm32f469xx.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@" "$<"

