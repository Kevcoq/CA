# engendre par ml2mips 
.text


/**
 *  de'claration de la fonction succ___1
 *    vue comme la classe : MLfun_succ___1
 */ 
class MLfun_succ___1 extends MLfun {

  private static int MAX = 1;

  MLfun_succ___1() {super();}

  MLfun_succ___1(int n) {super(n);}

  public MLvalue invoke(MLvalue MLparam){
    if (MLcounter == (MAX-1)) {
      return invoke_real(MLparam);
    }
    else {
      MLfun_succ___1 l = new MLfun_succ___1(MLcounter+1);l.MLaddenv(MLenv,MLparam); return l;
    }
  }


  MLvalue invoke_real(MLvalue x___2) {
T___3addiu $29 $29 -4
T___3lw $11 0($29)
add $3 $0 $11
addiu $29 $29 -4
addi $12 $0 1
sw $12 0($29)
lw $11 0($29)
add $3 $0 $11
addiu $29 $29 -4
addiu $29 $29 -4
lw $2 0($29)
j $31
  }

}
// fin de la classe MLfun_succ___1

  static MLvalue succ___1= new MLfun_succ___1(1);
  static MLvalue value___5;T___6addiu $29 $29 -4
T___6lw $11 0($29)
add $3 $0 $11
addiu $29 $29 -4
addi $12 $0 2
sw $12 0($29)
lw $11 0($29)
add $3 $0 $11
value___5((MLfun)addiu $29 $29 -4
).invoke(addiu $29 $29 -4
)value___5lw $11 0($29)
add $3 $0 $11
bidon___8addiu $29 $29 -4
bidon___8lw $11 0($29)
add $3 $0 $11
