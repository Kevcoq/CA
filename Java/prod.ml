open Types;;
open Typeur;;
open Env_typeur;;
open Env_trans;;
open Langinter;;
let compiler_name = ref "ml2mips";;
let object_suffix = ref ".s";;

(* Gestion des labels *)
let label = ref 0;;

let next_label() = 
  label := !label + 1;
  ("L"^(string_of_int !label));;

(*
 * pour trouver à quel décalage dans la pile se trouve une variable, 
 * on utilise findDec.
 * Registres utilisés pour les calculs
 *)


(* Ajouts *)
let vars = ref [];;


(* Trouve à quel décalage de la pile est la variable var *)
let findDec var = 12;;
  (*let rec aux i l =
    match l with 
	h::q -> 
	  if (String.compare h var) == 0 then
	    (i * 4)
	  else aux (i+1) q
      | [] -> failwith ("Not found : "^var) in
  aux 0 !vars;;
*)

(* Ajoute la variable var en tête du tableau *)
let add_var var =
  print_string (":::add de var = "^var^"\n"); 
  print_string (":::taille de la liste : "^(string_of_int (List.length !vars))^"\n");
  vars := var :: !vars;;

let remove_var = ();;
  (*match !vars with
    h::q -> vars := q
  | [] -> failwith "Pile vide";;
  *)
let reset =
  vars := [];;

(* Tests 
   reset;; 
   add_var "a";;
   Printf.fprintf stdout "%d\n" (findDec "a");;
   add_var "b";;
   print_endline (string_of_int (findDec "a"));;
   add_var "c";;
   print_endline (string_of_int (findDec "a"));;
   remove_var;;
   print_endline (string_of_int (findDec "a"));;
   print_endline (string_of_int (findDec "c"));;
*)    

(* des valeurs pour certains symboles de env_trans *)
pair_symbol:=",";;
cons_symbol:="::";;
ref_symbol:="ref";;

