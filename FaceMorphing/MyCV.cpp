#include "MyCV.h"
#include <cassert>

using namespace std;
using namespace mycv;
#ifndef DEBUG
Mat::Mat(Size s, uchar type) : Mat(s.height, s.width, type)
{
	size = rows * cols;
	type = type;
}

mycv::Mat::Mat(int row, int col, uchar type) : rows(row), cols(col), type(type)
{
	size = rows * cols;
	switch (type)
	{
	default:
		_data = (void*)new double[rows * cols];
		break;
	}
}

mycv::Mat::Mat(const Mat &m)
{
	rows = m.rows;
	cols = m.cols;
	type = m.type;
	size = m.size;
	switch (m.type)
	{
	default:
		_data = (void*) new double[size];
		memcpy(_data, m._data, sizeof(double) * size);
		break;
	}
}

mycv::Mat::Mat(int row, int col, const std::vector<double>& v) : Mat(row, col, CV_64F)
{
	for (int k = 0; k < v.size(); k++)
	{
		((double*)_data)[k] = v[k];
	}
}

mycv::Mat::Mat(int row, int col, uchar type, Point2d *v) : Mat(row, col, CV_64F)
{
	assert(col == 2);
	for (int i = 0; i < row; i++)
	{
		this->at<double>(i, 0) = (v + i)->x;
		this->at<double>(i, 1) = (v + i)->y;
	}
}

Mat mycv::Mat::t()
{
	Mat T(cols, rows, type);
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			T.at<double>(j, i) = this->at<double>(i, j);
		}
	}
	return T;
}

Mat& mycv::Mat::operator= (const Mat &m)
{
	if (this != &m)
	{
		if (_data)
			delete []_data;
		rows = m.rows;
		cols = m.cols;
		type = m.type;
		size = m.size;
		switch (m.type)
		{
		default:
			_data = new double[size];
			memcpy(_data, m._data, sizeof(double) * size);
			break;
		}
	}
	return *this;
}

Mat mycv::Mat::operator+(const Mat & m)
{
	assert(rows == m.rows && cols == m.cols);
	Mat s(m);
	for (int i = 0; i < rows; i++)
	{
		for (int j = 0; j < cols; j++)
		{
			s.at<double>(i, j) = s.at<double>(i, j) + m.at<double>(i, j);
		}
	}
	return s;
}

Mat mycv::Mat::operator*(const Mat & m)
{
	assert(cols == m.rows);
	Mat p(rows, m.cols, type);
	double sum;
	for (int i = 0; i < p.rows; i++)
	{
		for (int j = 0; j < p.cols; j++)
		{
			sum = 0;
			for (int k = 0; k < cols; k++)
			{
				sum += this->at<double>(i, k) * m.at<double>(k, j);
			}
			p.at<double>(i, j) = sum;
		}
	}
	return p;
}

Mat mycv::Mat::operator()(int x_min, int x_max, int y_min, int y_max)
{
	assert(x_min >= 0 && x_max < cols && x_min <= x_max);
	assert(y_min >= 0 && y_max < rows && y_min <= y_max);
	Mat m(y_max - y_min + 1, x_min - x_max + 1, CV_64F);
	for (int i = 0; i < m.rows; i++)
	{
		for (int j = 0; j < m.cols; j++)
		{
			m.at<double>(i, j) = this->at<double>(i + y_min, j + x_min);
		}
	}
	return Mat();
}

Mat mycv::Mat::inv()
{
	assert(rows == cols);
	auto A = *this;
	auto E = eyes(rows, CV_64F);
	int i, j, k;
	for (j = 0; j < cols; j++)
	{
		if (fabs(A.at<double>(0, j)) < 1e-12)
		{ 
			for (i = j + 1; i < rows; i++)
			{
				if (fabs(A.at<double>(i, j)) >1e-12)
				{
					//swap the row the first number of which is non-zero to the first row
					A.swap_row(i, 0);
					E.swap_row(i, 0);
					break;
				}
			}
			if (i == rows)
				return Mat::zeros(rows, rows, CV_64F);
		}


		for (i = 0; i < rows; i++)
		{
			if (i == 0)
			{
				for (k = 0; k < cols; k++)
				{
					E.at<double>(i, k) = E.at<double>(i, k) / A.at<double>(i, j);
				}
				for (k = cols - 1; k >= j; k--)
				{
					A.at<double>(i, k) /= A.at<double>(i, j);
				}
			}
			else
			{
				for (k = cols - 1; k >= 0; k--)
				{
					E.at<double>(i, k) = E.at<double>(i, k) - A.at<double>(i, j) / A.at<double>(0, j) * E.at<double>(0, k);
				}
				for (k = cols - 1; k >= j; k--)
				{
					A.at<double>(i, k) = A.at<double>(i, k) - A.at<double>(i, j) / A.at<double>(0, j) * A.at<double>(0, k);
				}
			}
		}
		A.swap_row(0, (j + 1) % rows);
		E.swap_row(0, (j + 1) % rows);
	}
	E.shift_row();
	return E;
}

