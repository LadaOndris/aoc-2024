
#ifndef AOC_2023_ORDEREDSET_H
#define AOC_2023_ORDEREDSET_H

#include <unordered_set>
#include <vector>
#include <stdexcept>

template <typename T, typename Hash = std::hash<T>, typename KeyEqual = std::equal_to<T>>
class OrderedSet {
private:
    std::unordered_set<T, Hash, KeyEqual> set;
    std::vector<T> order;

public:
    void insert(const T& value) {
        // Insert only if the value is not already in the set
        if (set.find(value) == set.end()) {
            set.insert(value);
            order.push_back(value);
        }
    }

    bool contains(const T& value) const {
        return set.find(value) != set.end();
    }

    void erase(const T& value) {
        if (set.find(value) != set.end()) {
            set.erase(value);
            order.erase(std::remove(order.begin(), order.end(), value), order.end());
        }
    }

    const T& operator[](std::size_t index) const {
        if (index >= order.size()) {
            throw std::out_of_range("Index out of range");
        }
        return order[index];
    }

    std::size_t size() const {
        return order.size();
    }

    typename std::vector<T>::const_iterator begin() const {
        return order.begin();
    }

    typename std::vector<T>::const_iterator end() const {
        return order.end();
    }
};

#endif
