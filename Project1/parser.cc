/*
 * Copyright (C) Rida Bazzi, 2020
 *
 * Do not share this file with anyone
 *
 * Do not post this file or derivatives of
 * of this file online
 *
 */
#include <iostream>
#include <algorithm>
#include <cstdlib>
#include "parser.h"
#include <vector>
#include <string>
#include <cstring>
#include <map>

using namespace std;

//Global Variables
Token token;
Token t;
Token t2;
int codeNum;
map<string, int> headers;
map<string, int> numArgs;
vector<int> errorLines;
vector<string> inputVars;
vector<string> argList;
vector<vector<string>> argListClone;
vector<string> argListCloneCompare;

void Parser::syntax_error()
{
    cout << "SYNTAX ERROR !!&%!!\n";
    exit(1);
}

// this function gets a token and checks if it is
// of the expected type. If it is, the token is
// returned, otherwise, synatx_error() is generated
// this function is particularly useful to match
// terminals in a right hand side of a rule.
// Written by Mohsen Zohrevandi
Token Parser::expect(TokenType expected_type)
{
    Token t = lexer.GetToken();
    if (t.token_type != expected_type)
        syntax_error();
    return t;
}

// Parsing

// This function is simply to illustrate the GetToken() function
// you will not need it for your project and you can delete it
// the function also illustrates the use of peek()
void Parser::ConsumeAllInput()
{
    Token token;
    int i = 1;
    
    token = lexer.peek(i);
    token.Print();
    while (token.token_type != END_OF_FILE)
    {
        i = i+1;
        token = lexer.peek(i);
        token.Print();
    }
   
    token = lexer.GetToken();
    token.Print();
    while (token.token_type != END_OF_FILE)
    {
        token = lexer.GetToken();
        token.Print();
    }

}

//Top level input parsing
void Parser::input() {
    program();
    inputs();

    expect(END_OF_FILE);
}

void Parser::inputs() {
    expect(NUM);
    token = lexer.peek(1);
    if (token.token_type == NUM) {
        inputs();
    }
}

//Program
void Parser::program() {
    poly_decl_section();
    start();
}

void Parser::poly_decl_section() {
    argList.clear();
    poly_decl();

    token = lexer.peek(1);
    if (token.token_type == POLY) {
        poly_decl_section();
    }
    else {
        return;
    }
}

void Parser::poly_decl() {
    expect(POLY);

    t = lexer.peek(1);

    map<string, int>::iterator it = headers.find(t.lexeme);

    if (it == headers.end()) { //don't find token in map (unique Lexeme)
        headers.insert(make_pair(t.lexeme, t.line_no)); //add ID string and line number to map
    }
    else { //do find token in map (dupe)
        if (find(errorLines.begin(), errorLines.end(), headers.at(t.lexeme)) == errorLines.end()) {
            errorLines.push_back(headers.at(t.lexeme)); //add line number of ID already stored in map to error vector
            codeNum = 1;
        }
        
        if (find(errorLines.begin(), errorLines.end(), t.line_no) == errorLines.end()) { //don't find line number of 
            errorLines.push_back(t.line_no);
        }
    }

    polynomial_header();
    expect(EQUAL);
    polynomial_body();
    expect(SEMICOLON);
}

void Parser::polynomial_header() {

    token = lexer.peek(2);
    if (token.token_type != LPAREN) {
        polynomial_name();
    }
    else {
        polynomial_name();
        expect(LPAREN);
        id_list();
        expect(RPAREN);
    }
}

void Parser::id_list() {

    t = lexer.peek(1);
    //vector containing parameter variable names
    argList.push_back(t.lexeme);

    expect(ID);
    token = lexer.peek(1);
    if (token.token_type == COMMA) {
        expect(COMMA);
        id_list();
    }
    else {
        return;
    }
}

void Parser::polynomial_body() {
    term_list();
}

void Parser::term_list() {
    term();
    token = lexer.peek(1);
    if (token.token_type == PLUS || token.token_type == MINUS) {
        token = lexer.peek(2);
        if (token.token_type == SEMICOLON) {
            syntax_error();
        }
        else {
            add_operator();
            term_list();
        }
    }
    else {
        return;
    }
}

void Parser::term() {
    token = lexer.peek(1);
    if (token.token_type == ID) {
        monomial_list();
    }
    else if (token.token_type == NUM) {

        token = lexer.peek(2);
        if (token.token_type == ID) {
            coefficient();
            monomial_list();
        }
        else {
            coefficient();
        }
    }
    else {
        syntax_error();
    }
}

