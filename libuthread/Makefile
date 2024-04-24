# Target library
lib := libuthread.a
CC := gcc
targets := queue uthread context sem preempt
objects := queue.o uthread.o context.o sem.o preempt.o

CFLAGS := -Wall -Wextra -MMD
CFLAGS += -g

ifneq ($(V),1)
Q = @
endif


all: $(lib)

deps :=deps := $(patsubst %.o,%.d,$(objects))
-include $(deps)
# TODO: Phase 1

%.o: %.c
	@echo "CC $@"
	$(Q)$(CC) $(CFLAGS) -c -o $@ $<

$(lib): $(objects)
	@echo "CC $@"
	$(Q) ar rcs $@ $^

clean:
	@echo "clean"
	$(Q)rm -f $(targets) $(objects) $(deps) $(lib)
