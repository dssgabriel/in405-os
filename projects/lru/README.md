# TP final 11.1
Dos Santos Gabriel   
Student #21807297
TD4

## Description
L'objectif de cet exercice est d'implémenter un programme dans lequel des threads fils envoient au thread père des demandes d'accès à la mémoire, par le biais d'adresses physiques. Le thread père répond aux fils lorsque la page contenant la mémoire demandée est disponible, en donnant l'adresse logique correspondante. Chaque thread fils va faire un nombre de demandes avant que le programme ne s'arrête.
Le programme est écrit dans un contexte multi-thread. Le processus père lit dans un fichier de paramétrage les informations concernant la configuration à utiliser, décrit ci-après. En fin d'exécution, il donne pour chaque thread fils, le pourcentage de _hit_ observés sur le nombre total de demande.
L'algorithme utilisé est le LRU (_Least Recently Used_), disponible dans les slides du CM. Le seul paramètre pris en compte est le nombre de pages qu'il peut stocker en mémoire rapide (_frames_).

Le fichier de paramétrage est constitué des informations suivantes, mises sur une seule ligne. Un exemple est donné ci-dessous.
- Nombre de _frames_
- Taille d'une page mémoire _s_
- Nombre de pages disponibles en mémoire lente _n_ (indices allant de 0 à _n_ non compris)
- Nombre de threads fils
- Nombre d'accès demandés par thread

**Exemple du fichier de configuration**

```
4 4096 256 10 100
```

## Compilation
Le programme est compilé à l'aide du Makefile :
```bash
$ make
```
Vous pouvez également compiler séparemment à l'aide de la règle `compile`:
```bash
$ make compile
$ ./main
```

### Notes
- Le code source utilise la bibliothèque "types.h" pour redéfinir les noms des types de variables sur le modèle du langage Rust. Bien que celui puisse être déroutant, cela m'est utile pour faire plus attention à la taille des variables que j'utilise et ainsi optimiser mon code. C'est aussi plus court de taper 'u8' ou 'i64' plutôt que 'unsigned char' ou 'long long' à chaque déclaration de variables.
- Le code source est commenté en anglais par habitude et n'est pas du copié-collé d'Internet :) Il en est de même pour les erreurs. Vous pouvez également consulter mon GitHub pour voir les dates de commits : 
<https://github.com/dssgabriel/>