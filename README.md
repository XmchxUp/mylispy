> [Build your own lisp](http://buildyourownlisp.com/contents)

### My Lisp in C

Run:
```bash
make
```

[Example](./examples):
```lisp
Demo1:
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

```lisp
Demo2:
\ {args body} {def (head args) (\ (tail args) body)}
def {fun} (\ {args body} {def (head args) (\ (tail args) body)})
fun {add-together x y} {+ x y} ;;; add-together 1 2 --> 3
(fun {len l} {
  if (== l {})
    {0}
    {+ 1 (len (tail l))}
}) ;;; len {a b c d} --> 4
(fun {reverse l} {
  if (== l {})
    {{}}
    {join (reverse (tail l)) (head l)}
}) ;;; reverse {a b c d} --> {d c b a}
```

Builtin:
```lisp
#true: 1
#false: 0
load: <builtin>
error: <builtin>
print: <builtin>
if: <builtin>
==: <builtin>
!=: <builtin>
>: <builtin>
<: <builtin>
>=: <builtin>
<=: <builtin>
!: <builtin>
not: <builtin>
&&: <builtin>
and: <builtin>
||: <builtin>
or: <builtin>
symbol: <builtin>
exit: <builtin>
lambda: <builtin>
\: <builtin>
def: <builtin>
=: <builtin>
list: <builtin>
head: <builtin>
second: <builtin>
car: <builtin>
cdr: <builtin>
tail: <builtin>
eval: <builtin>
join: <builtin>
len: <builtin>
cons: <builtin>
init: <builtin>
+: <builtin>
add: <builtin>
-: <builtin>
sub: <builtin>
*: <builtin>
mul: <builtin>
/: <builtin>
div: <builtin>
max: <builtin>
min: <builtin>
%: <builtin>
mod: <builtin>
```