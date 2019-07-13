PROG = ueb01

SRCDIR = src/
BUILDDIR = build/

vpath %.c $(SRCDIR)
vpath %.h $(SRCDIR)
vpath %.o $(OBJDIR)

CC = gcc
CCFLAGS = -Wall -Werror -O3
SRCS = $(shell find $(SRCDIR) -type f -name '*.c')
HEDS = $(shell find $(SRCDIR) -type f -name '*.h')
OBJS = $(SRCS:$(SRCDIR)%.c=$(BUILDDIR)%.o)

GL   = -lglut -lGLU -lGL -lGLEW
MATH = -lm
LIBS = $(MATH) $(GL)

INCLUDES = -I$(SRCDIR) -Iinclude

.PHONY: directories clean all doc debug

$(PROG): directories .depend $(OBJS)
	@echo "\e[1;34mBuilding" $@ "\e[0m"
	$(CC) $(CCFLAGS) -o $(BUILDDIR)$(PROG) $(OBJS) $(LIBS)
	@echo "\e[1;34mDONE Creating" $@ "in" $(BUILDDIR)$(PROG)"\e[0m"
	

debug: CCFLAGS += -g
debug: $(PROG)

all: $(PROG)

clean:
	rm -f  $(BUILDDIR)$(PROG)
	rm -f  $(OBJS)
	rm -f  .depend
	rm -rf $(BUILDDIR)

directories:
	mkdir -p $(BUILDDIR)

$(BUILDDIR)%.o : %.c
	$(CC) $(CCFLAGS) $(INCLUDES) -c $< -o $@

.depend : $(SRCS)
	$(CC) $(CCFLAGS) -MM $^ > .depend

include .depend
