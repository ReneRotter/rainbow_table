#define _GNU_SOURCE //extention GNU pour getline
#include <stdio.h> //FILE*, fopen, fprintf, perror
#include <stdlib.h> //malloc, realloc, free
#include <string.h> //strlen, strcmp, strchr, strcpy
#include <openssl/sha.h> //hashage SHA256

typedef struct { //On crée une structure pour pouvoir afficher en plus du
    char *hash; //hash
    char *text; //le text en clair egalement lors du lookup
} HashText; //et on nomme la structure HashText

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

int compare_hashes(const void *a, const void *b) { //On crée la fonction pratique de comparaison pour qsort et bsearch
    const HashText *ptrA = (const HashText*)a; //param a c'est un pointeur vers struct HashText
    const HashText *ptrB = (const HashText*)b; //idem pour b
    return strcmp(ptrA->hash, ptrB->hash); //et on compare seulement les has entre structures
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
    HashText *hash1 = malloc(tableSize * sizeof(HashText)); //Pointeurs vers pointeurs
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
            HashText *hash2 = realloc(hash1, tableSize * sizeof(HashText));//Et on alloue dynamiquement la mémoire
            if (hash2 == NULL) {
                perror("realloc");
                for (size_t k = 0; k < lineCount; k++) { //pour chaque ligne parcouru
                    free(hash1[k].text); //on libere les hash copiés
                    free(hash1[k].hash); //et les texts copiés
                }
                free(hash1); //on libere le pointeur vers structure
                free(linePtr); //le pointeur buffer
                fclose(open); //et on ferme le fichier
                return 1;
            }
            hash1 = hash2;
        }

        char *text = tab + 1; //on défini un pointeur qui pointe vers le text clair apres le tab
        size_t textLen = strlen(text); //et on prend son nombre de caracteres

        hash1[lineCount].hash = malloc (hashLen + 1); //Allocation mémoire pour copier le hash, +1 pour '\0'
        if (hash1[lineCount].hash == NULL) { //Si erreur
            perror("malloc hash");
            for (size_t k = 0; k < lineCount; k++) {
                free(hash1[k].hash); //On libère les hashes copiés
                free(hash1[k].text); //idem pour les texts clairs copiés
            }
            free(hash1); //On libère le pointeur vers struct
            free(linePtr); //On libère le pointeur
            fclose(open); //On ferme le fichier
            return 1;
        }
        strcpy(hash1[lineCount].hash, linePtr); //Sinon on copy le hash à l'adresse allouée

        hash1[lineCount].text = malloc(textLen + 1); //on alloue la taille necessaire pour le text clair
        if (hash1[lineCount].text == NULL) { //si erreur d'allocation
            perror("malloc text"); //msg d'erreur et
            free(hash1[lineCount].hash); //on libere le hash qu'on vient de stocker
            for (size_t k = 0; k < lineCount; k++) { //et pour chauqe ligne en mémoire
                free(hash1[k].hash); //on libère le pointeur vers le hash
                free(hash1[k].text); //idem vers le text clair
            }
            free(hash1); //on libere ensuite le pointeur vers struct
            free(linePtr); //le pointeur buffer
            fclose(open); //et on ferme la table
            return 1;
        }
        strcpy(hash1[lineCount].text, text); //on copie le text en clair dans l'espace memoire alloué

        lineCount++; //On le compte
    }
    free(linePtr); //On libère le buffeur
    fclose(open); //On ferme la table


    qsort(hash1, lineCount, sizeof(HashText), compare_hashes); //on trie le tableau de hash en mémoire pour bsearch

    FILE *read = fopen(hashes, "r"); //On ouvre en lecture le ficher de hashes à comparer
    if (read == NULL) { //Si erreur
        fprintf(stderr, "fopen %s: ", hashes); //On imprime le chemin du fichier
        perror(NULL); //Avec l'erreur système
        for (size_t i = 0; i < lineCount; i++) {
            free(hash1[i].hash); //On libère les hash
            free(hash1[i].text); //idem pour le text en clair
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

        HashText search_text; //on créer une struct temp
        search_text.hash = buffer; //et défini le buffer pour le hash qu'on cherche
        search_text.text = NULL; //pas besoin de .text, bsearch compare que les .hash

        HashText *result = bsearch(&search_text, hash1, lineCount, sizeof(HashText), compare_hashes); //on implement la fonction de recherche binaire
        if (result != NULL) { //si la bsearch trouve un match dans la table, et est donc non-NULL
            printf("%s\t%s", result->hash, result->text); //on l'affiche en sortie standard
            found++; //et on augmente le counter
        }

        //printf("\n"); //À dé-commenter pour débugger
    }
    printf("Hashes found: %d\n", found); //Affiche le nombre de hits
    fclose(read); //Ferme le fichier
    
    for(size_t i = 0; i < lineCount; i++) {
        free(hash1[i].hash); //Libère l'espace alloué pour les hash de la table
        free(hash1[i].text); //et celui alloué pour le text clair
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
