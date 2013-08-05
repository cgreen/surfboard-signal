CC=gcc
CXX=g++
RM=rm -f
CPPFLAGS=-I../http-client/
LDFLAGS=-lboost_system

SRCS=status.cpp ../http-client/HttpClient.cpp
OBJS=$(subst .cpp,.o,$(SRCS))

all:status

status: $(OBJS)
	$(CXX) $(CPPFLAGS) $(OBJS) -o $@ $(LDFLAGS)

depend: .depend

.depend: $(SRCS)
	rm -f ./.depend
	$(CXX) $(CPPFLAGS) -MM $^>>./.depend;

clean:
	$(RM) $(OBJS)

dist-clean: clean
	$(RM) *~ .dependtool

include .depend

