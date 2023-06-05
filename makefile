# Nome do arquivo executável
TARGET = minimizador

# Compilador
CXX = g++

# Flags de compilação
CXXFLAGS = -std=c++14

# Diretórios de código-fonte e objetos
SRCDIR = src
OBJDIR = obj

# Arquivos de código-fonte
SOURCES = $(wildcard $(SRCDIR)/*.cpp)

# Arquivos objetos gerados
OBJECTS = $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)

# Regra padrão (primeira a ser executada)
all: $(TARGET)

# Regra para compilar o executável
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS)
	./minimizador.exe

# Regra para compilar os objetos
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# Regra para limpar arquivos gerados no windows
clean:
	del $(OBJDIR)\*.o
	del $(TARGET).exe
	del *.dot
	del *.png