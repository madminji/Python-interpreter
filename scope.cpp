#include "scope.hpp"
#include <iomanip>
#include <stdexcept>
#include <sstream>
#include <string>
#include "interpreter.hpp"
#include "token.hpp"
#include "tokentype.hpp"
#include "runtime_exceptions.hpp"

void Scope::decRefCount(PyObject* value){

    if(value != nullptr) value->decRc();
}

void Scope::incRefCount(PyObject* value){

    if(value != nullptr) value->incRc();
}

void Scope::define(const std::string& name, PyObject* value) {

    auto it = values.find(name);

    if (it != values.end()) {
        decRefCount(it->second);
        it->second = value;
        incRefCount(it->second);
    } else {
        values.emplace(name, value);
        incRefCount(value);
    }
}

PyObject* Scope::get(const std::string& name) {

    auto it = values.find(name);

    if (it != values.end()) return it->second;

    if(parent) return parent->get(name);

    throw std::runtime_error("Undeclared variable '" + name + "'");
}


PyObject* Scope::peek(const std::string& name) {

    auto it = values.find(name);

    if (it != values.end()) return it->second;

    return  (PyObject*)nullptr;
}

Scope::~Scope() {
    for (auto& kv : values) {
        delete kv.second;
    }
}
