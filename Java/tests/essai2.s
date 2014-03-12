# engendre par ml2mips 

.data

Ci2 :	.word	2
Ci7 :	.word	7

.text

#récupération d'une constante
la $11 Ci2
sw $11 12($29)
#récupération d'une constante
la $11 Ci7
sw $11 12($29)
sw $11 12($29)
sw $11 12($29)
# Primitive +
lw $20 0($29)
lw $21 4($29)
add $11 $20 $21
sw $11 12($29)
lw $11 12($29)
sw $11 12($29)


# fin du fichier essai2.s
