# Calcexpr
## Short description
Text based Calculator that take C expression and parse it using Recursive Descent Parsing then interpret the AST
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

Multiplicative ::= Primary ("\*"|"/") Multiplicative | Primary

Primary ::= Number | Group | Negate

Number ::= [0-9]+ ("." [0-9]*)?

Group ::= "(" Logical_OR ")"

Negate ::= "!" Primary

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
## How to use it?
After installing it, You can execute it
```sh
./calcexpr
```
If you want to execute it everywhere, you can add the current directory as your PATH or move the binary to one of directory listed in your PATH
```sh
PATH="$PATH:$(pwd)"
```
## Examples
```sh
$ ./calcexpr
CALCULATOR
Press `q` to quit program
>> 5 + 5 - 3
7.000000
>> 5 + 2 \* 3
11.000000
>> 5 & 1
1.000000
>> 6 >> 1
3.000000
>> 10 * 2 == 50 && 21 * 3 == 0
0.000000
>> q
```
