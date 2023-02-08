APP_NAME = simple_computer.out
LIB_NAME = libsc.a
TEST_NAME = test

CFLAGS = -Wall -Wextra 
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

MODULE_1 = ram-operations
MODULE_2 = terminal

MODULES_LINK = $(OBJ_DIR)/$(MODULE_1).o $(OBJ_DIR)/$(MODULE_2).o

.PHONY: all
all: $(APP_PATH)

$(APP_PATH): $(MAIN_PATH)/main.c $(LIB_PATH)
	$(CC) $(CFLAGS) $(CPPFLAGS) $^ -o $@ $(LDFLAGS) $(LDLIBS)

$(LIB_PATH): $(MODULES_LINK)
	ar rcs $@ $^

$(OBJ_DIR)/$(MODULE_1).o : $(SRC_LIBS)/$(MODULE_1).c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $^ -o $@

$(OBJ_DIR)/$(MODULE_2).o : $(SRC_LIBS)/$(MODULE_2).c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) $^ -o $@

run:
	$(APP_PATH)

.PHONY: clean
clean:
	find . -name '*.o' -exec $(RM) '{}' \;
	find . -name '*.a' -exec $(RM) '{}' \;
	find . -name '*.so' -exec $(RM) '{}' \;
	find . -name '*.d' -exec $(RM) '{}' \;
	$(RM) $(APP_PATH)
	$(RM) $(TEST_PATH)

rebuild: clean all

test: $(TEST_PATH)

$(TEST_PATH) : $(SRC_TEST)/main.c $(SRC_TEST)/$(TEST_NAME).c $(LIB_PATH)
	$(CC) $(CFLAGS) $(CPPFLAGS) $(TESTFLAGS) $^ -o $@ $(LDFLAGS) $(LDLIBS)

test_run:
	$(TEST_PATH)
