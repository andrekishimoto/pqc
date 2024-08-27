CC = gcc
CFLAGS = -std=c17 -Wall -Wextra -Wpedantic -Wno-unused-result
OPTIMIZATION_LEVEL = -O0
LIBS = -lm

TARGET = pqc

OBJ_DIR = obj
KT_DIR = kt
SUBDIR = $(KT_DIR)

INC = $(wildcard *.h $(foreach fd, $(SUBDIR), $(fd)/*.h))
SRC = $(wildcard *.c $(foreach fd, $(SUBDIR), $(fd)/*.c))
OBJ = $(addprefix $(OBJ_DIR)/, $(SRC:c=o))
INC_DIRS = $(addprefix -I, $(SUBDIR))

.PHONY: all clean clean_obj show_files

all: $(TARGET)

clean: clean_obj
	-rm -f $(TARGET)

clean_obj:
	-rm -f $(OBJ)
	-rmdir -p $(foreach fd, $(SUBDIR), $(OBJ_DIR)/$(fd))

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(INC_DIRS) $(OPTIMIZATION_LEVEL) -o $@ $^ $(LIBS)

$(OBJ_DIR)/%.o: %.c $(INC)
	mkdir -p $(@D)
	$(CC) $(CFLAGS) $(INC_DIRS) $(OPTIMIZATION_LEVEL) -c -o $@ $< $(LIBS)

show_files:
	@echo "INC files: $(INC)"
	@echo "SRC files: $(SRC)"
	@echo "OBJ files: $(OBJ)"
	@echo "INC_DIRS: $(INC_DIRS)"
