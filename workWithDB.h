#pragma once
#include <vector>
#include <pqxx/pqxx> 

using namespace std;

void connectAndSaveData(vector<double>& unsortedArray, vector<double>& sortedArray);

vector<pair<int, pair<vector<double>, vector<double>>>> getAllArraysFromDatabase(pqxx::connection& C);

vector<double> getArrayFromDatabase(pqxx::connection& C, int id);

bool isIdCorrect(pqxx::connection& C, int arrayId, vector<double>& unsortedArray, vector<double>& sortedArray);