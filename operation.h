//
// Created by indalamar on 03.06.19.
//

#ifndef NUCLEAR_OPERATION_H
#define NUCLEAR_OPERATION_H

#include <string>
#include <set>


class operation {
public:
    mutable bool result = false;
    std::string operator_sign;

    virtual std::string print() const = 0;

    virtual size_t hash() const = 0;

    virtual bool operator==(operation *oper) const = 0;

    virtual bool operator!=(operation *oper) const {
        return !(this->operator==(oper));
    }

    virtual bool evaluate() const = 0;

    virtual ~operation() = default;




};

class abstract_operaiton : virtual public operation {
public :
    operation *left;
    operation *right;

    bool evaluate() const override {
        result = action(left->evaluate(), right->evaluate());
        return result;
    }

    virtual bool action(bool first, bool second) const = 0;

    abstract_operaiton(operation *left, operation *right) : left(left), right(right) {};

    std::string print() const override {
        return "(" + left->print() + ")" + operator_sign + "(" + right->print() + ")";
    }

    size_t hash() const override {
        return left->hash() * 31 + right->hash() * 17 + (std::hash<std::string>{}(operator_sign));
    }

    bool operator==(operation *oper) const override {
        if (dynamic_cast<abstract_operaiton *> (oper) != nullptr) {
            auto *box = dynamic_cast<abstract_operaiton *> (oper);
            return operator_sign == box->operator_sign && left->operator==(box->left) &&
                   right->operator==(box->right);
        }
        return false;
    }


};

using AO = abstract_operaiton;
using first = operation;

class AND : virtual public AO {
public :
    AND(first *left, first *right) : abstract_operaiton(left, right) {
        this->operator_sign = "&";
    };

    bool action(bool first, bool second) const override {
        return first & second;
    }
};

class OR : virtual public AO {
public :
    OR(first *left, first *right) : AO(left, right) {
        this->operator_sign = "|";
    };

    bool action(bool first, bool second) const override {
        return first | second;
    }
};

class SO : virtual public AO {
public :
    SO(first *left, first *right) : AO(left, right) {
        this->operator_sign = "->";
    };

    bool action(bool first, bool second) const override {
        return (!first) | second;
    }
};

class DEC : virtual public operation {
public :
    first *mid;

    explicit DEC(first *mid) : mid(mid) { this->operator_sign = "!"; };

    std::string print() const override {
        return "!" + mid->print();
    }

    size_t hash() const override {
        return mid->hash() * 71;
    }

    bool operator==(operation *oper) const override {
        if (dynamic_cast<DEC * > (oper) != nullptr) {
            auto *box = dynamic_cast<DEC *> (oper);
            return mid->operator==(box->mid);
        }
        return false;
    }

    bool evaluate() const override {
        result = !mid->evaluate();
        return result;
    }

};


class TERM : virtual public operation {
};

class BINARYTERM : virtual public TERM {
public:
    operation *left;
    operation *right;

    BINARYTERM(operation *left, operation *right) : left(left), right(right) {};

    std::string print() const override {
        return "(" + left->print() + operator_sign + right->print() + ")";
    }

    size_t hash() const override {
        return left->hash() * 13 + right->hash() * 23 + std::hash<std::string>{}(operator_sign);
    }

    bool operator==(operation *oper) const override {
        if (dynamic_cast<BINARYTERM *>(oper) != nullptr) {
            auto *box = dynamic_cast<BINARYTERM *>(oper);
            return operator_sign == box->operator_sign && left->operator==(box->left) && right->operator==(box->right);
        }
        return false;
    }

    bool evaluate() const override { return true; }

};

class SUM : virtual public BINARYTERM {
public :
    SUM(operation *left, operation *right) : BINARYTERM(left, right) {
        operator_sign = "+";
    }
};

class MUL : virtual public BINARYTERM {
public :
    MUL(operation *left, operation *right) : BINARYTERM(left, right) {
        operator_sign = "*";
    }
};

class VAR : virtual public TERM {
public :
    std::string var;


    explicit VAR(std::string &var) : var(var) {
        this->operator_sign = "var";
    };

    std::string print() const override {
        return var;
    }

    size_t hash() const override {
        return (std::hash<std::string>{}(var));
    }

    bool operator==(operation *oper) const override {
        if (dynamic_cast<VAR *> (oper) != nullptr) {
            auto *box = dynamic_cast<VAR *> (oper);
            return var == box->var;
        }
        return false;
    }

    bool evaluate() const override {
        return result;
    }

};

struct comparator {
    bool operator()(const TERM *left, const TERM *right) {
        return left->print() < right->print();
    }
};

class FUNCTOR : virtual public TERM {
public:

    FUNCTOR(std::string &name, std::multiset<TERM *, comparator> &set) : name(name), terms(set) {
        this->operator_sign = "functor";
    };

    std::string print() const override {
        auto res = name + "(";
        for (auto &i : terms) res += i->print() + ",";
        res.pop_back();
        res.push_back(')');
        return res;
    }

