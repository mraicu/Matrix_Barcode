#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/objdetect.hpp>
#include <cmath>
#include <iostream>

using namespace std;
using namespace cv;

class Scanner {
public:
	int square_size = 0, I, J, mat_size, partition, arr_i = 1;
	Mat img = imread("img.jpg");
	//Mat img;
	Mat w_cam;
	bool M[500][500], bin_arr[500];

	void find_largest_square(const vector<vector<Point> >& squares, vector<Point>& biggest_square)
	{
		if (!squares.size())
		{
			// no squares detected
			return;
		}

		int max_width = 0;
		int max_height = 0;
		int max_square_idx = 0;
		const int n_points = 4;

		for (size_t i = 0; i < squares.size(); i++)
		{

			// Convert a set of 4 unordered Points into a meaningful cv::Rect structure.
			Rect rectangle = boundingRect(Mat(squares[i]));

			cout << "find_largest_square: #" << i << " rectangle x:" << rectangle.x << " y:" << rectangle.y << " " << rectangle.width << "x" << rectangle.height << endl;

			// Store the index position of the biggest square found
			if ((rectangle.width >= max_width) && (rectangle.height >= max_height) && (rectangle.width == rectangle.height))
			{
				max_width = rectangle.width;
				max_height = rectangle.height;
				max_square_idx = i;
			}
		}

		biggest_square = squares[max_square_idx];
	}

	static double angle(Point pt1, Point pt2, Point pt0)
	{
		double dx1 = pt1.x - pt0.x;
		double dy1 = pt1.y - pt0.y;
		double dx2 = pt2.x - pt0.x;
		double dy2 = pt2.y - pt0.y;
		return (dx1 * dx2 + dy1 * dy2) / sqrt((dx1 * dx1 + dy1 * dy1) * (dx2 * dx2 + dy2 * dy2) + 1e-10);
	}

	void find_square(Mat& image, vector<vector<Point> >& squares)
	{
		if (image.empty()) {
			return;
		}
		// blur will enhance edge detection
		Mat blurred(image);
		medianBlur(image, blurred, 9);

		Mat gray0(blurred.size(), CV_8U), gray;
		vector<vector<Point> > contours;

		// find squares in every color plane of the image
		for (int c = 0; c < 3; c++)
		{
			int ch[] = { c, 0 };
			mixChannels(&blurred, 1, &gray0, 1, ch, 1);

			// try several threshold levels
			const int threshold_level = 2;
			for (int l = 0; l < threshold_level; l++)
			{
				// Use Canny instead of zero threshold level!
				// Canny helps to catch squares with gradient shading
				if (l == 0)
				{
					Canny(gray0, gray, 10, 20, 3);

					// Dilate helps to remove potential holes between edge segments
					dilate(gray, gray, Mat(), Point(-1, -1));
				}
				else
				{
					gray = gray0 >= (l + 1) * 255 / threshold_level;
				}

				// Find contours and store them in a list
				findContours(gray, contours, RETR_LIST, CHAIN_APPROX_SIMPLE);

				// Test contours
				vector<Point> approx;
				for (size_t i = 0; i < contours.size(); i++)
				{
					// approximate contour with accuracy proportional
					// to the contour perimeter
					approxPolyDP(Mat(contours[i]), approx, arcLength(Mat(contours[i]), true) * 0.02, true);

					// Note: absolute value of an area is used because
					// area may be positive or negative - in accordance with the
					// contour orientation
					if (approx.size() == 4 &&
						fabs(contourArea(Mat(approx))) > 1000 &&
						isContourConvex(Mat(approx)))
					{
						double maxCosine = 0;

						for (int j = 2; j < 5; j++)
						{
							double cosine = fabs(angle(approx[j % 4], approx[j - 2], approx[j - 1]));
							maxCosine = MAX(maxCosine, cosine);
						}

						if (maxCosine < 0.3)
							squares.push_back(approx);
					}
				}
			}
		}
	}

	void img_capture() {
		bool a = false;
		vector<Point> largest_square;
		vector<vector<Point> > squares;

		// Creating VideoCapture object and opening webcam

		VideoCapture cap(0);
		//Checking if opened  
		if (!cap.isOpened())
		{
			cout << "Error opening Web cam" << endl;
			return;
		}

		bool stop_cap = true;
		while (stop_cap)
		{
			// Capture frame-by-frame
			cap >> w_cam;

			// If the frame is empty, break immediately
			if (w_cam.empty())
				break;

			// Display the frame
			imshow("VdoFrame", w_cam);

			// Detect all regions in the image that are similar to a rectangle
			find_square(w_cam, squares);


			// The largest of them
			if (squares.size()) {
				find_largest_square(squares, largest_square);
				if (largest_square.size()) {
					//draw rectangle
					Point p(largest_square[0].x, largest_square[0].y);
					Point op(largest_square[2].x, largest_square[2].y);
					rectangle(w_cam, p, op, Scalar(120, 190, 33), 1, LINE_4, 0);
					imshow("VdoFrame", w_cam);

					//using distance to find square
					int dist_p0_p1 = (int)sqrt((largest_square[1].x - largest_square[0].x) * (largest_square[1].x - largest_square[0].x) + (largest_square[1].y - largest_square[0].y) * (largest_square[1].y - largest_square[0].y));
					int dist_p1_p2 = (int)sqrt((largest_square[2].x - largest_square[1].x) * (largest_square[2].x - largest_square[1].x) + (largest_square[2].y - largest_square[1].y) * (largest_square[2].y - largest_square[1].y));

					if (dist_p0_p1 == dist_p1_p2) {
						a = true;
						stop_cap = false;
					}
				}
			}

			waitKey(25);

			if (a) {
				//crop img and save in img.jpg
				int h = (int)sqrt((largest_square[2].x - largest_square[1].x) * (largest_square[2].x - largest_square[1].x) + (largest_square[2].y - largest_square[1].y) * (largest_square[2].y - largest_square[1].y));
				int w = (int)sqrt((largest_square[1].x - largest_square[0].x) * (largest_square[1].x - largest_square[0].x) + (largest_square[1].y - largest_square[0].y) * (largest_square[1].y - largest_square[0].y));
				crop_img(w_cam, largest_square[0].x + 3, largest_square[0].y + 3, w - 3, h - 3);
			}

		}

		// Print the x,y coordinates of the square
		cout << "Point 0: " << largest_square[0] << endl;
		cout << "Point 1: " << largest_square[1] << endl;
		cout << "Point 2: " << largest_square[2] << endl;
		cout << "Point 3: " << largest_square[3] << endl;

		// When everything done, release the video capture object
		cap.release();

		// Closes all the frames
		destroyAllWindows();

	}

