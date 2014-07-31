LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

OPENCV_CAMERA_MODULES:=on
OPENCV_INSTALL_MODULES:=on
#OPENCV_LIB_TYPE:=STATIC

include /home/helder/Documents/tese/adt/opencv-android/sdk/native/jni/OpenCV.mk

define all-cpp-files-under
$(patsubst ./%, %, \
  $(shell cd $(LOCAL_PATH) ; \
          find $(1) -name "*.cpp" -and -not -name ".*") \
 )
endef
LOCAL_CPPFLAGS += -std=c++11
LOCAL_SRC_FILES := $(call all-cpp-files-under, .)
LOCAL_MODULE    := image_processing
#LOCAL_SRC_FILES := image_processing.cpp HorizonDetection.cpp MainLinesDetection.cpp ObjectSegmentation.cpp Segmentation/segment-image.cpp BackgroundSimplicity.cpp HueCount.cpp
LOCAL_LDLIBS +=  -llog -ldl

include $(BUILD_SHARED_LIBRARY)
