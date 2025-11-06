# **👷🏻 Crée ta propre Rainbow Table 🌈**
## **Découvre comment créer ta table de correspondance à l'aide d'un dictionnaire et de Docker!**

- [**Pré-requis**](#pré-requis)
	- [1. Docker](#1-docker)
	- [2. Git](#2-git)
- [**Documentation**](#documentation)
	- [1. Dictionnaires](#1-dictionnaires)
	- [2. Hashes](#2-hashes)
- [**Utilisation**](#utilisation)
	- [1. Clonage du dépot](#1-clonage-du-dépot)
	- [2. Déploiement de l'image Docker](#2-déploiement-de-limage-docker)
	- [3. Lance le conteneur](#3-lance-le-conteneur)
	- [4. Compile le programme](#4-compile-le-programme)
	- [5. Par précaution, vérifie la fuite mémoire](#5-par-précaution-vérifie-la-fuite-mémoire)
	- [6. Génère ta table de correspondance](#6-génère-ta-table-de-correspondance)
	- [7. Compare avec tes hash](#7-compare-avec-tes-hash)
	- [8. Quitter](#8-quitter)
- [**Personnalisation**](#personnalisation)
- [**Bonus :**](#bonus-)
	- [1. Personnalisation du mode Generate en ligne de commande](#1-personnalisation-du-mode-generate-en-ligne-de-commande)
	- [2. Personnalisation du mode Lookup en ligne de commande](#2-personnalisation-du-mode-lookup-en-ligne-de-commande)


## **Pré-requis**
### 1. Docker
🌎[Site officiel de Docker](https://docs.docker.com/get-started/get-docker/)
- suivez les instructions d'installation

### 2. Git 
🌎[Site officiel de GitHub](https://github.com/git-guides/install-git)
- suivez les instructions d'installation


## **Documentation**

### 1. Dictionnaires
- le dépot possède déjà un dictionnaire, libre à toi de le modifier ou non
- Pour le modifier il te suffit de choisir parmi les dictionnaires dans le dossier dic/more/
- Il te faudra ensuite le place dans le répertoire dic/ directement
- Et le renommer "dictionnaire.txt"


### 2. Hashes
- Le format du condensat de hash que tu souhaites utiliser doit être au format .txt
- Les hash doivent être sous forme de liste séparés par un saut de ligne
- Le document doit être placé dans le dossier hsh/ 
- Et renommer "hashes.txt"


## **Utilisation**

### 1. Clonage du dépot
- Ouvre un terminal
- Verifie que tu as bien git d'installer 
```bash
git --version
```
- Tu devrais avoir la version qui s'affiche

- Choisi ensuite où cloner le dépot, par exemple : 

```bash
cd ~/Desktop/
mkdir -p Projet/
```
- Clone le répertoire avec l'URL affiché en haut

```bash
git clone https://github.com/ReneRotter/rainbow_table.git
```

- Place toi dans le dossier
```bash
cd Projet/
```
- Avant de continuer, vérifie la présence d'un DockerFile, MakeFile

### 2. Déploiement de l'image Docker

- Lance la commande suivante pour créer ton image d'une machine linux

```bash
docker build -t linux .
```

### 3. Lance le conteneur 

- Lance la commande suivante pour démarrer ton conteneur

```bash
docker run --rm -it -v "$PWD":/rainbow_table linux bash
```

### 4. Compile le programme 

- Utilise la commande make défini dans le Makefile pour compiler

```bash
make
```

### 5. Par précaution, vérifie la fuite mémoire

- On utilise Valgrind pour verifier qu'il n'y a aucune fuite et la bonne utilisation de l'outil

```bash
make mem
```

### 6. Génère ta table de correspondance

- Vérifie bien que tu as suivi les indications dans la Documentation !

```bash
make generate
```

### 7. Compare avec tes hash 

- Vérifie bien que tu as bien renommé ton document "hashes.txt"
- Le document doit être dans le dossier hsh/

```bash
make lookup
```

### 8. Quitter

- Pour quitter le conteneur il te suffit de taper *CTRL+D*

## **Personnalisation**

## **Bonus :**

### 1. Personnalisation du mode Generate en ligne de commande 

- Attentions les documents doivent être placé à minima à la racine du dossier rainbow_table/
- Tu peux entrer en ligne de commande le dictionnaire et la table de corresponsance avec la commande :
```bash
make generate DICO=<nom_du_dictionnaire.txt> TABLE=<nom_de_table.t3c>
```

### 2. Personnalisation du mode Lookup en ligne de commande 


- Attentions les documents doivent être placé à minima à la racine du dossier rainbow_table/
- Tu peux entrer en ligne de commande la table et le document de hash avec la commande :

```bash
make lookup TABLE=<nom_de_table.t3c> HASH=<nom_de_ton_hash.txt>
```
