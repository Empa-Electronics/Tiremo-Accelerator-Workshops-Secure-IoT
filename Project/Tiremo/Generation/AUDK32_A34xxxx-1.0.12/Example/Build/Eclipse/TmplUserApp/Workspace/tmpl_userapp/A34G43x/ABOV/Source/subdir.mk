################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../../../../../../../Framework/CMSIS/Device/ABOV/Source/startup_a34xxxx.c \
../../../../../../../Framework/CMSIS/Device/ABOV/Source/system_a34xxxx.c 

OBJS += \
./ABOV/Source/startup_a34xxxx.o \
./ABOV/Source/system_a34xxxx.o 

C_DEPS += \
./ABOV/Source/startup_a34xxxx.d \
./ABOV/Source/system_a34xxxx.d 


# Each subdirectory must supply rules for building sources it contributes
ABOV/Source/startup_a34xxxx.o: ../../../../../../../Framework/CMSIS/Device/ABOV/Source/startup_a34xxxx.c ABOV/Source/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU Arm Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mlittle-endian -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -g3 -DEXTRN_SUBFAMILY_A34G43x -DEXTRN_ABOV_MODULE_CONFIG -I"../../../../../../../Framework/CMSIS/Core/Include" -I"../../../../../../../Framework/CMSIS/Device/ABOV/Include" -I"../../../../../../../ProductConfig/Config" -I"../../../../../../../Platform/HAL/Include" -I"../../../../../../../Platform/HAL/HPL/Include" -I"../../../../../../../Platform/Library/ABOV/Debug/Include" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

ABOV/Source/system_a34xxxx.o: ../../../../../../../Framework/CMSIS/Device/ABOV/Source/system_a34xxxx.c ABOV/Source/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU Arm Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mlittle-endian -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -g3 -DEXTRN_SUBFAMILY_A34G43x -DEXTRN_ABOV_MODULE_CONFIG -I"../../../../../../../Framework/CMSIS/Core/Include" -I"../../../../../../../Framework/CMSIS/Device/ABOV/Include" -I"../../../../../../../ProductConfig/Config" -I"../../../../../../../Platform/HAL/Include" -I"../../../../../../../Platform/HAL/HPL/Include" -I"../../../../../../../Platform/Library/ABOV/Debug/Include" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


