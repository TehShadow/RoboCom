# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pthread -O2 -Iinclude -Iinclude/messages

# Common sources
COMMON_SRCS = \
    src/Discovery.cpp \
    src/PeerToPeerTcpTransport.cpp \
    src/TcpTransport.cpp \
    src/UdpTransport.cpp

# Example sources
EXAMPLES = \
    examples/example_service_client.cpp \
    examples/example_service_server.cpp \
    examples/example_pub_sub.cpp \
    examples/example_udp_pub_sub.cpp \
    examples/example_tcp_pub_sub.cpp

# Example targets
EXAMPLE_BINARIES = \
    build/example_service_client \
    build/example_service_server \
    build/example_pub_sub \
    build/example_udp_pub_sub \
    build/example_tcp_pub_sub

# Default target: build all
all: $(EXAMPLE_BINARIES)

# Build rules
build/example_service_client: examples/example_service_client.cpp $(COMMON_SRCS)
	@mkdir -p build
	$(CXX) $(CXXFLAGS) -o $@ $^ 

build/example_service_server: examples/example_service_server.cpp $(COMMON_SRCS)
	@mkdir -p build
	$(CXX) $(CXXFLAGS) -o $@ $^

build/example_pub_sub: examples/example_pub_sub.cpp $(COMMON_SRCS)
	@mkdir -p build
	$(CXX) $(CXXFLAGS) -o $@ $^

build/example_udp_pub_sub: examples/example_udp_pub_sub.cpp $(COMMON_SRCS)
	@mkdir -p build
	$(CXX) $(CXXFLAGS) -o $@ $^

build/example_tcp_pub_sub: examples/example_tcp_pub_sub.cpp $(COMMON_SRCS)
	@mkdir -p build
	$(CXX) $(CXXFLAGS) -o $@ $^

# Clean build
clean:
	rm -rf build

# Run targets
run_service_client:
	./build/example_service_client

run_service_server:
	./build/example_service_server

run_pub_sub:
	./build/example_pub_sub

run_udp_pub_sub:
	./build/example_udp_pub_sub

run_tcp_pub_sub:
	./build/example_tcp_pub_sub