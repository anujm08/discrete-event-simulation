ODIR = obj
CPP = g++
CFLAGS = std=c++11

DEPS = core.h event_handler.h request.h server.h thread.h user.h
_OBJ = core.o event_handler.o request.o server.o thread.o user.o simulation.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

all: $(ODIR) simulation

simulation: $(OBJ)
	$(CPP) -o $@ $^

$(ODIR):
	mkdir -p $(ODIR)

$(ODIR)/%.o: %.cpp $(DEPS)
	$(CPP) -g --std=c++11 -c -o $@ $<


.PHONY: clean
clean:
	$(RM) -r $(ODIR)
