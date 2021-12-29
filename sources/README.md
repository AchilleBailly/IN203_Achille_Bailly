**2.1 Mesure du temps**
La version initiale a le profil de temps suivant avec affichage:
Itération totale : 0.0666084
Premier jour de l'annee : 0.0009495
Premier jour de grippe : 5e-06
Calcul des stats de la grille : 0.00118092
Mise a jour de la grille : 0.0244454
Affichage de la grille : 0.0406821

Sans affichage :
Itération totale : 0.024639
Premier jour de l'annee : 0.000728667
Premier jour de grippe : 3e-06
Calcul des stats de la grille : 0.00107131
Mise a jour de la grille : 0.0233648
Affichage de la grille : 4.75561e-08

On voit ainsi que c'est l'affichage qui prend la majorité du temps de boucle, la mise à jour de la grille étant en seconde position (il faut parcourir toute la grille dans les deux cas).

**2.2 Parallélisation aﬀichage contre simulation**
Avec cette version, on obtient le profil suivant :
