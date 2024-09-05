CC = gcc
C_FLAGS = -Wall -Wextra
C_EXFLAGS = 

ifndef $(C_RW_MUTEX_ROOT)
C_RW_MUTEX_ROOT= ..
include $(C_RW_MUTEX_ROOT)/Makefile
endif

DEBUG_FLAGS = -O0 -fsanitize=address -g
RELEASE_FLAGS = -O2

all: release

debug: C_FLAGS += $(DEBUG_FLAGS)
debug: clean main

release: C_FLAGS += $(RELEASE_FLAGS)
release: clean main

main: main.c c_rw_mutex.o
	$(CC) $(C_FLAGS) $(C_EXFLAGS) c_rw_mutex.o -g main.c -o main

test_clean:
ifeq ($(wildcard main), main)
	rm main
endif

clean: test_clean 
