################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/adc_fft.c \
../Core/Src/camera_code.c \
../Core/Src/config.c \
../Core/Src/fsm.c \
../Core/Src/main.c \
../Core/Src/motorposition.c \
../Core/Src/ov7670.c \
../Core/Src/servomotors.c \
../Core/Src/steppermotors.c \
../Core/Src/stm32f4xx_hal_msp.c \
../Core/Src/stm32f4xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32f4xx.c 

OBJS += \
./Core/Src/adc_fft.o \
./Core/Src/camera_code.o \
./Core/Src/config.o \
./Core/Src/fsm.o \
./Core/Src/main.o \
./Core/Src/motorposition.o \
./Core/Src/ov7670.o \
./Core/Src/servomotors.o \
./Core/Src/steppermotors.o \
./Core/Src/stm32f4xx_hal_msp.o \
./Core/Src/stm32f4xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32f4xx.o 

C_DEPS += \
./Core/Src/adc_fft.d \
./Core/Src/camera_code.d \
./Core/Src/config.d \
./Core/Src/fsm.d \
./Core/Src/main.d \
./Core/Src/motorposition.d \
./Core/Src/ov7670.d \
./Core/Src/servomotors.d \
./Core/Src/steppermotors.d \
./Core/Src/stm32f4xx_hal_msp.d \
./Core/Src/stm32f4xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32f4xx.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/adc_fft.cyclo ./Core/Src/adc_fft.d ./Core/Src/adc_fft.o ./Core/Src/adc_fft.su ./Core/Src/camera_code.cyclo ./Core/Src/camera_code.d ./Core/Src/camera_code.o ./Core/Src/camera_code.su ./Core/Src/config.cyclo ./Core/Src/config.d ./Core/Src/config.o ./Core/Src/config.su ./Core/Src/fsm.cyclo ./Core/Src/fsm.d ./Core/Src/fsm.o ./Core/Src/fsm.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/motorposition.cyclo ./Core/Src/motorposition.d ./Core/Src/motorposition.o ./Core/Src/motorposition.su ./Core/Src/ov7670.cyclo ./Core/Src/ov7670.d ./Core/Src/ov7670.o ./Core/Src/ov7670.su ./Core/Src/servomotors.cyclo ./Core/Src/servomotors.d ./Core/Src/servomotors.o ./Core/Src/servomotors.su ./Core/Src/steppermotors.cyclo ./Core/Src/steppermotors.d ./Core/Src/steppermotors.o ./Core/Src/steppermotors.su ./Core/Src/stm32f4xx_hal_msp.cyclo ./Core/Src/stm32f4xx_hal_msp.d ./Core/Src/stm32f4xx_hal_msp.o ./Core/Src/stm32f4xx_hal_msp.su ./Core/Src/stm32f4xx_it.cyclo ./Core/Src/stm32f4xx_it.d ./Core/Src/stm32f4xx_it.o ./Core/Src/stm32f4xx_it.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32f4xx.cyclo ./Core/Src/system_stm32f4xx.d ./Core/Src/system_stm32f4xx.o ./Core/Src/system_stm32f4xx.su

.PHONY: clean-Core-2f-Src

