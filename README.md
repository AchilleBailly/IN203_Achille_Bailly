# IN203_Achille_Bailly
# TP 2
**Question de cours 1**
  Si le processus 2 reçoit d'abord le message du processes 1, le processus 0 ne passera jamais à la ligne suivante car le processus 2 attendra cette ligne pour envoyer le-dit message.
	Il y a une chance sur deux que ça bloque.

**Question de cours 2**
	Elle aura au maximum une accélération de 10. En prenant 10 noeuds de calcul, l'accélération théorique est de 5 environ et l'utilisation sera de 50% pour chaque noeud. Si elle a une accélération de 4, elle devrait avoir 6 noeuds, avec la loi de Gustafon et des données 2 fois plus grandes, cela fait une accélération de 5.75.

**Matvec**
	Etonnemment, aucune des deux méthodes accélère réellement le code, même si le découpage par ligne parait plus naturel et éfficace
