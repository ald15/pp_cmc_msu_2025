CXX = g++
CXXFLAGS = -std=c++11 -O3 -Wall -Wextra
LDFLAGS = -pthread

TARGET = count_positive
SOURCES = main.cpp

all: run_all

seq:
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET)_seq $(LDFLAGS)

auto:
	$(CXX) $(CXXFLAGS) -DPARALLEL $(SOURCES) -o $(TARGET)_auto $(LDFLAGS)

parallel2:
	$(CXX) $(CXXFLAGS) -DPARALLEL -DNTHREADS=2 $(SOURCES) -o $(TARGET)_par2 $(LDFLAGS)

all_versions: seq auto parallel2

run_seq: seq
	./$(TARGET)_seq

run_auto: auto
	./$(TARGET)_auto

run_all: all_versions
	./$(TARGET)_seq
	./$(TARGET)_auto
	./$(TARGET)_par2

clean:
	rm -f $(TARGET)_seq $(TARGET)_auto $(TARGET)_par2