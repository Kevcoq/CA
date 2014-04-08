#include <Dfg.h>
	
// crée un arc et le renvoie
Arc_t* new_arc(int del, t_Dep d, Node_dfg *n) {
  Arc_t *arc = new Arc_t;
  arc->delai=del;
  arc->dep=d;
  arc->next=n;
	   
  return arc;
}
	
//rend le delay entre 2 instructions pour la dépendance donnée
int get_delay(t_Dep dep, Instruction *from, Instruction *to) {
  switch(dep){
  case WAW:
    return WAW_DELAY;
  case WAR:
    return WAR_DELAY;
  case MEMDEP:
    return MEMDEP_DELAY;
  case CONTROL:
    return 0;
  case NONE:
    return 0;
  case RAW:
    int ret=t_delay[from->get_type()][to->get_type()];
    if(ret<1)
      return 1;
    return ret;
  }
	   
  return 0;
}
	
// A REMPLIR
Dfg::Dfg(Basic_block *bb){
	  
  list<Arc_t*>::iterator ita;
	
	  
  _bb=bb;
  _index_branch=-1;
  _nb_arc=0;  
  _length=bb->get_nb_inst();
  _read= new int[_length];
  Node_dfg **_noeuds = (Node_dfg**) malloc(sizeof(Node_dfg*)*_length);
	  
  bb->comput_pred_succ_dep();
	  
  // A COMPLETER
  // stop et debut
  Instruction *stop, *itmp;
	
	
  if(bb->is_delayed_slot(bb->get_last_instruction())) {
    stop = bb->get_last_instruction()->get_prev();
    _delayed_slot.push_back(new Node_dfg(stop->get_next()));
    _index_branch = stop->get_index();
  } else {
    stop = bb->get_last_instruction();
  }
	
	
  // parcourt instruction
  for(itmp=bb->get_first_instruction(); ; itmp = itmp->get_next()) {
	
    // noeud      
    Node_dfg *node = new Node_dfg(itmp);
    list_node_dfg.push_back(node);
    _read[itmp->get_index()] = 1;
    _noeuds[itmp->get_index()] = node;
	    
    // prec
    if(itmp->get_nb_pred() == 0) {
      _roots.push_back(node);
    }  
	    
    // parcours dep prec
    for(int i = 0; i<itmp->get_nb_pred(); i++) {
      // delay
      dep *_dep = itmp->get_pred_dep(i); 
      int delay = -1;
      if(_dep->type == WAW) {
	delay = WAW_DELAY;
      } else if(_dep->type == WAR) {
	delay = WAR_DELAY;
      } else if(_dep->type == MEMDEP) {
	delay = MEMDEP_DELAY;
      } else if(_dep->type == RAW) {
	delay = t_delay[_dep->inst->get_type()][itmp->get_type()];
      } else if(_dep->type == CONTROL) {
	delay = 0;
      }
	      
      // ajout
      if(delay != -1) {
	Arc_t *arc = (Arc_t*) malloc(sizeof(Arc_t));
	arc->delai = delay;
	arc->dep = _dep->type;
	arc->next = node;
	_noeuds[_dep->inst->get_index()]->add_arc(arc);
      }
    }
    if(itmp == stop)
      break;
  }
}  
	
// methode auxiliaire pour la construction du Dfg, pas forcément utile, dépend de comment vous envisagez de faire...
void Dfg::build_dfg(Node_dfg*,bool) {
	  
  // A REMPLIR OU NON !
  return;
}
	
Dfg::~Dfg(){}
	
	
void Dfg::display(Node_dfg * node, bool first){
	   
	   
  list<Node_dfg*>::iterator it;
  it=_roots.begin();
	
  if(first)	
    for(int i=0; i<_length; i++)	
      _read[i]=0;  	
	   
  for (unsigned int j=0; j<_roots.size();j++ ){ 
    if(first) node = *it;	
				
	
    if(!_read[node->get_instruction()->get_index()]){
      _read[node->get_instruction()->get_index()]=1;
      cout<<"pour i"<<node->get_instruction()->get_index()<<endl;
      cout<<"l'instruction "<<node->get_instruction()->get_content()<<endl;
				
      //On affiche ses successeurs s'il en a
      for(int i=0;i<node->get_nb_arcs();i++){
	if (node->get_arc(i)){
	  cout<< " -> succ i"
	      << node->get_arc(i)->next->get_instruction()->get_index()
	      << " : "
	      << node->get_arc(i)->next->get_instruction()->get_content()
	      << endl;
	}
      }
      for(int i=0;i<node->get_nb_arcs();i++){
	if (node->get_arc(i))
	  display(node->get_arc(i)->next, false);
      }
    }
    it++;
	
  }
}
	
#define DEP(x) ((x==NONE)?"":((x==RAW)?"raw":				\
			      ((x==WAR)?"war":				\
			       ((x==WAW)?"waw":				\
				((x==MEMDEP)?"mem": "control")))))
	
	
