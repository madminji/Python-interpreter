#include "len.hpp"
#include "pyStr.hpp"

PyObject* calculate_total_length(std::vector<PyObject*>& values)
{
    size_t totalLength = 0;

    for (const PyObject* obj : values) {
        if (obj->isStr()) {
            const PyStr* strObj = dynamic_cast<const PyStr*>(obj);
            totalLength += strObj->getStr().length();
        }
        else {
            // Or handle the error as needed
            return nullptr;
        }
    }

    // Create a new PyObject representing the length
    return new PyInt(static_cast<int>(totalLength));
}
