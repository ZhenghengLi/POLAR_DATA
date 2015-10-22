#include <iostream>
#include <iomanip>
#include "../include/Constants.hpp"

using namespace std;

int main(int argc, char** argv) {
	int match[64];
	for (int i = 0; i < 64; i++) {
		match[TriggerIndex[i]] = i;
	}
	for (int i = 0; i < 64; i++) {
		cout << match[i] << ", ";
		if ((i + 1) % 8 == 0)
			cout << endl;
	}
	return 0;
}
