#include <iostream>
#include <cstring>
#include <string>
#include "Generator.cpp"

using namespace std;
int main() {
	char date[100];
	int k, c_bin, len, nr_one;
	cout << "Introduceti datele: ";
	cin.get(date, 50);
	len = strlen(date);
	Generator code(date, len);
	code.to_binary(date);
	code.validation();
	code.create();
	code.show_qr(30);
}