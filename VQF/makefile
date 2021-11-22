TARGETS= test

OPT=-Ofast -g -mbmi -mbmi2 -mavx -mavx2 -mavx512f

ARCH=-msse4.2

ifeq ($(P),1)
   OPT=-g -no-pie
endif

HAVE_AVX512=$(filter-out 0,$(shell lscpu | grep avx512bw | wc -l))

ifeq ($(THREAD),1)
   OPT +=-DENABLE_THREADS
endif

CXX = g++ -std=c++11 -fgnu-tm -frename-registers # -march=native
CC = gcc -std=gnu11 -fgnu-tm -frename-registers # -march=native
LD = g++ -std=c++11

LOC_INCLUDE=../include
LOC_SRC=src
OBJDIR=obj

CXXFLAGS += -Wall $(DEBUG) $(PROFILE) $(OPT) $(ARCH) -m64 -I. -I$(LOC_INCLUDE)

CFLAGS += -Wall $(DEBUG) $(PROFILE) $(OPT) $(ARCH) -m64 -I. -I$(LOC_INCLUDE)

LDFLAGS += $(DEBUG) $(PROFILE) $(OPT) -lpthread -lm -litm

#
# declaration of dependencies
#

all: $(TARGETS)

# dependencies between programs and .o files
test:   $(OBJDIR)/test.o $(OBJDIR)/vqf_filter.o $(OBJDIR)/shuffle_matrix_512.o $(OBJDIR)/shuffle_matrix_512_16.o
test:   $(patsubst $(LOC_INCLUDE)/%.cpp, $(OBJDIR)/%.o, $(wildcard $(LOC_INCLUDE)/*.cpp))


#
# generic build rules
#

$(TARGETS):
	$(LD) $^ $(LDFLAGS) -o $@

$(OBJDIR)/%.o: ../include/%.cpp ../include/%.h | $(OBJDIR)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c -o $@ $<

$(OBJDIR)/%.o: %.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c -o $@ $<

$(OBJDIR)/%.o: %.c | $(OBJDIR)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c -o $@ $<

$(OBJDIR)/%.o: $(LOC_SRC)/%.cc | $(OBJDIR)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c -o $@ $<

$(OBJDIR)/%.o: $(LOC_SRC)/%.c | $(OBJDIR)
	$(CXX) $(CFLAGS) $(INCLUDE) -c -o $@ $<

$(OBJDIR):
	@mkdir -p $(OBJDIR)

clean:
	rm -rf $(OBJDIR) core $(TARGETS)

