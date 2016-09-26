#pragma once

#include <iostream>
#include <core/core.hpp>
#include <highgui/highgui.hpp>
#include <imgproc/imgproc.hpp>

using namespace std;
using namespace cv;

//提取车牌
class position
{
public:
	position(void);
	~position(void);

	vector<Mat> getPlates(Mat);


private:
	//定位车牌
	vector<Rect> LocatePosition();
	//矫正角度
	vector<Mat> TurnPosition(vector<Rect>);

	Mat sourceFrame;
};

