CC=gcc
CFLAGS=-g -Wall
LDLIBS=

MKDIR_P=mkdir -p

EXE=shellso

SRC=src
BIN=bin
OBJ=build
TEST=tests
TESTSCRIPT=$(TEST)/test_scripts
TESTBIN=tests/bin

SRCS=$(wildcard $(SRC)/*.c) $(wildcard $(SRC)/*/*.c)
OBJS=$(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(SRCS))

TESTS=$(filter-out $(TEST)/test_utils.c, $(wildcard $(TEST)/*.c) $(wildcard $(TEST)/*/*.c))


TESTSCRIPTS=$(wildcard $(TESTSCRIPT)/*.sh)
TESTSCRIPTSOUT=$(patsubst $(TESTSCRIPT)/%.sh, $(TESTSCRIPT)/%_output.txt, $(TESTSCRIPTS))

UNIT_TEST=$(TESTBIN)/unit_test.o



.PHONY: help
help: ## Show a help message
help:
	@echo -e 'Usage: make [\033[36mtarget\033[0m]\ntargets:'
	@grep -E '^[a-zA-Z_-]+:.*?## .*$$' $(MAKEFILE_LIST) | \
		sort | \
		sed \
			-e :a \
			-e 's/^\([^:]\{1,9\}\):/\1 :/;ta' \
			-e 's/^\([^:]*\): \+##/    \x1b[36m\1\x1b[0m/'
# sed -e :a \                                      Create a label "a"
# 	-e 's/^\([^:]\{1,9\}\):/\1 :/;ta' \            Loop through the string adding white space padding
# 	-e 's/^\([^:]*\): \+##/    \x1b[36m\1\x1b[0m/' Replace target with colored output

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
test: CLEAN_TESTS $(UNIT_TEST) 
	$(UNIT_TEST) -j1

.PHONY: test_files
test_files: ## Run test scripts comparing lines to the sh output
test_files: debug
test_files: $(TESTSCRIPTSOUT)

.PHONY: docs
docs: ## Generate Doxygen docs and try to open
docs:
	doxygen
	xdg-open docs/doxygen/html/index.html

.PHONY: relatorio
relatorio: ## Generate a report based on docs/relatorio.tex
relatorio:
	pdflatex -output-directory=. docs/relatorio.tex

.PHONY: clean
clean: ## Clean all builded files
	$(RM) -r $(BIN) $(OBJ) $(TESTBIN) $(TESTSCRIPTSOUT)

.PHONY: CLEAN_TESTS
CLEAN_TESTS:
	$(RM) $(UNIT_TEST) $(TESTBIN)/*.gcno $(TESTBIN)/*.gcda




$(OBJ)/%.o: $(SRC)/%.c $(OBJ)
	@$(MKDIR_P) $(@D)
	$(CC) $(CFLAGS) $(LDLIBS) -c $< -o $@

$(BIN)/$(EXE): $(OBJS) | $(BIN)
	$(CC) $(CFLAGS) $(LDLIBS) $^ -o $@


$(UNIT_TEST): $(TESTS) $(TEST)/test_utils.c | $(TESTBIN)
	$(CC) $(CFLAGS) $(LDLIBS) -Wl,-rpath,include $^ -o $@


$(TESTSCRIPT)/%_output.txt: $(TESTSCRIPT)/%.sh
	diff <(sh $<) <($(BIN)/$(EXE) -f $<)


# Directories
$(BIN):
	$(MKDIR_P) $@

$(OBJ):
	$(MKDIR_P) $@

$(TESTBIN):
	$(MKDIR_P) $@

