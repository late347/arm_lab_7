################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: Lab7.3rgbColorUSB.axf

# Tool invocations
Lab7.3rgbColorUSB.axf: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU C++ Linker'
	arm-none-eabi-c++ -nostdlib -L"C:\Users\Lauri\Documents\MCUXpressoIDE_10.2.1_795\workspace\lab7PWM_Workspace\lpc_board_nxp_lpcxpresso_1549\Debug" -L"C:\Users\Lauri\Documents\MCUXpressoIDE_10.2.1_795\workspace\lab7PWM_Workspace\lpc_chip_15xx\Debug" -L"C:\Users\Lauri\Documents\MCUXpressoIDE_10.2.1_795\workspace\lab7PWM_Workspace\FreeRTOS\Debug" -Xlinker -Map="Lab7.3rgbColorUSB.map" -Xlinker --gc-sections -Xlinker -print-memory-usage -mcpu=cortex-m3 -mthumb -T "Lab7_3rgbColorUSB_Debug.ld" -o "Lab7.3rgbColorUSB.axf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '
	$(MAKE) --no-print-directory post-build

# Other Targets
clean:
	-$(RM) $(CC_DEPS)$(C++_DEPS)$(EXECUTABLES)$(C_UPPER_DEPS)$(CXX_DEPS)$(OBJS)$(CPP_DEPS)$(C_DEPS) Lab7.3rgbColorUSB.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size "Lab7.3rgbColorUSB.axf"; # arm-none-eabi-objcopy -v -O binary "Lab7.3rgbColorUSB.axf" "Lab7.3rgbColorUSB.bin" ; # checksum -p LPC1549 -d "Lab7.3rgbColorUSB.bin";
	-@echo ' '

.PHONY: all clean dependents post-build

-include ../makefile.targets
