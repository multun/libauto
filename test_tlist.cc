#include <cstddef>
#include <iostream>
#include "type_utils.hxx"

using size_list = TList<char, int, long long>;

int main() {
    using my_list = TList<char, int>;

    static_assert(static_type_eq<my_list,
                                 decltype(my_list::template apply<TList>())>(),
                  "map test failed");

    static_assert(size_list::template reduce<SizeofReducer>()()
		  == sizeof(long long),
		  "reduce test failed");
}
