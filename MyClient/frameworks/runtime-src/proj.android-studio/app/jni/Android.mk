LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := cocos2dlua_shared

LOCAL_MODULE_FILENAME := libcocos2dlua

LOCAL_SRC_FILES := \
protobuf-lite/google/protobuf/io/coded_stream.cc \
protobuf-lite/google/protobuf/stubs/common.cc \
protobuf-lite/google/protobuf/extension_set.cc \
protobuf-lite/google/protobuf/generated_message_util.cc \
protobuf-lite/google/protobuf/message_lite.cc \
protobuf-lite/google/protobuf/stubs/once.cc \
protobuf-lite/google/protobuf/stubs/atomicops_internals_x86_gcc.cc \
protobuf-lite/google/protobuf/repeated_field.cc \
protobuf-lite/google/protobuf/wire_format_lite.cc \
protobuf-lite/google/protobuf/io/zero_copy_stream.cc \
protobuf-lite/google/protobuf/io/zero_copy_stream_impl_lite.cc \
protobuf-lite/google/protobuf/stubs/stringprintf.cc \
../../../Classes/AppDelegate.cpp \
../../../Classes/Common/Common.cpp \
../../../Classes/Common/MyLuaRegister.cpp \
../../../Classes/lpeg/lpcap.c \
../../../Classes/lpeg/lpcode.c \
../../../Classes/lpeg/lpprint.c \
../../../Classes/lpeg/lptree.c \
../../../Classes/lpeg/lpvm.c \
../../../Classes/Net/NetManager.cpp \
../../../Classes/Net/Network.cpp \
../../../Classes/Net/TCPClient.cpp \
../../../../external/src/alloc.c \
../../../../external/src/array.c \
../../../../external/src/bootstrap.c \
../../../../external/src/context.c \
../../../../external/src/decode.c \
../../../../external/src/map.c \
../../../../external/src/pattern.c \
../../../../external/src/pbc-lua.c \
../../../../external/src/proto.c \
../../../../external/src/register.c \
../../../../external/src/rmessage.c \
../../../../external/src/stringpool.c \
../../../../external/src/varint.c \
../../../../external/src/wmessage.c \
hellolua/main.cpp

LOCAL_C_INCLUDES := $(LOCAL_PATH)/protobuf-lite \
					$(LOCAL_PATH)/../../../Classes \
					$(LOCAL_PATH)/../../../../external \
					$(LOCAL_PATH)/../../../../cocos2d-x/tools/simulator/libsimulator/lib/protobuf-lite

# _COCOS_HEADER_ANDROID_BEGIN
# _COCOS_HEADER_ANDROID_END

LOCAL_STATIC_LIBRARIES := cocos2d_lua_static

# _COCOS_LIB_ANDROID_BEGIN
# _COCOS_LIB_ANDROID_END

include $(BUILD_SHARED_LIBRARY)

$(call import-module,scripting/lua-bindings/proj.android)

# _COCOS_LIB_IMPORT_ANDROID_BEGIN
# _COCOS_LIB_IMPORT_ANDROID_END
