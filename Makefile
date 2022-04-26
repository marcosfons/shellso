CC=gcc
CFLAGS=-g -Wall
LDLIBS=

MKDIR_P=mkdir -p

EXE=shellso

SRC=src
BIN=bin
OBJ=build
TEST=tests
TESTBIN=tests/bin

SRCS=$(wildcard $(SRC)/*.c) $(wildcard $(SRC)/*/*.c)
OBJS=$(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SRCS))

TESTS=$(filter-out $(TEST)/test_utils.c, $(wildcard $(TEST)/*.c) $(wildcard $(TEST)/*/*.c))

UNIT_TEST=$(TESTBIN)/unit_test.o



.PHONY: help
help: ## Show a help message
help:
	@echo -e 'Usage: make [target] ...\ntargets:'
	@grep -E '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | \
		sort | \
		awk 'BEGIN {FS = ":.*?## "}; {printf "   \033[36m%-20s\033[0m %s\n", $$1, $$2}'

.PHONY: debug
debug: ## Build an executable to debug 
debug: $(BIN)/$(EXE)

.PHONY: release
release: ## Build a release executable
release: CFLAGS=-Wall -O2 -DNDEBUG
release: clean
release: $(BIN)/$(EXE)

.PHONY: test
test: ## Build and run tests
test: LDLIBS += -lcriterion
test: CFLAGS += --coverage
test: $(TESTBIN) $(UNIT_TEST) 
	$(UNIT_TEST) -j1

.PHONY: docs
docs: ## Generate Doxygen docs and try to open
docs:
	doxygen
	xdg-open docs/doxygen/html/index.html

.PHONY: clean
clean: ## Clean all builded files
	$(RM) -r $(BIN) $(OBJ) $(TESTBIN)




$(OBJ)/%.o: $(SRC)/%.c $(OBJ)
	@$(MKDIR_P) $(@D)
	$(CC) $(CFLAGS) $(LDLIBS) -c $< -o $@

$(BIN)/$(EXE): $(OBJS) | $(BIN)
	$(CC) $(CFLAGS) $(LDLIBS) $^ -o $@


$(UNIT_TEST): $(TESTS) $(TEST)/test_utils.c 
	$(CC) $(CFLAGS) $(LDLIBS) -Wl,-rpath,include $^ -o $@



# Directories
$(BIN):
	$(MKDIR_P) $@

$(OBJ):
	$(MKDIR_P) $@

$(TESTBIN):
	$(MKDIR_P) $@