void mycv::Mat::swap_row(int i0, int i1)
{
	if (i0 == i1)
		return;
	double *tmp = new double[cols];
	memcpy(tmp, ((double*)_data) + i0 * cols, sizeof(double) * cols);
	memcpy(((double*)_data) + i0 * cols, ((double*)_data) + i1 * cols, sizeof(double) * cols);
	memcpy(((double*)_data) + i1 * cols, tmp, sizeof(double) * cols);
	delete []tmp;
}

void mycv::Mat::shift_row()
{
	double *tmp = new double[cols];
	memcpy(tmp, ((double*)_data), sizeof(double) * cols);
	memcpy(((double*)_data), ((double*)_data) + cols, sizeof(double) * cols * (rows - 1));
	memcpy(((double*)_data) + cols * (rows - 1), tmp, sizeof(double) * cols);
	delete[]tmp;
}

template<typename T>
const T& Mat::at(int i, int j) const
{
	return ((T*)_data)[i * cols + j];
}

template<typename T>
T& Mat::at(int i, int j)
{
	return ((T*)_data)[i * cols + j];
}

Mat mycv::Mat::zeros(mycv::Size s, uchar type)
{
	Mat m(s, type);
	switch (type)
	{
	default:
		m.fillWithValue(double(0));
		break;
	}
	return m;
}

Mat mycv::Mat::zeros(int rows, int cols, uchar type)
{
	return Mat::zeros(Size(cols, rows), type);
}

Mat mycv::Mat::ones(int rows, int cols, uchar type)
{
	Mat m(rows, cols, type);
	switch (type)
	{
	default:
		m.fillWithValue(double(1));
		break;
	}
	return m;
}

Mat mycv::Mat::eyes(int size, int type)
{
	Mat m(size, size, type);
	m.fillWithValue(double(0));
	for (int i = 0; i < m.cols; i++)
	{
		m.at<double>(i, i) = 1;
	}
	return m;
}



template<typename T>
void Mat::fillWithValue(const T &value)
{
	for (int i = 0; i < size; i++)
	{
		((T*)_data)[i] = value;
	}
}

ostream & mycv::operator<<(ostream & out, const mycv::Mat & m)
{
	for (int i = 0; i < m.rows; i++)
	{
		if (i == 0)
			out << "[";
		else
			out << " ";
		for (int j = 0; j < m.cols; j++)
		{
			out << m.at<double>(i, j);
			if (j < m.cols - 1)
				out << ", ";
			else if (i < m.rows - 1)
				out << ";";
		}
		if (i < m.rows - 1)
			out << endl;
		else
			out << "]" << endl;
	}
	return out;
}

#endif

void mycv::hconcat(const Mat & m1, const Mat & m2, Mat & m)
{
	assert(m1.rows == m2.rows);
	m = Mat(m1.rows, m1.cols + m2.cols, CV_64F);
	for (int i = 0; i < m.rows; i++)
	{
		for (int j = 0; j < m.cols; j++)
		{
			if (j < m1.cols)
			{
				m.at<double>(i, j) = m1.at<double>(i, j);
			}
			else
			{
				m.at<double>(i, j) = m2.at<double>(i, j - m1.cols);
			}
		}
	}
}

void mycv::vconcat(const Mat & m1, const Mat & m2, Mat & m)
{
	assert(m1.cols == m2.cols);
	m = Mat(m1.rows + m2.rows, m1.cols, CV_64F);
	for (int i = 0; i < m.rows; i++)
	{
		for (int j = 0; j < m.cols; j++)
		{
			if (i < m1.rows)
			{
				m.at<double>(i, j) = m1.at<double>(i, j);
			}
			else
			{
				m.at<double>(i, j) = m2.at<double>(i - m1.rows, j);
			}
		}
	}
}


