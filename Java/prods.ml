type hash = Elem of string * int;;

let s = ref ""
let const = ref ""
let constString = ref ""
let hash = Hastbl.create 22
let label = ref 0

let next_label() = 
  label := !label + 1;
  "L"^!label


let prod instr = 
  match instr with
      CONST c -> prodConst c 
    | VAR v t -> 
	let num = (Hashtbl.find hash v) + 5 in 
	  s := !s^"$"^num;
	  "$"^num
    | IF i1 i2 i3 ->
	let regi1 = prod i1 in
	let l1 = next_label() and l2 = next_label() in
	  s := !s^"bne $0 "^regi1^l1^"\n";
	  prod i3;
	  s := !s^"j "^l2^"\n";
	  s := !s^l1^" :\n";
	  prod i2;
	  s := !s^l2^" :\n";
	  0
    | PRIM (s, t) l ->
	(match s with 
	     "+" ->
