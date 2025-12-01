CXX = mpic++
CXXFLAGS = -O3 -Wall -std=c++17
TARGET = main
SRC = main.cpp

all: run_np4

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

run_np2: $(TARGET)
	mpirun -np $(NP_2) ./$(TARGET)

run_np3: $(TARGET)
	mpirun -np 3 ./$(TARGET)

run_np4: $(TARGET)
	mpirun -np 4 ./$(TARGET)

run_np8: $(TARGET)
	mpirun -np 8 ./$(TARGET)

run_np12: $(TARGET)
	mpirun -np 12 ./$(TARGET)

clean:
	rm -f $(TARGET) *.o