void Parser::monomial_list() {
    monomial();

    token = lexer.peek(1);
    if (token.token_type == ID) {
        monomial_list();
    }
    else {
        return;
    }
}

void Parser::monomial() {
    token = lexer.peek(2);

    t = lexer.peek(1);

    if (argList.empty()) {
        if (t.lexeme != "x") {
            codeNum = 2;
            errorLines.push_back(t.line_no);
        }
    }
    else {
        if (find(argList.begin(), argList.end(), t.lexeme) == argList.end()) {
            codeNum = 2;
            errorLines.push_back(t.line_no);
        }
    }

    if (token.token_type == POWER) {
        expect(ID);
        exponent();
    }
    else {
        expect(ID);
    }
}

void Parser::exponent() {
    expect(POWER);
    expect(NUM);
}

void Parser::coefficient() {
    expect(NUM);
}

void Parser::add_operator() {
    token = lexer.peek(1);
    if (token.token_type == PLUS) {
        expect(PLUS);
    }
    else {
        expect(MINUS);
    }
}

void Parser::start() {
    expect(START);
    statement_list();
}

void Parser::statement_list() {
    statement();

    token = lexer.peek(1);
    if (token.token_type == INPUT || token.token_type == ID) {
        statement_list();
    }
    else {
        return;
    }
}

void Parser::statement() {
    token = lexer.peek(1);
    if (token.token_type == INPUT) {
        input_statement();
    }
    else if (token.token_type == ID) {
        poly_evaluation_statement();
    }
    else {
        syntax_error();
    }
}

void Parser::input_statement() {
    expect(INPUT);

    //store INPUT variables within vector inputVars
    t = lexer.peek(1);
    inputVars.push_back(t.lexeme);

    expect(ID);
    expect(SEMICOLON);
}

void Parser::poly_evaluation_statement() {
    t = lexer.peek(1);
    map<string, int>::iterator it = headers.find(t.lexeme);

    if (it == headers.end()) { //If we don't find the variable name (INVALID POLY EVAL STATEMENT)
        codeNum = 3;
        errorLines.push_back(t.line_no);
    }

    polynomial_evaluation();
    expect(SEMICOLON);
}

void Parser::polynomial_evaluation() {
    polynomial_name();
    expect(LPAREN);
    argument_list();
    expect(RPAREN);
}

void Parser::argument_list() {
    argument();
    token = lexer.peek(1);
    if (token.token_type == COMMA) {
        expect(COMMA);
        argument_list();
    }
    else {
        return;
    }
}

void Parser::argument() {
    token = lexer.peek(1);

    if (token.token_type == ID) {

        token = lexer.peek(2);
        if (token.token_type == LPAREN) {

            //Finding invalid polynomial name within an argument
            t = lexer.peek(1);

            map<string, int>::iterator it = headers.find(t.lexeme);

            if (it == headers.end()) { //If we don't find the variable name (INVALID POLY EVAL STATEMENT)
                codeNum = 3;
                errorLines.push_back(t.line_no);
            }

            polynomial_evaluation();
        }
        else {
            t2 = lexer.peek(1);

            //If ID we are looking at was not initialized by an INPUT statement (Error Code 5)
            if (find(inputVars.begin(), inputVars.end(), t2.lexeme) == inputVars.end()) {
                codeNum = 5;
                errorLines.push_back(t2.line_no);
            }
            expect(ID);
        }
    }
    else if (token.token_type == NUM) {
        expect(NUM);
    }
    else {
        syntax_error();
    }
}

void Parser::polynomial_name() {

    expect(ID);
}


int main()
{
	// note: the parser class has a lexer object instantiated in it. You should not be declaring
    // a separate lexer object. You can access the lexer object in the parser functions as shown in the
    // example method Parser::ConsumeAllInput
    // If you declare another lexer object, lexical analysis will not work correctly
    Parser parser;

    parser.input();

    //If any errors exist run this
    if (!errorLines.empty()) {
        sort(errorLines.begin(), errorLines.end()); //sort vector containing error line numbers (ascending)
        printf("Error Code %d:", codeNum);
        for (int i = 0; i < errorLines.size(); i++) {
            cout << " " << errorLines.at(i);
        }
    }
}
