
DEBUG=0

DIRS := .  
SOURCE  := $(shell find $(DIRS) -name "*.cpp") 
PROG := ~/proxy_server

ifeq ($(DEBUG),1)
CXXFLAGS_EXTRA=-pg -g -O3
DEP_TAIL=.dd
OBJ_TAIL=.do
else
CXXFLAGS_EXTRA=-O3
DEP_TAIL=.d
OBJ_TAIL=.o
endif

OBJS    := $(SOURCE:.cpp=$(OBJ_TAIL))
DEPS    := $(SOURCE:.cpp=$(DEP_TAIL))
CXX = g++
CPPFLAGS = -I . 
CXXFLAGS = -Wall -Werror $(CXXFLAGS_EXTRA)
LDFLAGS = -ldl \
		-lpthread \
		-rdynamic \
		/usr/local/lib/libboost_thread.a \
		/usr/local/lib/libboost_regex.a \
		/usr/local/lib/libboost_system.a \
		/usr/local/lib/libboost_filesystem.a \
		/usr/local/lib/libboost_date_time.a \
		/usr/local/lib/libboost_program_options.a \
		-lrt

$(PROG):	$(OBJS)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS)  $(OBJS) -o $@ $(LDFLAGS)
	
%$(DEP_TAIL): %.cpp
	@set -e; rm -f $@; \
	$(CXX) -MM $(CPPFLAGS) $< -MT $(patsubst %.cpp,%.o,$<) > $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1$(OBJ_TAIL) $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

%$(OBJ_TAIL): %.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c -o $@ $<

ifneq ($(MAKECMDGOALS),clean)
include $(DEPS)
endif

.PHONY : clean
clean:
	rm -rf $(OBJS) $(DEPS) 


