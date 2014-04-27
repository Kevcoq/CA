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
    if(itmp->get_nb_pred() == 0 && !bb->is_delayed_slot(itmp)) {
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
  for(unsigned int i=0; i<_roots.size(); i++, it++)
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
  for(unsigned int i=0; i<_roots.size(); i++, it++)
    if((*it)->get_weight() > max)
      max = (*it)->get_weight();
 
  return max;
}


// fonction auxiliaire de comput_nb_descendant
int calcul_desc (Node_dfg *node) {
  int res = node->get_nb_arcs();  // nb de succ directs
  list <Arc_t*>::iterator it = node->arcs_begin();
  
  for (int i = 0 ; i<node->get_nb_arcs() ; i++, it++) {
    res += calcul_desc((*it)->next);
  }

  node->set_nb_descendant(res);
  return res;
}

// calcul le nombre de descendant de chaque Node
void Dfg::compute_nb_descendant () {
  
  list <Node_dfg*>::iterator it = _roots.begin();
  for (unsigned int i = 0 ; i<_roots.size() ; i++, it++) {
    (*it)->set_nb_descendant(calcul_desc(*it));
  }

}


void Dfg::display_nb_descendant () {
  list <Node_dfg*>::iterator it = _roots.begin();

  cout << "\n\nNbDescendants : \n";

  for (unsigned int i = 0 ; i<_roots.size() ; i++, it++) {
    cout << "i" << (*it)->get_instruction()->get_index() << " : " << (*it)->get_nb_descendant() << endl;
  }

  cout << "\nFin\n\n";

}



// Ajoute dans _inst_ready les nouveaux noeuds prêts
bool delayed_slot_ajoute = false;
void Dfg::add_node_now_ready () {
  list<Node_dfg*>::iterator it = list_node_dfg.begin();
  list<Node_dfg*>::iterator it2;
  bool add;

  for (unsigned int i = 0 ; i<list_node_dfg.size() ; i++, it++) {
    if (((*it)->get_traitee() == -1) && !contains(&_inst_ready, *it)) {  // pas encore schedulé && pas déjà dans la liste
      
      add = true;
      it2 = (*it)->pred_begin();  // iterator des prédécesseurs
      for (int j = 0 ; j<(*it)->nb_preds() ; j++, it2++) {
	if ((*it2)->get_traitee() == -1) {
	  add = false;
	}
      }
      if (add) {
	_inst_ready.push_back(*it);
      }
    }
  }

  // quand toutes les instructions sont passées, on ajoute le delayed slot s'il y en a un
  if (!delayed_slot_ajoute && !_delayed_slot.empty() && _inst_ready.empty()) {
    _inst_ready.push_back(_delayed_slot.front());
    delayed_slot_ajoute = true;
  }

}


bool compareWeight (Node_dfg *n1, Node_dfg *n2) {
  return n1->get_weight() > n2->get_weight();
}

bool compareLatency (Node_dfg *n1, Node_dfg *n2) {
  return n1->get_instruction()->get_latency() < n2->get_instruction()->get_latency();
}

/* Le nombre de successeurs directs est le nombre d'arcs */
bool compareNbSucc (Node_dfg *n1, Node_dfg *n2) {
  return n1->get_nb_arcs() > n2->get_nb_arcs();
}

bool compareNbDesc (Node_dfg *n1, Node_dfg *n2) {
  return n1->get_nb_descendant() < n2->get_nb_descendant();
}

bool compareIndex (Node_dfg *n1, Node_dfg *n2) {
  return n1->get_instruction()->get_index() > n2->get_instruction()->get_index();
}
	

