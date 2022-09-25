#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>
#include <cstring>

using namespace std;
using namespace cv;

class Generator {
public:
	int k, len;
	bool m[100][100] = { {0},{0} }, bin_arr[500] = { 0 };
	char date[100];

	void read() {
		cout << "Dataset: ";
		cin.get();
		cin.get(date, 100);
		this->len = strlen(date);
	}
	
	void type() {
		if (isalpha(date[0])) {
			this->k = 8;
		}
		else this->k = 4;
		this->len = this->len * this->k;
	}

	//transforming to binary
	void to_binary() {
		int n = this->len;
		int b = this->k;
		int m = 1;
		for (int i = 0; i < strlen(this->date); ++i) {
			int copy_s = (int)this->date[i];
			int copy_k = this->k;
			b = copy_k * m;
			m++;
			while (copy_k) {
				this->bin_arr[b--] = (copy_s % 2) ? true : false;
				copy_s /= 2;
				n--;
				copy_k--;
			}
		}
	}

	//the number of one(for validation)
	int nr_one() {
		int nr_1 = 0;
		for (int i = 1; i <= this->len; i++) {
			if (this->bin_arr[i] == true) nr_1++;
		}
		return nr_1;
	}

	//minim size of matrix
	int size() {
		int i;
		for (i = 9; i <= 50; ++i) {
			if ((i * i) - 27 >= this->len) return i;
		}
	}

	//creating matrix
	void create() {
		int n, i, j;
		bool ok = true;
		this->m[1][2] = 1;
		this->m[1][3] = 1;
		if (this->k == 8) {
			this->m[3][2] = 1;
			this->m[4][3] = 1;
		}
		for (int i = 1; i <= this->size(); ++i) {
			this->m[i][1] = 1;
		}
		n = this->size();
		int bin_arr_size = 1;
		for (i = 1; i <= n && bin_arr_size <= this->max_for(); i++) {
			for (j = 1; j <= this->size(); j++) {
				if (this->bin_arr[bin_arr_size]) {
					if (ok && j >= 4) {
						this->m[i][j] = 1;
					}
					if (!ok) {
						this->m[j][n] = 1;
					}
				}
				if (j >= 4 && ok) bin_arr_size++;
				if (!ok) ++bin_arr_size;
				if (ok && j == n) {
					j = i;
					ok = false;
				}
			}
			n--;
			ok = true;
		}

	}

	int max_for() {
		return this->size() * (this->size() + 1);
	}

	void validation() {
		int n, s = 0, ind_v = 0, o = this->nr_one();
		bool v[8] = { 0 };
		n = (int)log2(o);
		if ((int)pow(2, n) == o) {
			v[n] = 1;
		}
		else {
			for (int i = n; i >= 0; --i)
			{
				s += (int)pow(2, i);
				v[i] = 1;
				if (s == o) break;
				if (s > o) {
					s = s - (int)pow(2, i);
					v[i] = 0;
				}
			}
		}

		for (int i = 3; i >= 2; --i) {
			for (int j = 6; j <= 9; ++j) {
				if (v[ind_v] == 1) {
					this->m[j][i] = 1;
				}
				ind_v++;
			}
		}
	}

	void show_arr() {
		for (int i = 1; i <= this->len; i++) {
			cout << this->bin_arr[i] << " ";
		}
	}

	void show_mat() {
		for (int i = 1; i <= this->size(); i++) {
			for (int j = 1; j <= this->size(); j++) {
				cout << m[i][j] << " ";
			}
			cout << endl;
		}
	}

	void show_validation() {
		for (int i = 6; i <= 9; ++i) {
			for (int j = 2; j <= 3; ++j) {
				cout << this->m[i][j] << " ";
			}
			cout << endl;
		}
	}

	//creating QR
	void show_qr(int l) {
		int n = this->size();
		int l_img = n * l + (2 * l);
		Mat img(l_img, l_img, CV_8UC3, Scalar(255, 255, 255));
		for (int i = 1; i <= n; i++) {
			for (int j = 1; j <= n; j++) {
				if (this->m[i][j]) {
					rectangle(img,
						Point(j * l, i * l),
						Point(j * l + l, i * l + l),
						Scalar(0, 0, 0),
						FILLED,
						LINE_8);
				}
			}
		}
		imshow("Img", img);
		waitKey(0);
	}
};