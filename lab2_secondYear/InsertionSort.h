#pragma once
#include <iostream>
#include <vector>
#include "input.h"
//#include "workWithFiles.h"

using namespace std;
enum typeOfSortArrayMenu{ IncreaseSort = 1, DecreaseSort = 2};


class InsertionSort {

public:
	InsertionSort() {}

	// Функция выбора типа сортировки массива
	void typeOfSortArray(vector<double>& arrayForInsertionSort) {
		int userChoice = 0;
		userChoice = inputInt("Выберите порядок сортировки: \n[1]От меньшего к большему\n[2]От большего к меньшему", 0, 3);
		switch (userChoice) {
		case(IncreaseSort): {
			insertionSortIncrease(arrayForInsertionSort);
			break;
		}
		case(DecreaseSort): {
			insertionSortDecrease(arrayForInsertionSort);
			break;
		}
		default: {
			cout << "Неверный ввод!" << endl;
			break;
		}
		}
	}



private:

	// Функция сортировки вставками по возрастанию
	void insertionSortIncrease(vector<double>& arrayForInsertionSort) {
		for (int i = 1; i < arrayForInsertionSort.size(); i++) { // Проходим по всем элементам массива, начиная с 1-го
			for (int j = i; j > 0; j--) { // Вставляем текущий элемент на нужное место в отсортированной части массива
				if (arrayForInsertionSort[j-1] <= arrayForInsertionSort[j]) { // Если элемент на нужной позиции, выходим из цикла
					break; 
				}
				swap(arrayForInsertionSort[j-1], arrayForInsertionSort[j]); // Иначе меняем элементы массива местами
			}

		}
	}

	// Функция сортировки вставками по убыванию
	void insertionSortDecrease(vector<double>& arrayForInsertionSort) { 
		for (int i = 1; i < arrayForInsertionSort.size(); i++) { // Проходим по всем элементам массива, начиная с 1-го
			for (int j = i; j > 0; j--) { // Вставляем текущий элемент на нужное место в отсортированной части массива
				if (arrayForInsertionSort[j - 1] >= arrayForInsertionSort[j]) { // Если элемент на нужной позиции, выходим из цикла
					break;
				}
				swap(arrayForInsertionSort[j - 1], arrayForInsertionSort[j]); // Иначе меняем элементы массива местами
			}

		}
	}
};