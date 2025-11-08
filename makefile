CXX = g++
CXXFLAGS = -std=c++23 -O2 -Wall -Wextra

# Attiva DEBUG se passato come variabile
ifeq ($(DEBUG),1)
    CXXFLAGS += -DDEBUG -g
endif

SRCS = prova.cpp bus.cpp cpu.cpp memory.cpp clock.cpp addressCalculator.cpp \
       alu.cpp cacheManager.cpp controlUnit.cpp decoder.cpp helpers.cpp \
       instruction_code_map.cpp instruction.cpp opcode_map.cpp operands.cpp \
       registerFile.cpp device.cpp pipeline.cpp

OBJDIR = build
OBJS = $(patsubst %.cpp,$(OBJDIR)/%.o,$(SRCS))

TARGET = $(OBJDIR)/prova

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(OBJDIR)/%.o: %.cpp
	@mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(OBJDIR)
