################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
C:/Users/sinan/Desktop/esp32c3_tiremoconnect/Tiremo/Generation/AUDK32_A34xxxx-1.0.12/Example/Source/TmplUserApp/Libraries/EC200_4G/EMPA_Ec200.c \
C:/Users/sinan/Desktop/esp32c3_tiremoconnect/Tiremo/Generation/AUDK32_A34xxxx-1.0.12/Example/Source/TmplUserApp/Libraries/EC200_4G/ec200_core.c \
C:/Users/sinan/Desktop/esp32c3_tiremoconnect/Tiremo/Generation/AUDK32_A34xxxx-1.0.12/Example/Source/TmplUserApp/Libraries/EC200_4G/ec200_port_abov.c 

OBJS += \
./Main/Libraries/EC200_4G/EMPA_Ec200.o \
./Main/Libraries/EC200_4G/ec200_core.o \
./Main/Libraries/EC200_4G/ec200_port_abov.o 

C_DEPS += \
./Main/Libraries/EC200_4G/EMPA_Ec200.d \
./Main/Libraries/EC200_4G/ec200_core.d \
./Main/Libraries/EC200_4G/ec200_port_abov.d 


# Each subdirectory must supply rules for building sources it contributes
Main/Libraries/EC200_4G/EMPA_Ec200.o: C:/Users/sinan/Desktop/esp32c3_tiremoconnect/Tiremo/Generation/AUDK32_A34xxxx-1.0.12/Example/Source/TmplUserApp/Libraries/EC200_4G/EMPA_Ec200.c Main/Libraries/EC200_4G/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU Arm Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mlittle-endian -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -g3 -DEXTRN_SUBFAMILY_A34G43x -DEXTRN_ABOV_MODULE_CONFIG -I"C:/Users/sinan/Desktop/esp32c3_tiremoconnect/Tiremo/Generation/AUDK32_A34xxxx-1.0.12/Example/Build/Eclipse/TmplUserApp/Workspace/tmpl_userapp/../../../../../../Framework/CMSIS/Core/Include" -I"C:/Users/sinan/Desktop/esp32c3_tiremoconnect/Tiremo/Generation/AUDK32_A34xxxx-1.0.12/Example/Build/Eclipse/TmplUserApp/Workspace/tmpl_userapp/../../../../../../Framework/CMSIS/Device/ABOV/Include" -I"C:/Users/sinan/Desktop/esp32c3_tiremoconnect/Tiremo/Generation/AUDK32_A34xxxx-1.0.12/Example/Build/Eclipse/TmplUserApp/Workspace/tmpl_userapp/../../../../../../ProductConfig/Config" -I"C:/Users/sinan/Desktop/esp32c3_tiremoconnect/Tiremo/Generation/AUDK32_A34xxxx-1.0.12/Example/Build/Eclipse/TmplUserApp/Workspace/tmpl_userapp/../../../../../../Platform/HAL/Include" -I"C:/Users/sinan/Desktop/esp32c3_tiremoconnect/Tiremo/Generation/AUDK32_A34xxxx-1.0.12/Example/Build/Eclipse/TmplUserApp/Workspace/tmpl_userapp/../../../../../../Platform/HAL/HPL/Include" -I"C:/Users/sinan/Desktop/esp32c3_tiremoconnect/Tiremo/Generation/AUDK32_A34xxxx-1.0.12/Example/Build/Eclipse/TmplUserApp/Workspace/tmpl_userapp/../../../../../../Platform/Library/ABOV/Debug/Include" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Main/Libraries/EC200_4G/ec200_core.o: C:/Users/sinan/Desktop/esp32c3_tiremoconnect/Tiremo/Generation/AUDK32_A34xxxx-1.0.12/Example/Source/TmplUserApp/Libraries/EC200_4G/ec200_core.c Main/Libraries/EC200_4G/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU Arm Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mlittle-endian -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -g3 -DEXTRN_SUBFAMILY_A34G43x -DEXTRN_ABOV_MODULE_CONFIG -I"C:/Users/sinan/Desktop/esp32c3_tiremoconnect/Tiremo/Generation/AUDK32_A34xxxx-1.0.12/Example/Build/Eclipse/TmplUserApp/Workspace/tmpl_userapp/../../../../../../Framework/CMSIS/Core/Include" -I"C:/Users/sinan/Desktop/esp32c3_tiremoconnect/Tiremo/Generation/AUDK32_A34xxxx-1.0.12/Example/Build/Eclipse/TmplUserApp/Workspace/tmpl_userapp/../../../../../../Framework/CMSIS/Device/ABOV/Include" -I"C:/Users/sinan/Desktop/esp32c3_tiremoconnect/Tiremo/Generation/AUDK32_A34xxxx-1.0.12/Example/Build/Eclipse/TmplUserApp/Workspace/tmpl_userapp/../../../../../../ProductConfig/Config" -I"C:/Users/sinan/Desktop/esp32c3_tiremoconnect/Tiremo/Generation/AUDK32_A34xxxx-1.0.12/Example/Build/Eclipse/TmplUserApp/Workspace/tmpl_userapp/../../../../../../Platform/HAL/Include" -I"C:/Users/sinan/Desktop/esp32c3_tiremoconnect/Tiremo/Generation/AUDK32_A34xxxx-1.0.12/Example/Build/Eclipse/TmplUserApp/Workspace/tmpl_userapp/../../../../../../Platform/HAL/HPL/Include" -I"C:/Users/sinan/Desktop/esp32c3_tiremoconnect/Tiremo/Generation/AUDK32_A34xxxx-1.0.12/Example/Build/Eclipse/TmplUserApp/Workspace/tmpl_userapp/../../../../../../Platform/Library/ABOV/Debug/Include" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

