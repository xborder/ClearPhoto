LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

OPENCV_CAMERA_MODULES:=on
OPENCV_INSTALL_MODULES:=on
#OPENCV_LIB_TYPE:=STATIC

include /home/helder/Documents/tese/adt/opencv-android/sdk/native/jni/OpenCV.mk

LOCAL_MODULE    := image_processing
LOCAL_SRC_FILES := image_processing.cpp HorizonDetection.cpp
LOCAL_LDLIBS +=  -llog -ldl

include $(BUILD_SHARED_LIBRARY)
