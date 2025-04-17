# Calcexpr
## Short description
Calculator that take C expression and parse it using Recursive Descent Parsing
## Grammar
Logical_OR ::= Logical_AND "||" Logical_OR | Logical_AND
Logical_AND ::= Bitwise_OR "&&" Logical_AND | Bitwise_OR
Bitwise_OR ::= Bitwise_XOR "|" Bitwise_OR | Bitwise_XOR
Bitwise_XOR ::= Bitwise_AND "^" Bitwise_XOR | Bitwise_AND
Bitwise_AND ::= Comp_EQ "&" Bitwise_AND | Comp_EQ
Comp_EQ ::= Comp_RL ("=="|"!=") Comp_EQ | Comp_RL
Comp_RL ::= Shift ("<"|"<="|">="|">") Comp_RL | Shift
Shift ::= Additive ("<<"|">>") Shift | Additive
Additive ::= Multiplicative ("+"|"-") Additive | Multiplicative
Multiplicative ::= Primary ("*"|"/") Multiplicative | Primary
Primary ::= Number | Group
Number ::= [0-9]+ ("." [0-9]*)?
Group ::= "(" Logical_OR ")"
## How to install it?
First clone this repository
```sh
git clone https://github.com/Vgwws/Calcexpr.git
```
Make a build directory and change directory to it
```sh
mkdir build
cd build
```
Use CMake to make build files
```sh
cmake ..
```
Build it
```sh
make
```
