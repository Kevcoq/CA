#include <iostream>
#include <OPLabel.h>
#include <Program.h>
#include <OPExpression.h>
#include <OPRegister.h>
#include <OPImmediate.h>
#include <Cfg.h>

/* 
   parsing d'un fichier assembleur 
   affichage du programme
   calcul des fonction, affichage du nombre de fonction
   EXEMPLE DE TEST des méthodes que vous devez coder
*/

int main(int argc, char ** argv){
  if (argc < 2){
    cerr << "pas de fichier assembleur en paramètre, pb !" << endl;
    return -1;
  }
  Program p2(argv[1]) ;
  p2.display() ;

 

  /* Test DU CALCUL DES FONCTIONS */
  /* Calcul des fonctions contenues dans le fichier parsé */
 
  
  p2.comput_function();
  cout << "dans le fichier " << argv[1]  << " nb function : " << p2.nbr_func() << endl;
  /*
    for(int i = 0; i < p2.nbr_func(); i++) {
    cout << "Function n° " << i << " :\n";
    p2.get_function(i)->display();
    }
  */
  
  /* TEST DU CALCUL DES BB d'une fonction */
  /* Pour tester le calcul des BB d'une fonction */ 
  Function * fct = p2.get_function(2);
  fct -> comput_basic_block(); 
  fct -> display();
  for(int i = 0; i < fct->nbr_BB(); i++) {
    cout << "Block n° " << i << " :\n";
    fct->get_BB(i)->display();
    cout << "\n";
  }
  
  
  /* EXEMPLE DE TEST */
  /* Pour tester le calcul des blocs de bases puis du calcul des BB pred/succ des BB */ 
  

  /* 

     Function * fct = p2.get_function(0);
     fct -> comput_basic_block(); 
     fct -> comput_succ_pred();
     fct -> test(); 

  */


  /* TEST DE LA CONSTRUCTION DES CFG et donc des succ/pred des BB */
  /*  if (p2.nbr_func() > 0){
      p2.get_function(0)->display();
      p2.comput_CFG();
      Cfg * cfg = p2.get_CFG(p2.nbr_func()>2?2:0);
      cfg -> restitution(NULL,"./tmp/graph2.dot");
      }
  */
  return 0;
}
