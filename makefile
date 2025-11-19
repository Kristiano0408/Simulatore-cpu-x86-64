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

# Core sources excluding prova.cpp
CORE_SOURCES = $(filter-out $(SRCDIR)/prova.cpp, $(shell find $(SRCDIR) -name '*.cpp'))
CORE_OBJS = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(CORE_SOURCES))

# Prova obj
PROVA_OBJ = $(OBJDIR)/prova.o

# Pybind11 (CLONATO LOCALMENTE) + include Python del venv
PYBIND11_INCLUDES = -Ipybind11/include $(shell python3-config --includes)

# Binding file
PYBIND_CPP = $(GUI_SRCDIR)/bindings.cpp
PYBIND_OBJ = $(OBJDIR)/bindings.o

# Python module
PYTHON_MODULE = $(GUI_SRCDIR)/simulator$(shell python3-config --extension-suffix)
PYTHON_LDFLAGS := $(shell python3-config --ldflags)

# Executable
EXEC = $(OBJDIR)/prova

# Build dirs
DIRS = $(sort $(dir $(CORE_OBJS) $(PROVA_OBJ)))

# -----------------------
# Target principali
# -----------------------
all: dirs $(CORE_OBJS) $(PROVA_OBJ) $(PYBIND_OBJ) $(PYTHON_MODULE) $(EXEC)

dirs:
	@mkdir -p $(DIRS)

# Core C++
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Pybind11 wrapper
$(PYBIND_OBJ): $(PYBIND_CPP)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(PYBIND11_INCLUDES) -c $< -o $@

# Python module
$(PYTHON_MODULE): $(CORE_OBJS) $(PYBIND_OBJ)
	$(CXX) $(CXXFLAGS) -shared -o $@ $^ $(PYTHON_LDFLAGS)

# Executable prova (versione senza GUI)
$(EXEC): $(CORE_OBJS) $(PROVA_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Pulizia
clean:
	rm -rf $(OBJDIR) $(GUI_SRCDIR)/*.o $(PYTHON_MODULE)

.PHONY: all clean dirs