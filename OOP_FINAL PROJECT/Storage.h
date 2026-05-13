#ifndef STORAGE_H
#define STORAGE_H

#include "HospitalException.h"

template <typename T>
class Storage {
private:
    T data[100]; // Required by assignment constraints
    int count;

public:
    Storage() : count(0) {}

    void add(const T& item) {
        if (count >= 100) {
            throw InvalidInputException();
        }
        data[count++] = item;
    }

    // Returns a pointer to the item if found, nullptr otherwise
    T* findById(int id) {
        for (int i = 0; i < count; i++) {
            // Assumes T has a getId() method
            if (data[i].getId() == id) {
                return &data[i];
            }
        }
        return nullptr;
    }

    bool removeById(int id) {
        for (int i = 0; i < count; i++) {
            if (data[i].getId() == id) {
                // Shift elements left to fill the gap
                for (int j = i; j < count - 1; j++) {
                    data[j] = data[j + 1];
                }
                count--;
                return true;
            }
        }
        return false;
    }

    // Expose the raw array for FileHandler or looping
    T* getAll() {
        return data;
    }

    int size() const {
        return count;
    }
};

#endif
