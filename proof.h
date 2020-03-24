//
// Created by indalamar on 05.06.19.
//

#ifndef NUCLEAR_PROOF_H
#define NUCLEAR_PROOF_H

#include <string>
#include "operation.h"
#include <unordered_map>
#include <vector>
#include "parser.h"
#include <unordered_set>
#include "axiom.h"
#include <fstream>
#include "axiom_arifmetic.h"
#include "A9.h"
#include "axiom_formal.h"
#include <iostream>


struct operation_hash {
    std::size_t operator()(const operation *k) const {
        return k->hash();
    }
};

struct operation_equality {
    bool operator()(operation *k, operation *s) const {
        return k->operator==(s);
    }
};


struct META {
    int way = -1;
    int id = -1;
};

struct MP {
    std::unordered_set<operation *, operation_hash, operation_equality> possible_M = {};
};

struct MP_total {
    operation *L = nullptr;
};


class proof {
private :

    int cur_string = 0;

    std::string A;

    std::string first_shit;

    operation *A_op = nullptr;

    std::vector<axiom> axioms;
    std::vector<axiom_arifmetic> axioms_arifmetic;
    axiom_formal axiom_f;
    A9 a9;


    std::unordered_map<operation *, int, operation_hash, operation_equality>
            hypothesis;

    std::unordered_map<operation *, META, operation_hash, operation_equality> printed;


    std::unordered_map<operation *, MP, operation_hash, operation_equality> MP_check;

    std::unordered_map<operation *, MP_total, operation_hash, operation_equality> MP_correct;


    std::vector<operation *> proofment;


    parser parser1;


public :
    explicit proof() {
        std::vector<std::string> axiomss(10);
        axioms.resize(10);
        axiomss[0] = "A(0)->(B(0)->A(0))";
        axiomss[1] = "(A(0)->B(0))->((A(0)->B(0)->C(0))->(A(0)->C(0)))";
        axiomss[2] = "A(0)->(B(0)->(A(0)&B(0)))";
        axiomss[3] = "(A(0)&B(0))->A(0)";
        axiomss[4] = "(A(0)&B(0))->B(0)";
        axiomss[5] = "A(0) ->(A(0)|B(0))";
        axiomss[6] = "B(0)->(A(0)|B(0))";
        axiomss[7] = "(A(0)->C(0))->((B(0)->C(0))->((A(0)|B(0))->C(0)))";
        axiomss[8] = "(A(0)->B(0))->((A(0)->!B(0))->!A(0))";
        axiomss[9] = "(!!A(0))->A(0)";
        for (int i = 0; i < 10; ++i) axioms[i] = {parser1.parse(axiomss[i])};
        std::vector<std::string> arifmetic_axioms(8);
        axioms_arifmetic.resize(8);
        arifmetic_axioms[0] = "a=b->a'=b'";
        arifmetic_axioms[1] = "a=b->a=c->b=c";
        arifmetic_axioms[2] = "a'=b'->a=b";
        arifmetic_axioms[3] = "!a'=0";
        arifmetic_axioms[4] = "a+b'=(a+b)'";
        arifmetic_axioms[5] = "a+0=a";
        arifmetic_axioms[6] = "a*0=0";
        arifmetic_axioms[7] = "a*b'=a*b+a";
        for (auto i = 0; i < arifmetic_axioms.size(); ++i) axioms_arifmetic[i] = {parser1.parse(arifmetic_axioms[i])};


    }

    bool check_proofment() {
        take_context();
        if (get_proof()) {
            if (!check_last()) std::cout << "Required hasn't been proven";
            else std::cout << "Proof is correct";
        } else std::cout << "Line #" << cur_string << " can't be obtained";
        for (auto &i : printed) delete i.first;
        return true;
    }

    void take_context() {
        std::getline(std::cin, first_shit);
        int stop_position = find_hamma_symbol();
        fill_hypothesis(stop_position);
        get_A(stop_position);


    }

    void get_A(int stp_pstn) {
        for (int i = stp_pstn + 2; i < first_shit.size(); ++i) A.push_back(first_shit[i]);
        A_op = parser1.parse(A);
    }

    int find_hamma_symbol() {
        for (int i = 1; i < first_shit.size(); ++i)
            if (first_shit[i] == '-' && first_shit[i - 1] == '|') return i - 1;
        return 0;
    }

    void fill_hypothesis(int stp_pstn) {
        std::string str;
        int count = 1;
        for (int i = 0; i < stp_pstn; ++i) {
            if (first_shit[i] == ',') {
                hypothesis[parser1.parse(str)] = count;
                ++count;
                str.clear();
            } else str.push_back(first_shit[i]);
        }
        if (!str.empty()) hypothesis[parser1.parse(str)] = count;
    }

    bool is_printed(operation *oper) {
        return printed.find(oper) != printed.end();
    }

    bool check_last() {
        return proofment.back()->operator==(A_op);
    }

    bool get_proof() {
        std::string box;
        while (std::getline(std::cin, box)) {
            if (box.empty()) return true;
            ++cur_string;
            operation *compose = parser1.parse(box);

            if (!is_printed(compose)) {
                if (is_hypothesis(compose) || is_scheme(compose) || is_modus_ponens(compose) || is_formal(compose) ||
                    is_A9(compose) || is_bernaice_exist(compose) || is_bernaice_forall(compose)) {
                    proofment.push_back(compose);
                    it_can_be_modens_ponens(compose);
                } else return false;
            }
        }
        return true;
    }


