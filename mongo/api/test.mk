.SUFFIXES:.h .cpp .o

CC=$(CXX) -g -m64 -D__DEBUG__
COMPILE_FLAG=-fPIC
 

SRCS=test.cpp   
DEPINCLUDE=

OBJS=$(SRCS:.cpp=.o)
PROGRAM=test

CP=cp
RM=rm

DRIVER_PATH=/app/mongodb/3.2driver
ZHJS_INC_PATH=	-I. -I$(DRIVER_PATH)/include -I/usr/local/include
ZHJS_LIB_PATH=-L. -L$(DRIVER_PATH)/lib -L/usr/local/lib -L/usr/lib64
ZHJS_LIBS=-pthread\
	-lmongodbapi\
        -lmongoclient \
        -lboost_thread-mt -lboost_filesystem -lboost_program_options -lboost_system -lboost_regex \
        -lssl -lsasl2
$(PROGRAM):$(OBJS)
	$(CC) $(COMPILE_FLAG) -o $(PROGRAM)  $(OBJS) $(ZHJS_LIB_PATH) $(ZHJS_LIBS)


$(OBJS):$(SRCS) $(DEPINCLUDE)
	$(CC) $(COMPILE_FLAG) $(PTHREAD_FLAG) -c  $(MACRO) $(SRCS) $(ZHJS_INC_PATH) 

.PHONY:clean
clean:
	$(RM) $(OBJS) $(PROGRAM)
