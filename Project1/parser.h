/*
 * Copyright (C) Rida Bazzi, 2019
 *
 * Do not share this file with anyone
 */
#ifndef __PARSER_H__
#define __PARSER_H__

#include <string>
#include "lexer.h"

class Parser {
  public:
    void ConsumeAllInput();
    void input();
    Token expect(TokenType expected_type);


  private:
    LexicalAnalyzer lexer;
    void syntax_error();
    void poly_decl_section();
    void program();
    void start();
    void statement_list();
    void statement();
    void input_statement();
    void poly_evaluation_statement();
    void polynomial_evaluation();
    void argument_list();
    void polynomial_name();
    void argument();
    void poly_decl();
    void polynomial_header();
    void polynomial_body();
    void id_list();
    void term_list();
    void term();
    void add_operator();
    void monomial_list();
    void monomial();
    void exponent();
    void coefficient();
    void inputs();
};

#endif

