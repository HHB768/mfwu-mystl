#include "string.hpp"

namespace mfwu{

class unit_test_string {
public:
    bool use_mfwu_string();
    bool use_mfwu_tiny_string();

private:
    template <typename Str>
    void print_basic_info(const Str& str) {
        std::cout << "";
    }
};  // endof class unit_test_string

bool unit_test_string::use_mfwu_string() {
    std::cout << "\n------- Test: use mfwu::string -------\n";

    return 0;
}

bool unit_test_string::use_mfwu_tiny_string() {


    return 0;
}

}  // endof namespace mfwu

