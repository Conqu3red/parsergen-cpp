#include "parsergen/grammar_ast.hpp"
#include "parsergen/utils.hpp"

namespace Parsergen {
    GrammarLexer::GrammarLexer() : Lexer(){
        rules = {
            token_match_fast("COLON", ":"),
            token_match_fast("OR", "|"),
            token_match_fast("STAR", "*"),
            token_match_fast("PLUS", "+"),
            token_match_fast("QMARK", "?"),
            token_match_fast("LPAREN", "("),
            token_match_fast("RPAREN", ")"),
            token_match_fast("TERMINATE", ";"),
            token_match_fast("EQ", "="),
            token_match_fast("NOT", "!"),
            token_match_fast("AND", "&"),
            token_match_fast("AT", "@"),
            token_match("ID", "[a-z0-9_]+"),
            token_match("TOKEN", "[A-Z0-9_]+"),
            token_match("ACTION", "\\{([\\s\\S]+?)\\}\\s*(?=;\\s*(\n|$))",
                [this] (Token &tok, utils::svmatch &sm) {
                    //fmt::print("ACTION MATCHED: \n\n BEGIN:\n {}\nEND\n", sm.str());
                    
                    tok.value = sm.str(1);
                    utils::trim(tok.value);

                }
            ),
            token_match_fast("STRING", "'", 
                [this] (Token &tok) {
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
            token_match("RETURN_TYPE", "\\[(.+?)\\]",
                [this] (Token &tok, utils::svmatch &sm) {
                    tok.value = sm.str(1);
                }
            ),
            token_match_fast("NEWLINE", "\n",
                [this] (Token &tok) {
                    newline();
                    throw NoToken();
                }
            ),
            token_match("ignore", "[ \t]+",
                [this] (Token &tok, utils::svmatch &sm) {
                    throw NoToken();
                }
            )
        };
    }
}