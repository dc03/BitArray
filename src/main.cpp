#include "BitArray.hpp"

#include <iostream>

void print(BitArray<std::uint8_t> &arr) {
    for (auto it = arr.rbegin(); it != arr.rend(); it++) {
        std::cout << *it;
    }
}

int main() {
    BitArray<std::uint8_t> arr{24};

    arr.set_all();

    for (int i = 0; i < arr.size(); i++) {
        arr.clear(i);
        print(arr);
        std::cout << '\n';
        arr.set(i);
    }

    for (int i = arr.size() - 1; i >= 0; i--) {
        arr.clear(i);
        print(arr);
        std::cout << '\n';
        arr.set(i);
    }

    return 0;
}
