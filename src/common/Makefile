CC = g++
C = gcc
AR = ar

EVENT_SOURCE_DIR = ../../3rd/libevent
LOG4CPP_SOURCE_DIR = ../../3rd/log4cpp
RAKNET_SOURCE_DIR = ../../3rd/RakNet
TO_LUA_SOURCE_DIR = ../../3rd/tolua++

LFLAGS = 

CFLAGS = -c -fPIC
CFLAGS += -I.
CFLAGS += -I./$(RAKNET_SOURCE_DIR)/Source
CFLAGS += -I/usr/local/include
CFLAGS += -I/usr/local/include/hiredis



CPPFLAGS = -std=c++11

EVENT_LIB = /usr/local/lib/libevent.a 
EVENT_LIB_CORE = /usr/local/lib/libevent_core.a 
EVENT_LIB_EXT = /usr/local/lib/libevent_extra.a 
EVENT_LIB_PTHREAD = /usr/local/lib/libevent_pthreads.a
LOG_LIB = /usr/local/lib/liblog4cpp.a
RAKNET_LIB = ../../3rd/RakNet/lib/libraknet.a
ZLOG_LIB =  /usr/local/lib/libzlog.a
HIREDIS_LIB = /usr/local/lib/libhiredis.a



EVENT_LIB_PREFIX = --prefix=/usr/local
LOG_LIB_PREFIX = --prefix=/usr/local

TARGET = ../lib/libcommon.a
SOURCES = $(wildcard *.cpp) $(wildcard json/*.cpp) $(wildcard LuaBridge/*.cpp)
OBJS += $(patsubst %.cpp,objs/%.o,$(SOURCES)) 

LUA_SOURCES = $(wildcard lua/*.c)
LUA = $(patsubst %.c,objs/%.o,$(LUA_SOURCES))

CJSON_SOURCES = $(wildcard cjson/*.c)
CJSON = $(patsubst %.c,objs/%.o,$(CJSON_SOURCES))

PF = LINUX
DEPS_LIB = $(LOG_LIB) $(EVENT_LIB) $(RAKNET_LIB)


ifneq ($(shell uname), Linux) 
	PF = MACOS
	EVENT_LIB_PREFIX += CFLAGS="-I/usr/local/Cellar/openssl/1.0.2h_1/include" LDFLAGS="-I/usr/local/Cellar/openssl/1.0.2h_1/lib"
endif

CFLAGS += -D$(PF)

ifdef RELEASE
	CFLAGS += -Wall -Werror -g -O2 
else
	CFLAGS += -Wall -Werror -g -D_DEBUG
endif





default : $(TARGET)

#libevent
$(EVENT_LIB) : $(EVENT_SOURCE_DIR)/Makefile
	$(MAKE) -C $(EVENT_SOURCE_DIR)
	$(MAKE) -C $(EVENT_SOURCE_DIR) install

$(EVENT_SOURCE_DIR)/Makefile : $(EVENT_SOURCE_DIR)/configure
	$(EVENT_SOURCE_DIR)/configure --prefix=/usr/local

#log4cpp
$(LOG_LIB) : $(LOG4CPP_SOURCE_DIR)/Makefile
	$(MAKE) -C $(LOG4CPP_SOURCE_DIR)
	$(MAKE) -C $(LOG4CPP_SOURCE_DIR) install

$(LOG4CPP_SOURCE_DIR)/Makefile : $(LOG4CPP_SOURCE_DIR)/configure
	$(LOG4CPP_SOURCE_DIR)/configure --prefix=/usr/local

#raknet
$(RAKNET_LIB) : $(RAKNET_SOURCE_DIR)/Makefile
	$(MAKE) -C $(RAKNET_SOURCE_DIR)


objs/lua/%.o: lua/%.c
	$(C) $(CFLAGS) -DLUA_COMPAT_ALL -o $@ $<

objs/cjson/%.o: cjson/%.c
	$(C) $(CFLAGS) -DLUA_COMPAT_ALL -o $@ $<
	
objs/%.o: %.cpp
	$(CC) $(CFLAGS) $(CPPFLAGS)  -o $@ $<

clean :
	rm -f $(TARGET) objs/*.o objs/json/*.o *.o objs/lua/*.o objs/LuaBridge/*.o objs/lua/*.o objs/cjson/*.o
	

deps: $(DEPS_LIB)
	
clean_deps:
	$(MAKE) -C $(RAKNET_SOURCE_DIR) clean
	$(MAKE) -C $(EVENT_SOURCE_DIR) clean
	$(MAKE) -C $(LOG4CPP_SOURCE_DIR) clean
	$(MAKE) -C $(EVENT_SOURCE_DIR) uninstall
	$(MAKE) -C $(LOG4CPP_SOURCE_DIR) uninstall




$(TARGET) : $(TEMP) $(OBJS) $(LUA) $(CJSON)
	cp -f $(RAKNET_LIB) ../lib/
	cp -f $(EVENT_LIB) ../lib/
	cp -f $(EVENT_LIB_CORE) ../lib/
	cp -f $(EVENT_LIB_EXT) ../lib/
	cp -f $(EVENT_LIB_PTHREAD) ../lib/
	cp -f $(LOG_LIB) ../lib/
	cp -f $(ZLOG_LIB) ../lib/
	cp -f $(HIREDIS_LIB) ../lib/
	$(AR) $(LFLAGS) rcs $@ $(OBJS) $(LUA) $(CJSON)
	