Main/Libraries/EC200_4G/ec200_port_abov.o: C:/Users/sinan/Desktop/esp32c3_tiremoconnect/Tiremo/Generation/AUDK32_A34xxxx-1.0.12/Example/Source/TmplUserApp/Libraries/EC200_4G/ec200_port_abov.c Main/Libraries/EC200_4G/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: GNU Arm Cross C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mlittle-endian -Og -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -g3 -DEXTRN_SUBFAMILY_A34G43x -DEXTRN_ABOV_MODULE_CONFIG -I"C:/Users/sinan/Desktop/esp32c3_tiremoconnect/Tiremo/Generation/AUDK32_A34xxxx-1.0.12/Example/Build/Eclipse/TmplUserApp/Workspace/tmpl_userapp/../../../../../../Framework/CMSIS/Core/Include" -I"C:/Users/sinan/Desktop/esp32c3_tiremoconnect/Tiremo/Generation/AUDK32_A34xxxx-1.0.12/Example/Build/Eclipse/TmplUserApp/Workspace/tmpl_userapp/../../../../../../Framework/CMSIS/Device/ABOV/Include" -I"C:/Users/sinan/Desktop/esp32c3_tiremoconnect/Tiremo/Generation/AUDK32_A34xxxx-1.0.12/Example/Build/Eclipse/TmplUserApp/Workspace/tmpl_userapp/../../../../../../ProductConfig/Config" -I"C:/Users/sinan/Desktop/esp32c3_tiremoconnect/Tiremo/Generation/AUDK32_A34xxxx-1.0.12/Example/Build/Eclipse/TmplUserApp/Workspace/tmpl_userapp/../../../../../../Platform/HAL/Include" -I"C:/Users/sinan/Desktop/esp32c3_tiremoconnect/Tiremo/Generation/AUDK32_A34xxxx-1.0.12/Example/Build/Eclipse/TmplUserApp/Workspace/tmpl_userapp/../../../../../../Platform/HAL/HPL/Include" -I"C:/Users/sinan/Desktop/esp32c3_tiremoconnect/Tiremo/Generation/AUDK32_A34xxxx-1.0.12/Example/Build/Eclipse/TmplUserApp/Workspace/tmpl_userapp/../../../../../../Platform/Library/ABOV/Debug/Include" -std=gnu11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


