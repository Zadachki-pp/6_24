#include <iostream>
#include <utility>


template <typename T>
class Vector {};


template <>
class Vector<bool> {
    u_char* _array;
    size_t _size;

    size_t _bits_count;


    void _create_array(size_t size) {
        _array = new u_char[size]();
    }

    u_char* _copy_array() {
        auto copy = new u_char[_size]();
        std::copy(_array, _array + _size, copy);
        return copy;
    }

    void _copy_to_array(u_char* array, size_t size) {
        std::copy(array, array + size, _array);
    }

    void _expand_array() {
        auto new_size = _size+1;

        auto temp_copy = _copy_array();
        _delete_array();
        _create_array(new_size);
        _copy_to_array(temp_copy, _size);
        delete[] temp_copy;

        _size = new_size;
    }

    void _delete_array() {
        delete[] _array;
    }

    static size_t _calculate_size(size_t bits_count) {
        /*  1-8   -> 1
         *  9-16  -> 2
         * 17-24  -> 3
         * */

        return (bits_count-1) / 8 + 1;
    }

    static std::pair<size_t, size_t> _calculate_position(size_t index) {
        /*  0-7   -> 0, ...
         *  8-15  -> 1, ...
         * 16-23  -> 2, ...
         * */

        return std::make_pair((index) / 8, index % 8);
    }

public:
    explicit Vector(size_t bits_count) { // NOLINT(cppcoreguidelines-pro-type-member-init)
        if (bits_count == 0) {
            std::cerr << "Invalid bits count" << std::endl;
            exit(1);
        }

        _bits_count = bits_count;
        _size = _calculate_size(bits_count);
        _create_array(_size);
    }

    ~Vector() {
        _delete_array();
    }

    Vector(const Vector& other) : Vector(other._bits_count) {  // NOLINT(cppcoreguidelines-pro-type-member-init)
        _copy_to_array(other._array, other._size);
    }

    Vector& operator=(const Vector& other) {
        if (this == &other) return *this;

        _size = other._size;

        _delete_array();
        _create_array(_size);
        _copy_to_array(other._array, other._size);
    }

    size_t size() const {
        return _bits_count;
    }

    bool get(size_t index) {
        auto position = _calculate_position(index);

        return (_array[position.first] >> (8-1-position.second)) & 1;
    }

    void set(size_t index, bool value) {
        auto position = _calculate_position(index);

        if (value)
            _array[position.first] = _array[position.first] | (1 << (8-1-position.second));
        else
            _array[position.first] = _array[position.first] & ~(1 << (8-1-position.second));
    }

    void push_back(bool value) {
        if (_bits_count % 8 == 0) {
            _expand_array();
        }
        set(_bits_count, value);

        _bits_count++;
    }

    void insert(size_t index, bool value) {
        if (_bits_count % 8 == 0) {
            _expand_array();
        }
        _bits_count++;

        for(auto last_index = _bits_count-1; last_index > index; last_index--) {
            set(last_index, get(last_index-1));
        }
        set(index, value);
    }

    void erase(size_t index) {
        _bits_count--;

        for(auto first_index = index; first_index < _bits_count; first_index++) {
            set(first_index, get(first_index+1));
        }
        set(_bits_count, false);
    }

    bool at(size_t index) {
        if (index > size()-1) {
            std::cerr << "Invalid index" << std::endl;
            exit(1);
        }
        return get(index);
    }

    bool operator[](size_t index) {
        return get(index);
    }

    void print() {
        for (size_t i = 0; i < size(); i++) {
            std::cout << (*this)[i];
        }
        std::cout << std::endl;
    }
};


int main() {
    std::cout << "Set && push_back" << std::endl;
    Vector<bool> vector(8);
    vector.set(0, true);
    vector.set(1, true);
    vector.set(2, false);
    vector.set(3, false);
    vector.set(4, false);
    vector.set(5, true);
    vector.set(6, false);
    vector.set(7, true);
    vector.push_back(true);
    vector.push_back(true);
    vector.push_back(false);
    vector.push_back(true);

    std::cout << std::endl;
    std::cout << "Get" << std::endl;
    std::cout << vector.size() << std::endl;
    vector.print();

    std::cout << std::endl;
    std::cout << "Insert" << std::endl;

    vector.insert(7, false);
    vector.insert(7, true);
    std::cout << vector.size() << std::endl;
    vector.print();

    std::cout << std::endl;
    std::cout << "Erase" << std::endl;
    vector.erase(7);
    vector.erase(7);
    std::cout << vector.size() << std::endl;
    vector.print();

    return 0;
}
