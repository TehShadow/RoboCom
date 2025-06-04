# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -pthread -Iinclude

# Sources
SRCS = src/UdpTransport.cpp
OBJS = $(SRCS:.cpp=.o)

# Targets
TARGETS = publisher subscriber

# Build rules
all: $(TARGETS)

publisher: main_pub.cpp $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ main_pub.cpp $(OBJS)

subscriber: main_sub.cpp $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ main_sub.cpp $(OBJS)

src/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGETS)