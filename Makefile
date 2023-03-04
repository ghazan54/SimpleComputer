APP_NAME = simple_computer.out
LIB_NAME = libsc.a
TEST_NAME = test

CFLAGS = -g -Wall -Wextra 
CPPFLAGS = -I include/
TESTFLAGS = -I thirdparty/
LDFLAGS =
LDLIBS =

BIN_DIR = bin
OBJ_DIR = obj
SRC_DIR = src
LIB_DIR = libs

APP_PATH = $(BIN_DIR)/$(APP_NAME)
LIB_PATH = $(LIB_DIR)/$(LIB_NAME)
MAIN_PATH = $(SRC_DIR)/main
TEST_PATH = $(BIN_DIR)/$(TEST_NAME).out

# LIB_OBJECTS = $(OBJ_DIR)/libsc.o
SRC_LIBS = $(SRC_DIR)/$(LIB_DIR)
SRC_TEST = $(SRC_DIR)/$(TEST_NAME)

MODULE_0_LIB = myInterface
MODULE_1_LIB = mySimpleComputer
MODULE_2_LIB = myTerm
MODULE_3_LIB = myBigChars
MODULE_0 = interface
MODULE_1 = ram-operations
MODULE_2 = terminal
MODULE_3 = bigchar

MODULES_LINK = $(LIB_DIR)/$(MODULE_0_LIB).a $(LIB_DIR)/$(MODULE_1_LIB).a $(LIB_DIR)/$(MODULE_2_LIB).a $(LIB_DIR)/$(MODULE_3_LIB).a

.PHONY: all

all: create_dirs $(APP_PATH)

create_dirs: create_bin create_obj create_libs

create_bin:
	if [ ! -d $(BIN_DIR) ]; then mkdir $(BIN_DIR); fi

create_obj:
	if [ ! -d $(OBJ_DIR) ]; then mkdir $(OBJ_DIR); fi

create_libs:
	if [ ! -d $(LIB_DIR) ]; then mkdir $(LIB_DIR); fi

$(APP_PATH): $(MAIN_PATH)/main.c $(MODULES_LINK)
	$(CC) $(CFLAGS) $(CPPFLAGS) $^ -o $@ $(LDFLAGS) $(LDLIBS)

$(LIB_DIR)/$(MODULE_0_LIB).a: $(OBJ_DIR)/$(MODULE_0).o
	ar rcs $@ $^

$(LIB_DIR)/$(MODULE_1_LIB).a: $(OBJ_DIR)/$(MODULE_1).o
	ar rcs $@ $^

$(LIB_DIR)/$(MODULE_2_LIB).a: $(OBJ_DIR)/$(MODULE_2).o
	ar rcs $@ $^

$(LIB_DIR)/$(MODULE_3_LIB).a: $(OBJ_DIR)/$(MODULE_3).o
	ar rcs $@ $^

$(OBJ_DIR)/$(MODULE_0).o : $(SRC_LIBS)/$(MODULE_0).c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $^ -o $@

$(OBJ_DIR)/$(MODULE_1).o : $(SRC_LIBS)/$(MODULE_1).c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $^ -o $@

$(OBJ_DIR)/$(MODULE_2).o : $(SRC_LIBS)/$(MODULE_2).c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $^ -o $@

$(OBJ_DIR)/$(MODULE_3).o : $(SRC_LIBS)/$(MODULE_3).c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $^ -o $@

run:
	$(APP_PATH)

.PHONY: clean
clean:
	$(RM) -r $(BIN_DIR)
	$(RM) -r $(OBJ_DIR)
	$(RM) -r $(LIB_DIR)

rebuild: clean all

test: create_dirs $(BIN_DIR)/test_$(MODULE_1).out $(BIN_DIR)/test_$(MODULE_2).out $(BIN_DIR)/test_$(MODULE_3).out

# $(TEST_PATH) : $(SRC_TEST)/main.c $(SRC_TEST)/$(TEST_NAME).c $(LIB_DIR)/$(MODULE_1_LIB).a $(LIB_DIR)/$(MODULE_2_LIB).a
# 	$(CC) $(CFLAGS) $(CPPFLAGS) $(TESTFLAGS) $^ -o $@ $(LDFLAGS) $(LDLIBS)

$(BIN_DIR)/test_$(MODULE_1).out: $(SRC_TEST)/main_test_$(MODULE_1).c $(SRC_TEST)/test_$(MODULE_1).c $(LIB_DIR)/$(MODULE_1_LIB).a
	$(CC) $(CFLAGS) $(CPPFLAGS) $(TESTFLAGS) $^ -o $@ $(LDFLAGS) $(LDLIBS)

$(BIN_DIR)/test_$(MODULE_2).out: $(SRC_TEST)/main_test_$(MODULE_2).c $(SRC_TEST)/test_$(MODULE_2).c $(LIB_DIR)/$(MODULE_2_LIB).a
	$(CC) $(CFLAGS) $(CPPFLAGS) $(TESTFLAGS) $^ -o $@ $(LDFLAGS) $(LDLIBS)

$(BIN_DIR)/test_$(MODULE_3).out: $(SRC_TEST)/main_test_$(MODULE_3).c $(SRC_TEST)/test_$(MODULE_3).c $(LIB_DIR)/$(MODULE_3_LIB).a $(LIB_DIR)/$(MODULE_2_LIB).a
	$(CC) $(CFLAGS) $(CPPFLAGS) $(TESTFLAGS) $^ -o $@ $(LDFLAGS) $(LDLIBS)

# test_run:
# 	$(TEST_PATH)
