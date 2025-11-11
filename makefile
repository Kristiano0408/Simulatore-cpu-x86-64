CXX = g++
CXXFLAGS = -std=c++23 -O2 -Wall -Wextra -Iinclude

# Attiva DEBUG se passato come variabile
ifeq ($(DEBUG),1)
    CXXFLAGS += -DDEBUG -g
endif

SRCDIR = src
OBJDIR = build

# Trova tutti i file .cpp nella cartella src e sotto-cartelle
SOURCES = $(shell find $(SRCDIR) -name '*.cpp')
OBJS = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SOURCES))

TARGET = $(OBJDIR)/prova

# Crea tutte le directory necessarie per gli .o
DIRS = $(sort $(dir $(OBJS)))

# Regola principale
all: dirs $(TARGET)

# Crea le directory prima di compilare
dirs:
	@mkdir -p $(DIRS)

# Link finale
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compilazione dei singoli .cpp
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Pulizia
clean:
	rm -rf $(OBJDIR)
