################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../Core/MDK-ARM/startup_stm32f446xx.s 

C_SRCS += \
../Core/MDK-ARM/config.c 

OBJS += \
./Core/MDK-ARM/config.o \
./Core/MDK-ARM/startup_stm32f446xx.o 

S_DEPS += \
./Core/MDK-ARM/startup_stm32f446xx.d 

C_DEPS += \
./Core/MDK-ARM/config.d 


# Each subdirectory must supply rules for building sources it contributes
Core/MDK-ARM/%.o Core/MDK-ARM/%.su Core/MDK-ARM/%.cyclo: ../Core/MDK-ARM/%.c Core/MDK-ARM/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F446xx -c -I../Core/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Core/MDK-ARM/%.o: ../Core/MDK-ARM/%.s Core/MDK-ARM/subdir.mk
	arm-none-eabi-gcc -mcpu=cortex-m4 -g3 -DDEBUG -c -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@" "$<"

clean: clean-Core-2f-MDK-2d-ARM

clean-Core-2f-MDK-2d-ARM:
	-$(RM) ./Core/MDK-ARM/config.cyclo ./Core/MDK-ARM/config.d ./Core/MDK-ARM/config.o ./Core/MDK-ARM/config.su ./Core/MDK-ARM/startup_stm32f446xx.d ./Core/MDK-ARM/startup_stm32f446xx.o

.PHONY: clean-Core-2f-MDK-2d-ARM

