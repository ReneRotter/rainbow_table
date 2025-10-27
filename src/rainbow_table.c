#define _GNU_SOURCE //extention GNU pour getline
#include <stdio.h> //FILE*, fopen, fprintf, perror
#include <stdlib.h> //malloc, realloc, free
#include <string.h> //strlen, strcmp, strchr, strcpy
#include <errno.h> 
#include <openssl/sha.h> //hashage SHA256

int generate(char *dic, char *out) { //2 paramètres pointeur vers le fichier de lecture et d'écriture
    FILE *open = fopen(dic, "r"); //Pointe vers l'objet type FILE, et ouvre en mode lecture
    if (open == NULL) { //Verifie si le fichier manque
        fprintf(stderr, "fopen %s: ", dic); //Imprime le chemin en sortie
        perror(NULL); //Avec le message d'erreur système
        return 1;
    }

    FILE *write = fopen(out, "w"); //Ouvre en mode écriture cette fois-ci
    if (write == NULL) {
        fprintf(stderr, "fopen %s: ", out);
        perror(NULL);
        fclose(open); //Ferme le fichier
        return 1;
    }

    char *linePtr = NULL; //Buffer qui pointe chaque ligne du dictionnaire par getline
    size_t  lineSize = 0; //Indique la taille en octet avec getline

    while (getline(&linePtr, &lineSize, open) != -1) { //Charge les lignes du dictionnaire en mémoire
        if (linePtr[0] == '\0' || linePtr[0] == '\n') { // Continue si la igne es vide
            continue;
        }

        unsigned char digest[32]; //Format du hash en 32 octets
        SHA256((unsigned char*)linePtr, strlen(linePtr), digest); //Hashage du buffer en binaire

        const char *H = "0123456789abcdef"; //Encode en hex avec la liste des caractères
        char hexd[65]; // 64 chars pour 32 octets +1 pour le '\0'
        for (int i = 0; i < 32; i++) { //Boucle pour chaque octet 
            hexd[i * 2] = H[digest[i] >> 4]; //Nibble haut >> on garde la gauche
            hexd[i * 2 + 1] = H[digest[i] & 15]; //Nibble bas, on garde la droite
        }
        hexd[64]='\0'; //Termine la chaine avec le flag'\0'

        fprintf(write, "%s\t%s", hexd, linePtr); //Ecriture au format hex, tab, puis texte clair et saut
    }
    free(linePtr); //Libre le pointer buffer
    fclose(open); //Ferme le fichier
    fclose(write); //Arrête d'écrire
    return 0;
}

