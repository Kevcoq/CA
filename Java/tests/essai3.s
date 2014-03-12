# engendre par ml2mips 

.data

Csbonjour :	.asciiz	"bonjour"

.text

#récupération d'une constante
la $11 Csbonjour
sw $11 12($29)
lw $11 12($29)
sw $11 12($29)


# fin du fichier essai3.s
