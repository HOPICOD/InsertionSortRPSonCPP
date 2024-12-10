#include <string>
#include <vector>
#include <sstream>
#include <iomanip>

using namespace std;

string arrayToString(const vector<double>& array) {
    stringstream ss;
   
    ss << fixed << setprecision(3);
    for (size_t i = 0; i < array.size(); ++i) {
        ss << array[i];
        if (i != array.size() - 1) {
            ss << ", ";
        }
    }
    return ss.str();
}