#include <string>
#include <vector>
#include <iostream>
#include <stdlib.h>

#include "slpy-lex.hh"
#include "slpy-ast.hh"
#include "slpy-parse.hh"
#include "slpy-util.hh"
    
Expn_ptr parseLeaf(TokenStream& tks) {
    if (tks.at("(")) {

        //
        // <leaf> ::= ( <expn> ) 
        //
        tks.eat("(");
        Expn_ptr expn = parseExpn(tks);
        tks.eat(")");

        return expn;

    } else if (tks.at("input")) {

        //
        // <leaf> ::= input ( <strg> )
        //
        Locn locn = tks.locate();
        tks.eat("input");
        tks.eat("(");
        //
        std::string strg = tks.eat_string();
        Inpt_ptr inpt = std::shared_ptr<Inpt> { new Inpt {strg,locn} };
        //
        tks.eat(")");

        return inpt;

    } else if (tks.at("int")) {

        //
        // <leaf> ::= int ( <expn> )
        //
        tks.eat("int");
        tks.eat("(");
        //
        Expn_ptr expn = parseExpn(tks);
        //
        tks.eat(")");

        return expn;

    } else if (tks.at_number()) {

        //
        // <leaf> ::= <nmbr> 
        //
        Locn locn = tks.locate();
        int valu = tks.eat_number();
        
        return std::shared_ptr<Nmbr> { new Nmbr {valu, locn} };

    } else if (tks.at_name()) {

        //
        // <leaf> ::= <name> 
        //
        Locn locn = tks.locate();
        std::string name = tks.eat_name();
    
        return std::shared_ptr<Lkup> { new Lkup {name, locn} };

    } else {

        Locn locn = tks.locate();
        Token tkn = tks.current();
        std::string msg = "";
        msg += "Syntax error: unexpected '" + tkn.token;
        msg += "' seen when parsing a leaf expression.\n";
        throw SlpyError { locn, msg };
        
    }
}

Expn_ptr parseMult(TokenStream& tks) {
    //
    // <mult> ::= <leaf> * <leaf> * ... * <leaf>
    //
    Expn_ptr expn1 = parseLeaf(tks);
    while (tks.at("*") || tks.at("//")) {
        Locn locn = tks.locate();
        if (tks.at("*")) {
            tks.eat("*");       
            Expn_ptr expn2 = parseLeaf(tks);
            expn1 = std::shared_ptr<Tmes> { new Tmes {expn1, expn2, locn} };
        } else {
            tks.eat("//");       
            Expn_ptr expn2 = parseLeaf(tks);
            expn1 = std::shared_ptr<IDiv> { new IDiv {expn1, expn2, locn} };
        }
    }

    return expn1;
}

Expn_ptr parseAddn(TokenStream& tks) {
    //
    // <addn> ::= <mult> +/- <mult> +/- ... +/- <mult>
    //
    Expn_ptr expn1 = parseMult(tks);
    while (tks.at("+") || tks.at("-")) {
        Locn locn = tks.locate();
        if (tks.at("+")) {
            tks.eat("+");
            Expn_ptr expn2 = parseMult(tks);
            expn1 = std::shared_ptr<Plus> { new Plus {expn1, expn2, locn} };
        } else {
            tks.eat("-");
            Expn_ptr expn2 = parseMult(tks);
            expn1 = std::shared_ptr<Mnus> { new Mnus {expn1, expn2, locn} };
        }
    }

    return expn1;
}

Expn_ptr parseExpn(TokenStream& tks) {
    //
    // <expn> ::= <addn>
    //
    return parseAddn(tks);
}

Stmt_ptr parseStmt(TokenStream& tks) {
    if (tks.at("print")) {

        //
        // <stmt> ::= print ( <expn> )
        //
        Locn locn = tks.locate();
        tks.eat("print");
        tks.eat("(");
        Expn_ptr expn = parseExpn(tks);
        tks.eat(")");

        return std::shared_ptr<Prnt> { new Prnt { expn, locn } };

    } else if (tks.at("pass")) {

        //
        // <stmt> ::= pass
        //
        Locn locn = tks.locate();
        tks.eat("pass");

        return std::shared_ptr<Pass> { new Pass { locn } };

    } else {
        
        //
        // <stmt> ::= <name> = <expn>
        //
        std::string name = tks.eat_name();
        Locn locn = tks.locate();
        tks.eat("=");
        Expn_ptr expn = parseExpn(tks);

        return std::shared_ptr<Asgn> { new Asgn { name, expn, locn } };
    }
}

Blck_ptr parseBlck(TokenStream& tks) {

    //
    // <blck> ::= <stmt> EOLN <stmt> EOLN ... <stmt> EOLN
    // 
    Stmt_vec stms { };
    Locn locn = tks.locate();
    do {
        Stmt_ptr stmt = parseStmt(tks);
        tks.eat_EOLN();
        stms.push_back(stmt);
    } while (!tks.at_EOF());

    return std::shared_ptr<Blck> { new Blck { stms, locn } };
}

Prgm_ptr parsePrgm(TokenStream& tks) {

    //
    // <prgm> ::= <blck>
    //
    Locn locn = tks.locate();
    Prgm* prgm = new Prgm {parseBlck(tks), locn};

    return std::shared_ptr<Prgm> { prgm };
}

Prgm_ptr parse(TokenStream& tks) {
    return parsePrgm(tks);
}
