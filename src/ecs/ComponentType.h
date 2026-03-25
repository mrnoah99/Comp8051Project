#include <cstddef>

//alias for standard size t, used for representing sizes/indices because it's unsigned and long long
using ComponentTypeID = std::size_t;

//free function defined in header, each cpp file that includes it would get its own definition which causes linker
//errors. using inline merges them into one definition, returns a unique number every time we call it.
//making id static means we keep its value across multiple calls of the function
inline ComponentTypeID getComponentTypeID() {
    static ComponentTypeID id = 0;
    return id++;
}

//template allows use of generic types
//template is inline by default
template <typename T>
ComponentTypeID getComponentTypeID() {
    static ComponentTypeID typeID = getComponentTypeID();
    return typeID;
}