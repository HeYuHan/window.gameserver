CC = g++
#yum install zlib-devel
#yum install zlib
#yum install openssl
#yum install openssl-devel

LFLAGS += -ldl -lpthread -lz
CFLAGS = -c -fPIC -std=c++11
RAKNET_SOURCE_DIR = ../../3rd/RakNet
COMMON_LIB = ../lib/lib*.a
#PROTO_MESSAGE_LIB = ../lib/libmessage.a
CURL_LIB = /usr/local/lib/libcurl.a
TARGET = ../bin/broadserver
SOURCES = $(wildcard *.cpp)
OBJS = $(patsubst %.cpp,objs/%.o,$(SOURCES)) $(COMMON_LIB) $(PROTO_MESSAGE_LIB) $(CURL_LIB)


ifneq ($(shell uname), Linux) 
	PF = MACOS
else
	LFLAGS += -lrt
endif


ifdef RELEASE
	CFLAGS += -Wall -Werror -g -O2
	LFLAGS += -Wall -Werror -g -O2
else
	CFLAGS += -g -D_DEBUG 
	LFLAGS += -g -rdynamic
endif
CFLAGS += -I.
CFLAGS += -I./../common
CFLAGS += -I/usr/local/include
CFLAGS += -I./../proto/out
CFLAGS += -I./$(RAKNET_SOURCE_DIR)/Source


#DEPS = $(patsubst %.cpp,objs/%.d,$(SOURCES))


default : $(TARGET)

objs/%.o: %.cpp
	$(CC) $(CFLAGS) -o $@ $<

#-include $(DEPS)

clean :
	rm -f $(TARGET) objs/*.o

$(TARGET) : $(TEMP) $(OBJS)
	$(CC) $(LFLAGS) -o $@ $(OBJS) 
	