	void crop_img(Mat img, int left_corner_x, int left_corner_y, int width, int height) {
		if (width <= img.cols && height <=img.rows) {
			Mat cropped_image = img(Rect(left_corner_x, left_corner_y, width, height));
			cvtColor(cropped_image, cropped_image, COLOR_BGR2GRAY);
			imwrite("img.jpg", cropped_image);
			this->img = cropped_image;
		}
		else return;
	}

	void detect_i_j() {
		for (int i = 10; i < this->img.rows; i++) {
			for (int j = 10; j < this->img.cols; j++) {
				Vec3b bgr_pixel = this->img.at<Vec3b>(i, j);
				if (bgr_pixel[0] < 50 && bgr_pixel[1] < 50 && bgr_pixel[2] < 50) {
					this->I = i;
					this->J = j;
					return;
				}
			}
		}
	}

	void lil_square_size() {
		for (int i = 0; i < this->img.rows - 5; i++) {
			if (this->I + i < this->img.rows - 5) {
				Vec3b bgr_box = this->img.at<Vec3b>(I + i, J + i);
				if (bgr_box[0] > 100 && bgr_box[1] > 100 && bgr_box[2] > 100) {
					this->square_size = i;
					return;
				}
			}
		}
	}
	
	void matrix_size() {
		for (int i = this->I+this->square_size/2; i <= img.rows; i = i + this->square_size) {
			if (i+15+this->square_size / 2 < this->img.rows) {
				Vec3b bgr_matrix_size = this->img.at<Vec3b>(i, J);
				if (bgr_matrix_size[0] > 50 && bgr_matrix_size[1] > 50 && bgr_matrix_size[2] > 50) {
					this->mat_size = (i-this->I) / this->square_size;
				}
			}else this->mat_size = (i - this->I) / this->square_size+1;
		}
	}

	void create() {
		int  i_m = 1, j_m = 1;
		int last_poz_i = this->square_size * this->mat_size ;//198
		int last_poz_j = this->square_size * this->mat_size ;//198
		for (int i = this->I+10; i <= last_poz_i; i = i + this->square_size) {//I=10, J=13, square_size=22
			j_m = 1;
			for (int j = this->J; j <= last_poz_j; j = j + this->square_size) {
				Vec3b bgr_box_mat = this->img.at<Vec3b>(i, j);
				this->M[i_m][j_m] = (bgr_box_mat[0] < 50 && bgr_box_mat[1] < 50 && bgr_box_mat[2] < 50) ? true : false;
				j_m++;
			}
			i_m++;
		}
	}

	void show_mat() {
		for (int i = 1; i <= this->mat_size; i++) {
			for (int j = 1; j <= this->mat_size; j++) {
				cout << M[i][j] << " ";
			}
			cout << endl;
		}
	}

	void read_type() {
		this->partition = (this->M[3][2] == 1 && M[4][3] == 1) ? 8 : 4;
	}

	void data_to_arr() {
		bool ok = true;
		int i, j;
		int n = this->mat_size;
		for (i = 1; i <= n; i++) {
			for (j = 1; j <= this->mat_size; j++) {
				if (ok && j >= 4) {
					this->bin_arr[this->arr_i] = this->M[i][j];
				}
				if (!ok) {
					this->bin_arr[this->arr_i] = this->M[j][n];
				}
				if (j >= 4 && ok) this->arr_i++;
				if (!ok)++this->arr_i;
				if (ok && j == n) {
					j = i;
					ok = false;
				}
			}
			n--;
			ok = true;
		}
	}

	void show_arr() {
		for (int i = 1; i <= this->arr_i; i++) {
			cout << this->bin_arr[i] << " "<<endl;
		}
	}

	void read_arr() {
		int multiple = 0;
		int cpy_bin_i = this->arr_i;
		while (cpy_bin_i && multiple <= this->arr_i / this->partition) {
			bool ok = false;
			int p = this->partition - 1, ascii = 0;
			for (int i = multiple * this->partition + 1; i <= multiple * this->partition + this->partition; ++i) {
				if (this->bin_arr[i] == 1) ok = true;
				if (ok) {
					ascii = ascii + this->bin_arr[i] * pow(2, p);
				}
				cpy_bin_i--;
				p--;
			}
			cout << (char)ascii;
			multiple++;
		}


	}

};