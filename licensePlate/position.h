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
	//横向跳变检测
	vector<int> HorizonJumpCheck(Mat,vector<Rect>);
	//矫正角度
	vector<Mat> TurnPosition(vector<Rect>);
	//图像变量
	Mat sourceFrame, shapeKernal;
	//图像中的轮廓
	vector<vector<Point> > contourAll;
	vector<Vec4i> hierarchy;
};

