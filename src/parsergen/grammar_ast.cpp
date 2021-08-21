#include "parsergen/grammar_ast.hpp"
#include "parsergen/utils.hpp"

namespace Parsergen {
    GrammarLexer::GrammarLexer() : Lexer(){
        rules = {
            S_RULE("ID", "[a-z0-9_]+"),
            S_RULE("TOKEN", "[A-Z0-9_]+"),
            S_RULE("COLON", "\\:"),
            S_RULE("OR", "\\|"),
            S_RULE("STAR", "\\*"),
            S_RULE("PLUS", "\\+"),
            S_RULE("QMARK", "\\?"),
            S_RULE("LPAREN", "\\("),
            S_RULE("RPAREN", "\\)"),
            S_RULE("TERMINATE", "\\;"),
            S_RULE("EQ", "\\="),
            S_RULE("NOT", "\\!"),
            S_RULE("AND", "\\&"),
            S_RULE("AT", "\\@"),
            F_RULE("ACTION", "\\{([\\s\\S]+?)\\}\\s*(?=;\\s*(\n|$))",
                [this] (Token &tok, utils::svmatch &sm) {
                    //fmt::print("ACTION MATCHED: \n\n BEGIN:\n {}\nEND\n", sm.str());
                    
                    tok.value = sm.str(1);
                    utils::trim(tok.value);

                }
            ),
            F_RULE("STRING", "'", 
                [this] (Token &tok, utils::svmatch &sm) {
                    char end = '\'';
                    bool escape = false;
                    std::string result = "";

                    while (true){
                        if (source_left()){
                            char cur = source[0];
                            StepSource(1);
                            if (!escape){
                                if (cur == '\\'){
                                    if (!source_left()) continue;
                                    cur = source[0]; // move to char that is escaped
                                    StepSource(1);
                                    escape = true;
                                }
                                else if (cur == end){
                                    tok.value = result;
                                    return;
                                }
                            }
                            if (cur == '\n')
                                newline();
                            result += cur;
                            escape = false;
                        }
                        else throw std::runtime_error("No End of string");
                    }

                }
            ),
            F_RULE("RETURN_TYPE", "\\[([a-zA-Z0-9_:<>]+)\\]",
                [this] (Token &tok, utils::svmatch &sm) {
                    tok.value = sm.str(1);
                }
            ),
            F_RULE("NEWLINE", "\n",
                [this] (Token &tok, utils::svmatch &sm) {
                    newline();
                    throw NoToken();
                }
            ),
            F_RULE("ignore", "[ \t]+",
                [this] (Token &tok, utils::svmatch &sm) {
                    throw NoToken();
                }
            )
        };
    }
}