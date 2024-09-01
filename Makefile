CC = gcc
C_FLAGS = -Wall -Wextra
C_EXFLAGS = 

c_rw_mutex_src := $(or $(C_RW_MUTEX_ROOT), .)

DEBUG_FLAGS = -O0 -fsanitize=address -g
RELEASE_FLAGS = -O2

all: release

debug: C_FLAGS += $(DEBUG_FLAGS)
debug: clean c_rw_mutex.o

release: C_FLAGS += $(RELEASE_FLAGS)
release: clean c_rw_mutex.o

c_rw_mutex.c := $(c_rw_mutex_src)/c_rw_mutex.c

c_rw_mutex.o: $(c_rw_mutex.c)
	$(CC) $(C_FLAGS) $(C_EXFLAGS) -c $(c_rw_mutex.c)

c_rw_mutex_clean:
ifeq ($(wildcard c_rw_mutex.o), c_rw_mutex.o)
	rm c_rw_mutex.o
endif

clean: c_rw_mutex_clean
