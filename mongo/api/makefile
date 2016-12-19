.SUFFIXES:.h .cpp .o

CC=$(CXX) -g -m64 -D__DEBUG__ -DD_BUG 
COMPILE_FLAG=-fPIC
 

SRCS=mongodbapi.cpp    
DEPINCLUDE= mongodbapi.h

OBJS=$(SRCS:.cpp=.o)
LIB = lib$(OBJS:.o=.a)
#LIB=lib$(OBJS:.o=.so)

CP=cp
RM=rm

DRIVER_PATH=/zhjs/settle/
BOOST_PATH=/zhjs/settle/
ZHJS_INC_PATH=  -I$(DRIVER_PATH)/include\
		-I$(BOOST_PATH)/include\
		-I.  
ZHJS_LIB_PATH=-L$(DRIVER_PATH)/lib -L$(BOOST_PATH)/libso
ZHJS_LIBS=-pthread\
	 -lmongoclient\
	 -lboost_thread -lboost_filesystem -lboost_program_options -lboost_system -lboost_regex

AR=ar rc
LD=$(CC) -shared -o


$(LIB):$(OBJS)
	$(AR) $(LIB) $(OBJS) 
	$(CP) $(LIB) $(SETTLE_HOME)/lib 
	$(CP) mongodbapi.h $(SETTLE_HOME)/include

$(OBJS):$(SRCS) $(DEPINCLUDE)
	$(CC) $(COMPILE_FLAG) $(PTHREAD_FLAG) -c  $(MACRO) $(SRCS) $(ZHJS_INC_PATH) 

.PHONY:clean
clean:
	$(RM) $(OBJS) $(PROGRAM) $(LIB)

