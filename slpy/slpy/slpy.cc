#include <iostream>
#include <cstring>

#include "slpy-lex.hh"
#include "slpy-ast.hh"
#include "slpy-parse.hh"
#include "slpy-util.hh"

bool check_flag(int argc, char** argv, std::string flag) {
    for (int i=1; i<argc; i++) {
        if (strcmp(flag.c_str(),argv[i]) == 0) return true;
    }
    return false;
}

char* extract_filename(int argc, char** argv) {
    for (int i=1; i<argc; i++) {
        if (argv[i][0] != '-') return argv[i];
    }
    return nullptr;
}

int main(int argc, char** argv) {
    
    //
    // Process the command-line, including any flags.
    //
    bool  debug_lex = check_flag(argc,argv,"--tokens");
    bool  pprint    = check_flag(argc,argv,"--pprint");
    bool  testing   = check_flag(argc,argv,"--test");
    char* filename  = extract_filename(argc,argv);
    
    if (filename) {
        //
        // Catch SLPY errors.
        //
        try {
            
            //
            // Process file to create a stream of tokens.
            //
            Tokenizer lexer { filename };
            TokenStream tks = lexer.lex();

            //
            // Report the tokens.
            //
            if (debug_lex) {
                std::cout << "----------------------------------" << std::endl;
                std::cout << "#";
                while (!tks.at_EOF()) {
                    std::cout << tks.current() << "#";
                    tks.advance();
                }
                std::cout << std::endl;
                std::cout << "----------------------------------" << std::endl;
            }

            //
            // Parse.
            //
            tks.reset();
            Prgm_ptr prgm = parse(tks);

            //
            // Make sure the whole source code was parsed.
            //
            if (!tks.at_EOF()) {
                //
                // Untested because of how parseBlck works.
                //
                throw SlpyError { tks.locate(), "Error: extra unparsed characters.\n" };
            }

            //
            // Either pretty print or run the parsed code.
            //
            if (pprint) {
                prgm->output(std::cout);
            } else {
                prgm->run();
            }
            
        } catch (SlpyError se) {
            
            //
            // If --test then just give "ERROR" message
            //
            if (testing) {
                std::cout << "ERROR" << std::endl;
            } else {
                std::cerr << se.what();
            }
        }
    } else {
        std::cerr << "usage: "
                  << argv[0]
                  << " [--tokens] [--pprint] [--test] file"
                  << std::endl;
    }
}
