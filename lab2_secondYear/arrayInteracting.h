#pragma once
#include <vector>
#include <iostream>
using namespace std;

vector<double> setArrayByUserInput();

void printArray(vector<double>& array, size_t size, string message);

void loadDataFromFile(vector<double>& array);

void isNeedToSaveArrays(vector<double>& array, vector<double>& arrayForInsertionSort);

void replaceNegativeZero(vector<double>& array);