#include <iostream>
#include <cstring>
#include "Generator.cpp"
#include "Scanner.cpp"

using namespace std;
using namespace cv;
int main() {
	int k, c_bin, len, nr_one, select;
	cout << "Select: " << endl << "1. Generator " << endl << "2. Scanner" << endl;
	cin >> select;
	if (select == 1) {
		Generator code;
		code.read();
		code.type();
		code.to_binary();
		code.create();
		code.validation();
		code.show_qr(30);
		
	}
	else if (select == 2) {
		Scanner sc;
		sc.img_capture();
		sc.detect_i_j();
		sc.lil_square_size();
		sc.matrix_size();
		sc.create();
		sc.show_mat();
		sc.read_type();
		sc.data_to_arr();
		sc.read_arr();
		
	}
	
}
//to do: input cod qr, functie de orientare