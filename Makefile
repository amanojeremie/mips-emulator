CC = g++
CFLAGS = -g -Wall -std=c++11
vpath %.h src
vpath %.cpp src
OBJDIR = obj
LIBS = 
OBJECTS = $(addprefix $(OBJDIR)/, main.o mips.o)

default: mips-emulator

mips-emulator: $(OBJDIR) $(OBJECTS)
	$(CC) $(CFLAGS) -o mips-emulator $(OBJECTS) $(LIBS)

$(OBJDIR): 
	mkdir -p $(OBJDIR)

$(OBJDIR)/%.o: %.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

.PHONY: clean
clean:
	rm -r $(OBJDIR)/*.o