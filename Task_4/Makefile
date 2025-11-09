CXX = g++
CXXFLAGS = -mavx -O3 -Wall -std=c++11
TARGET = main
SRC = main.cpp

all: run

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)
	@echo "Build successful! Run with: ./$(TARGET)"

run: $(TARGET)
	./$(TARGET)

clean: 
	rm -f $(TARGET)