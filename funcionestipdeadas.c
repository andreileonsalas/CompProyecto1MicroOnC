Enlaces de interes

http://informatica.uv.es/docencia/iiguia/asignatu/2000/PL/2010/microscanner.c
https://github.com/MatiasMorsa/TP-Sintaxis-UTN-2016-Compilador-del-Lenguaje-Micro-Fischer-/blob/master/src/Funciones.c
https://github.com/MatiasMorsa/TP-Sintaxis-UTN-2016-Compilador-del-Lenguaje-Micro-Fischer-/blob/master/src/Main.c
http://fivedots.coe.psu.ac.th/Software.coe/Compilers/code/expressions/exprParse0.c
http://fivedots.coe.psu.ac.th/Software.coe/Compilers/code/expressions/
https://freesoft.dev/program/118072250

otro scanner pero no de micro

https://www.eecis.udel.edu/~chester/courses/471hw/scanner.cc




















//A parsing procedure including semantic processing
void expression (expr rec *result)
{
	expr_rec left_operand, right_operand;
	op_rec op;

	primary (&left_operand);
	while (next_token() == PLUSOP ||next_token() == MINUSOP)
	{
		add_op (&op);
		primary (&right_operand);
		left_operand = gen_infix (left_operand, op, right_operand);
	}
	*result = left_operand;
}
