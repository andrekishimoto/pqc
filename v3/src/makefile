CC = gcc
CFLAGS = -std=c17 -Wall -Wextra -Wpedantic -Wno-unused-result
OPTIMIZATION_LEVEL = -O0
LIBS = -lm

TARGET = pqc

OBJ_DIR = obj

INC = $(wildcard *.h)
SRC = $(wildcard *.c)
OBJ = $(addprefix $(OBJ_DIR)/, $(SRC:c=o))

.PHONY: all clean clean_obj show_files

all: $(TARGET)

clean: clean_obj
	-rm -f $(TARGET)

clean_obj:
	-rm -f $(OBJ)
	-rmdir $(OBJ_DIR)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(OPTIMIZATION_LEVEL) -o $@ $^ $(LIBS)

$(OBJ_DIR)/%.o: %.c $(INC)
	mkdir -p $(@D)
	$(CC) $(CFLAGS) $(OPTIMIZATION_LEVEL) -c -o $@ $< $(LIBS)

show_files:
	@echo "INC files: $(INC)"
	@echo "SRC files: $(SRC)"
	@echo "OBJ files: $(OBJ)"
