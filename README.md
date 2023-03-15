# Projet Informatique Graphique

## Description

Dans le cadre du projet d'informatique graphique de seconde année de Master, j'ai amélioré un outil que j'avais créé 
l'année précédente et qui permet de visualiser des courbes de bézier ainsi que des surfaces de bézier.
Cet outil permet maintenant de visualiser une surface avec une BRDF de Cook-Torrance et un Bloom appliqué sur les 
lumières. La partie linear blend skinning est encore en travaux.


## Installation

Clion : ouvrir le projet, le build et lancer l'éxecutable "main".

En ligne de commande : Se positionner dans le dossier
```bash
mkdir build
cd build
cmake ..
make
./main
```

## Usage

Les touches Z-Q-S-D et la souris permettent de se déplacer dans l'espace.

Appuyer sur N permet d'afficher/cacher les normales à la surface.

Appuyer sur O permet d'enlever le contrôle de la caméra via la souris.

Appuyer sur P permet de rétablir le contrôle de la caméra via la souris.

Appuyer sur ↓ réduit la taille de l'angle de rotation du deuxième os.

Appuyer sur ↑ augmente la taille de l'angle de rotation du deuxième os.

Appuyer sur K, L, M font tourner le deuxième os sur l'axe X, Y, Z respectivement.



