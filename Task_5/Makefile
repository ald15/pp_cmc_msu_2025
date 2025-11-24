CXX = g++
CXXFLAGS = -O3 -Wall -std=c++17 -fopenmp
TARGET = main
SRC = main.cpp

all: run_parallel_4

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

run_parallel_4: $(TARGET)
	./$(TARGET) --threads 4

run_parallel_2: $(TARGET)
	./$(TARGET) --threads 2

run_parallel_8: $(TARGET)
	./$(TARGET) --threads 8

run_parallel_16: $(TARGET)
	./$(TARGET) --threads 16

run_serial: $(TARGET)
	./$(TARGET) --serial

clean:
	rm -f $(TARGET)