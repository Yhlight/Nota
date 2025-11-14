#include <cassert>
#include <iostream>

void run_tests() {
    // A simple test case
    assert(1 + 1 == 2);
    std::cout << "Simple test passed." << std::endl;
}

int main() {
    run_tests();
    std::cout << "All tests passed!" << std::endl;
    return 0;
}
