//
// Created by indalamar on 04.08.19.
//

#ifndef NUCLEAR_AXIOM_ARIFMETIC_H
#define NUCLEAR_AXIOM_ARIFMETIC_H


#include <unordered_map>
#include "operation.h"

class axiom_arifmetic {
public :
    operation *kernel = nullptr;
    std::unordered_map<std::string, operation *> flow_map;

    axiom_arifmetic() = default;

    axiom_arifmetic(operation *kernel) : kernel(kernel) {};


    bool is_axiom(operation *expr, operation *kern) {
        if (dynamic_cast<abstract_operaiton *>(kern) != nullptr) {
            if (expr->operator_sign != kern->operator_sign) return false;
            auto *box1 = dynamic_cast<abstract_operaiton *>(expr);
            auto *box2 = dynamic_cast<abstract_operaiton *>(kern);
            return is_axiom(box1->left, box2->left) && is_axiom(box1->right, box2->right);
        }
        if (dynamic_cast<BINARYTERM *>(kern) != nullptr) {
            if (expr->operator_sign != kern->operator_sign) return false;
            auto *box1 = dynamic_cast<BINARYTERM *>(expr);
            auto *box2 = dynamic_cast<BINARYTERM *>(kern);
            return is_axiom(box1->left, box2->left) && is_axiom(box1->right, box2->right);
        }
        if (dynamic_cast<DEC *>(kern) != nullptr) {
            if (expr->operator_sign != kern->operator_sign) return false;
            auto *box1 = dynamic_cast<DEC *>(expr);
            auto *box2 = dynamic_cast<DEC *>(kern);
            return is_axiom(box1->mid, box2->mid);
        }
        if (dynamic_cast<INCR *>(kern) != nullptr) {
            if (dynamic_cast<INCR *>(expr) == nullptr) return false;
            auto *box1 = dynamic_cast<INCR *> (expr);
            auto *box2 = dynamic_cast<INCR *> (kern);
            return is_axiom(box1->mid, box2->mid);
        }
        if (dynamic_cast<EQUAL *>(kern) != nullptr) {
            if (expr->operator_sign != kern->operator_sign) return false;
            auto *box1 = dynamic_cast<EQUAL *>(expr);
            auto *box2 = dynamic_cast<EQUAL *>(kern);
            return is_axiom(box1->left, box2->left) && is_axiom(box1->right, box2->right);
        }
        if (dynamic_cast<VAR *>(kern) != nullptr) {
            if (expr->operator_sign != kern->operator_sign && expr->print() != "0") return false;
            std::string save_string = kern->print();
            if (flow_map.count(save_string) == 0) {
                flow_map[save_string] = expr;
                return true;
            } else return flow_map[save_string]->operator==(expr);
        }
        if (dynamic_cast<ZERO *>(kern) != nullptr) {
            return kern->print() == expr->print();
        }
        return false;
    }

    bool is_axi(operation *expr) {
        flow_map.clear();
        return is_axiom(expr, kernel);
    }

};


#endif //NUCLEAR_AXIOM_ARIFMETIC_H
