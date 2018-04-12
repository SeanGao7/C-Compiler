program --> declarations \
declarations --> declarations declaration | declaration \
declaration --> var-declaration | fun-declaration \
var-declaration --> type ID; | type ID[NUM]; \
type --> int | void \
fun-declaration --> type ID (formals) block \
formals --> void | parameters \
parameters --> parameters, parameter | parameter \
paramter --> type ID | type ID[] \
block --> {local-vars statements} \
local-vars --> local-vars var-declaration | e \
statements --> statements statement | e \
statement --> expr-stmt | block | selection | reptition | return-stmt | $ expr \
expr-stmt --> expression; | ; \
selection --> if (comparison) statement | if (comparison) statement else statement \
repitition --> while (comparison) statement \ 
return-stmt --> return; | return expression; \
expression --> var = expression | arith-expr \
var --> ID | ID[expression] \
comparison --> arith-expr relop arith-expr | ! (comparison) \
relop --> <= | < | > | >= | == | != \
arith-expr --> arith-term | arith-expr addop arith-term \
arith-term --> arith-factor | arith-term mulop arith-factor \
addop --> + | - \
mulop --> * | / \
arith-factor --> var | call | NUM | (expression) \
call --> ID(actuals) \ 
actuals --> e | arguments \
arguments --> arguments, argument | argument \
argument --> expression
