#include <cstddef>
#include <iostream>
#include "type_utils.hxx"


// template<class T>
// struct SizeofReducer {
//     constexpr auto operator()() {
//         return sizeof(T);
//     }

//     template<class NT>
//     constexpr auto feed() {
//         if constexpr (sizeof(NT) > sizeof(T))
//             return SizeofReducer<NT>{};
//         else
//             return SizeofReducer<T>{};
//     }
// };

using size_list = TList<char, int, long long>;
constexpr size_t max_size = size_list::template reduce<SizeofReducer>()();

int main() {
    using my_list = TList<char, int>;

    static_assert(static_type_eq<my_list,
                  decltype(my_list::template apply<TList>())>());

    std::cout << max_size << std::endl;
}
