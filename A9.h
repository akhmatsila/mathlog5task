//
// Created by indalamar on 04.08.19.
//

#ifndef NUCLEAR_A9_H
#define NUCLEAR_A9_H


#include "operation.h"

class A9 {
public :
    operation *replaces_var = nullptr;

    bool is_scheme(operation *expr) {
        replaces_var = nullptr;
        if (expr->operator_sign == "->") {
            auto *box = dynamic_cast<abstract_operaiton *>(expr);
            auto *formula = box->right;
            auto *left = box->left;
            if (left->operator_sign == "&") {
                auto *box2 = dynamic_cast<abstract_operaiton *>(left);
                auto *substance = box2->left;
                if (box2->right->operator_sign == "@") {
                    auto *box3 = dynamic_cast<FORALL *>(box2->right);
                    std::string var = box3->var->print();
                    if (box3->expr->operator_sign == "->") {
                        auto *box4 = dynamic_cast<abstract_operaiton *>(box3->expr);
                        auto *subformula = box4->right;
                        auto first_comp = is_substance(var, formula, substance);
                        if (dynamic_cast<ZERO *>(replaces_var) == nullptr) return false;
                        replaces_var = nullptr;
                        auto second_comp = is_substance(var, formula, subformula);
                        if (dynamic_cast<INCR *>(replaces_var) == nullptr) return false;
                        auto *box5 = dynamic_cast<INCR *>(replaces_var);
                        if (dynamic_cast<VAR *>(box5->mid) == nullptr) return false;
                        return box5->mid->print() == var && second_comp && first_comp;
                    }
                }
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
                if (dynamic_cast<VAR *>(right) != nullptr) {
                    auto box2 = dynamic_cast<VAR *>(right);
                    return box->print() == box2->print();
                }
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
};


#endif //NUCLEAR_A9_H
