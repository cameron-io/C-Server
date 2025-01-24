#!make
include .env

OS_NAME = $(shell uname -s)
OS_BITNESS = $(shell getconf LONG_BIT)
BIN_PATH = $(BIN_DIR)/$(OS_NAME)$(OS_BITNESS)/bin

.PHONY: compile
compile: $(SOURCE_DIRS)
	cmake -S. -B./$(BUILD_DIR)
	cmake --build ./$(BUILD_DIR)

.PHONY: run
run: compile
	./$(BIN_PATH)/HttpServer $(ARGS)

.PHONY: test
test: compile
	ctest --test-dir ./$(BUILD_DIR)

.PHONY: dev
dev: build
	docker compose up -d server

.PHONY: build
build:
	docker build -t $(SERVER_NAME):$(BUILD_TAG) .

.PHONY: down
down:
	docker compose down

.PHONY: teardown
teardown: down
	docker image prune

clean:
	rm -rf ./$(BUILD_DIR) ./$(BIN_DIR)
