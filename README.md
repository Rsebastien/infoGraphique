# infoGraphique

# Implémentation de courbes et surfaces de Bézier

## Description

Dans le cadre du projet d'informatique graphique de 1ère année de Master, j'ai développé un outil 
permettant de visualiser des courbes de bézier ainsi que des surfaces de bézier.

## Travail réalisé

Le logiciel source contient un dossier "includes" qui contient les principales librairies utiles et un autre dossier appelé "src".
Ce dossier contient tous les fichiers sources ainsi que deux sous-dossier, un nommé "images" contenant les textures et un autre nommé 
"shaders" contenant les codes des différents shaders utilisés à travers le code.

La classe bezierCurve implémente le calcul des courbes de bézier, bézierSurface la surface de bézier et sa discrétisation (position, normale, coordonnées UV).
La classe Mesh implémente l'initialisation des buffers objects pour les courbes ou la surface ainsi que les fonctions draw. Shader.h s'occupe de
générer le shader program pour les shaders passé en paramètre de la classe ainsi que les fonctions pour définir les valeurs des uniforms dans les shaders.
camera.hpp va implémenter tous les calculs liés au déplacement de la caméra (angles d'Euler, position, zoom, etc). Le fichier main va s'occuper de définir 
la position des points de contrôles, le cube pour les lumières, générer les shaders program, charger les textures, gérer les inputs clavier/souris et tout afficher. 


La discrétisation de l'espace paramètrique pour l'approximation de la courbe de Bézier par des segments de droite uniformes 
dans l'espace paramètrique a été réalisé (le nombre de segments est donné en paramètre de la classe BézierCurve, 50 par défaut).

La visualisation de la courbe ainsi que son polygone est implémentée.

La discrétisation de la surface avec comme attributs la position, la normale et les coordonnées u et v sur les sommets est aussi implémentée.

Visualisation de la surface à l'aide du modèle de Phong (contenant l'ombrage diffus Lambertien comme demandé) ainsi que des
normales aux sommets sur la surface. L'éclairage consiste en 3 lumière dont une liée à l'arrière de la caméra, toutes les lumières
sont atténuées en fonction de la distance (ici distance de 160, voir https://wiki.ogre3d.org/tiki-index.php?page=-Point+Light+Attenuation)
en utilisant le modèle explicité dans le tutoriel LearnOpenGl (chapitre Light casters). Une texture est appliqué à la surface (image de LIC sur une image de bruit que j'ai
récupéré de mon projet) et l'éclairage se fait à l'aide d'une diffuse map et d'une specular map pour les reflets.

La plupart des implémentations ont été réalisées en suivant le tutoriel LearnOpenGl (partie Getting started,
Lighting, Model loading et le chapitre geometry shader de la partie Advanced Opengl pour la visualisation des normales).

## Problèmes

Je n'ai pas encore réussi à implémenter la BRDF demandé ainsi que l'éclairage 3 points lié à la caméra (je n'ai pas vraiment compris
comment lié une lumière à la fois à la direction de la caméra et à sa position tout en la déplaçant dans les coordonnées monde)
et la discrétisation de l'espace paramètrique pour l'approximation de la courbe par des segments de droite
uniformes en longueur de segment (j'ai préféré privilégier d'autres parties que je trouvais plus intéréssantes à développer
dans le temps qu'il me restait).

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

Appuyer sur X permet d'afficher/cacher le maillage triangulaire.

Appuyer sur N permet d'afficher/cacher les normales à la surface.

Appuyer sur C permet d'afficher/cacher les courbes de bézier les points de contrôles.

Appuyer sur V permet d'afficher/cacher la surface.
