# Copyright (C) 2009 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
OPENCV_LIB_TYPE:=STATIC
OPENCV_INSTALL_MODULES:=on 
include /home/helder/Documents/tese/adt/opencv-android/sdk/native/jni/OpenCV.mk

LOCAL_SRC_FILES 	:= FaceDetection.cpp
LOCAL_C_INCLUDES 	+= $(LOCAL_PATH)
LOCAL_LDLIBS     	+= -llog -ldl

LOCAL_MODULE := facedetection

include $(BUILD_SHARED_LIBRARY)
