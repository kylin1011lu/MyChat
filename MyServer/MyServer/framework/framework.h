#ifndef __FRAMEWORK_H__
#define __FRAMEWORK_H__

#define _SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS 1

#if defined(__cplusplus)
#include <new>
#include <iostream>
#include <vector>
#include <list>
#include <set>
#include <string>
#include <map>
#include <hash_map>
#include <algorithm>
#include <typeinfo>
#include <functional>
#endif

#include <stdio.h>
#include <time.h>
#include <stdint.h>
#include <assert.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <signal.h>

#if defined(WIN32)
#include <winsock2.h>
#include <ws2tcpip.h>
#include <io.h>
#include <pthread.h>
typedef int ssize_t;
#define strcasecmp stricmp
#define __gnu_cxx std
#else
#define closesocket				close
typedef int						SOCKET;
#define ToUTF8(s)				s
#endif

#if defined(LINUX)
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/mman.h>
#include <linux/limits.h>
#include <pwd.h>
#include <execinfo.h>
#include <unistd.h>
#include <dirent.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#endif


//dep include
#include "mysql/mysql.h"
#include "mysql/errmsg.h"

#define USE_RAPIDXML	1

#if (USE_LIBXML2)
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#endif

#if (USE_RAPIDXML)
#include "rapidxml/rapidxml.hpp"
#endif

//external include
#include "google/protobuf/message_lite.h"

//framework include
#include "game_struct.h"
#include "sq_thread.h"
#include "define.h"
#include "getopt.h"
#include "sq_misc.h"
#include "sq_time.h"
#include "sq_timer.h"
#include "sq_logger.h"
#include "constructor.h"
#include "sq_object_pool.h"
#include "sq_network.h"
#include "sq_network_object.h"
#include "protocol.h"
#include "sq_socket.h"
#include "sq_listening.h"
#include "sq_epoll.h"
#include "sq_registry.h"
#include "sq_xml_parser.h"
#include "sq_msg_queue.h"
#include "sq_client.h"
#include "sq_mysql.h"


//src include
#include "common.pb.h"
#include "utility.h"




#endif
