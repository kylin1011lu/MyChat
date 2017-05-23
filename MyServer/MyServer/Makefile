TARGET	= server

CC=g++
CXXFLAGS=
CPPFLAGS=
LIBS =  $(PROTOBUF-LITE)
# LIBS = $(ICONV) $(MYSQL) $(LIBEVENT) $(LUA) $(JSONCPP) $(LUACURL)

INSTALL_DIR = bin

SOURCES +=	\
			framework/sq_client.cpp \
			framework/sq_epoll.cpp \
			framework/sq_listening.cpp \
			framework/sq_logger.cpp \
			framework/sq_misc.cpp \
			framework/sq_msg_queue.cpp \
			framework/sq_network.cpp \
			framework/sq_registry.cpp \
			framework/sq_socket.cpp \
			framework/sq_thread.cpp \
			framework/sq_time.cpp \
			framework/sq_timer.cpp \
			framework/sq_xml_parser.cpp \
			framework/protocol.cpp \
			protobuf/common.pb.cc \
			src/server.cpp \
			src/sq_handler.cpp \
			src/sq_handler_user.cpp \
			src/sq_user_client.cpp 

include generic.mk
