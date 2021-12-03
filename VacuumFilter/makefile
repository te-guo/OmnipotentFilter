TARGETS = test
INCLUDE_PATH = ../include
FLAGS = -Ofast -Wall -std=c++14 -I$(INCLUDE_PATH)
LFLAGS = $(FLAGS)
OBJDIR = obj
HEADERS = $(wildcard *.h) $(wildcard $(INCLUDE_PATH)/*.h)

all: test

# dependencies between programs and .o files
test: $(patsubst %.cpp, $(OBJDIR)/%.o, $(wildcard *.cpp))
test: $(patsubst $(INCLUDE_PATH)/%.cpp, $(OBJDIR)/%.o, $(wildcard $(INCLUDE_PATH)/*.cpp))

# make targets
$(TARGETS): $(HEADERS) | $(OBJDIR)
	g++ $(filter %.o, $^) $(LFLAGS) -o $@ 

# make include files
$(OBJDIR)/%.o: ../include/%.cpp $(HEADERS) | $(OBJDIR)
	g++ $(FLAGS) -c -o $@ $<

# make source files
$(OBJDIR)/%.o: %.cpp $(HEADERS) | $(OBJDIR)
	g++ $(FLAGS) -c -o $@ $<

# create object directory
$(OBJDIR):
	@mkdir -p $(OBJDIR)

clean:
	rm -rf $(OBJDIR) $(TARGETS)
