# DPLL
Davis-Putnam-Logemann-Loveland is an algorithm that checks if a formula is satisfiable. In particular, it allows you to find an affection for each of the variables of your formula for which this one is satisfiable.
## How to install
Clone the repository. Go into the directory and compile :

**Linux** :  
```
make
```  
**Windows (or if you don't have make)** :
```
gcc -Wall -g -pedantic dpll.c cnf.c -o dpll
gcc -Wall -g -pedantic test_dpll.c cnf.c -o test_dpll
```
## How to use
To check if your formula is **satisfiable**, you must use the dpll program in this way :
```
./dpll "{ {x, y, z}, {:x, :z}, {:z} }"
```
**":"** represents the **NOT()**.  
**BUT**, you can replace it with any **non-alphabetic character**. Be careful to not use shell characters ( . , \* , \$...) or add a '\\' before ( \\\. , \\\* , \\\$...)  
For example :  
```
./dpll "{{x, y, z}, {y, /z, /x}, {/z, /y}}"
./dpll "{{x, y, z}, {y, \*z, \*x}, {\*z, \*y}}"
```
## Preview
![dpll_1](https://user-images.githubusercontent.com/95108507/177996951-13581d84-233d-4885-8554-92cd985062db.png)
![dpll_2](https://user-images.githubusercontent.com/95108507/177996959-c101866e-9b3b-47b3-9e8c-63afd47f914a.png)
