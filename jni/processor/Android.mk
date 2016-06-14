LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)

LOCAL_MODULE    := imageprocessor
  
LOCAL_LDLIBS    += -llog

LOCAL_SRC_FILES := 	src/imageprocessor.cpp

LOCAL_STATIC_LIBRARIES := jpeg-turbo

include $(BUILD_SHARED_LIBRARY)