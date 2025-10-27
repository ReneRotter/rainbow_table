0. Installation de VScode & Docker

1. J'ouvre le terminal dans Docker

2. Création d'un répertoire rainbow_table/
	->$ cd ~/Desktop/r
	->$ mkdir rainbow_table
	->$ cd rainbow_table/

3. Création d'un répertoire src/
	->$ mkdir src/

4. Création du README.md
	->$ vi README.md

5. Création d'un Dockerfile
	->$ vi Dockerfile

6. Test la création de l'image du conteneur depuis le dossier parent
	->$ docker build -t linux . (-t pour tag : donner un nom à l'image du conteneur)

7. Lancer le conteneur depuis l'image créée
	->$ docker run --rm -it -v "$PWD":/rainbow_table linux
		--rm	: remove/supprime le conteneur après l'avoir quitter, 
		-i 	: interactif tu peux écrire au programme, 
		-t	: pseudo-terminal(tty),
		-v	: volume pour monter le répertoire dans le conteneur en persistance,
	->$ CTRL+D 	: pour quitter

8. Création du squelette du fichier Makefile avec les cibles generate et lookup
	->$ vi Makefile

9. Recherche d'un dictionnaire
	-> GitHub : RickyWilder/rockyou.txt

10. Programmation du rainbow_table.c
	-> recherche sur les méthodes de hashage
	-> le choix du sha-256
	-> l'utilisation d'OpenSSL(Homebrew) et l'installation sur mon mac et rajouter dans le makefile le pkg-config -libssl
	-> programmation de main, generate, lookup, et parse

11. Test du Generate

12. Test du Lookup
	-> shuf -n 1000 tbl/tables.t3c | cut -f1 > hsh/hashes.txt
	-> make lookup

	