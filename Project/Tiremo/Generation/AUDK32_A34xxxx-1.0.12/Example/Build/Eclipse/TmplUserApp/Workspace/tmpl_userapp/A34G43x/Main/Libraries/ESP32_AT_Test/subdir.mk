################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../../../../../../Source/TmplUserApp/Libraries/ESP32_AT_Test/ESP32_AT_Test.c 

OBJS += \
./Main/Libraries/ESP32_AT_Test/ESP32_AT_Test.o 

C_DEPS += \
./Main/Libraries/ESP32_AT_Test/ESP32_AT_Test.d 


# Each subdirectory must supply rules for building sources it contributes
Main/Libraries/ESP32_AT_Test/ESP32_AT_Test.o: ../../../../../../Source/TmplUserApp/Libraries/ESP32_AT_Test/ESP32_AT_Test.c Main/Libraries/ESP32_AT_Test/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU Arm Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mlittle-endian -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -g3 -DEXTRN_SUBFAMILY_A34G43x -DEXTRN_ABOV_MODULE_CONFIG -I"../../../../../../../Framework/CMSIS/Core/Include" -I"../../../../../../../Framework/CMSIS/Device/ABOV/Include" -I"../../../../../../../ProductConfig/Config" -I"../../../../../../../Platform/HAL/Include" -I"../../../../../../../Platform/HAL/HPL/Include" -I"../../../../../../../Platform/Library/ABOV/Debug/Include" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


