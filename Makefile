CXX = clang
CFLAGS = -g -O0 -lstdc++ 

BUILD_DIR = build

# default target : mingxicc compiler 
all: $(BUILD_DIR)/mc

# make build dir
$(BUILD_DIR):
	@mkdir $(BUILD_DIR)

# generate lexical analysis file by lex
$(BUILD_DIR)/lex.yy.cpp: mc_lex.l $(BUILD_DIR)/y.tab.h
	lex -t $< > $@

# generate lexical analysis obj 
$(BUILD_DIR)/lex.yy.o: $(BUILD_DIR)/lex.yy.cpp
	$(CXX) $(CFLAGS) -Wno-unused-function -c $< -o $@

# generate syntax analysis file by yacc
$(BUILD_DIR)/y.tab.cpp $(BUILD_DIR)/y.tab.h: mc_yacc.y
	yacc -d $< -o $(BUILD_DIR)/y.tab.cpp

# generate syntax analysis obj
$(BUILD_DIR)/y.tab.o: $(BUILD_DIR)/y.tab.cpp 
	$(CXX) $(CFLAGS) -c $< -o $@

# generate assembly obj
$(BUILD_DIR)/assembly.o: assembly.cpp 
	$(CXX) $(CFLAGS) -c $< -o $@

# build final target compiler program
$(BUILD_DIR)/mc: $(BUILD_DIR) $(BUILD_DIR)/lex.yy.o $(BUILD_DIR)/y.tab.o $(BUILD_DIR)/assembly.o main.cpp
	$(CXX) $(CFLAGS) -o $@ main.cpp $(BUILD_DIR)/*.o

# clean build files
.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)

# build test program
$(BUILD_DIR)/run_tests: run_tests.c $(BUILD_DIR)/mc
	$(CXX) $(CFLAGS) $< -o $@

# run test
.PHONY: test
test: $(BUILD_DIR)/run_tests
	@./$^

# format source file
.PHONY: format
format:
	find -name "*.[ch]" -o -name "*.cpp" -type f | xargs clang-format -i
