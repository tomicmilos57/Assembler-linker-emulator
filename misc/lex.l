%{
#include "parser.tab.h"
#include <string.h>
#include <stdlib.h>
%}

%%

[ \t\r\n]+                  ;  /* Ignore whitespace */

"#".*                      ;  /* Ignore comments starting with # */

halt                       { return HALT; }
int                        { return INT; }
iret                       { return IRET; }
call                       { return CALL; }
ret                        { return RET; }
jmp                        { return JMP; }
beq                        { return BEQ; }
bne                        { return BNE; }
bgt                        { return BGT; }
push                       { return PUSH; }
pop                        { return POP; }
xchg                       { return XCHG; }
add                        { return ADD; }
sub                        { return SUB; }
mul                        { return MUL; }
div                        { return DIV; }
not                        { return NOT; }
and                        { return AND; }
or                         { return OR; }
xor                        { return XOR; }
shl                        { return SHL; }
shr                        { return SHR; }
ld                         { return LD; }
st                         { return ST; }
csrrd                      { return CSRRD; }
csrwr                      { return CSRWR; }

.global                    { return GLOBAL; }
.extern                    { return EXTERN; }
.section                   { return SECTION; }
.word                      { return WORD; }
.skip                      { return SKIP; }
.ascii                     { return ASCII; }
.equ                       { return EQU; }
.end                       { return END; }

0[xX][0-9a-fA-F]+ {
    yylval.num = (int)strtol(yytext, NULL, 16);
    return NUMBER; 
}

\$[a-zA-Z_][a-zA-Z0-9_]* {
    yylval.s = strdup(yytext + 1);
    return SYMBOL;
}

\$[0-9]+ {
    yylval.num = atoi(yytext + 1);
    return NUMBER;
}

[0-9]+ {
    yylval.num = atoi(yytext);
    return NUMBER;
}

%[a-zA-Z0-9_]+ {
    yylval.s = strdup(yytext);
    return REGISTER;
}

[a-zA-Z_][a-zA-Z0-9_]* {
    yylval.s = strdup(yytext);
    return SYMBOL;
}

\"([^\\"]|\\.)*\" {
    yylval.s = strdup(yytext);
    return STRING;
}

,          { return ','; }
:          { return ':'; }
;          { return ';'; }
\[         { return '['; }
\]         { return ']'; }
\+         { return '+'; }

.          { return *yytext; }

%%

int yywrap(void) {
    return 1;
}
