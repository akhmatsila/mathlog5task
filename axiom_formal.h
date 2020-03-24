//
// Created by indalamar on 04.08.19.
//

#ifndef NUCLEAR_AXIOM_FORMAL_H
#define NUCLEAR_AXIOM_FORMAL_H


#include "operation.h"

class axiom_formal {
public :


    bool is_scheme(operation *expr) {
        replaces_var = nullptr;
        if (expr->operator_sign == "->") {
            auto *box = dynamic_cast<abstract_operaiton *>(expr);
            auto *left = box->left;
            if (left->operator_sign == "@" || box->right->operator_sign == "?") {
                QUANTER *box1 = nullptr;
                if (left->operator_sign == "@") box1 = dynamic_cast<FORALL *>(left);
                else box1 = dynamic_cast<EXIST *>(box->right);
                auto quanter_var = box1->var->print();
                auto ans = is_substance(quanter_var, box1->expr, left->operator_sign == "@" ? box->right : box->left);
                if (left->operator_sign=="@"&&!check_correctness(box1->expr , replaces_var)) return false;
                return ans;
            }
            return false;
        }
        return false;
    }

    bool is_substance(std::string &var, operation *left, operation *right) {
        if (dynamic_cast<abstract_operaiton *>(left) != nullptr) {
            if (left->operator_sign != right->operator_sign) return false;
            auto *box1 = dynamic_cast<abstract_operaiton *>(left);
            auto *box2 = dynamic_cast<abstract_operaiton *>(right);
            return is_substance(var, box1->left, box2->left) && is_substance(var, box1->right, box2->right);
        }
        if (dynamic_cast<DEC *>(left) != nullptr) {
            if (left->operator_sign == right->operator_sign) {
                auto *box = dynamic_cast<DEC *>(left);
                auto *box2 = dynamic_cast<DEC *>(right);
                return is_substance(var, box->mid, box2->mid);
            }
            return false;
        }
        if (dynamic_cast<BINARYTERM *>(left) != nullptr) {
            if (left->operator_sign != right->operator_sign) return false;
            auto *box = dynamic_cast<BINARYTERM *>(left);
            auto *box2 = dynamic_cast<BINARYTERM *>(right);
            return is_substance(var, box->left, box2->left) && is_substance(var, box->right, box2->right);
        }
        if (dynamic_cast<INCR *>(left) != nullptr) {
            if (left->operator_sign != right->operator_sign) return false;
            auto *box = dynamic_cast<INCR *>(left);
            auto *box2 = dynamic_cast<INCR *>(right);
            return is_substance(var, box->mid, box2->mid);
        }
        if (dynamic_cast<QUANTER *>(left) != nullptr) {
            if (left->operator_sign != right->operator_sign) return false;
            auto *box = dynamic_cast<QUANTER *>(left);
            auto *box2 = dynamic_cast<QUANTER *>(right);
            if (box->var->print() != box2->var->print()) return false;
            return is_substance(var, box->expr, box2->expr);
        }
        if (dynamic_cast<PREDICATE *>(left) != nullptr) {
            if (left->operator_sign != right->operator_sign) return false;
            if (left->operator_sign == "=") {
                auto *box = dynamic_cast<EQUAL *>(left);
                auto *box2 = dynamic_cast<EQUAL *>(right);
                return is_substance(var, box->left, box2->left) && is_substance(var, box->right, box2->right);
            } else {
                auto *box = dynamic_cast<PRED *>(left);
                auto *box2 = dynamic_cast<PRED *>(right);
                if (box->name != box2->name) return false;
                if (box->terms.size() != box2->terms.size()) return false;
                auto lI = box->terms.begin();
                auto rI = box2->terms.begin();
                for (auto i = 0; i < box->terms.size(); ++i) {
                    if (!is_substance(var, *lI, *rI)) return false;
                    ++lI, ++rI;
                }
                return true;
            }
        }
        if (dynamic_cast<ZERO *>(left) != nullptr) {
            return (dynamic_cast<ZERO *>(right) != nullptr);
        }
        if (dynamic_cast<FUNCTOR *>(left) != nullptr) {
            if (left->operator_sign != right->operator_sign) return false;
            auto *box = dynamic_cast<FUNCTOR *>(left);
            auto *box2 = dynamic_cast<FUNCTOR *>(right);
            if (box->name != box2->name) return false;
            if (box->terms.size() != box2->terms.size()) return false;
            auto lI = box->terms.begin();
            auto rI = box2->terms.begin();
            for (auto i = 0; i < box->terms.size(); ++i) {
                if (!is_substance(var, *lI, *rI)) return false;
                ++lI, ++rI;
            }
            return true;
        }
        if (dynamic_cast<VAR *>(left) != nullptr) {
            auto *box = dynamic_cast<VAR *>(left);
            if (left->print() != var) {
                if (dynamic_cast<VAR *>(right) != nullptr) return left->print() == right->print();
                return false;
            }
            if (replaces_var == nullptr) {
                replaces_var = right;
                return true;
            } else {
                return replaces_var->operator==(right);
            }
        }
        return false;
    }

    void find_all_variables_in_term(operation *expr, std::set<std::string> &variables) {
        if (dynamic_cast<FUNCTOR *>(expr) != nullptr) {
            auto *box = dynamic_cast<FUNCTOR *>(expr);
            for (auto &i : box->terms) find_all_variables_in_term(i, variables);
        }
        if (dynamic_cast<BINARYTERM *>(expr) != nullptr) {
            auto *box = dynamic_cast<BINARYTERM *>(expr);
            find_all_variables_in_term(box->left, variables);
            find_all_variables_in_term(box->right, variables);
        }
        if (dynamic_cast<INCR *>(expr) != nullptr) {
            auto *box = dynamic_cast<INCR *>(expr);
            find_all_variables_in_term(box->mid, variables);
        }
        if (dynamic_cast<VAR *>(expr) != nullptr) {
            auto *box = dynamic_cast<VAR *>(expr);
            variables.insert(box->print());
        }
    }

    void find_all_locked_variables_quanter(operation *expr, std::set<std::string> &variables) {
        if (dynamic_cast<abstract_operaiton *>(expr) != nullptr) {
            auto *box = dynamic_cast<abstract_operaiton *>(expr);
            find_all_locked_variables_quanter(box->left, variables);
            find_all_locked_variables_quanter(box->right, variables);
        }
        if (dynamic_cast<DEC *>(expr) != nullptr) {
            auto *box = dynamic_cast<DEC *>(expr);
            find_all_locked_variables_quanter(box->mid, variables);
        }
        if (dynamic_cast<QUANTER *>(expr) != nullptr) {
            auto *box = dynamic_cast<QUANTER *>(expr);
            variables.insert(box->var->print());
            find_all_locked_variables_quanter(box->expr, variables);
        }
    }

    bool check_correctness(operation *left_part, operation *term) {
        std::set<std::string> term_variables ;
        std::set<std::string> left_part_varialbes;
        find_all_variables_in_term(term , term_variables) ;
        find_all_locked_variables_quanter(left_part , left_part_varialbes) ;
        for (auto & i : term_variables) if (left_part_varialbes.count(i) > 0 ) return false ;
        return true ;
    }


    operation *replaces_var = nullptr;
};


#endif //NUCLEAR_AXIOM_FORMAL_H
