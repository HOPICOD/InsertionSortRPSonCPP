#include <iostream>
#include <vector>
#include "menu.h"
#include "input.h"
#include "arrayInteracting.h"
#include "InsertionSort.h"

enum menu { SetArrayByUser = 1, SetArrayByFile, SortArray, ExitProgram };


int main()
{
    setlocale(LC_ALL, "Russian");
    bool repeat = true;
    int userChoice = 0;
    InsertionSort sorter;
    vector<double> array = {};
    showGreeting();
    do {
        showMenu();
        userChoice = inputInt("Введите", 0, 5);
        switch (userChoice) {
        case(SetArrayByUser): {
            array = setArrayByUserInput();
            printArray(array, array.size(), "Ваш массив: ");
            break;
        }
        case(SetArrayByFile): {
            loadDataFromFile(array);
            printArray(array, array.size(), "Ваш массив: ");
            break;
        }
        case(SortArray): {
            vector<double> arrayForInsertionSort = array;
            if (arrayForInsertionSort.empty()) { // Проверка на пустой массив
                cout << "Массив пуст!" << endl;
                break;
            }
            sorter.typeOfSortArray(arrayForInsertionSort);
            printArray(array, array.size(), "Ваш оригинальный массив: ");
            printArray(arrayForInsertionSort, arrayForInsertionSort.size(), "Ваш отсортированный массив: ");
            isNeedToSaveArrays(array, arrayForInsertionSort);
            break;
        }
        case(ExitProgram): {
            repeat = false;
        }
        }

    } while (repeat);

}

