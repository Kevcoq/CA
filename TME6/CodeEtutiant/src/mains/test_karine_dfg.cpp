#include <iostream>
#include <OPLabel.h>
#include <Program.h>
#include <OPExpression.h>
#include <OPRegister.h>
#include <OPImmediate.h>
#include <Cfg.h>


int main(int argc, char ** argv){
	Program p2(argv[1]) ;
	p2.display() ;
	p2.comput_function();
	cout << "nb function " << p2.nbr_func();
	Function * fct = p2.get_function(0);
	fct -> comput_basic_block();
	fct -> comput_label();
	fct -> comput_succ_pred_BB();
	if (fct -> nbr_BB() > 0) {
	  Basic_block * BB = fct -> get_BB(0);
	  BB ->display();
	  Dfg * dfg = new Dfg(BB);
	  dfg->restitute(NULL,"./tmp/graph_dfg0.dot", true);
	  cout << "temps critique : "<< dfg->get_critical_path() << endl;
	}
	if (fct -> nbr_BB()  > 1){
	  Basic_block * BB = fct -> get_BB(1);
	  BB ->display();
	  Dfg *  dfg = new Dfg(BB);
	  dfg->restitute(NULL,"./tmp/graph_dfg1.dot", true);
	  cout << "temps critique : "<< dfg->get_critical_path() << endl;
	  
	}
	if (fct -> nbr_BB()  > 2){
	  Basic_block * BB = fct -> get_BB(2);
	  BB ->display();
	  Dfg *  dfg = new Dfg(BB);
	  dfg->restitute(NULL,"./tmp/graph_dfg2.dot", true);
	  cout << "temps critique : "<< dfg->get_critical_path() << endl;
	}
}
