#Configuration des raccourci

#Pour compiler
CC=gcc

#Les warnings de compilation de base, bonus, boucle optimal, et debug
CFLAGS?=-Wall -Wextra -O2 -g $(shell pkg-config --cflags openssl)

#Les lib de compilation de hashage
LDLIBS=$(shell pkg-config --libs openssl)

#Le nom du prog et son chemin src
BIN=MyRainbowTable
SRC=src/rainbow_table.c

#les varibales pour le dico et le T3C
DICO?=dic/dictionnaire.txt
TABLE=tbl/table.t3c

#Le chemin des hashs a comparer
HASH?=hsh/hashes.txt

#Choix de l'algo
ALGO?=sha256

#Declaration des cibles
.PHONY: all clean generate lookup mem

#Lance la compile
all: $(BIN)

$(BIN): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

#Verif des fuites memoires
mem: all
	valgrind --leak-check=full ./$(BIN) generate "$(DICO)" "$(TABLE)"

#Ecrit le T3C
generate: all
	./$(BIN) generate "$(DICO)" "$(TABLE)"

#Compare les hash au T3C
lookup: all
	./$(BIN) lookup "$(TABLE)" "$(HASH)"

#Supprime l'ancienne table 
clean:
	rm -rf $(BIN) "$(TABLE)"

