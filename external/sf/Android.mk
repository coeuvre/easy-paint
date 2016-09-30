LOCAL_PATH := $(call my-dir)/src

include $(CLEAR_VARS)

LOCAL_MODULE := sf

LOCAL_C_INCLUDES := $(LOCAL_PATH)/../include

LOCAL_SRC_FILES :=	\
	array.c		\
	list.c		\
	log.c		\
	pool.c		\
	utils.c

LOCAL_CFLAGS := -DANDROID

include $(BUILD_STATIC_LIBRARY)
