Voici les temps mesurés pour les différentes méthodes et pour 2 tailles de population différentes.

**100'000 individus**  
Code de base:  
Itération totale : 0.0613127  
Premier jour de l'annee : 0.000907763  
Premier jour de grippe : 4.219e-06  
Calcul des stats de la grille : 0.000950018  
Mise a jour de la grille : 0.0273156  
Affichage de la grille : 0.0327578  
  
MPI affichage synchrone :  
Itération du process de simu : 0.0383562  
Premier jour de l'année : 0.00131696  
Premier jour de grippe : 4.898e-06  
Calcul des stats de la grille : 0.00121465  
Mise a jour de la grille : 0.032493  
Envoi des données au process 0 : 0.00427575  
Itération du process d'affichage : 0.0371761  
Affichage : 0.0358049  
Réception des données : 0.00122627  
  
MPI affichage asynchrone :  
Itération du process de simu : 0.0306368.  
Premier jour de l'année : 0.000487967  
Premier jour de grippe : 3.56e-06.  
Calcul des stats de la grille : 0.000849692.  
Mise a jour de la grille : 0.0294136.  
Envoi des données au process 0 : 0.000103006.  
Itération du process d'affichage : 0.0905171.  
Affichage : 0.0327399.  
Réception des données : 0.057639.  
  
MPI affichage asynchrone et OpenMP :  
Itération du process de simu : 0.0313223.  
Premier jour de l'année : 0.000567896.  
Premier jour de grippe : 3.509e-06.  
Calcul des stats de la grille : 0.000890298.  
Mise a jour de la grille : 0.0300498.  
Envoi des données au process 0 : 8.9018e-05.  
Itération du process d'affichage : 0.0919358.  
Affichage : 0.033269.  
Réception des données : 0.0585272.  
  
Affichage asynchrone et accélération de la simu avec MPI :  
Itération du process de simu : 0.019731.  
Premier jour de l'année : 6.4211e-05.  
Premier jour de grippe : nan.  
Calcul des stats de la grille : 0.0133492.  
Mise a jour de la grille : 0.0059761.  
Envoi des données au process 0 : 0.00126903.  
Itération du process d'affichage : 0.089298.  
Affichage : 0.0703314.  
Réception des données : 0.0189663.  
  
**400'000 individus**  
Code de base:  
Itération totale : 0.246968.  
Premier jour de l'annee : 0.00521354.  
Premier jour de grippe : 5.208e-06.  
Calcul des stats de la grille : 0.00450749.  
Mise a jour de la grille : 0.120676.  
Affichage de la grille : 0.120714.  
  
MPI affichage synchrone :   
Itération du process de simu : 0.15339.  
Premier jour de l'année : 0.00391897.  
Premier jour de grippe : 6.026e-06.  
Calcul des stats de la grille : 0.00630914.  
Mise a jour de la grille : 0.139991.   
Envoi des données au process 0 : 0.00572637.  
Itération du process d'affichage : 0.151809.  
Affichage : 0.142656.  
Réception des données : 0.00795232.  
  
MPI affichage asynchrone :  
Itération du process de simu : 0.134339.  
Premier jour de l'année : 0.00325466.  
Premier jour de grippe : 5.465e-06.  
Calcul des stats de la grille : 0.00504678.  
Mise a jour de la grille : 0.127636.  
Envoi des données au process 0 : 0.000450489.  
Itération du process d'affichage : 0.285295.  
Affichage : 0.129629.  
Réception des données : 0.15462.  
  
Affichage asynchrone et accélération de la simu avec MPI (8 process de calcul) :  
Itération du process de simu : 0.0802605.  
Premier jour de l'année : 0.00167606.  
Premier jour de grippe : nan.  
Calcul des stats de la grille : 0.0359114.  
Mise a jour de la grille : 0.0425457.  
Envoi des données au process 0 : 0.00186085.  
Itération du process d'affichage : 0.382146.  
Affichage : 0.297416.  
Réception des données : 0.0847291.  

**2.1**  
On voit que dans le code de base, c'est surtout l'affichage et la mise à jour de la grille (comprendre itération sur la population) qui prennent la majorité du temps d'exécution.  
  
**2.2**  
En déplaçant l'affichage sur un autre processus en synchrone, on observer une accélération de 1.6x, ce qui est tout à fait logique. En effet, sur le processus de simulation, on ne s'arrête plus de calculer pour afficher mais seulement pour envoyer les données, et pendant qu'on calcul l'étape suivante, l'autre processus affiche. On gagne donc le temps d'affichage qui représentait une bonne moitié de l'itération.  
  
**2.3**  
Ici, on n'envoie au processus d'affichage si et seulement il est prêt à recevoir les données, on n'attends donc pas qu'il ait fini d'afficher pour continuer les calculs. On est ainsi 1.8x à 2x plus rapide que le code de base, et 1.15x à 1.25x plus rapide que la version précédente. Cela ne fonctionne seulement parce que l'affichage est légèrement plus long que le calcul, il arrive donc assez souvent que le calcul suivant finisse avant la fin de l'affichage précédent, d'oú le léger gain de temps.  
  
**2.4**  
Malheureusement, étant sur Mac, j'ai encore des problèmes pour faire fonctionner OpenMP et MPI en même temps. OMP s'importe bien mais reste bloquée à un thread (peut se voir avec omp_get_num_threads()). Je peux compiler sans mpic++ mais avec g++ ou clang++, mais dans ce cas, il y a des pertes énorme de performances à des endroits bizarres.  
  
**2.5**  
Ici, on partage la population entre plusieurs processus. Il a fallu modifier légèrement le code de l'agent pathogène et de la grippe pour avoir un moteur aléatoire qui vienne de l'individu, ainsi, l'ordre de mise à jour des individus n'a pas d'influence. Cette version avec 8 process de calculs (9 en tout), est 3.12x plus rapide que le code de base avec 100'000 et 3.08x avec 400'000.  
Pour 400'000 individus on a les accélérations suivantes :  
2 processus : 1.57x  
3 processus : 2.47x  
5 processus : 3.1x  
9 processus : 2.7x  
17 processus : 2.3x  
  
Pour 100'000 individus par processus on a les temps suivants :  
2 processus : 2.7x  
3 processus : 3.2x  
4 processus : 3.1x  
5 processus : 2.5x  
6 processus : 2.9x  
7 processus : 3.0x
  
L'accélération à nombre d'individu par processus constant semble plus ou moins constante.  
Encore une fois, la partie OMP combiné avec MPI n'est pas faisable sur mon ordi en l'état, et étant chez moi, celle en utilisant plusieurs machines non plus.  
  
**2.5.2**  
J'ai eu du mal à avoir des résultats cohérents au début avec le makefile que j'avais fait, mais après l'avoir modifié tout a fait beaucoup plus de sens même si OpenMp ne fonctionnait plus. Il est très certainement possible d'avoir des accélérations encore meilleures avec OpenMP, mais déjà avec MPI seul, avoir un programme 3 fois plus rapide est impressionnant.  
Ce projet m'a permit de découvrir un peu plus les Makefile, ce qui sera très certainement utile par la suite. L'affichage en asynchrone m'a surpris, je ne pensais pas que ça pouvait faire tant de différence. Enfin, le challenge de retrouver les mêmes résultats avec plusieurs processus de simulation était assez casse-tête mais force à bien comprendre la parallélisation.



 
