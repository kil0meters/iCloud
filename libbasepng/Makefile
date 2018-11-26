TARGET   = libbasepng.so

CC       = gcc
# compiling flags here
CFLAGS   = -fPIC -std=gnu99 -Wall -I.

LINKER   = gcc
# linking flags here
LFLAGS   = -shared -Wall -I. -lm -lpng

# change these to proper directories where each file should be
SRCDIR   = src
OBJDIR   = .
BINDIR   = .

SOURCES  := $(wildcard $(SRCDIR)/*.c)
INCLUDES := $(wildcard $(SRCDIR)/*.h)
OBJECTS  := $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
rm       = rm -f


$(BINDIR)/$(TARGET): $(OBJECTS)
	@$(LINKER) $(OBJECTS) $(LFLAGS) -o $@
	@echo "Linking complete."

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.c
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo "Compiled "$<""

.PHONY: clean
clean:
	@$(rm) $(OBJECTS)
	@echo "Cleanup complete."

.PHONY: remove
remove: clean
	@$(rm) $(BINDIR)/$(TARGET)
	@echo "Executable removed."


