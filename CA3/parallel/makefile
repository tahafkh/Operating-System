CC = g++
BUILD_DIR = build
CFLAGS = -std=c++11 -pthread

MAIN_EXECUTABLE_FILE = ImageFilters.out

MAIN_OBJECTS = \
	$(BUILD_DIR)/main.o \
	$(BUILD_DIR)/tools.o

MainSensitivityList = \
	main.cpp \
	tools.hpp 

ToolsSensitivityList = \
	tools.cpp \
	tools.hpp 

all: $(BUILD_DIR) $(MAIN_EXECUTABLE_FILE)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(MAIN_EXECUTABLE_FILE): $(MAIN_OBJECTS)
	$(CC) $(MAIN_OBJECTS) $(CFLAGS) -o $(MAIN_EXECUTABLE_FILE)

$(BUILD_DIR)/main.o: $(MainSensitivityList)
	$(CC) -c main.cpp $(CFLAGS) -o $(BUILD_DIR)/main.o

$(BUILD_DIR)/tools.o: $(ToolsSensitivityList)
	$(CC) -c tools.cpp $(CFLAGS) -o $(BUILD_DIR)/tools.o

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR) *.o *.out 