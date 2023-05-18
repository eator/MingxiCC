CC = clang
CFLAGS = -Wall -Wextra -g -O0 -std=gnu99 -fstack-protector-all -ftrapv

BUILD_DIR = build

# default target : mingxicc compiler 
all: $(BUILD_DIR)/mc

# make build dir
$(BUILD_DIR):
	@mkdir $(BUILD_DIR)

# generate lexical analysis file by lex
$(BUILD_DIR)/lex.yy.c: mc_lex.l $(BUILD_DIR)/y.tab.h
	lex -t $< > $@

# generate lexical analysis obj 
$(BUILD_DIR)/lex.yy.o: $(BUILD_DIR)/lex.yy.c
	$(CC) $(CFLAGS) -Wno-unused-function -c $< -o $@

# generate syntax analysis file by yacc
$(BUILD_DIR)/y.tab.c $(BUILD_DIR)/y.tab.h: mc_yacc.y
	yacc -d $< -o $(BUILD_DIR)/y.tab.c

# generate syntax analysis obj
$(BUILD_DIR)/y.tab.o: $(BUILD_DIR)/y.tab.c syntax.c stack.c
	$(CC) $(CFLAGS) -c $< -o $@

# generate stack obj
$(BUILD_DIR)/stack.o: stack.c
	$(CC) $(CFLAGS) -c $< -o $@

# generate assembly obj
$(BUILD_DIR)/assembly.o: assembly.c syntax.c env.c
	$(CC) $(CFLAGS) -c $< -o $@

# generate syntax obj
$(BUILD_DIR)/syntax.o: syntax.c list.c
	$(CC) $(CFLAGS) -c $< -o $@

# generate list obj
$(BUILD_DIR)/list.o: list.c
	$(CC) $(CFLAGS) -c $< -o $@

# generate context obj
$(BUILD_DIR)/context.o: context.c
	$(CC) $(CFLAGS) -c $< -o $@

# generate env obj
$(BUILD_DIR)/env.o: env.c
	$(CC) $(CFLAGS) -c $< -o $@

# build final target compiler program
$(BUILD_DIR)/mc: $(BUILD_DIR) $(BUILD_DIR)/lex.yy.o $(BUILD_DIR)/y.tab.o $(BUILD_DIR)/syntax.o $(BUILD_DIR)/env.o $(BUILD_DIR)/assembly.o $(BUILD_DIR)/stack.o $(BUILD_DIR)/context.o $(BUILD_DIR)/list.o main.c
	$(CC) $(CFLAGS) -o $@ main.c $(BUILD_DIR)/*.o

# clean build files
.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)

# build test program
$(BUILD_DIR)/run_tests: run_tests.c $(BUILD_DIR)/mc
	$(CC) $(CFLAGS) $< -o $@

# run test
.PHONY: test
test: $(BUILD_DIR)/run_tests
	@./$^

# format source file
.PHONY: format
format:
	find -name "*.[ch]" -type f | xargs clang-format -i
