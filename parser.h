//
// Created by indalamar on 03.06.19.
//

#ifndef NUCLEAR_PARSER_H
#define NUCLEAR_PARSER_H

#include "operation.h"
#include <string>


class parser {
public :

    int current = 0;
    std::string str = "";

    parser() = default;

    std::string delete_spaces(std::string &exp) {
        std::string answer;
        for (auto &i : exp) if (i != ' ') answer.push_back(i);
        return answer;
    }

    operation *parse(std::string &exp) {
        str = delete_spaces(exp);
        current = 0;
        return parseSO();
    }


    operation *parseSO() {
        operation *answer = parseOR();
        while (current < str.size() && str[current] == '-') {
            current += 2;
            operation *answer2 = parseSO();
            answer = new SO(answer, answer2);
        }
        return answer;
    }

    operation *parseOR() {
        operation *answer = parseAND();
        while (current < str.size() && str[current] == '|') {
            ++current;
            operation *answer2 = parseAND();
            answer = new OR(answer, answer2);
        }
        return answer;
    }

    operation *parseAND() {
        operation *answer = parseB();
        for (;;) {
            if (current == str.size() || str[current] != '&') return answer;
            current++;
            operation *answer2 = parseB();
            answer = new AND(answer, answer2);
        }
    }

    operation *parseB() {
        if (str[current] == '!') {
            current++;
            operation *answer = parseB();
            return new DEC(answer);
        } else {
            if (str[current] == '@') {
                current++;
                operation *var = parseVar();
                ++current ;
                operation *expr = parseSO();
                return new FORALL(var, expr);
            }
            if (str[current] == '?') {
                current++;
                operation *var = parseVar();
                ++current ;
                operation *expr = parseSO();
                return new EXIST(var, expr);
            }
            if (str[current] == '(') {
                ++current;
                operation *answer = parseSO();
                ++current;
                return answer;
            }

        }
        return parsePRE();
    }

    operation *parsePRE() {
        if (str[current] >= 'A' && str[current] <= 'Z') {
            std::string var = getName();
            if (str[current] == '(') ++current;
            std::multiset<TERM *, comparator> terms;
            terms.insert(parseTerm());
            while (str[current] == ',') {
                ++current;
                terms.insert(parseTerm());
            }
            ++current ;
            return new PRED(var , terms);

        }
        TERM* left  = parseTerm() ;
        ++current ;
        TERM* right = parseTerm() ;
        return new EQUAL(left,  right) ;
    }

    TERM *parseTerm() {
        TERM *answer = parseSUM();
        while (current < str.size() && str[current] == '+') {
            ++current;
            operation *answer2 = parseSUM();
            answer = new SUM(answer, answer2);
        }
        return answer;
    }

    TERM *parseSUM() {
        TERM *answer = parseMUL();
        while (current < str.size() && str[current] == '*') {
            ++current;
            TERM *answer2 = parseMUL();
            answer = new MUL(answer, answer2);
        }
        return answer;
    }

    TERM *parseMUL() {
        TERM *answer = nullptr;
        if (str[current] == '0') {
            ++current;
            answer = new ZERO();
        } else {
            if (str[current] == '(') {
                ++current ;
                answer = parseTerm();
                ++current;
            } else {
                std::string name = getName();
                if (str[current] != '(') {
                    answer = new VAR(name);
                } else {
                    ++current;
                    std::multiset<TERM *, comparator> terms;
                    terms.insert(parseTerm());
                    while (str[current] == ',') {
                        ++current ;
                        terms.insert(parseTerm());
                    }
                    ++current;
                    answer = new FUNCTOR(name, terms);
                }
            }
        }
        while (current < str.size() && str[current] == '\'') {
            answer = new INCR(answer);
            ++current;
        }
        return answer;
    }

    std::string getName() {
        std::string var;
        var.push_back(str[current]);
        ++current;
        while (current < str.size() && is_digit(str[current])) {
            var.push_back(str[current]);
            current++;
        }
        return var;
    }

    operation *parseVar() {
        std::string var = getName();
        operation *operation1 = new VAR(var);
        return operation1;
    }

    bool is_digit(char t) {
        return (t >= '0') && (t <= '9');
    }

   /* bool is_operation(char t) {
        return t == '-' || t == '&' || t == '!' || t == '|' || t == '(' || t == ')';
    }*/

};

#endif //NUCLEAR_PARSER_H
