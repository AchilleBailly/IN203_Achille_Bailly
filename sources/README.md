

On voit ainsi que c'est l'affichage qui prend la majorité du temps de boucle, la mise à jour de la grille étant en seconde position (il faut parcourir toute la grille dans les deux cas). Compilé et exécuté avec MPI, le programme est plus lent.

On voit ici une nette dégradation des performances, environ 6 fois plus lent sur la mise à jour de la grille compilé avec g++ et 1.5 fois plus lent compilé avec MPI. Cependant, compilé avec MPI, une itération entière prend plus ou moins le même temps, le fait d'afficher avec un processus différent et que la transmission soit rapide rattrape le temps perdu sur la mise à jour de la grille. 

**2.3**
Itération du process de simu : 0.201068\n
Premier jour de l'année : 0.0196799\n
Premier jour de grippe : 2.1553e-05\n
Calcul des stats de la grille : 0.00985157\n
Mise a jour de la grille : 0.186356\n
Envoi des données au process 0 : 0.00280543\n
Itération du process d'affichage : 0.202634\n
Affichage : 0.0615294\n
Réception des données : 0.13537\n


**400'000 individus**
Code de base:
Itération totale : 0.246968
Premier jour de l'annee : 0.00521354
Premier jour de grippe : 5.208e-06
Calcul des stats de la grille : 0.00450749
Mise a jour de la grille : 0.120676
Affichage de la grille : 0.120714

MPI affichage synchrone : 
Itération du process de simu : 0.15339
Premier jour de l'année : 0.00391897
Premier jour de grippe : 6.026e-06
Calcul des stats de la grille : 0.00630914
Mise a jour de la grille : 0.139991
Envoi des données au process 0 : 0.00572637
Itération du process d'affichage : 0.151809
Affichage : 0.142656
Réception des données : 0.00795232

MPI affichage asynchrone :
Itération du process de simu : 0.134339
Premier jour de l'année : 0.00325466
Premier jour de grippe : 5.465e-06
Calcul des stats de la grille : 0.00504678
Mise a jour de la grille : 0.127636
Envoi des données au process 0 : 0.000450489
Itération du process d'affichage : 0.285295
Affichage : 0.129629
Réception des données : 0.15462

Affichage asynchrone et accélération de la simu avec MPI (8 process de calcul) :
Itération du process de simu : 0.0802605
Premier jour de l'année : 0.00167606
Premier jour de grippe : nan
Calcul des stats de la grille : 0.0359114
Mise a jour de la grille : 0.0425457
Envoi des données au process 0 : 0.00186085
Itération du process d'affichage : 0.382146
Affichage : 0.297416
Réception des données : 0.0847291

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
Itération du process de simu : 0.0306368
Premier jour de l'année : 0.000487967
Premier jour de grippe : 3.56e-06
Calcul des stats de la grille : 0.000849692
Mise a jour de la grille : 0.0294136
Envoi des données au process 0 : 0.000103006
Itération du process d'affichage : 0.0905171
Affichage : 0.0327399
Réception des données : 0.057639

MPI affichage asynchrone et OpenMP :
Itération du process de simu : 0.0313223
Premier jour de l'année : 0.000567896
Premier jour de grippe : 3.509e-06
Calcul des stats de la grille : 0.000890298
Mise a jour de la grille : 0.0300498
Envoi des données au process 0 : 8.9018e-05
Itération du process d'affichage : 0.0919358
Affichage : 0.033269
Réception des données : 0.0585272

Affichage asynchrone et accélération de la simu avec MPI :
Itération du process de simu : 0.019731
Premier jour de l'année : 6.4211e-05
Premier jour de grippe : nan
Calcul des stats de la grille : 0.0133492
Mise a jour de la grille : 0.0059761
Envoi des données au process 0 : 0.00126903
Itération du process d'affichage : 0.089298
Affichage : 0.0703314
Réception des données : 0.0189663
