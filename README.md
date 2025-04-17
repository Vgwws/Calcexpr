# Calcexpr
Calculator that take math expression and calculate it
## Grammar
Comp_op ::= "<" | "<=" | "==" | ">" | ">="
Comp ::= Expr Comp_op Comp | Expr
Expr ::= Term ("+"|"-") Expr | Term
Term ::= Factor ("*"|"/") Term | Factor
Factor ::= Number | Group
Number ::= [0-9]+ ("." [0-9]*)?
Group ::= "(" Expr ")"
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
Use CMake to make build files(based on Generator backend you used)
```sh
cmake ..
```
Build it
```sh
make
```
