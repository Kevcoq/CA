# engendre par ml2mips 

.data

Ci5 :	.word	5
Ci8 :	.word	8
Ci4 :	.word	4
Ci9 :	.word	9

.text

#récupération d'une constante
la $11 Ci5
sw $11 12($29)
#récupération d'une constante
la $11 Ci8
sw $11 12($29)
# Primitive =
lw $20 0($29)
lw $21 4($29)
beq $20 $21 L1
addi $11 $0 0
j L2
L1 : 
addi $11 $0 1
L2 : 
sw $11 12($29)
lw $10 12($29)
beq $10 $0 L3
#récupération d'une constante
la $11 Ci4
sw $11 12($29)
sw $11 12($29)
j L4
L3:
#récupération d'une constante
la $11 Ci9
sw $11 12($29)
sw $11 12($29)
L4:
lw $11 12($29)
sw $11 12($29)


# fin du fichier essai4.s
