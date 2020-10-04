Enlaces de interes

http://informatica.uv.es/docencia/iiguia/asignatu/2000/PL/2010/microscanner.c
https://github.com/MatiasMorsa/TP-Sintaxis-UTN-2016-Compilador-del-Lenguaje-Micro-Fischer-/blob/master/src/Funciones.c
https://github.com/MatiasMorsa/TP-Sintaxis-UTN-2016-Compilador-del-Lenguaje-Micro-Fischer-/blob/master/src/Main.c
http://fivedots.coe.psu.ac.th/Software.coe/Compilers/code/expressions/exprParse0.c
http://fivedots.coe.psu.ac.th/Software.coe/Compilers/code/expressions/
https://freesoft.dev/program/118072250

otro scanner pero no de micro

https://www.eecis.udel.edu/~chester/courses/471hw/scanner.cc

void system_goal(void)
{
    program();
    match(SCANEOF);
}

void program(void)
{
    match(BEGIN);
    statement_list();
    match(END);
}

void statement_list(void)
{
    statement();
    white (TRUE) {
        switch (next_token()){
            case ID:
            case READ:
            case WRITE:
                statement ();
                break;
            default:
                return;
        }
    }
}

void statement(void)
{
    token tok = next_token();
    switch (tok){
        case ID:
            /* <statement> ::= ID := <expresion>; */
            match (ID); match (ASSIGNOP);
            expression(); match (SEMICOLON);
            break;

        case READ:
            /* <statement> ::= READ (<id list>) ; */
            match(READ); match(LPAREN);
            id_list();match(RPAREN);
            match(SEMICOLON);
            break;

        case WRITE:
            /* <statement> ::= WRITE (<id list>) ; */
            match(WRITE); match(LPAREN);
            expr_list();match(RPAREN);
            match(SEMICOLON);
            break;
        
        default:
            syntax_error(tok);
            break;
    }
}

void id_list(void)
{
    /* <id list> : ID { , ID} */
    match(ID);

    while (next_token()==COMMA){
        match(COMMA);
        match(ID);
    }
}
void expression(void)
{
    token t;
    /*
    *   <expression> ::= <primary>
    *               {<add op> <primary>}
    * 
    * 
    */
    primary();
    for (t = next_token();t == PLUSOP || t == MINUSOP; t = next_token()) {
        add_op();
        primary();
    }
    
}

void expr_list(void)
{
    /* <expr list> ::= <expression> { , <expression> } */
    expression();

    while (next_token() == COMMA) {
        match(COMMA);
        expression();
    }

}

void add_op(void)
{
    token tok = next_token();

    /* <addop> ::= PLUSOP | MINUSOP */
    if (tok == PLUSOP || tok == MINUSOP)
        match(tok);
    else
    {
        syntax_error(tok);
    }
}

void primary (void)
{
    token tok = next_token();
    switch (tok){
    case LPAREN:
        /* <primary> ::= { <expression> }*/
        match (LPAREN); expression();
        match (RPAREN);
        break;
    case ID:
        /* <primary> ::= ID */
        match(ID);
        breakl
    case INTLITERAL:
        /* <primary> ::= INTLITERAL */
        match(INTLITERAL);
        break;
    default:
        syntax_error(tok);
        break;
    }
}

#define MAXIDLEN
typedef char string[MAXIDLEN];

typedef struct operator { /* for operator */
    enum op { PLUS, MINUS} operator;
}   op_rec;

/* expression types */
enum expr { IDEXPR, LIETRALEXPR, TEMPEXPR};

/* for <primary> and <epxression> */
typedef struct expression {
    enum expr kind;
    union {
        string name; /* for IDEXPR, TEMPEPR */
        int val; /* for LITERALEXPR */
    }
} expr_rec;

/* Is s in the symbol table? */
extern int lookup (string s);

/* Put s unconditionally into symbol table. */
extern void enter (string s);

void check_id(string s)
{
    if (! lookup(s)) {
        enter (s);
        generate("Declare",s,"Integer","");
    }
}

char *get_temp(void)
{
    /* max temporary allocated so far */
    static int max_temp = 0
    static chat tempname [MAXIDLEN];
    
    max_temp++;
    sprintf(tempname, "Temp&%d",max_temp);
    check_id(tempname);
    return tempname;
}

void start (void)
{
    /* semantic initialization, none needed. */
}

void finish (void)
{
    /* Generate code to finish program */
    generate("Halt","","","");
}

void assign (expr_rec target, expr_rec source)
{
    /* Generate code for assignment. */
    generate ("Store", extract (source), target.name,"");
}

op_rec process_op(void)
{
    /* Produce operator descriptor. */
    op_rec 0;
    if (current_token == PLUSOP)
        o.operator=PLUS;
    else
        o.operator=MINUS;
    return o;
}

expr_rec gen_infix(expr_rec e1, op_rec op, expr_rec e2)
{
    expr_rec e_rec;
    /* An expr_rec with temp variant set. */
    e_rec.kind = TEMPEXPR;
    /* 
    * Generate code for infix operation.
    * Get result temp and set up semantic record
    * for result
    * 
    */
    strcpy(erec.name,get_temp());
    generate (extract(op), extract(e1),extract(e2), erec.name);
    return erec;
}

void read_id(expr_rec in_var)
{
    /* Generate code for read.*/
    generate("Read", in_var.name,"Integer","");
}

expr_rec process_id(void)
{
    expr_rec t;
    /*
    * Declare ID and build a corresponding semantic record.
    */
   check_id(token_buffer);
   t.kind = IDEXPR;
   strcpy (t.name, token_buffer);
   return t;
}

expr_rec process_literal(void)
{
    expr_rec t;
    /*
    *   Convert literal to a numeric representation and build semantic record.
    */
   t.kind = LITERALEXPR;
   (void) sscanf(token_buffer, "%d", & t.val);
   return t;
}

void write_expr(expr_rec out_expr)
{
    generate("Write", extract(out_expr),"Integer","");
}

void expression (expr rec *result) 
{
    expr_rec left_operand, right_operand; 
    op_rec op; 

    primary (& left_operand) ; 
    while (next_token() == PLUSOP ||next_token() == MINUSOP) 
    {
    add_op (& op) ; 
    primary (& right_operand) ; 
    left_operand = Gen_infix (left_operand, op, right_operand);
    } 
    *result = left_operand;
}
