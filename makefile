# Nome do arquivo executável
TARGET = minimizador

# Compilador
CXX = g++

# Flags de compilação
CXXFLAGS = -std=c++14 -Wall

# Diretórios de código-fonte e objetos
SRCDIR = src
OBJDIR = obj

# Verifica o sistema operacional
ifeq ($(OS),Windows_NT)
	# Comandos e diretivas para o Windows
	RM = del
	EXECUTABLE_EXTENSION = .exe
else
	# Comandos e diretivas para o Linux
	RM = rm -f
	EXECUTABLE_EXTENSION =
endif

# Código-fonte
SOURCES = $(wildcard $(SRCDIR)/*.cpp)

# Objetos gerados
OBJECTS = $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

all: $(TARGET)

# Compilar o executável
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET)$(EXECUTABLE_EXTENSION) $(OBJECTS)
	./$(TARGET)$(EXECUTABLE_EXTENSION)

# Compilar os objetos
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Limpar arquivos gerados
clean:
	$(RM) $(OBJDIR)/*.o
	$(RM) $(TARGET)$(EXECUTABLE_EXTENSION)
	$(RM) output/*.dot
	$(RM) output/*.png
