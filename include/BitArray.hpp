#pragma once

/* Copyright (C) 2021 Dhruv Chawla */
/* See LICENSE at project root for license details */

#ifndef BITARRAY_HPP
#define BITARRAY_HPP

#include <cstdint>
#include <stdexcept>
#include <type_traits>

#define INSTANTIATION_ERROR(T, msg)                                                                                    \
    if constexpr (always_true<T>) {                                                                                    \
        static_assert(always_false<T>,                                                                                 \
            "\n\t|-----------------------------------------------------------------|\n\t| " msg                        \
            "\n\t|-----------------------------------------------------------------|");                                \
    }

/**
 * @brief Class to represent an array of bits
 *
 * Accepts any type fulfilling the @c std::is_integral constraint with a power of 2 size (in bytes) as its underlying
 * block type.
 *
 * @tparam T The type to use as the underlying block type
 */
template <typename T>
class BitArray {
    using block_type = T;

    static_assert(std::is_integral_v<block_type>, "Need integral type");
    static_assert((sizeof(block_type) & (sizeof(block_type) - 1)) == 0, "Need power of 2 size block type");

    /**
     * @brief The number of bits stored in a block, equivalent to the size of the type in bits
     */
    static constexpr std::size_t bits_per_block = sizeof(block_type) * __CHAR_BIT__;

    /**
     * @brief The default number of bits to be allocated for the bit array
     */
    static constexpr std::size_t default_size = 16;

    template <typename... Ts>
    static constexpr bool always_false = false;
    template <typename... Ts>
    static constexpr bool always_true = true;

    /**
     * @brief The array storing the actual blocks of bits
     */
    block_type *blocks = nullptr;

    /**
     * @brief The number of bits stored in the array
     */
    long long bits = 0;

    /**
     * @brief Return the bit at a given index
     *
     * @param index The value of the index
     * @return true If the bit at the index is set
     * @return false If the bit at the index is not set
     */
    [[nodiscard]] bool bit_at(std::size_t index) const noexcept {
        return blocks[index / bits_per_block] & block_bitmask(index);
    }

    /**
     * @brief Return a bitmask representing the bit at a particular index
     *
     * While the index used in the calculation refers to the array as a whole, the returned bitmask only applies to the
     * block which contains the bit at the accessed index
     *
     * @param index The index
     * @return block_type The mask used to extract the required bit
     */
    static block_type block_bitmask(std::size_t index) noexcept {
        return (1 << (bits_per_block - (index & (bits_per_block - 1)) - 1));
    }

    template <typename IterType>
    class Iterator;
    template <typename IterType>
    class ReverseIterator;

  public:
    using value_type = bool;
    using iterator = Iterator<block_type>;
    using const_iterator = Iterator<const block_type>;
    using reverse_iterator = ReverseIterator<block_type>;
    using const_reverse_iterator = ReverseIterator<const block_type>;

    /**
     * @brief Construct a new Bit Array object
     *
     * Calls @code{.cpp} BitArray(long long num_bits) @endcode with the default number of bits
     */
    BitArray() : BitArray(default_size) {}

    /**
     * @brief Construct a new Bit Array object of a given size
     *
     * Note that exactly @c num_bits bits are not stored, the size is rounded up to the nearest multiple of the size of
     * the underlying block type
     *
     * All bits are initialized to the cleared state
     *
     * @param num_bits The number of bits to use
     */
    BitArray(long long num_bits)
        : blocks{new block_type[(num_bits / bits_per_block) + !!(num_bits % bits_per_block)]},
          /* Effectively a ceiling function */
          bits{num_bits} {
        clear_all();
    }

    /**
     * @brief Sets all bits
     */
    void set_all() {
        for (long long i = 0; i < bits; i++) {
            set(i);
        }
    }

    /**
     * @brief Clears all bits
     */
    void clear_all() {
        for (long long i = 0; i < bits; i++) {
            clear(i);
        }
    }

    /**
     * @brief Checks if a bit being accessed is within bounds
     *
     * @param bit The index of the bit
     * @return true If the bit is within bounds
     * @return false If the bit is out of bounds
     */
    [[nodiscard]] bool accessible(std::size_t bit) const noexcept { return bit < bits; }

    /**
     * @brief Sets a bit
     *
     * Applies the bitmask returned by @c block_bitmask to the required block
     *
     * @param index The bit to be set
     */
    void set(std::size_t index) { blocks[index / bits_per_block] |= block_bitmask(index); }

    /**
     * @brief Clears a bit
     *
     * Does the exact opposite of @c set , applies the bit negation of the block bitmask to the required block
     *
     * @param index The bit to be cleared
     */
    void clear(std::size_t index) { blocks[index / bits_per_block] &= ~block_bitmask(index); }

    /**
     * @brief Access a bit at an index
     *
     * @throws std::range_error When the accessed bit is out of bounds
     * @param index The bit to be accessed
     * @return true If the bit is set
     * @return false If the bit is cleared
     */
    bool at(std::size_t index) {
        if (accessible(index)) {
            return bit_at(index);
        } else {
            throw std::range_error{"index out of range"};
        }
    }

    /**
     * @brief Accesses a bit at an index (unchecked)
     *
     * This function is potentially dangerous, as it does not check if the index is out of bounds
     *
     * @param index The bit to be accessed
     * @return true If the bit is set
     * @return false If the bit is cleared
     */
    [[nodiscard]] bool operator[](std::size_t index) const noexcept { return bit_at(index); }

    /**
     * @brief Return the size of the array in terms of number of bits
     *
     * @return std::size_t The size of the array
     */
    [[nodiscard]] std::size_t size() const noexcept { return bits; }

