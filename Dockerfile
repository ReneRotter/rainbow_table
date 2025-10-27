# Choisi l'environnement Linux: Debian
FROM debian:stable-slim

# MAJ la liste des paquets et installe:
# GDB, Valgrind, build-essential, l'utilitaire pkg-config et la librairie OpenSSL 
RUN apt-get update && apt-get install -y --no-install-recommends build-essential gdb valgrind pkg-config libssl-dev && rm -rf /var/lib/apt/lists/*

# Dossier de travail du conteneur
WORKDIR /rainbow_table

# Lance le shell dans le conteneur
CMD ["/bin/bash"]