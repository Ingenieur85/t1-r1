# Nome dos executáveis
SERVER = server
CLIENT = client

# Compilador
CC = gcc

# Flags de compilação
CFLAGS = -g -O0 -Wall -Wextra -Wpedantic -std=gnu2x #pkg-config --cflags glib-2.0
LFLAGS = -lm #pkg-config --libs glib-2.0

# Diretórios
SRCDIR = src
INCDIR = include
OBJDIR = objs

# Lista de arquivos para distribuição
DISTFILES = $(SRCDIR)/*.c $(INCDIR)/*.h makefile
DISTDIR = fasf22
DISTNAME = fasf22.tar.gz

# Automatically find all .c files in the source directory
SRCS_ALL = $(wildcard $(SRCDIR)/*.c)

# Filter out client.c for SRCS_SERVER
SRCS_SERVER = $(filter-out $(SRCDIR)/client.c, $(SRCS_ALL))

# Filter out server.c for SRCS_CLIENT
SRCS_CLIENT = $(filter-out $(SRCDIR)/server.c, $(SRCS_ALL))

# Create object files
OBJECTS_SERVER = $(SRCS_SERVER:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
OBJECTS_CLIENT = $(SRCS_CLIENT:$(SRCDIR)/%.c=$(OBJDIR)/%.o)


# Alvo principal
all: $(SERVER) $(CLIENT)

# Compilar o server
$(SERVER): $(OBJECTS_SERVER)
	$(CC) $(OBJECTS_SERVER) -o $(SERVER) $(LFLAGS)

# Compilar o client
$(CLIENT): $(OBJECTS_CLIENT)
	$(CC) $(OBJECTS_CLIENT) -o $(CLIENT) $(LFLAGS)

# Compilar arquivos .c em .o
$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -I$(INCDIR) -c $< -o $@

# Criar diretório de objetos, se não existir
$(OBJDIR):
	mkdir -p $(OBJDIR)

# Limpar arquivos temporários
clean:
	@echo "Limpando arquivos temporários ..."
	@rm -f *~ $(OBJDIR)/*

# Limpar tudo, incluindo executáveis e objetos
purge: clean
	@echo "Limpando todos os arquivos gerados ..."
	@rm -f $(SERVER) $(CLIENT) $(OBJDIR)/*.o core a.out $(DISTDIR) $(DISTDIR).tar

# Gerar o arquivo de distribuição
dist: purge
	@echo "Gerando arquivo de distribuição ($(DISTNAME)) ..."
	@mkdir -p $(DISTDIR) && cp -r $(DISTFILES) $(DISTDIR)
	@tar -czvf $(DISTNAME) $(DISTDIR)
	@rm -rf $(DISTDIR)

.PHONY: all clean purge dist
