CC = g++
BUILD_DIR = build
INCLUDE_DIR = includes
CFLAGS = -std=c++17 -lstdc++fs

MAIN_EXECUTABLE_FILE = main.out
MAP_EXECUTABLE_FILE = map.out
REDUCE_EXECUTABLE_FILE = reduce.out

MAIN_OBJECTS = \
	$(BUILD_DIR)/Main.o \
	$(BUILD_DIR)/helper.o

MAP_OBJECTS = \
	$(BUILD_DIR)/Map.o \
	$(BUILD_DIR)/helper.o

REDUCE_OBJECTS = \
	$(BUILD_DIR)/Reduce.o \
	$(BUILD_DIR)/helper.o


MainSensitivityList = \
	MainProcess.cpp \
	$(INCLUDE_DIR)/consts.h \
	$(INCLUDE_DIR)/helper.h

ReduceSensitivityList = \
	ReduceProcess.cpp \
	$(INCLUDE_DIR)/consts.h \
    $(INCLUDE_DIR)/helper.h

MapSensitivityList = \
	MapProcess.cpp \
	$(INCLUDE_DIR)/consts.h \
    $(INCLUDE_DIR)/helper.h

HelperSensitivityList = \
	helper.cpp \
	$(INCLUDE_DIR)/helper.h

all: $(BUILD_DIR) $(MAIN_EXECUTABLE_FILE) $(MAP_EXECUTABLE_FILE) $(REDUCE_EXECUTABLE_FILE)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(MAIN_EXECUTABLE_FILE): $(MAIN_OBJECTS)
	$(CC) $(MAIN_OBJECTS) $(CFLAGS) -o $(MAIN_EXECUTABLE_FILE)

$(MAP_EXECUTABLE_FILE): $(MAP_OBJECTS)
	$(CC) $(MAP_OBJECTS) $(CFLAGS) -o $(MAP_EXECUTABLE_FILE)

$(REDUCE_EXECUTABLE_FILE): $(REDUCE_OBJECTS)
	$(CC) $(REDUCE_OBJECTS) $(CFLAGS) -o $(REDUCE_EXECUTABLE_FILE)

$(BUILD_DIR)/Main.o: $(MainSensitivityList)
	$(CC) -c MainProcess.cpp $(CFLAGS) -o $(BUILD_DIR)/Main.o

$(BUILD_DIR)/Reduce.o: $(ReduceSensitivityList)
	$(CC) -c ReduceProcess.cpp $(CFLAGS) -o $(BUILD_DIR)/Reduce.o

$(BUILD_DIR)/Map.o: $(MapSensitivityList)
	$(CC) -c MapProcess.cpp $(CFLAGS) -o $(BUILD_DIR)/Map.o

$(BUILD_DIR)/helper.o: $(HelperSensitivityList)
	$(CC) -c helper.cpp $(CFLAGS) -o $(BUILD_DIR)/helper.o


.PHONY: clean
clean:
	rm -rf $(BUILD_DIR) *.o *.out \
