COMPILER=gcc

SRC_DIR := ./src
INC_DIR := $(addsuffix /include,$(SRC_DIR))
INC_FLAGS := $(addprefix -I,$(INC_DIR))

SRCS := $(shell find $(SRC_DIR)/*.c)


APP_FILES=src/app.c
SLAVE_FILES=src/slave.c
VIEW_FILES=src/view.c
UTILS_FILES=src/utils.c
SHMADT_FILES=src/shmADT.c
DISPADT_FILES=src/dispatcherADT.c

DEBUG ?= 0

ifeq ($(DEBUG), 1)
	CFLAGS=-pedantic -std=gnu99 -Wall -fsanitize=address -lrt -pthread -g
else
	CFLAGS=-pedantic -std=gnu99 -Wall -fsanitize=address -lrt -pthread
endif

OUTPUT_APP=app
OUTPUT_SLAVE=slave
OUTPUT_VIEW=view
OUT_FOLDER=out

.PHONY: rebuild all clean debug

rebuild: clean all

check:
	cppcheck --quiet --enable=all --force --inconclusive .
	pvs-studio-analyzer trace -- make
	pvs-studio-analyzer analyze
	plog-converter -a '64:1,2,3;GA:1,2,3;OP:1,2,3' -t tasklist -o report.tasks PVS-Studio.log

all: 
	$(COMPILER) $(CFLAGS) $(INC_FLAGS) -o $(OUT_FOLDER)/$(OUTPUT_APP) $(APP_FILES) $(UTILS_FILES) $(SHMADT_FILES) $(DISPADT_FILES)
	$(COMPILER) $(CFLAGS) $(INC_FLAGS) -o $(OUT_FOLDER)/$(OUTPUT_SLAVE) $(SLAVE_FILES) $(UTILS_FILES)
	$(COMPILER) $(CFLAGS) $(INC_FLAGS) -o $(OUT_FOLDER)/$(OUTPUT_VIEW) $(VIEW_FILES) $(UTILS_FILES) $(SHMADT_FILES) $(DISPADT_FILES)

clean:
	rm -rf $(OUT_FOLDER)/*
