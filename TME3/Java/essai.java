/**
 *  essai.java engendre par ml2java 
 */


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

    { 
      MLvalue T___3;
      MLvalue T___4;
      T___3=x___2;
      T___4=new MLint(1);
      return MLruntime.MLaddint( (MLint )T___3,(MLint )T___4);
    }
  }

}
// fin de la classe MLfun_succ___1
/**
 * 
 */
class essai {

  static MLvalue succ___1= new MLfun_succ___1(1);
  static MLvalue value___5;

public static void main(String []args) {

{ 
  MLvalue T___6;
  MLvalue T___7;
  T___6=essai.succ___1;
  T___7=new MLint(2);
  value___5=((MLfun)T___6).invoke(T___7);
}
{ 
  MLvalue bidon___8;
  bidon___8=MLruntime.MLlrp;
  bidon___8=MLruntime.MLprint( (MLvalue )value___5);
}
}}

// fin du fichier essai.java
