################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (9-2020-q2-update)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/mydrivers/Src/stm32f401xe_GPIO.c \
../Core/mydrivers/Src/stm32f401xe_I2C.c \
../Core/mydrivers/Src/stm32f401xe_RCC.c 

OBJS += \
./Core/mydrivers/Src/stm32f401xe_GPIO.o \
./Core/mydrivers/Src/stm32f401xe_I2C.o \
./Core/mydrivers/Src/stm32f401xe_RCC.o 

C_DEPS += \
./Core/mydrivers/Src/stm32f401xe_GPIO.d \
./Core/mydrivers/Src/stm32f401xe_I2C.d \
./Core/mydrivers/Src/stm32f401xe_RCC.d 


# Each subdirectory must supply rules for building sources it contributes
Core/mydrivers/Src/%.o: ../Core/mydrivers/Src/%.c Core/mydrivers/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F401xE -c -I../Core/Inc -I../Core/mydrivers/Inc -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

