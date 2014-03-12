(* type *)
type expr = Kvar of string
	    | Klambda of string * expr
	    | Kapp of expr * expr

type instr = Grab
	     | Push of int
	     | Access of int
	     | Label of int

(* compile *)

let count = ref 0;;
let new_label () =
  count := !count + 1;
  !count

let reset_label () =
  count := 0

let index v env =
  let rec ind env pos =
    (match env with
	h::q -> if h == v then pos else ind q (pos+1)
      | [] -> failwith "Not found")
  in 
    ind env 1


let rec compiler terme env =
  match terme with 
      Kvar v -> Access (index v env) 
    | Klambda (s, e) -> Grab::compiler e (s::env)
    | Kapp (e1, e2) -> 
	let l = new_label() in 
	Push l::(compiler e1 env)@Label l::(compiler e2 env)



(* assemble *)
let assemble code =
  match code with 
      [] -> []
    | Push::rest -> PushI::(assemble rest)
    | Grab::rest -> GrabI::(assemble rest)
    | Access::rest -> AccessI::(assemble rest)
    | Label::rest -> LabelI::(assemble rest)



(* type *)
type closure = C of int * closure list



(* interprete *)
let find_label_i n code =
  let rec aux pos code =
    match code with
	LabelI l::rest -> if l == n then pos else aux (pos+1) rest
      | _::rest -> aux (pos+1) rest
      | [] -> failwith "Not found label"
  in
    aux 1 code

let find_env_j n env =
  match env with 
      so::s -> if n == 1 then so else find_env_j (n-1) s
    | [] -> failwith "Not found env"

let interprete code =
  let rec interp env pc stack =
    match (List.nth pc code) with 
	GrabI -> (match stack with
	    [] -> failwith "Error"
	  | so::s -> interp so::env (pc+1) s)
      | PushI n -> interp env (pc+1) (C(n, env))::stack
      | AccessI n -> 
	  let i = find_label_i n code in
	  let c = find_env_j n env in
	    (match c with
		 C(n, e) -> interp e (i+1) stack)
  in
    interp [] 1 []
