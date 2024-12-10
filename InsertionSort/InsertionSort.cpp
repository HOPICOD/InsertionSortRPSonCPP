// InsertionSort.cpp : Определяет функции для статической библиотеки.
//

#include "pch.h"
#include "framework.h"
#include "InsertionSort.h"


void InsertionSort::sortIncrease(std::vector<double>& array) {
    for (size_t i = 1; i < array.size(); ++i) {
        for (size_t j = i; j > 0; --j) {
            if (array[j - 1] <= array[j]) break;
            std::swap(array[j - 1], array[j]);
        }
    }
}

void InsertionSort::sortDecrease(std::vector<double>& array) {
    for (size_t i = 1; i < array.size(); ++i) {
        for (size_t j = i; j > 0; --j) {
            if (array[j - 1] >= array[j]) break;
            std::swap(array[j - 1], array[j]);
        }
    }
}