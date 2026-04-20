#ifndef ACTION_H
#define ACTION_H

#include <stdio.h>

/* initialize / finish code generation */
void start(void);
void finish(void);

/* assignment / io */
void assign(const char *lhs);
void read_id(const char *id);
void write_expr(void);

/* expression helpers */
void process_id(const char *id);
void process_literal(const char *literal);
void process_op(const char *op);
void gen_infix(void);
void gen_condition(void);

/* while hooks required by assignment */
void open_tmp(void);
void write_temp(void);

/*If Hooks*/
void begin_if(void);
void emit_else(void);
void end_if(void);


#endif