//Pour générer le fichier .dot: dot -Tps graph.dot -o graph.ps
void Dfg::restitute(Node_dfg * node, string const filename, bool first){
  if(first)
    remove(filename.c_str());
  ofstream monflux(filename.c_str(), ios::app);
  list<Node_dfg*>::iterator it;
	 
  if(first && _length){
	     
    for(int i=0; i<_length; i++)
      _read[i]=0;
	      
    it = _delayed_slot.begin();
	      
    int index_min = _length;
	      
    for(unsigned int i=0; i < _delayed_slot.size(); i++){
      _read[(*it)->get_instruction()->get_index()] = 1;
      if (index_min > (*it)->get_instruction()->get_index())
	index_min = (*it)->get_instruction()->get_index();
      it++;
    }
	
    monflux<<"digraph G1 {"<<endl;
    for(int i=0; i<index_min; i++){
      monflux<<"i"<<i<< ";"<<endl;
		 
    }
  }	
  it=_roots.begin();
  for (unsigned int j=0; j<_roots.size();j++ ){ 		
	
    if(first) node = *it;
			
    if(monflux){			
      //monflux.open(filename.c_str(), ios::app);
      if(!_read[node->get_instruction()->get_index()]){
	_read[node->get_instruction()->get_index()]=1;
						
	//On affiche ses successeurs s'il en a
	for(int i=0; i<node->get_nb_arcs(); i++){
	  if (node->get_arc(i)){
		   
	    monflux<<"i"<<node->get_instruction()->get_index();
	    monflux<<" ->  i" << node->get_arc(i)->next->get_instruction()->get_index();
	
	    // monflux<<"i"<<node->get_instruction()->get_index()<<"_"<<node->get_weight();
	    // monflux<<" ->  i" << node->get_arc(i)->next->get_instruction()->get_index();
	    // monflux<<"_"<<node->get_arc(i)->next->get_weight();
	
	    monflux<<" [label= \""<< DEP(node->get_arc(i)->dep) << node->get_arc(i)->delai<<"\"];"<<endl;
	  }
	}
	monflux.close();
		
	for(int i=0;i<node->get_nb_arcs();i++){
	  if (node->get_arc(i))
	    restitute(node->get_arc(i)->next,filename.c_str(),false);	
	}
      }
    }
    if((j+1) < _roots.size())	monflux.open(filename.c_str(), ios::app);
    it++;
  }
	
  if (first && _length){
    monflux.open(filename.c_str(), ios::app);
    monflux<<"}"<<endl;
    monflux.close();
  }
  return;
	 
}
	
bool Dfg::read_test(){
  for(int i=0; i<_length; i++)	if(_read[i]==0)	return true;
  return false;
}
	
	
bool contains(list<Node_dfg*>* l, Node_dfg* n){
  list<Node_dfg*>::iterator it;
	   
  for(it=l->begin(); it!= l->end(); it++){
    if( (*it)==n ){
      return true;
    }
  }
  return false;
}
	
//A FAIRE (fct aux pour comput_critical_path)
int comput_path_aux(Node_dfg* node) {
  // si deja vue
  if(node->get_weight() > 0)
    return node->get_weight();
  
  // sinon
  int max = 1;
  for(int i = 0; i<node->get_nb_arcs(); i++) {
    // on parcourt les arcs
    Arc_t *arc = node->get_arc(i);
    int tmp = comput_path_aux(arc->next);
    
    // si le poid est sup, on remplace
    if(tmp+arc->delai > max)
      max = tmp+arc->delai;
  }
  node->set_weight(max);
  return max;
}


//A FAIRE
void Dfg::comput_critical_path(){
  Node_dfg *end = _delayed_slot.front();
  // init delayed slot
  if(end != NULL) 
    end->set_weight(2);
  
  // parcourt racine
  list <Node_dfg*>::iterator it =_roots.begin();
  for(int i=0; i<_roots.size(); i++, it++)
    comput_path_aux(*it);
  
  
#ifdef DEBUG
  it=list_node_dfg.begin();
  for(unsigned int k = 0; k < list_node_dfg.size(); k++, it++){
    Node_dfg* node = *it;
    cout << "node inst " << node -> get_instruction() -> get_index() << " poids=" << node->get_weight() << " nb succ=" << node->get_nbr_arc() << endl;
  }
#endif
}
	
	
	
// A FAIRE
int Dfg::get_critical_path(){
  comput_critical_path();
  int max = 0;

  list <Node_dfg*>::iterator it =_roots.begin();
  for(int i=0; i<_roots.size(); i++, it++)
    if((*it)->get_weight() > max)
      max = (*it)->get_weight();
 
  return max;
}
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
void  Dfg::scheduling(){
}
	
void Dfg::display_sheduled_instr(){
  list <Node_dfg*>::iterator it;
  Instruction *inst;
  for(it=new_order.begin(); it!=new_order.end(); it++){
    inst=(*it)->get_instruction();
    cout<<"i"<<inst->get_index()<<": "<<inst->get_content()<<endl;
  }
}
	
