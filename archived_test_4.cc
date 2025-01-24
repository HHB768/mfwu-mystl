// #include <type_traits>
// #include <iterator>

// // Function template with InputIterator template argument
// template<typename InputIterator, 
//          typename = typename std::enable_if<
//              std::is_base_of<std::input_iterator_tag, typename std::iterator_traits<InputIterator>::iterator_category>::value
//          >::type>
// void myFunction(InputIterator it) {
//     // Function implementation
// }

// // Example usage
// #include <vector>
// #include <iostream>

// int main() {
//     std::vector<int> vec = {1, 2, 3, 4, 5};
//     myFunction(vec.begin());

//     return 0;
// }

// #include <type_traits>
// #include <iterator>

// // Define the is_input_iterator type trait
// template<typename T>
// struct is_input_iterator {
//     static constexpr bool value = std::is_base_of<std::input_iterator_tag, typename std::iterator_traits<T>::iterator_category>::value;
// };

// // Helper variable template (available in C++14 and later)
// template<typename T>
// constexpr bool is_input_iterator_v = is_input_iterator<T>::value;

// // Function template using is_input_iterator
// template<typename InputIterator, 
//          typename std::enable_if<is_input_iterator_v<InputIterator>, int>::type = 0>
// void myFunction(InputIterator it) {
//     // Function implementation
// }

// // Example usage
// #include <vector>
// #include <iostream>

// int main() {
//     std::vector<int> vec = {1, 2, 3, 4, 5};
//     myFunction(1);

//     return 0;
// }

#include <type_traits>
#include <iterator>
// Example usage
#include <vector>
#include <iostream>
#include <type_traits>
#include <iterator>
#include "utils.hpp"
#include "iterator.hpp"
#include "vector.hpp"

// Define the is_input_iterator type trait with std::void_t
template<typename, typename = std::void_t<>>
struct is_input_iterator : std::false_type {};

template<typename T>
struct is_input_iterator<T, mfwu::void_t<typename mfwu::iterator_traits<T>::iterator_category>>
    :  mfwu::is_base_of<mfwu::input_iterator_tag, typename mfwu::iterator_traits<T>::iterator_category> {};

// // Helper variable template (available in C++14 and later)
// template<typename T>
// constexpr bool is_input_iterator_v = is_input_iterator<T>::value;

// Function template specialization for input iterators
template<typename InputIterator,
         typename std::enable_if<is_input_iterator<InputIterator>::value, int>::type = 0>
void myFunction(InputIterator it) {
    // Function implementation for input iterators
    std::cout << "Called myFunction with input iterator" << std::endl;
}

// Overload to handle double type
void myFunction(double it) {
    // Function implementation for double
    std::cout << "Called myFunction with double" << std::endl;
}

int main() {
    mfwu::vector<int> vec = {1, 2, 3, 4, 5};
    myFunction(vec.begin()); // Calls the template function for input iterators

    myFunction(1.0); // Calls the overload for double
    myFunction(1);   // Calls the overload for double

    return 0;
}