(* l'environnement initial du traducteur en liaison avec la Runtime *)
let build (s,equiv)  = 
  let t = 
    try List.assoc s !initial_typing_env  
    with Not_found -> 
      failwith ("building initial_trans_env : for symbol : "^s)
  in (s,(equiv,type_instance t));;




initial_trans_env:=       
  (
    List.map build 
      ["true" ,"1";
       "false","$0";
       "+","add";
       "-","sub";
       "*","mult";
       "/","div";
       "=","beq";
       "<","bltz";
       "<=","blez";
       ">","bgtz";
       ">=", "bgez";
	 
      ]
  )
;;


(* des fonctions d'I/O *)
let output_channel = ref stdout;;
let change_output_channel oc = output_channel := oc;;
let shift_string = String.make 256 ' ';;
let out s = output_string !output_channel s;;
let out_start s nb = out ("\n"^(String.sub shift_string 0 (2*nb))^s);;
let out_end s nb = out ("\n"^(String.sub shift_string 0 nb)^"}\n");;
let out_line s = out (s^"\n");;

let out_before (fr,sd,nb) = 
  out sd;;

let out_after  (fr,sd,nb) = 
  out sd;;

(* des fonctions utilitaires pour commenter un peu la production *)
let header_main  s = 
  List.iter out 
    ["# engendre par ml2mips \n\n";
     ".data\n\n";     
     ]
;;

let footer_main  s = 
  List.iter out
    ["\n\n# fin du fichier " ^ s ^ ".s\n"]
;;

let prod_global_var instr = match instr with
    VAR (v,t) -> out_start ("static "^"MLvalue "^(*(string_of_type t)*)v^";") 1 
  | FUNCTION (ns,t1,ar,(p,t2), instr) ->
    out_start ("static MLvalue "(*"fun_"^ns^" "*)^ns^"= new MLfun_"^ns^"("^(string_of_int ar)^");") 1
  | _ -> ()
;;

let get_param_type lv = 
  List.map (function (VAR(name,typ)) -> typ 
        | _ -> failwith "get_param_type" ) lv;;



let make_const_name c =
  match c with
      INT i -> ("Ci"^(string_of_int i))
    | FLOAT f -> ("Cf"^(string_of_int (int_of_float f)))
    | STRING s -> ("Cs"^s)
    | _ -> "";;



let prod_const c dest = 
  match c with 
    INT i ->   
      print_string (":::Constante c = "^(string_of_int i)^"\n");
      out ("#récupération d'une constante\n");
      let name = make_const_name c in
      out ("la $"^dest^" "^name^"\n");
  | FLOAT f ->   
    print_string (":::Constante c = "^(string_of_float f)^"\n");
   out ("#récupération d'une constante\n");
      let name = make_const_name c in
      out ("la $"^dest^" "^name^"\n");
  | BOOL b -> 
    out ("addiu $29 $29 -4\n");
    if b then (
      out ("addi $1 $0 1\n");
      out ("sw $1 0($29)\n"))
    else
      out ("sw $0 0($29)\n");
    add_var (if b then "true" else "false")
  | STRING s ->
    out ("#récupération d'une constante\n");
    let name = make_const_name c in
    out ("la $"^dest^" "^name^"\n");
  | EMPTYLIST -> () 
  | UNIT -> ()
;;


let rec prod_instr (fr,sd,nb,dest) instr  = match instr with 
    CONST c -> 
      prod_const c dest;
  | VAR (v,t) ->
    print_string (":::Var : "^v^"  dest = '"^dest^"'\n");
    if (nb = 0) && ( sd = "") then ()
    else 
      begin
      	(* Cas déclaration d'une variable *)
	if (dest = "") then (
	  out "addiu $29 $29 -4\n";
	  add_var v)
	else (* Cas utilisation d'une variable *)
	  if sd="" && not fr then (
	     let x = findDec sd in
	     out ("lw $"^dest^" "^(string_of_int x)^"($29)\n");)
      end 
  | IF(i1,i2,i3) -> 
    let l1 = next_label() and l2 = next_label() in
    (* on calcule la condition dans $10 *)
    prod_instr (false,"",nb, "10") i1;
    out ("beq $10 $0 "^l1^"\n");
    prod_instr (fr,sd,nb+1, "") i2 ;
    out ("j "^l2^"\n");
    out (l1^":\n");
    prod_instr (fr,sd,nb+1, "") i3;
    out (l2^":\n")
  | RETURN i -> 
    prod_instr (true,"",nb, "2") i;
    out "j $31";
  | AFFECT (v,i) -> 
    prod_instr (false,v,nb, "11") i;
    let x = findDec v in
    out ("sw $11 "^(string_of_int x)^"($29)\n");
  | BLOCK(l,i) ->
    List.iter (fun (v,t,i) -> prod_instr (false,v,nb+1, dest) i) l;
    prod_instr (fr,sd,nb+1, dest) i;
    
  | APPLY(i1,i2) -> 
    prod_instr (false,"",nb, dest) i1;
    out ")";
    out ".invoke(";
    prod_instr (false,"",nb, dest) i2;     
    out")";
    out_after(fr,sd,nb)
  | PRIM ((name,typ),instrl) ->
    print_string (":::Primitive name = "^name^"\n");
    (match name with
	"add" -> (
	  out ("# Primitive +\n");
	  out ("lw $20 0($29)\n");
	  out ("lw $21 4($29)\n");
	  out ("add $"^dest^" $20 $21\n");)
      | "sub" -> (
	out ("# Primitive -\n");
	out ("lw $20 0($29)\n");
	out ("lw $21 4($29)\n");
	out ("sub $"^dest^" $20 $21\n");) 
      | "mult" -> (
	out ("# Primitive *\n");
	out ("lw $20 0($29)\n");
	out ("lw $21 4($29)\n");
	out ("mult $20 $21\n");
	out ("mflo $"^dest^"\n"))
      | "div" -> (
	out ("# Primitive /\n");
	out ("lw $20 0($29)\n");
	out ("lw $21 4($29)\n");
	out ("div $20 $21\n");
	out ("mflo $"^dest^"\n"))
      | "beq" -> (
	let l1 = next_label() and l2 = next_label() in
	out ("# Primitive =\n");
	out ("lw $20 0($29)\n");
	out ("lw $21 4($29)\n");
	out ("beq $20 $21 "^(l1)^"\n");
	out ("addi $"^dest^" $0 0\n");
	out ("j "^(l2)^"\n");
	out ((l1)^" : \n");
	out ("addi $"^dest^" $0 1\n");
	out ((l2)^" : \n");)
      | "bltz" -> (
	let l1 = next_label() and l2 = next_label() in
	out ("# Primitive <\n");
	out ("lw $20 0($29)\n");
	out ("lw $21 4($29)\n");
	out ("sub $22 $20 $21\n");
	out ("bltz $22 "^(l1)^"\n");
	out ("addi $"^dest^" $0 0\n");
	out ("j "^(l2)^"\n");
	out ((l1)^" : \n");
	out ("addi $"^dest^" $0 1\n");
	out ((l2)^" : \n");)
      | "blez" -> (
	let l1 = next_label() and l2 = next_label() in
	out ("# Primitive <=\n");
	out ("lw $20 0($29)\n");
	out ("lw $21 4($29)\n");
	out ("sub $22 $20 $21\n");
	out ("blez $22 "^(l1)^"\n");
	out ("addi $"^dest^" $0 0\n");
	out ("j "^(l2)^"\n");
	out ((l1)^" : \n");
	out ("addi $"^dest^" $0 1\n");
	out ((l2)^" : \n");)
      | "bgtz" -> (
	let l1 = next_label() and l2 = next_label() in
	out ("# Primitive <=\n");
	out ("lw $20 0($29)\n");
	out ("lw $21 4($29)\n");
	out ("sub $22 $20 $21\n");
	out ("bgtz $22 "^(l1)^"\n");
	out ("addi $"^dest^" $0 0\n");
	out ("j "^(l2)^"\n");
	out ((l1)^" : \n");
	out ("addi $"^dest^" $0 1\n");
	out ((l2)^" : \n");)
      | "bgez" -> (
	let l1 = next_label() and l2 = next_label() in
	out ("# Primitive <=\n");
	out ("lw $20 0($29)\n");
	out ("lw $21 4($29)\n");
	out ("sub $22 $20 $21\n");
	out ("bgez $22 "^(l1)^"\n");
	out ("addi $"^dest^" $0 0\n");
	out ("j "^(l2)^"\n");
	out ((l1)^" : \n");
	out ("addi $"^dest^" $0 1\n");
	out ((l2)^" : \n");)
	
      | _ -> (
	let ltp = get_param_type instrl in 
	prod_instr (false,"",nb+1, dest) (List.hd instrl);
	List.iter2 (fun x y -> 
	  prod_instr (false,"",nb+1, dest) x) 
	  (List.tl instrl) (List.tl ltp);)       
    )	
  | FUNCTION _ -> ()
;;


let fun_header fn cn  = 
  List.iter out 
    ["\n\n";
     "/**\n";
     " *  de'claration de la fonction "^fn^"\n";
     " *    vue comme la classe : "^cn^"\n";
     " */ \n"]
;;
let prod_invoke cn  ar = 
  List.iter out_line 
    ["  public MLvalue invoke(MLvalue MLparam){";
     "    if (MLcounter == (MAX-1)) {"
    ];

  out "      return invoke_real(";
  for i=0 to ar-2 do 
    out ("MLenv["^(string_of_int i)^"], ")
  done;
  out_line "MLparam);";     

  List.iter out_line 
    ["    }";
     "    else {";
     "      "^cn^" l = new "^cn^"(MLcounter+1);l.MLaddenv(MLenv,MLparam); return l;";
     "    }";
     "  }"
    ]
;;

let prod_invoke_fun cn ar t lp instr = 
  out_start "MLvalue invoke_real(" 1;
  out ("MLvalue "^(List.hd lp));
  List.iter (fun x -> out (", MLvalue "^x)) (List.tl lp);
  out_line ") {";
  prod_instr (true,"",2, "") instr;
  
  out_start "}" 1;
  out_line ""
;;

let prod_fun instr = match instr with 
    FUNCTION (ns,t1,ar,(lp,t2),instr) -> 
      let class_name = "MLfun_"^ns in
      fun_header ns class_name ;
      out_line ("class "^class_name^" extends MLfun {");
      out_line "";
      out_line ("  private static int MAX = "^(string_of_int ar)^";") ;
      out_line "";
      out_line ("  "^class_name^"() {super();}") ;
      out_line "";
      out_line ("  "^class_name^"(int n) {super(n);}") ;      
      out_line "";
      prod_invoke class_name ar;
      out_line "";
      prod_invoke_fun class_name ar t1 lp instr;
      out_line "";           
      out_line "}";
      out_line ("// fin de la classe "^class_name)
	
	
  |  _ -> ()
;;

let rec the_first_prod instr =
  match instr with
      CONST c -> (
	let name = make_const_name c in
	match c with
	    INT i ->
	      out (name^" :\t.word\t"^(string_of_int i)^"\n");
	  | FLOAT f ->
	    out (name^" :\t.word\t"^(string_of_float f)^"\n");
	  | BOOL b ->
	    if b then
	      out (name^" :\t.word\t 1 \n")
	    else
	      out (name^" :\t.word\t 0 \n")
	  | STRING s ->
	    out (name^" :\t.asciiz\t\""^s^"\"\n")
	  | _ -> ())
    | VAR (v, t) -> ()
    | IF (i1, i2, i3) -> 
      the_first_prod i1;
      the_first_prod i2;
      the_first_prod i3
    | PRIM ((name, typ), instrl) -> 
      List.iter (the_first_prod) instrl
    | RETURN i -> 
      the_first_prod i
    | APPLY (i1, i2) -> 
      the_first_prod i1;
      the_first_prod i2
    | AFFECT (v, i) ->
      the_first_prod i
    | BLOCK (l, i) -> 
      let rec traiter_liste_in_block l =
	match l with
	    [] -> ()
	  | (s, t, instr)::rest -> 
	    the_first_prod instr;
	    traiter_liste_in_block rest
      in
      traiter_liste_in_block l;
      the_first_prod i; 
    | FUNCTION _ -> ()
;;


let first_prod ast_li =
  print_string ":::Premier passage \n";
  List.iter (the_first_prod) ast_li;;


let prod_one  ast_li = 
  List.iter prod_fun ast_li
;;
let prod_two  ast_li = 
  List.iter prod_global_var ast_li
;;
let prod_three  ast_li =
  print_string ":::Démarrage !\n";
  out ("\n.text\n\n");
  List.iter (prod_instr  (false,"",0, "")) ast_li
;;
let prod_file filename ast_li = 
  let obj_name = filename ^ !object_suffix in 
  let oc = open_out obj_name in 
  change_output_channel oc;  
  module_name:=filename;
  try
    header_main  filename;
    (*  header_one  filename; *)
    prod_one  ast_li;
    first_prod ast_li;
    (* footer_one  filename;
       header_two  filename;*)
   (* prod_two  ast_li; *)
    (*    footer_two  filename;
	  header_three  filename;*)
    prod_three  ast_li;
    (*    footer_three  filename;  *)
	  footer_main  filename;
    close_out oc
  with x -> close_out oc; raise x;;

