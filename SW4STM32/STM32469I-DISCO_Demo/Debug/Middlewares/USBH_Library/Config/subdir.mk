################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/home/dwegmull/STM32CubeIDE/workspace_1.4.0/STemWin/Config/usbh_conf.c 

OBJS += \
./Middlewares/USBH_Library/Config/usbh_conf.o 

C_DEPS += \
./Middlewares/USBH_Library/Config/usbh_conf.d 


# Each subdirectory must supply rules for building sources it contributes
Middlewares/USBH_Library/Config/usbh_conf.o: /home/dwegmull/STM32CubeIDE/workspace_1.4.0/STemWin/Config/usbh_conf.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DSTM32F469xx -DUSE_HAL_DRIVER -DUSE_STM32469I_DISCOVERY -DUSE_USB_FS -DUSE_IOEXPANDER -c -I../../../Config -I../../../Gui/STemWin_Addons -I../../../Gui/Target -I../../../Core/Inc -I../../../Utilities/CPU -I../../../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../../../Drivers/STM32F4xx_HAL_Driver/Inc -I../../../Drivers/BSP/STM32469I-Discovery -I../../../Drivers/BSP/Components -I../../../Drivers/BSP/Components/Common -I../../../Middlewares/ST/STM32_USB_Host_Library/Core/Inc -I../../../Middlewares/ST/STM32_USB_Host_Library/Class/MSC/Inc -I../../../Middlewares/ST/STM32_USB_Device_Library/Class/MSC/Inc -I../../../Middlewares/ST/STemWin/inc -I../../../Middlewares/Third_Party/FatFs/src -I../../../Middlewares/Third_Party/FatFs/src/drivers -I../../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I../../../Middlewares/Third_Party/FreeRTOS/Source/include -I../../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../../../Modules/audio_player -I../../../Modules/audio_recorder -I../../../Modules/Common -I../../../Drivers/CMSIS/Include -Os -ffunction-sections -Wall -fstack-usage -MMD -MP -MF"Middlewares/USBH_Library/Config/usbh_conf.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

