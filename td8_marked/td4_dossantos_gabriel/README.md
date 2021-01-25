# Description
Le dossier td8-m contient les fichiers suivants :
- le fichier README.md, que vous êtes en train de lire.
- le fichier lottery.cfg, fichier de configuration exemple que vous pouvez
  donner au serveur pour lancer une lotterie, sa description est donnée
  plus bas.
- le fichier scenario-test.sh (exécutable), script de test effectuant un
  scénario simple.
- le fichier Makefile permettant la compilation et l'exécution de l'application "million".
- les fichiers "se_fichier.c" et "se_fichier.h", bibliothèque pour l'ouverture/fermeture de fichier ainsi que la la lecture/écriture d'entiers, caractères et chaînes de caractères dans fichiers.
- le fichier "million.c", code source de l'application "million"

# Utilisation
Pour utiliser directement le script de test, veuillez utiliser la commande suivante :
```
$ make
```

Vous pouvez également utiliser les différentes règles du fichier Makefile :
- pour l'exécution du serveur uniquement :
```
$ make server
```
- pour l'exécution du client uniquement : 
```
$ make client
```
- pour la compilation du fichier "million.c" :
```
$ make compile
```
- pour supprimer les exécutables, fichiers objets et fichiers headers pré-compilés :
```
$ make clean
```
- pour créer une archive .tar de ce dossier : 
```
$ make archive
```

# Configuration
Le fichier de configuration est écrit comme suit :
```
4                   # nombre de numéros
6 25 18 39          # numéros gagnants
4 9865              # gain pour 4 chiffres
3 345               # gain pour 3 chiffres
2 19                # gain pour 2 chiffres
1 2                 # gain pour 1 chiffre
```
