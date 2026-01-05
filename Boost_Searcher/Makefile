HTTP_SERVER=http_serverRSER=parser
DUG=debug
HTTP_SERVER=http_server
cpp=g++

CXXFLAGS = -std=c++11 -I./ -Wall -g

.PHONY:all
all:$(PARSER) $(DUG) $(HTTP_SERVER)

$(PARSER):parser.cpp
	$(cpp) $(CXXFLAGS) -o $@ $^ -lboost_system -lboost_filesystem
$(DUG):debug.cpp
	$(cpp) $(CXXFLAGS) -o $@ $^ -ljsoncpp
$(HTTP_SERVER):http_server.cpp
	$(cpp) $(CXXFLAGS) -o $@ $^ -ljsoncpp -lpthread

.PHONY:clean
clean:
	rm -f $(DUG) $(PARSER) $(HTTP_SERVER)

