**Temps initiaux**
Dans un premier temps, on a les temps suivants pour tiny et small Lena Gray:
Temps de transformation : 1.47106
Temps de sélection des coef : 0.000316068
Temps de reconstitution : 0.159491
Temps total : 1.63087

Temps de transformation : 245.811
Temps de sélection des coef : 0.00628627
Temps de reconstitution : 28.0967
Temps total : 273.91

**Parallélisation avec OpenMP**
Le plus logique est dans ce cas là de d'abord paralléliser la boucle la plus haute dans la DFT et dans la reconstitution.
Dans le premier cas, on parallélise en fréquence et dans le deuxième en ligne de pixels.
Pour tiny Lena Gray on obtient les temps suivant:
Temps de transformation : 0.421625
Temps de sélection des coef : 0.000357787
Temps de reconstitution : 0.0444893
Temps total : 0.466473

Et small Lena Gray:
Temps de transformation : 67.9423
Temps de sélection des coef : 0.00632136
Temps de reconstitution : 7.61873
Temps total : 75.5673

Soit un accélération de 3.5x et 3.6x respectivement, avec 4 treads (le plus optimal sur cet ordi qui a 4 coeurs).
C'est tout à fait logique d'obtenir ces temps, 4 threads font le travail en même donc modulo le thread management d'OpenMP, l'accélération est de l'ordre de 4.
Plus l'image serait grande, plus l'accélération s'approcherait de 4.


**Première parallélisation avec MPI**
Avec 2, 4, 8, 16 processus on a les temps suivants (pour tiny Lena Gray):
Temps de transformation : 0.706209
Temps du MPI_Reduce : 0.0751397
Temps de sélection des coef : 0.000375361
Temps de reconstitution : 0.169059
Temps total : 0.950783

Temps de transformation : 0.324642
Temps du MPI_Reduce : 0.0913802
Temps de sélection des coef : 0.000397078
Temps de reconstitution : 0.16917
Temps total : 0.58559

Temps de transformation : 0.255765
Temps du MPI_Reduce : 0.153635
Temps de sélection des coef : 0.00035737
Temps de reconstitution : 0.172185
Temps total : 0.581942

Temps de transformation : 0.197565
Temps du MPI_Reduce : 0.396376
Temps de sélection des coef : 0.00033802
Temps de reconstitution : 0.170551
Temps total : 0.76483

Sur la DFT, cela donne ainsi les accélérations suivantes : 2x, 4.6x, 5.6x, 7.35x
On voit que l'accélération continue d'augmenter avec un nombre de processus qui augmente aussi, cependant, le temps du Reduce augmente aussi, ce qui finit par contrebalancer l'accélération du programme total.
Sur le programme entier on a les accélérations suivantes : 1.7x, 2.8x, 2.8x, 2.1x.
Sur small Lena Gray, l'accélération globale est de 2.5x avec 16 proc, et de 2.7x avec 8. On retrouve encore les mêmes pertes que pour tiny, avec le reduce qui desavantage grandement les 16 proc.
