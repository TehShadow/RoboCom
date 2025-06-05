# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -pthread -O2 -Iinclude -Iinclude/messages

# Common sources
COMMON_SRCS = \
    src/Discovery.cpp \
    src/PeerToPeerTcpTransport.cpp \
    src/TcpTransport.cpp \
    src/UdpTransport.cpp

# TCP Publisher app
PUB_SRC = src/pub_main.cpp
PUB_TARGET = build/pub_node

# TCP Subscriber app
SUB_SRC = src/sub_main.cpp
SUB_TARGET = build/sub_node

# UDP Publisher app
PUB_UDP_SRC = src/pub_udp_main.cpp
PUB_UDP_TARGET = build/udp_pub_node

# UDP Subscriber app
SUB_UDP_SRC = src/sub_udp_main.cpp
SUB_UDP_TARGET = build/udp_sub_node

# Service Server app
SERVICE_SERVER_SRC = src/service_server_main.cpp
SERVICE_SERVER_TARGET = build/service_server_node

# Service Client app
SERVICE_CLIENT_SRC = src/service_client_main.cpp
SERVICE_CLIENT_TARGET = build/service_client_node

# Default: build all
all: $(PUB_TARGET) $(SUB_TARGET) $(PUB_UDP_TARGET) $(SUB_UDP_TARGET) $(SERVICE_SERVER_TARGET) $(SERVICE_CLIENT_TARGET)

# TCP Publisher build
$(PUB_TARGET): $(PUB_SRC) $(COMMON_SRCS)
	@mkdir -p build
	$(CXX) $(CXXFLAGS) -o $(PUB_TARGET) $(PUB_SRC) $(COMMON_SRCS)

# TCP Subscriber build
$(SUB_TARGET): $(SUB_SRC) $(COMMON_SRCS)
	@mkdir -p build
	$(CXX) $(CXXFLAGS) -o $(SUB_TARGET) $(SUB_SRC) $(COMMON_SRCS)

# UDP Publisher build
$(PUB_UDP_TARGET): $(PUB_UDP_SRC) $(COMMON_SRCS)
	@mkdir -p build
	$(CXX) $(CXXFLAGS) -o $(PUB_UDP_TARGET) $(PUB_UDP_SRC) $(COMMON_SRCS)

# UDP Subscriber build
$(SUB_UDP_TARGET): $(SUB_UDP_SRC) $(COMMON_SRCS)
	@mkdir -p build
	$(CXX) $(CXXFLAGS) -o $(SUB_UDP_TARGET) $(SUB_UDP_SRC) $(COMMON_SRCS)

# Service Server build
$(SERVICE_SERVER_TARGET): $(SERVICE_SERVER_SRC) $(COMMON_SRCS)
	@mkdir -p build
	$(CXX) $(CXXFLAGS) -o $(SERVICE_SERVER_TARGET) $(SERVICE_SERVER_SRC) $(COMMON_SRCS)

# Service Client build
$(SERVICE_CLIENT_TARGET): $(SERVICE_CLIENT_SRC) $(COMMON_SRCS)
	@mkdir -p build
	$(CXX) $(CXXFLAGS) -o $(SERVICE_CLIENT_TARGET) $(SERVICE_CLIENT_SRC) $(COMMON_SRCS)

# Clean build
clean:
	rm -rf build

# Run TCP Pub/Sub
run_pub:
	./build/pub_node

run_sub:
	./build/sub_node

# Run UDP Pub/Sub
run_udp_pub:
	./build/udp_pub_node

run_udp_sub:
	./build/udp_sub_node

# Run Service Server
run_service_server:
	./build/service_server_node

# Run Service Client
run_service_client:
	./build/service_client_node
