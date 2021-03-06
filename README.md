# IN203_Achille_Bailly
# TP 2
**Question de cours 1**
  Si le processus 2 reçoit d'abord le message du processes 1, le processus 0 ne passera jamais à la ligne suivante car le processus 2 attendra cette ligne pour envoyer le-dit message.
	Il y a une chance sur deux que ça bloque.

**Question de cours 2**
	Elle aura au maximum une accélération de 10. En prenant 10 noeuds de calcul, l'accélération théorique est de 5 environ et l'utilisation sera de 50% pour chaque noeud. Si elle a une accélération de 4, elle devrait avoir 6 noeuds, avec la loi de Gustafon et des données 2 fois plus grandes, cela fait une accélération de 5.75.

**Matvec**
	Etonnemment, aucune des deux méthodes accélère réellement le code, même si le découpage par ligne parait plus naturel et éfficace

# TP3
**1 Produit scalaire**
Changer le nombre de threads ne semble pas affecter la vitesse du produit scalaire.
Pour la version avec std::thread, le plus rapide est avec 1 seul thread, le reste est environ 1.5 et 2 fois plus lent. 
OpenMP reste plus rapide d'un facteur 2 par rapport std::thread.
On est très certainement memory bound et de plus, la version std::thread est probablement trop lourde.

**Produit Matrice-Matrice**
1) Le produit entre 1023/1024 ou 1025/1024 est plus de 4 fois plus lent. Après explications, c'est un problème de mémoire cache, qui stock suivant un modulo de la taille des données. Comme la taille du cache est une puissance de 2, avec un tableau de taille puissance de 2, il stockera chaque block au même endroit et devra donc renvoyer en RAM à chaque fois.

2) Les matrices sont stockées par colonnes, en évitant de faire des sauts de mémoires non contigue, il faut donc que la boucle la plus imbriquée soit celle qui fasse les sauts de mémoire contigue

3) 1 thread : 6.7s
   2 threads : 3.9s
   4 threads : 2.7s
   8 threads : 2.6s
   16 threads : 2.6s
   L'accélération est clairement limitée, il est clair que la vitesse est limtée par les accés mémoires. 

4) On pourrait faire le calcul par blocs, en effet, 

# TP4
**OpenMP et MPI**
Voir Mandelbrot_openmp.cpp
Le meilleur temps obtenu est de 0.27s environ avec 17 threads MPI (16 qui calculent réellement) et et nt = 30. Le temps original était de 6.5s soit une accélération de 24.
Avec TBB et en choisissant aussi nt=30, on obtient 5.3s, changer le nombre de threads et la taille des blocs ne semblent pas impacter grand chose.

**RT avec TBB**
Avec un static_partitionner(), le temps est à chaque fois de 1.58s.
Avec auto_partitionner(), on peut descendre à 1.48s. 
TBB fait bien un équilibrage car ce temps de 1.48s est très stable par rapport à un statict_partitionner() (qui ne fait pas d'équilibrage). 
On n'arrive par ailleurs pas à descendre en dessous de ce nombre.

