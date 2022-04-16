TARGET_EXEC ?= main.o

BUILD_DIR ?= build
BIN_DIR ?= bin
SRC_DIRS ?= src

CFLAGS=-g -Wall
LDLIBS=

SRCS := $(shell find $(SRC_DIRS) -name *.c)
OBJS := $(SRCS:%=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

INC_DIRS := $(shell find $(SRC_DIRS) -type d)
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

TEST := tests
TESTBIN := tests/bin

TESTS=$(shell find . -type f -path "*/tests/**/*.c")
TESTBINS=$(patsubst $(TEST)/%.c, $(TEST)/bin/%, $(SRCS))


UNIT_TEST := $(TESTBIN)/unit_test.o

AA=$(BUILD_DIR/%.c.o)

CRITERION := LD_LIBRARY_PATH=/usr/local/lib

$(BIN_DIR)/$(TARGET_EXEC): $(OBJS)
	$(MKDIR_P) $(dir $@)
	echo $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

# c source
$(AA): %.c
	$(MKDIR_P) $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

$(TEST)/bin:
	$(MKDIR_P) $(dir $@)

test: $(TESTBINS)
	for test in $(TESTBINS) ; do ./$$test ; done


tests_run: unit_test
tests_run:
	chmod +x $(UNIT_TEST)
	$(CRITERION) $(UNIT_TEST) -j1

tests_run_debug: unit_test
tests_run_debug:
	chmod +x $(UNIT_TEST)
	$(CRITERION) $(UNIT_TEST) --crash -j1

unit_test: LDLIBS += -lcriterion
unit_test: CFLAGS += --coverage
unit_test: $(AA)
	$(MKDIR_P) $(dir $(UNIT_TEST))
	$(CC) $(CFLAGS) tests/shell/command_parser.c -o $(UNIT_TEST) $(LDLIBS)



# CGLAG=--coverage will create .gcda and .gcno
# Create rule to delete them


.PHONY: clean

clean:
	$(RM) -r $(BUILD_DIR) $(TESTBIN)

-include $(DEPS)

MKDIR_P ?= mkdir -p

