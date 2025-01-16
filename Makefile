SOURCE_DIR = app lib
BIN_DIR = ./.bin
BUILD_DIR = ./.build

OS_NAME = $(shell uname -s)
OS_BITNESS = $(shell getconf LONG_BIT)
BIN_PATH = $(BIN_DIR)/$(OS_NAME)$(OS_BITNESS)

.PHONY: compile
compile: $(SOURCE_DIR)
	cmake -S. -B$(BUILD_DIR)
	cmake --build $(BUILD_DIR)

.PHONY: run
run: compile
	$(BIN_PATH)/Release/bin/$(APP)

.PHONY: test
test: compile
	ctest --test-dir ./$(BUILD_DIR)

clean:
	rm -rf ./$(BUILD_DIR) ./$(BIN_DIR)
