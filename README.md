# BitArray

`BitArray` is a tiny header-only library for implementing a generic bit array that works with multiple integer types such as `char`, `(u)int8_t`, `(u)int16_t`, `(u)int32_t` etc.

Each element of the provided integer type (`type`) comprises a block in an array, containing `sizeof(type) * __CHAR_BIT__` bits within it.

```cpp
BitArray<std::uint8_t> arr{17};

for (int i = 0; i < arr.size(); i++) {
    arr.set(i);
    for (bool bit : arr) {
        std::cout << bit;
    }
    std::cout << '\n';
    arr.clear(i);
}
```

## Methods

### Bit modification

- `set` - Sets the bit at an (unchecked) index
- `clear` - Clears the bit at an (unchecked) index
- `set_all` - Sets all the bits in the array
- `clear_all` - Clears all the bits in an array

### Data accessing

- `accessible` - Checks if the provided index is within bounds
- `at` - Checked indexing, throws `std::range_error` on out of range
- `operator[]` - Unchecked indexing
- `size` - Returns the number of bits stored in the array

### Iterators

- `begin`/`end` - Non-const bi-directional forward iterator methods
- `cbegin`/`cend` - Const bi-directional forward iterator methods
- `rbegin`/`rend` - Non-const bi-directional reverse iterator methods
- `crbegin`/`crend` - Const bi-directional reverse iterator methods

`BitArray` does not support random access iterators. It is also not possible to
mutate the bit array through the iterators.

## Documentation

The documentation for this project is hosted at [readthedocs](https://bitarray.readthedocs.io/en/latest)

## License

This project uses the MIT license.