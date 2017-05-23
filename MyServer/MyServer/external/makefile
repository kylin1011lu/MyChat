LOCAL_OBJS_DIR = objs
LOCAL_DEPS_DIR = deps

LOCAL_TARGET	= libprotobuf-lite

LOCAL_DEPS = $(LOCAL_DEPS_DIR)/$(LOCAL_TARGET).d

LOCAL_CPPFLAGS=
LOCAL_CXXFLAGS = -I protobuf-lite \
				-std=gnu++11
	
LOCAL_SRC= protobuf-lite/google/protobuf/io/coded_stream.cc \
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
			protobuf-lite/google/protobuf/stubs/stringprintf.cc
			
OBJ_FILES:=$(patsubst %.cpp, $(LOCAL_OBJS_DIR)/%.o, $(notdir $(LOCAL_SRC)))
OBJ_FILES:=$(patsubst %.cc, $(LOCAL_OBJS_DIR)/%.o, $(OBJ_FILES))
OBJ_FILES:=$(patsubst %.c, $(LOCAL_OBJS_DIR)/%.o, $(OBJ_FILES))


all: remove_target $(LOCAL_TARGET)

$(LOCAL_TARGET): $(OBJ_FILES)
	@echo Linking $@ 
	@ar cr $(LOCAL_TARGET).a $(OBJ_FILES)
	@echo "  build protobuf-lite.a [ $@ ]"
	@echo

$(LOCAL_OBJS_DIR)/%.o:
	@echo protobuf-lite Compiling $<
	@$(CXX) -c $(LOCAL_CXXFLAGS) $(LOCAL_CPPFLAGS) -o $@ $<

remove_target:
	@rm -fr $(LOCAL_TARGET).a 

dep:
	@echo "Generating protobuf-lite dependencies..."
	@mkdir -p $(LOCAL_OBJS_DIR)
	@mkdir -p $(LOCAL_DEPS_DIR)
	@$(CXX) $(LOCAL_CXXFLAGS) $(LOCAL_CPPFLAGS) -MM $(LOCAL_SRC) | sed "/^[_a-zA-Z0-9.]*\.o:/s/^/\.\/$(LOCAL_OBJS_DIR)\//" > $(LOCAL_DEPS)

clean:
	@echo "protobuf-lite Cleaning up..."$(OBJ_FILES)
	@rm -fr  $(LOCAL_OBJS_DIR) $(LOCAL_DEPS_DIR) 

-include $(LOCAL_DEPS)
