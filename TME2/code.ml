type expr =
    Kapp of expr * expr
|   Kid of string
|   Klambda of string * expr
|   Kconst of int
;;

type instr =
     Grab           (* met le sommet de la pile dans l'environnement *)
|    Push of int    (*  *)
|    Access of int  (* acces \`a la n-ieme variable de l'environnement *)
|    Label of int   (* etiquetage d'une partie de code *)
|    Pushenv of int
;;
type instrI =
     GrabI          (* met le sommet de la pile dans l'environnement *)
|    PushI of int   (*  *)
|    AccessI of int (* acces \`a la n-ieme variable de l'environnement *)
|    PushenvI of int
;;

let new_label,reset_label  =
  let c = ref 0 in
      ( function () -> (incr c);!c),(function () -> c:=0;!c)
;;

let aCompiler = ref ([]:instr list);;

let index a =
 let rec index_rec i = function
     []  -> raise Not_found
  | b::l -> if a = b then i else index_rec (succ i) l
in index_rec 0
;;

let rec mk_lab env term = let lab = new_label() in
        (aCompiler := ( Label lab)::((compiler env term)@(!aCompiler));
         lab)
and
  compiler env = function
  Kid v         -> [ Access ((index v env)+1) ]
| Klambda (v,e) ->  Grab::(compiler (v::env) e)
| Kapp (f,a)    ->  let u = (mk_lab env a) in
                    Push u :: (compiler env f)
| Kconst n      -> [ Pushenv n ]
;;

let cc e = reset_label();
           aCompiler :=[];
           let sub_code = (compiler [] e)
           in sub_code @ (!aCompiler)
;;

let longueur_code = function
     Grab     -> 1
|    Push _   -> 1
|    Pushenv _ -> 1
|    Access _ -> 1
|    Label _  -> 0
;;

let pass_one =
  let rec pass_rec pc = function
     [] -> []
  |   Label n :: rest -> (n,pc):: pass_rec pc rest
  |  instr :: rest  -> pass_rec (pc+(longueur_code instr)) rest
  in  pass_rec 1
;;

let assemble code =
  let list_label = pass_one code
  in
    let rec asm = function 
      [] -> []
    | Label n :: rest  -> asm rest
    | Push  l:: rest ->  PushI  (List.assoc l list_label) :: asm rest
    | Pushenv n:: rest -> PushenvI n:: asm rest
    | Grab :: rest   ->  GrabI :: asm rest
    | Access n :: rest ->  AccessI n :: asm rest
    in
      asm code
;;

type closure = C of int * (closure list) ref
;;
let rec nth l a = match l with 
  [] -> failwith "nth"
| h::t -> if a=1 then h else nth t (a-1)
;;

let interprete code =
  let rec interp env pc stack = match (nth code pc) with
     AccessI n -> (try 
                     let u = nth env  n  
                     in
                       match u with 
                         (C  (n,e)) -> interp !e n stack
                    with 
                      x -> (C (pc, ref env)))
  | PushI n  -> interp env (pc+1) ((C (n,ref env))::stack)
  | PushenvI n ->  (try 
                      let u = nth env  n  
                      in
                       match u with 
                         (C  (n,e)) -> interp !e n stack
                    with 
                      x -> (C (pc, ref env)))
  | GrabI    -> (match  stack with 
                  [] -> (C  (pc,ref env))
                | so::s -> interp (so::env) (pc+1) s)
  in
    interp   [] 1  []
;;

let kall exp =    interprete (assemble (cc exp))
;;


let ex1 =
(Kapp
 ((Kapp
    ((Klambda ("f",(Klambda ("x",(Kapp ((Kid "f"),(Kid "x"))))))),
        (Klambda ("x",(Kid "x"))))),
          (Klambda ("x",(Kid "x")))))
          ;;
cc ex1;;
assemble (cc ex1);;
interprete (assemble(cc ex1));;



let ex2 =
(Kapp
 ((Kapp
    ((Klambda ("f",(Klambda ("x",(Kapp ((Kid "f"),(Kid "x"))))))),
        (Klambda ("x",(Kconst 5))))),
          (Klambda ("x",(Kid "x")))))
          ;;
cc ex2;;
assemble (cc ex2);;
interprete (assemble(cc ex2));;


