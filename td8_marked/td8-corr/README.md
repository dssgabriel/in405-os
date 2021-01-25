# Description
L'archive td8-correction.tar contient les fichiers suivants :
- le fichier README.md, que vous êtes en train de lire ;
- le fichier lottery.cfg, fichier de configuration exemple que vous pouvez
  donner au serveur pour lancer une lotterie, sa description est donnée
  plus bas ;
- le fichier scenario-test.sh (exécutable), script de test effectuant un
  scénario simple ;
- les fichiers sources/en-tête se_fichier.{c/h}, issus du TP sur la
  bibliothèque du système de fichiers ;
- le fichier source million.c, répondant au projet ;
- le fichier compile.sh (exécutable), script compilant les sources.

# Petite explication sur la modification du script scenario-test.sh
La condition suivante est effectuée au début du test :
```
while [ ! -p "tube_in" ]; do
    :
done
```
Elle permet de tester que le tube nommé "tube_in" existe avant de commencer
l'exécution des clients. Le test '-p' vérifie l'existent du tube donné.
L'instruction ':' équivaut à une non-instruction. C'est un poil plus propre
que de faire un 'sleep 1' dans la boucle (car n'a pas besoin d'attendre une
seconde pleine avant de continuer), mais pourrait être améliorée par un
mécanisme de notification. Pour ceux que ça intéresse, inotifywait permet
d'être prévenu lorsqu'une modification intervient dans le système de
fichiers.

Evidemment, cette condition ne pouvait pas être directement écrite dans ce
test, ne connaissant pas le nom des tubes que vous alliez utilisés.
