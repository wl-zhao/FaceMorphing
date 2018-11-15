#pragma once
#include <iostream>
#include <vector>
#include <opencv2/opencv.hpp>
using namespace std;
typedef std::string String;

//my own cv namespace
namespace mycv {
	typedef unsigned char uchar;
	typedef cv::Point2d Point2d;
	typedef cv::Size Size;
	typedef cv::Vec3b Vec3b;
	typedef cv::Rect Rect;
	class Mat
	{
	public:
		Mat() : rows(0), cols(0), _data(nullptr) { size = rows * cols; }
		Mat(Size s, uchar type);
		Mat(int row, int col, uchar type);
		Mat(const Mat& m);
		Mat(int row, int col, const std::vector<double>& v);
		Mat(int row, int col, uchar type, Point2d *v);

		Mat t();
		Mat& operator=(const Mat &m);
		Mat operator+(const Mat &m);
		Mat operator*(const Mat &m);
		Mat operator()(int x_min, int x_max, int y_min, int y_max);
		Mat inv();
		void swap_row(int i0, int i1);
		void shift_row();

		template<typename T>void fillWithValue(const T &value);
		~Mat() { if(_data) delete []_data; };
		template <typename T> const T& at(int i, int j) const;
		template <typename T> T& at(int i, int j);

		static Mat zeros(mycv::Size s, uchar type);
		static Mat zeros(int rows, int cols, uchar type);
		static Mat ones(int rows, int cols, uchar type);


		friend ostream & operator<< (ostream &out, const Mat &m);

		int rows;
		int cols;
		int type;
		int size;
		void *_data;
	private:
		static Mat eyes(int size, int type);
	};
    ostream & operator<< (ostream &out, const Mat &m);
	void hconcat(const Mat &m1, const Mat &m2, Mat &m);
	void vconcat(const Mat &m1, const Mat &m2, Mat &m);
};




