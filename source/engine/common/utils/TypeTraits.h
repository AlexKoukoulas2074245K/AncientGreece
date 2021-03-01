///------------------------------------------------------------------------------------------------
///  TypeTraits.h
///  Genesis
///
///  Created by Alex Koukoulas on 20/11/2019.
///-----------------------------------------------------------------------------------------------

#ifndef TypeTraits_h
#define TypeTraits_h

///-----------------------------------------------------------------------------------------------

#include <string>
#include <typeindex>
#include <utility> // hash

///-----------------------------------------------------------------------------------------------

static std::hash<std::string> hashFunction;

///-----------------------------------------------------------------------------------------------

class TypeID
{
    static size_t counter;

public:
    template<class T>
    static size_t value()
    {
        static size_t id = counter++;
        return id;
    }
};

///-----------------------------------------------------------------------------------------------
/// Compute a unique integer id for a given template class.
/// @tparam T the type class to generate the unique id for.
/// @returns a unique id for given template parameter class type.
template<class T>
inline std::size_t GetTypeHash()
{
    return TypeID::value<T>();
}

///-----------------------------------------------------------------------------------------------
/// Compute a unique hash for a given string.
/// @param[in] s the input string.
/// @returns the hashed input string.
inline std::size_t GetStringHash(const std::string& s)
{
    return hashFunction(s);
}

///-----------------------------------------------------------------------------------------------

#endif /* TypeTraits_h */

