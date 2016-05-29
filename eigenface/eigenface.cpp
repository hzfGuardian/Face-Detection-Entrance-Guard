#include "opencv2/opencv.hpp"
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include "eigenface.h"
using namespace std;
using namespace cv;
#define DEBUG 0
#define rows_of_avg 192
#define cols_of_avg 168
#define rows_of_tranning 192*168
#define cols_of_tranning 488
#define rows_of_pic 1
#define cols_of_pic

void readMat(Mat &src, fstream &input)
{

}
template<class T1,class T2>
void Mat2Line(Mat &src, Mat &dst)
{
	for (int x = 0; x < src.rows;x++)
	for (int y = 0; y < src.cols; y++)
		dst.at<T1>(0, x*src.cols + y) = src.at<T2>(x, y);
	return;
}
vector<Mat> origin;//store all the ordinates of the trained faces
Mat average,Max_evect;
void initEigenFace()
{
	string path, model;
	string pathNow;//read all the faces
	Mat temp;
	FileStorage fs;
	fs.open(model.c_str(), FileStorage::READ);
	fs["average_Matrix"] >> average;
	fs["Max_evect"] >> Max_evect;
	cout << temp;
	for (int i = 0; i < 486; i++){
		Mat temp;
		char direct[100];
		sprintf(direct, "name %02d", i);
		pathNow = path + direct;
		fs[direct] >> temp;
		//cout << temp;
		origin.push_back(temp);
	}

}
string EigenFace(Mat &d)
{
	//Mat d=imread(pic_name.c_str());
	cvtColor(d, d, CV_BGR2GRAY);
	equalizeHist(d, d);
	Mat data(1, d.cols*d.rows, CV_64FC1);
	Mat2Line<double, unsigned char>(d, data);
	data = data - average;
	Mat ord = data*Max_evect;
	//cout << ord;
	double Min = -1;
	int fa;
	for (int i = 0; i < origin.size(); i++)
	{
		double dist = 0;
		for (int j = 0; j < ord.cols; j++)
		{
			double a = origin[i].at<double>(0, j);
			double b = ord.at<double>(0, j);
			//cout << a << ' ' << b << endl;
			dist += (a - b)*(a - b);
		}
		dist = sqrt(dist);
		if (Min == -1){
			fa = i;
			Min = dist;
	}
		else{
			if (Min > dist)
			{
				Min = dist;
				fa = i;
			}
		}
	}

	cout << fa << endl;
	char result[100];
	sprintf(result,"name %02d.png", fa);
	string ans=result;
	return ans;
	// Mat res_pic = imread(result);
	// Mat vec_mult = Mat::zeros(res_pic.rows*res_pic.cols,1,CV_64FC1);//store the face combined by all the eigen vector
	// //linear combine a face using the eigen vectors
	// for (int i = 0; i < ord.cols; i++)
	// {
	// 	Mat tmp = ord.at<double>(0, i)*Max_evect.col(i);
	// 	vec_mult = vec_mult + tmp;
	// }
	// normalize(vec_mult, vec_mult, 1, 0, NORM_MINMAX);
	// vec_mult.convertTo(vec_mult, CV_8UC1, 255.0);
	// vec_mult = vec_mult.t();
	// Mat vec_result(res_pic.rows, res_pic.cols, CV_8UC1);
	// for (int x = 0; x < res_pic.rows; x++)
	// {
	// 	for (int y = 0; y < res_pic.cols; y++)
	// 	{
	// 		vec_result.at<unsigned char>(x, y) = vec_mult.at<unsigned char>(0,x*res_pic.cols + y);
	// 	}
	// }
	// imshow("combine", vec_result);
	// imshow("people", res_pic);
	// cvtColor(res_pic, res_pic, CV_BGR2GRAY);
	// Mat  final_res= 0.5*res_pic + 0.5*d;
	// imshow("result", final_res);
	// waitKey(0);
	// return 0;
}