    size_t hash() const override {
        auto res = std::hash<std::string>{}(name);
        for (auto &i : terms) res += i->hash() * 13;
        res += 23;
        return res;
    }

    bool operator==(operation *oper) const override {
        if (dynamic_cast<FUNCTOR *>(oper) != nullptr) {
            auto box = dynamic_cast<FUNCTOR *>(oper);
            if (name != box->name) return false;
            if (terms.size() != box->terms.size()) return false;
            auto iter1 = terms.begin();
            auto iter2 = box->terms.begin();
            for (auto i = 0; i < terms.size(); ++i) {
                if ((*iter1)->operator!=(*iter2)) return false;
                ++iter1, ++iter2;
            }
            return true;
        }
        return false;
    }

    bool evaluate() const override { return true; }

    std::string name;
    std::multiset<TERM *, comparator> terms;


};

class INCR : virtual public TERM {
public:


    explicit INCR(TERM *mid) : mid(mid) {};

    std::string print() const override {
        return mid->print() + "t";
    }

    size_t hash() const override {
        return mid->hash() + 87;
    }

    bool operator==(operation *oper) const override {
        if (dynamic_cast<INCR *>(oper) != nullptr) {
            auto *box = dynamic_cast<INCR *>(oper);
            return mid->operator==(box->mid);
        }
        return false;
    }

    bool evaluate() const override { return false; }

    TERM *mid;

};

class ZERO : virtual public TERM {
public:

    std::string print() const override {
        return "0";
    }

    size_t hash() const override {
        return 0;
    }

    bool operator==(operation *oper) const override {
        return dynamic_cast<ZERO *>(oper) != nullptr;
    }

    bool evaluate() const override { return true; }

};

class PREDICATE : virtual public operation {
};

class PRED : virtual public PREDICATE {
public:

    PRED(std::string &name, std::multiset<TERM *, comparator> &terms) : name(name), terms(terms) {
        this->operator_sign = "predicate";
    };

    std::string print() const override {
        auto res = name + "(";
        for (auto &i : terms) res += i->print() + ",";
        res.pop_back();
        res.push_back(')');
        return res;
    }

    size_t hash() const override {
        auto res = std::hash<std::string>{}(name);
        for (auto &i : terms) res += i->hash() * 13;
        res += 23;
        return res;
    }

    bool operator==(operation *oper) const override {
        if (dynamic_cast<PRED *>(oper) != nullptr) {
            auto box = dynamic_cast<PRED *>(oper);
            if (name != box->name) return false;
            if (terms.size() != box->terms.size()) return false;
            auto iter1 = terms.begin();
            auto iter2 = box->terms.begin();
            for (auto i = 0; i < terms.size(); ++i) {
                if ((*iter1)->operator!=(*iter2)) return false;
                ++iter1, ++iter2;
            }
            return true;
        }
        return false;
    }

    bool evaluate() const override { return true; }

    std::string name;
    std::multiset<TERM *, comparator> terms;

};

class EQUAL : virtual public PREDICATE {
public:
    EQUAL(TERM *left, TERM *right) : left(left), right(right) {
        this->operator_sign = "=";
    };

    std::string print() const override {
        return "(" + left->print() + ")=(" + right->print() + ")";
    }

    size_t hash() const override {
        return left->hash() * 13 + right->hash() * 23 + 17;
    }

    bool operator==(operation *oper) const override {
        if (dynamic_cast<EQUAL *>(oper) != nullptr) {
            auto *box = dynamic_cast<EQUAL *>(oper);
            return left->operator==(box->left) && right->operator==(box->right);
        }
        return false;
    }

    bool evaluate() const override { return false; }

    TERM *left;
    TERM *right;

};

class QUANTER : virtual public operation {
public:
    std::string print() const override {
        return operator_sign + var->print() + ".(" + expr->print() + ")";
    }

    size_t hash() const override {
        return std::hash<std::string>{}(operator_sign) * 13 + var->hash() * 23 + expr->hash() * 71;
    }

    bool operator==(operation *oper) const override {
        if (dynamic_cast<QUANTER *>(oper) != nullptr) {
            auto *box = dynamic_cast<QUANTER *>(oper);
            return operator_sign == box->operator_sign && var->operator==(box->var) && expr->operator==(box->expr);
        }
        return false;
    }

    bool evaluate() const override { return true; }

    QUANTER(operation *var, operation *expr) : var(var), expr(expr) {};

    operation *var;
    operation *expr;

};

class FORALL : virtual public QUANTER {
public :
    FORALL(operation *var, operation *expr) : QUANTER(var, expr) {
        operator_sign = "@";
    }
};

class EXIST : virtual public QUANTER {
public :
    EXIST(operation *var, operation *expr) : QUANTER(var, expr) {
        operator_sign = "?";
    }
};

#endif //NUCLEAR_OPERATION_H
