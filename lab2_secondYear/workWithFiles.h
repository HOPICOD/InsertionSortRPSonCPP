#pragma once
using namespace std;
#include <string>
#include <vector>

bool fileExists(string filename);

bool confirmOverwrite(string filename);

bool isFilepathValid(string filepath);

bool isFilenameValid(string filename);

string getValidFilepath();

string getOverwriteConfirmation(string& fullPath);

vector<double> loadArrayFromFile(string filename);

void saveArraysWithResultsToFile(vector<double>& array, vector<double>& arrayForInsertionSort);