void Dfg::scheduling(){
  /* FONCTION POUR LE PROJET */

  int cpt = 0;
  list<Node_dfg*> tmp;
  list<Node_dfg*> tmp2;
  list <Node_dfg*>::iterator it;
  list <Node_dfg*>::iterator it2;
  list <Arc_t*>::iterator arc;

  
  // liste des instructions prêtes
  _inst_ready.assign(_roots.begin(), _roots.end());
  
  while (_inst_ready.size() != 0) {
    
    if (!tmp.empty()) {
      cout << "Instruction choisie : i" << tmp.front()->get_instruction()->get_index() << endl;
    }

    tmp.clear();

    display_inst_ready();

    /* A chaque règle, on insert dans tmp les instructions qui satisfont la règle,
       s'il n'y en a plus qu'une, alors c'est celle-là ! sinon règle suivante */

    /* Règle 1 
       Instruction ne provoquant pas de cycle de gel
    */

    // it = noeud courant, it2 = noeuds prédecesseurs, arc = arcs des noeuds pred
    it = _inst_ready.begin();
    for (unsigned int i = 0 ; i<_inst_ready.size() ; i++, it++) {
      it2 = (*it)->pred_begin();
      for (int j = 0 ; j<(*it)->nb_preds() ; j++, it2++) {
	arc = (*it2)->arcs_begin();
	for (int k = 0 ; k<(*it2)->get_nb_arcs() ; k++, arc++) {
	  if ((*arc)->next == *it) {
	    if (!(((*arc)->delai != -1) && ((*arc)->delai + (*it2)->get_traitee() > (cpt+1)))) {
	      tmp.push_back(*it);
	    }
	  }
	}
      }
    }

    if (tmp.size() == 1) {
      tmp.front()->set_traitee(cpt+1);
      new_order.push_back(tmp.front());
      _inst_ready.remove(tmp.front());
      add_node_now_ready();
      cpt++;
      cout << "regle 1\n";
      continue;   // passage au tour du while suivant
    } else if (tmp.size() == 0) {
      // si tmp.size() = 0, cela veut dire que toutes les instructions provoquent un cycle de gel
      tmp.assign(_inst_ready.begin(), _inst_ready.end());
    }
    
    
    /* Règle 2 
       Instruction avec le poids le plus élevé
    */
    
    tmp2.clear();
    tmp2.assign(tmp.begin(), tmp.end());  // ici, tmp2 joue le rôle de _inst_ready dans la boucle précédente 
    
    tmp.clear();
    tmp2.sort(compareWeight); // tri décroissant selon le poids
    int poidsMax = tmp2.front()->get_weight();
    tmp.push_back(tmp2.front());
    it = tmp2.begin(); it++;  // on part du deuxième élem
    for (unsigned int i = 1 ; i<tmp2.size() ; i++, it++) {
      if ((*it)->get_weight() == poidsMax) {
	tmp.push_back(*it);
      }
    }

    if (tmp.size() == 1) {
      tmp.front()->set_traitee(cpt+1);
      new_order.push_back(tmp.front());
      _inst_ready.remove(tmp.front());
      add_node_now_ready();
      cpt++;
      cout << "regle 2\n";
      continue;   // passage au tour du while suivant
    }

 
    /* Règle 3
       Instruction avec la latence la plus élevée
     */

    tmp2.clear();
    tmp2.assign(tmp.begin(), tmp.end());
    tmp.clear();
    tmp2.sort(compareLatency);  // tri décroissant selon la latence
    int latenceMax = tmp2.front()->get_instruction()->get_latency();
    tmp.push_back(tmp2.front());
    it = tmp2.begin(); it++;  // on part du deuxième élem
    for (unsigned int i = 1 ; i<tmp2.size() ; i++, it++) {
      if ((*it)->get_instruction()->get_latency() == latenceMax) {
	tmp.push_back(*it);
      }
    }

    if (tmp.size() == 1) {
      tmp.front()->set_traitee(cpt+1);
      new_order.push_back(tmp.front());
      _inst_ready.remove(tmp.front());
      add_node_now_ready();
      cpt++;
      cout << "regle 3\n";
      continue;   // passage au tour du while suivant
    }


    /* Règle 4
       Instruction avec le plus grand nombre de successeurs
     */

    tmp2.clear();
    tmp2.assign(tmp.begin(), tmp.end());
    tmp.clear();
    tmp2.sort(compareNbSucc);  // tri décroissant selon le nombre de successeurs
    int nbSuccMax = tmp2.front()->get_nb_arcs();
    tmp.push_back(tmp2.front());
    it = tmp2.begin(); it++;  // on part du deuxième élem
    for (unsigned int i = 1 ; i<tmp2.size() ; i++, it++) {
      if ((*it)->get_nb_arcs() == nbSuccMax) {
	tmp.push_back(*it);
      }
    }

    if (tmp.size() == 1) {
      tmp.front()->set_traitee(cpt+1);
      new_order.push_back(tmp.front());
      _inst_ready.remove(tmp.front());
      add_node_now_ready();
      cpt++;
      cout << "regle 4\n";
      continue;   // passage au tour du while suivant
    }
    
    /* Règle 5
       Instruction avec le plus grand nombre de descendants
     */

    tmp2.clear();
    tmp2.assign(tmp.begin(), tmp.end());
    tmp.clear();
    tmp2.sort(compareNbDesc);  // tri décroissant selon le nombre de descendants
    int nbDescMax = tmp2.front()->get_nb_descendant();
    tmp.push_back(tmp2.front());
    it = tmp2.begin(); it++;  // on part du deuxième élem
    for (unsigned int i = 1 ; i<tmp2.size() ; i++, it++) {
      if ((*it)->get_nb_descendant() == nbDescMax) {
	tmp.push_back(*it);
      }
    }

    if (tmp.size() == 1) {
      tmp.front()->set_traitee(cpt+1);
      new_order.push_back(tmp.front());
      _inst_ready.remove(tmp.front());
      add_node_now_ready();
      cpt++;
      cout << "regle 5\n";
      continue;   // passage au tour du while suivant
    }
    
    /* Règle 6
       Instruction apparaissant en premier dans le code d'origine
     */

    tmp.sort(compareIndex);  // tri croissant selon l'index de l'instruction
    tmp.front()->set_traitee(cpt+1);
    new_order.push_back(tmp.front());
    _inst_ready.remove(tmp.front());
    add_node_now_ready();
    cout << "regle 6\n";
    
    cpt++;

    } // while

}


