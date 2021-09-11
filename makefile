COMPILER=gcc

SRC_DIR := ./src
INC_DIR := $(addsuffix /include,$(SRC_DIR))
INC_FLAGS := $(addprefix -I,$(INC_DIR))

SRCS := $(shell find $(SRC_DIR)/*.c)

APP_FILES=src/app.c
SLAVE_FILES=src/slave.c
UTILS_FILES=src/utils.c
CFLAGS=-pedantic -std=gnu99 -Wall -fsanitize=address
OUTPUT_APP=app
OUTPUT_SLAVE=slave
OUT_FOLDER=out


rebuild: clean all

all: 
	$(COMPILER) $(CFLAGS) $(INC_FLAGS) -o ${OUT_FOLDER}/$(OUTPUT_APP) $(APP_FILES) $(UTILS_FILES)
	$(COMPILER) $(CFLAGS) $(INC_FLAGS) -o ${OUT_FOLDER}/$(OUTPUT_SLAVE) $(SLAVE_FILES) $(UTILS_FILES)

clean:
	rm -rf ${OUT_FOLDER}/*
