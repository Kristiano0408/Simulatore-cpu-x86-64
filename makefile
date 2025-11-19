# Compilatore
CXX = g++
CXXFLAGS = -std=c++23 -O2 -Wall -Wextra -Iinclude -fPIC

# Debug
ifeq ($(DEBUG),1)
    CXXFLAGS += -DDEBUG -g
endif

# Directory
SRCDIR = src
OBJDIR = build
GUI_SRCDIR = GUI
BINDINGS_DIR = $(GUI_SRCDIR)/bindings

# Core sources excluding prova.cpp
CORE_SOURCES = $(filter-out $(SRCDIR)/prova.cpp, $(shell find $(SRCDIR) -name '*.cpp'))
CORE_OBJS = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(CORE_SOURCES))

# Prova obj
PROVA_OBJ = $(OBJDIR)/prova.o

# Pybind11 + includes Python
PYBIND11_INCLUDES = -Ipybind11/include $(shell python3-config --includes)

# All binding .cpp files in GUI/bindings/
BINDINGS_SOURCES = $(shell find $(BINDINGS_DIR) -name '*.cpp')
BINDINGS_OBJS = $(patsubst $(GUI_SRCDIR)/bindings/%.cpp,$(OBJDIR)/bindings/%.o,$(BINDINGS_SOURCES))

# Python module target
PYTHON_MODULE = $(GUI_SRCDIR)/simulator$(shell python3-config --extension-suffix)
PYTHON_LDFLAGS := $(shell python3-config --ldflags)

# Executable
EXEC = $(OBJDIR)/prova

# Build dirs
DIRS = $(sort $(dir $(CORE_OBJS) $(PROVA_OBJ) $(BINDINGS_OBJS)))

# -----------------------
# Target principali
# -----------------------
all: dirs $(CORE_OBJS) $(PROVA_OBJ) $(BINDINGS_OBJS) $(PYTHON_MODULE) $(EXEC)

dirs:
	@mkdir -p $(DIRS)

# Core C++
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Bindings C++
$(OBJDIR)/bindings/%.o: $(BINDINGS_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(PYBIND11_INCLUDES) -c $< -o $@

# Python module
$(PYTHON_MODULE): $(CORE_OBJS) $(BINDINGS_OBJS)
	$(CXX) $(CXXFLAGS) -shared -o $@ $^ $(PYTHON_LDFLAGS)

# Executable prova
$(EXEC): $(CORE_OBJS) $(PROVA_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Pulizia
clean:
	rm -rf $(OBJDIR) $(GUI_SRCDIR)/*.so $(GUI_SRCDIR)/*.o

.PHONY: all clean dirs