int lookup(char *table, char *hashes) { //2 paramètres pointeurs vers les fichiers à comparer
    FILE *open = fopen(table, "r"); //Ouvre le fichier en lecture
    if (open == NULL) { //Si le fichier manque 
        fprintf(stderr, "fopen %s: ", table); //Il imprime le chemin
        perror(NULL);//Avec l'erreur système
        return 1;
    }

    size_t tableSize = 1024; //Taille aléatoire donnée à la table en octet
    size_t lineCount = 0; //Counter pour le nombre de ligne
    char **hash1 = malloc(tableSize * sizeof(char*)); //Pointeurs vers pointeurs
    if (hash1 == NULL) { //Erreur d'allocation memoire
        perror("malloc hash1"); //Imprime le message d'erreur
        fclose(open); //On ferme le fichier
        return 1;
    }

    char *linePtr = NULL; //Le pointeur buffeur
    size_t lineSize = 0; //La taille en octet du hash

    while (getline(&linePtr, &lineSize, open) != -1) { //Chargement en mémoire des hash de la table
        char *tab = strchr(linePtr, '\t'); //Cherche la tabulation
        if(tab == NULL) {//ignore si manquante
            continue; 
        }
        *tab = '\0'; //Coupe la chaine et passe a l'autre ligne

        size_t hashLen = strlen(linePtr); //La taille en octet des hash
        if (hashLen == 0 || hashLen >= 65) { //Ignore les hors formats
            continue;
        }

        if (lineCount == tableSize) { //Lorsqu'on a atteint les 1024 lignes
            tableSize *= 2; //On double la taille de Table
            char **hash2 = realloc(hash1, tableSize * sizeof(char*));//Et on alloue dynamiquement la mémoire
            if (hash2 == NULL) {
                perror("realloc");
                free(linePtr);
                fclose(open);
                return 1;
            }
            hash1 = hash2;
        }

        hash1[lineCount] = malloc (hashLen + 1); //Allocation mémoire pour copier le hash, +1 pour '\0'
        if (hash1[lineCount] == NULL) { //Si erreur
            perror("malloc hash1[count]");
            for (size_t k = 0; k < lineCount; k++) {
                free(hash1[k]); //On libère les hashes copiés
            }
            free(linePtr); //On libère le pointeur
            fclose(open); //On ferme le fichier
            return 1;
        }
        strcpy(hash1[lineCount], linePtr); //Sinon on copy le hash à l'adresse allouée
        lineCount++; //On le compte
    }
    free(linePtr); //On libère le buffeur
    fclose(open); //On ferme la table



    FILE *read = fopen(hashes, "r"); //On ouvre en lecture le ficher de hashes à comparer
    if (read == NULL) { //Si erreur
        fprintf(stderr, "fopen %s: ", hashes); //On imprime le chemin du fichier
        perror(NULL); //Avec l'erreur système
        for (size_t i = 0; i < lineCount; i++) {
            free(hash1[i]); //On libère les hash
        }
        free(hash1); //On libère le buffer
        return 1;
    }


    char buffer[65]; //Format du buffer en 64 bytes +1 '\0'
    int found = 0; //Counter de trouvailles
    while (fgets(buffer, sizeof(buffer), read)) { //Prendre chaque ligne du fichier de hashes
        size_t hashLen = strlen(buffer); //Prends le nombre de caractère lu
        
        while (hashLen && (buffer[hashLen - 1] == '\n' || buffer[hashLen - 1] == '\r')) {
            --hashLen; //Retire le saut
            buffer[hashLen] = '\0';
        }
        if (hashLen == 0) { //Ignore si ligne vide
            continue;
        }

        for (size_t i = 0; i < lineCount; i++) { //Cherche parmi toutes les lignes de la tables
            if (strcmp(buffer, hash1[i]) == 0) { //Une correspondance exacte 
                printf("%s\n", buffer); //L'affiche en sortie standard
                found++; //Augmente le counter
                break; //Stop la boucle
            }
        }
        //printf("\n"); //À dé-commenter pour débugger
    }
    printf("Hashes found: %d\n", found); //Affiche le nombre de hits
    fclose(read); //Ferme le fichier
    
    for(size_t i = 0; i < lineCount; i++) {
        free(hash1[i]); //Libère l'espace alloué pour les hash de la table
    }
    free(hash1); //Puis le pointeur
    return 0;
}

int main(int argc, char **argv) {
    if (argc == 4 && strcmp(argv[1], "generate") == 0) { //Si la syntax de commande G est réussie
        return generate(argv[2], argv[3]); //Elle active le mode G avec le chemin du dictionnaire et la table
    }
    else if (argc == 4 && strcmp(argv[1], "lookup") == 0) { //Si la syntax de la commande L est réussie
        return lookup(argv[2], argv[3]); //Elle active le mode L avec le chemin de la table et des hashes
    }
    else { //Sinon
        fprintf(stderr, //Elle imprime la notice d'usage en cas d'erreur
            "Usage:\n"
            "   %s generate <dictionnaire.txt> <table.t3c>\n"
            "   %s lookup <table.t3c> <hashes.txt>\n",
            argv[0], argv[0]);
        return 1;
    }
}