    /**
     * @brief Non-const bi-directional forward iterator
     *
     * @return iterator The iterator pointing to the first bit
     */
    [[nodiscard]] iterator begin() const noexcept { return iterator{blocks, 0}; }

    /**
     * @brief Non-const bi-directional forward iterator
     *
     * @return iterator The iterator pointing to one past the last bit
     */
    [[nodiscard]] iterator end() const noexcept {
        return iterator{blocks + (bits / bits_per_block), (bits % bits_per_block)};
    }

    /**
     * @brief Const bi-directional forward iterator
     *
     * @return const_iterator The iterator pointing to the first bit
     */
    [[nodiscard]] const_iterator cbegin() const noexcept { return const_iterator{blocks, 0}; }

    /**
     * @brief Const bi-directional forward iterator
     *
     * @return const_iterator The iterator pointing one past the last bit
     */
    [[nodiscard]] const_iterator cend() const noexcept {
        return const_iterator{blocks + (bits / bits_per_block), (bits % bits_per_block)};
    }

    /**
     * @brief Non-const bi-directional reverse iterator
     *
     * @return reverse_iterator The iterator pointing to the last bit
     */
    [[nodiscard]] reverse_iterator rbegin() const noexcept {
        return reverse_iterator{
            blocks + (bits / bits_per_block), static_cast<long long>(bits % bits_per_block) - 1, bits};
    }

    /**
     * @brief Non-const bi-directional reverse iterator
     *
     * @return reverse_iterator The iterator pointing one before the first bit
     */
    [[nodiscard]] reverse_iterator rend() const noexcept { return reverse_iterator{blocks, -1, bits}; }

    /**
     * @brief Const bi-directional reverse iterator
     *
     * @return const_reverse_iterator The iterator pointing to the last bit
     */
    [[nodiscard]] const_reverse_iterator crbegin() const noexcept {
        return const_reverse_iterator{blocks + (bits / bits_per_block), (bits % bits_per_block) - 1, bits};
    }

    /**
     * @brief Const bi-directional reverse iterator
     *
     * @return const_reverse_iterator The iterator pointing one before the last bit
     */
    [[nodiscard]] const_reverse_iterator crend() const noexcept { return const_reverse_iterator{blocks, -1, bits}; }

    /**
     * @brief Destroy the Bit Array object
     */
    ~BitArray() {
        if (blocks) {
            delete[] blocks;
        }
    }
};

template <typename T>
template <typename IterType>
class BitArray<T>::Iterator {
    IterType *ptr{nullptr};
    long long current_idx{0};

  public:
    Iterator() { INSTANTIATION_ERROR(IterType, "BitArray::Iterator is not default-constructible"); }
    Iterator(IterType *ptr, long long idx) : ptr{ptr}, current_idx{idx} {}

    Iterator operator++() {
        if (current_idx + 1 < bits_per_block) {
            current_idx++;
        } else {
            current_idx = 0;
            ptr++;
        }
        return *this;
    }

    Iterator operator--() {
        if (current_idx == 0) {
            current_idx = bits_per_block - 1;
            ptr--;
        } else {
            current_idx--;
        }
        return *this;
    }

    Iterator operator++(int) {
        Iterator copy = *this;
        ++(*this);
        return copy;
    }

    Iterator operator--(int) {
        Iterator copy = *this;
        --(*this);
        return copy;
    }

    bool operator*() { return *ptr & block_bitmask(current_idx); }
    bool *operator->() {
        INSTANTIATION_ERROR(T, "operator-> is not available for BitArray::Iterator");
        return nullptr; // To appease the compiler}
    }

    bool operator==(Iterator other) const noexcept { return ptr == other.ptr && current_idx == other.current_idx; }
    bool operator!=(Iterator other) const noexcept { return ptr != other.ptr || current_idx != other.current_idx; }
};

template <typename T>
template <typename IterType>
class BitArray<T>::ReverseIterator {
    IterType *ptr{nullptr};
    long long current_idx{0};

  public:
    ReverseIterator() { INSTANTIATION_ERROR(IterType, "BitArray::ReverseIterator is not default-constructible"); }
    ReverseIterator(IterType *ptr, long long idx, long long nbits) : ptr{ptr}, current_idx{idx} {
        if (idx < 0) {
            current_idx = bits_per_block - 1;
            ptr--;
        }
    }

    ReverseIterator operator++() {
        if (current_idx == 0) {
            current_idx = bits_per_block - 1;
            ptr--;
        } else {
            current_idx--;
        }
        return *this;
    }

    ReverseIterator operator--() {
        if (current_idx + 1 < bits_per_block) {
            current_idx++;
        } else {
            current_idx = 0;
            ptr++;
        }
        return *this;
    }

    ReverseIterator operator++(int) {
        ReverseIterator copy = *this;
        ++(*this);
        return copy;
    }

    ReverseIterator operator--(int) {
        ReverseIterator copy = *this;
        --(*this);
        return copy;
    }

    bool operator*() { return *ptr & block_bitmask(current_idx); }
    bool *operator->() {
        INSTANTIATION_ERROR(T, "operator-> is not available for BitArray::ReverseIterator");
        return nullptr; // To appease the compiler}
    }

    bool operator==(ReverseIterator other) const noexcept {
        return ptr == other.ptr && current_idx == other.current_idx;
    }
    bool operator!=(ReverseIterator other) const noexcept {
        return ptr != other.ptr || current_idx != other.current_idx;
    }
};

#undef INSTANTIATION_ERROR

#endif
