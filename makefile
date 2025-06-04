# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -pthread

# Targets
TARGETS = publisher subscriber

# Sources
PUB_SRC = main_pub.cpp
SUB_SRC = main_sub.cpp

# Headers
HEADERS = Publisher.h Subscriber.h TopicPortMapper.h

# Build rules
all: $(TARGETS)

publisher: $(PUB_SRC) $(HEADERS)
	$(CXX) $(CXXFLAGS) -o $@ $(PUB_SRC)

subscriber: $(SUB_SRC) $(HEADERS)
	$(CXX) $(CXXFLAGS) -o $@ $(SUB_SRC)

# Clean
clean:
	rm -f $(TARGETS)
