#include "print_function.h"
#include <cstdlib>

// using
using namespace std;

void main_info(const string& section, const string& msg) {
    cout << section << " info: " << msg << endl;
}
void main_error(const string& section, const string& msg) {
    cerr << section << " Error: " << msg << endl;
    exit(1);
}
