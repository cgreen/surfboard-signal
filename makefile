CC=gcc
CXX=g++
RM=rm -f
CPPFLAGS=-I../http-client/ -I/usr/include/tidy/ -g
LDLIBS=-lboost_system -ltidy

SRCS=status.cpp ../http-client/HttpClient.cpp
OBJS=$(subst .cpp,.o,$(SRCS))

all:status

status: $(OBJS)
	$(CXX) $(CPPFLAGS) $^ -o $@ $(LDLIBS)

depend: .depend

.depend: $(SRCS)
	rm -f ./.depend
	$(CXX) $(CPPFLAGS) -MM $^>>./.depend;

clean:
	$(RM) $(OBJS)

dist-clean: clean
	$(RM) *~ .dependtool

include .depend