    bool is_hypothesis(operation *oper) {
        if (hypothesis.find(oper) != hypothesis.end()) {
            if (printed.find(oper) == printed.end()) printed[oper] = {0, 0};
            return true;
        }
        return false;
    }

    bool is_scheme(operation *oper) {
        for (int i = 0; i < 10; ++i) {
            if (axioms[i].is_scheme(oper)) {
                if (printed.find(oper) == printed.end()) printed[oper] = {1, i + 1};
                return true;
            }
        }
        for (int i = 0; i < 8; ++i) {
            if (axioms_arifmetic[i].is_axi(oper)) {
                if (printed.find(oper) == printed.end()) printed[oper] = {1, i + 1};
                return true;
            }
        }
        return false;
    }


    bool it_can_be_modens_ponens(operation *oper) {
        if (oper->operator_sign == "->") {
            auto *box = dynamic_cast<abstract_operaiton *> (oper);
            MP_check[box->right].possible_M.insert(box);
            return true;
        }
        return false;
    }


    bool is_modus_ponens(operation *oper) {
        if (MP_check.find(oper) == MP_check.end()) return false;
        if (!MP_check[oper].possible_M.empty()) {
            for (auto j : MP_check[oper].possible_M) {
                if (printed.find(dynamic_cast<abstract_operaiton *>(j)->left) != printed.end()) {
                    if (printed.find(oper) == printed.end())printed[oper] = {2, 0};
                    MP_correct[oper].L = j;
                    return true;
                }
            }
        }
        return false;
    }

    bool is_A9(operation *oper) {
        if (a9.is_scheme(oper)) {
            if (printed.find(oper) == printed.end()) printed[oper] = {1, 0};
            return true;
        }
        return false;
    }

    bool is_formal(operation *oper) {
        if (axiom_f.is_scheme(oper)) {
            if (printed.find(oper) == printed.end()) printed[oper] = {1, 0};
            return true;
        }
        return false;
    }

    bool is_bernaice_exist(operation *oper) {
        if (dynamic_cast<SO *>(oper) != nullptr) {
            auto *box = dynamic_cast<SO *>(oper);
            if (dynamic_cast<EXIST *>(box->left) != nullptr) {
                auto *box1 = dynamic_cast<EXIST *>(box->left);
                auto *check = new SO(box1->expr, box->right);
                if (printed.count(check) != 0) {
                    std::string locked_var = box1->var->print();
                    delete check;
                    if (is_chained(locked_var, box->right)) {
                        if (printed.count(oper) == 0) printed[oper] = {3, 0};
                        return true;
                    }
                    return false;
                }
            }
            return false;
        }
        return false;
    }

    template<class R>
    R *can_be_casted(operation *casted) {
        if (dynamic_cast<R *>(casted) != nullptr) {
            auto *box = dynamic_cast<R *>(casted);
            return box;
        }
        return nullptr;
    }

    bool is_bernaice_forall(operation *oper) {
        if (dynamic_cast<SO *>(oper) != nullptr) {
            auto *box = dynamic_cast<SO *>(oper);
            if (dynamic_cast<FORALL *>(box->right) != nullptr) {
                auto *box1 = dynamic_cast<FORALL *>(box->right);
                auto *check = new SO(box->left, box1->expr);
                if (printed.count(check) != 0) {
                    std::string locked_var = box1->var->print();
                    delete check;
                    if (is_chained(locked_var, box->left)) {
                        if (printed.count(oper) == 0) printed[oper] = {3, 0};
                        return true;
                    }
                }
                return false;
            }
            return false;
        }
        return false;
    }

    bool is_chained(std::string &var, operation *oper) {
        auto * ao = can_be_casted<abstract_operaiton>(oper);
        if (ao != nullptr ) {
            if (ao->operator_sign != "->") return is_chained(var, ao->left) && is_chained(var,ao->right);
            else return is_chained(var,ao->right);
        }
        auto *quanter = can_be_casted<QUANTER>(oper);
        if (quanter != nullptr) {
            if (quanter->var->print() == var) return true;
            return is_chained(var, quanter->expr);
        }
        auto *dec = can_be_casted<DEC>(oper);
        if (dec != nullptr) return is_chained(var, dec->mid);
        auto *predicate = can_be_casted<PREDICATE>(oper);
        if (predicate != nullptr) {
            if (oper->operator_sign == "=") {
                auto *equal = can_be_casted<EQUAL>(oper);
                return is_chained(var, equal->left) && is_chained(var, equal->right);
            } else {
                auto *pred = can_be_casted<PRED>(oper);
                for (auto &i : pred->terms) if (!is_chained(var, i)) return false;
                return true;
            }
        }
        auto *func = can_be_casted<FUNCTOR>(oper);
        if (func != nullptr) {
            for (auto &i : func->terms) if (!is_chained(var, i)) return false;
            return true;
        }
        auto *incr = can_be_casted<INCR>(oper);
        if (incr != nullptr) return is_chained(var, incr->mid);
        auto *varr = can_be_casted<VAR>(oper);
        if (varr != nullptr) return var != varr->print();
        return true;
    }


};


#endif //NUCLEAR_PROOF_H
