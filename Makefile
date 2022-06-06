CC = g++

CC_FLAGS = -g -Wall -Wextra -std=c++20
LFLAGS = -I./

TARGET = mipsimulator
TARGER_DIR = build

# .cpp files
CPP_SOURCE=$(wildcard *.cpp)

# .hpp files
HPP_SOURCE=$(wildcard *.hpp)

OBJS=$(addprefix $(TARGER_DIR)/, $(CPP_SOURCE:.cpp=.o))

all: $(TARGET)

release: CC_FLAGS += -O3
release: all

$(TARGET): $(OBJS)
	$(CC) -o $(TARGER_DIR)/$@ $^ $(CC_FLAGS)

$(TARGER_DIR)/%.o: %.cpp %.hpp
	$(CC) $(LFLAGS) -c -o $@ $< $(CC_FLAGS)

$(TARGER_DIR)/main.o: main.cpp 
	$(CC) $(LFLAGS) -c -o $@ $< $(CC_FLAGS)

.PHONY: clean

clean:
	$(RM) $(TARGER_DIR)/*.o *.o $(TARGER_DIR)/$(TARGET)

$(shell mkdir -p $(TARGER_DIR))
