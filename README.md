> [Build your own lisp](http://buildyourownlisp.com/contents)

### My Lisp in C

Run:
```bash
make
```

Example:
```lisp
(+ 1 1)
(symbol 0) ;;; print symbol
(symbol 1) ;;; print value
(symbol 3) ;;; print symbol: value
def {a} 2  ;;; define variable
head {ab {d} c} ;;; ab
tail {a b c} ;;; c
join {3} {a} {d} ;;; {3 a d}
len {a b c {d f}} ;;; 4
cons {a} {b} ;;; {a b}
```

Builtin Operation:
```lisp
symbol: <function>
def: <function>
list: <function>
head: <function>
tail: <function>
eval: <function>
join: <function>
len: <function>
cons: <function>
init: <function>
+: <function>
add: <function>
-: <function>
sub: <function>
*: <function>
mul: <function>
/: <function>
div: <function>
max: <function>
min: <function>
%: <function>
mod: <function>
```