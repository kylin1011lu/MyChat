OBJS_DIR = objs
DEPS_DIR = deps
DEPS = $(DEPS_DIR)/$(TARGET).d
TAGS = .tags
SUFFIX = $(findstring .a,$(TARGET))

BUILD_VERSION = 1.0.0 #$(shell svn info . | grep -E -i -w '修订版|Revision'|sed 's/：/ /g'|awk '{print $$2;}' )
BUILD_TIME = $(shell date '+%Y/%m/%d %H:%M:%S')
BUILD_AUTHOR = $(shell whoami)

AR = /usr/bin/ar
AR_FLAGS = cru

PLATFORM = $(shell uname -i)
ROOT_DIR = .

ifeq ($(PLATFORM),i386)
#error "DON'T IMPLEMENT i386 PLATFORM!!"
endif

ifeq ($(PLATFORM),x86_64)
CXXFLAGS += \
	-I. \
	-I/usr/include/libxml2 \
	-I$(ROOT_DIR) \
	-I$(ROOT_DIR)/framework \
	-I$(ROOT_DIR)/src \
	-I$(ROOT_DIR)/dep/linux/ \
	-I$(ROOT_DIR)/dep/linux/lua/ \
	-I$(ROOT_DIR)/dep/linux/libevent/ \
	-I$(ROOT_DIR)/dep/linux/jsoncpp/json/ \
	-I$(ROOT_DIR)/protocol/	\
	-I$(ROOT_DIR)/protobuf/	\
	-m64 \
	-D_REENTRANT 
BASELIB_PATH = /usr/lib64
ICONV = $(ROOT_DIR)/dep/linux/iconv/lib/libiconv.a
JSONCPP = $(ROOT_DIR)/dep/linux/jsoncpp/lib/libjsoncpp.a
LIBEVENT = $(ROOT_DIR)/dep/linux/libevent/lib/libevent.a
LUA = $(ROOT_DIR)/dep/linux/lua/lib/liblua.a
MYSQL = $(ROOT_DIR)/dep/linux/mysql/lib/libmysqlclient.a
LUACURL = $(ROOT_DIR)/dep/linux/luacurl/lib/luacurl.a
PROTOBUF-LITE=$(ROOT_DIR)/external/libprotobuf-lite.a
endif

LIB_DIRS = external/

LIBS += \
	-L$(BASELIB_PATH) \
	-lrt \
	-lz \
	-lm \
	-ldl \
	-lpthread \
	-lcurl

CPPFLAGS += \
	-g \
	-Wno-deprecated \
	-D_GNU_SOURCE \
	-D_LARGEFILE64_SOURCE \
	-D_FILE_OFFSET_BITS=64 \
	-D_DEBUG \
	-DDEBUG \
	-DLINUX \
	-DUNIX \
	-D_SQ64 \
	-pipe \
	-fno-rtti \
	-fno-strict-aliasing \
	-DBUILD_TIME="\"$(BUILD_TIME)\"" \
	-DBUILD_AUTHOR="\"$(BUILD_AUTHOR)\"" \
	-DBUILD_VERSION="\"$(BUILD_VERSION)\"" \
	 -frtti

ALL_OBJ_FILES:=$(patsubst %.cxx, $(OBJS_DIR)/%.o, $(notdir $(SOURCES)))
ALL_OBJ_FILES:=$(patsubst %.cpp, $(OBJS_DIR)/%.o, $(ALL_OBJ_FILES))
ALL_OBJ_FILES:=$(patsubst %.cc, $(OBJS_DIR)/%.o, $(ALL_OBJ_FILES))
ALL_OBJ_FILES:=$(patsubst %.c, $(OBJS_DIR)/%.o, $(ALL_OBJ_FILES))


all: remove_target $(TARGET)

$(TARGET): $(ALL_OBJ_FILES)
ifeq ($(SUFFIX),.a)
	@echo Linking library $@ 
	@$(AR) $(AR_FLAGS) $@ $(ALL_OBJ_FILES)
	@echo "  Built library [ $@ ]"
	@echo
else
	@echo Linking $@ 
	@$(CXX) -o $@ $(ALL_OBJ_FILES) $(LIBS)
	@echo "  Built executable [ $@ ]"
	@echo
	@mkdir -p $(INSTALL_DIR)
	@mv $(TARGET) $(INSTALL_DIR)
endif

$(OBJS_DIR)/%.o:
	@echo Compiling $<
	@$(CXX) -c $(CXXFLAGS) $(CPPFLAGS) -o $@ $<

clean:
	@echo "Cleaning up..."$(ALL_OBJ_FILES)
	@rm -fr $(TARGET) $(DEPS_DIR) $(OBJS_DIR) $(TAGS) $(INSTALL_DIR)/$(TARGET)

tag:
	@echo "Generating tags..."
	@ctags -f $(TAGS) -R $(ROOT_DIR)/shard/* $(ROOT_DIR)/dep/*

install:
	@echo "  Installing [ $(TARGET) ]"
	@echo
	@cp $(INSTALL_DIR)/$(TARGET) $(INSTALL_DIR)/$(TARGET)_bot
	@cp $(INSTALL_DIR)/$(TARGET) $(INSTALL_DIR)/$(TARGET)_u
	@cp $(INSTALL_DIR)/$(TARGET) $(INSTALL_DIR)/$(TARGET)_s

remove_target:
	@rm -fr $(TARGET) $(INSTALL_DIR)/$(TARGET)

	
dep:
	@echo "Generating dependencies..."
	@mkdir -p $(DEPS_DIR)
	@mkdir -p $(OBJS_DIR)
	@$(CXX) $(CXXFLAGS) $(CPPFLAGS) -MM $(SOURCES) | sed "/^[_a-zA-Z0-9.]*\.o:/s/^/\.\/$(OBJS_DIR)\//" > $(DEPS)

.PHONY: all clean tag dep install framework shard test

-include $(DEPS)
	
