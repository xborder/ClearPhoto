################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../jni/jni_part.cpp 

OBJS += \
./jni/jni_part.o 

CPP_DEPS += \
./jni/jni_part.d 


# Each subdirectory must supply rules for building sources it contributes
jni/%.o: ../jni/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/home/helder/Documents/tese/adt/ndk/sources/cxx-stl/gnu-libstdc++/4.6/include -I/home/helder/Documents/tese/adt/ndk/sources/cxx-stl/gnu-libstdc++/4.6/libs/armeabi-v7a/include -I/home/helder/Documents/tese/adt/opencv-android/sdk/native/jni/include -I/home/helder/Documents/tese/adt/ndk/platforms/android-18/arch-arm/usr/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