/*
  Met le champ _traitee de tous les noeuds de la liste à la valeur val.
*/
void set_all_traitee (list<Node_dfg*> *liste, int val) {
  list <Node_dfg*>::iterator it;

  for (it = (*liste).begin() ; it != (*liste).end() ; it++) {
    (*it)->set_traitee(val);
  }
}


/*
  Calcule le nombre de cycles nécessaires à l'éxécution de cette liste
  d'instructions.
*/
int Dfg::nb_cycles (list<Node_dfg*> *liste) {
  int cpt = 1;  // compteur du nombre de cycle
  list <Node_dfg*>::iterator it;
  list <Node_dfg*>::iterator prec;
  int delai;

  it = (*liste).begin();
  prec = (*liste).begin();

  set_all_traitee(liste, -1);

  cpt++;  // on lance la première inst
  (*it)->set_traitee(1);
  it++;   // on part de la deuxième inst de la liste
  
  for (; it != (*liste).end() ; it++) {
    for (; prec != (*liste).begin() ; prec--) {
      if ((*it)->get_instruction()->is_dep_RAW((*prec)->get_instruction())) {
	delai = t_delay[((*prec)->get_instruction()->get_type())][((*it)->get_instruction()->get_type())];
	if (delai != -1) {
	  while ((*prec)->get_traitee() + delai < cpt) {
	    cpt++;
	  }
	}
      }
      (*it)->set_traitee(cpt);
    }

    prec = it;
    cpt++;
  }

  return cpt;
}

// calcule le nombre de cycles de la liste new_order
int Dfg::nb_cycles2 () {
    return nb_cycles(&new_order);
}

  


void Dfg::display_inst_ready () {
  list <Node_dfg*>::iterator it;
  
  cout << "\nInstructions ready : \n";
  
  for(it = _inst_ready.begin(); it != _inst_ready.end(); it++){
    cout << "i" << (*it)->get_instruction()->get_index() << endl;
  }

  cout << "Fin\n";
  
}


	
void Dfg::display_sheduled_instr(){
  list <Node_dfg*>::iterator it;
  Instruction *inst;
  
  cout << "\n\nScheduled Order :\n";
  
  for(it=new_order.begin(); it!=new_order.end(); it++){
    inst=(*it)->get_instruction();
    cout<<"i"<<inst->get_index()<<": "<<inst->get_content()<<endl;
  }

  cout << "\nFin\n\n";

}
	
