# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -pthread -Iinclude

# Sources
SRCS = src/UdpTransport.cpp src/TcpTransport.cpp
OBJS = $(SRCS:.cpp=.o)

# Targets
TARGETS = publisher subscriber tcp_pub tcp_sub tcp_sub_client

# Build rules
all: $(TARGETS)

publisher: main_pub.cpp $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ main_pub.cpp src/UdpTransport.o

subscriber: main_sub.cpp $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ main_sub.cpp src/UdpTransport.o

tcp_pub: main_pub_tcp.cpp $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ main_pub_tcp.cpp src/TcpTransport.o

tcp_sub: main_sub_tcp.cpp $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ main_sub_tcp.cpp src/TcpTransport.o

tcp_sub_client: main_sub_tcp_client.cpp $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ main_sub_tcp_client.cpp src/TcpTransport.o

src/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGETS)
