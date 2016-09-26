#include "position.h"


position::position()
{
	shapeKernal = getStructuringElement(MORPH_RECT, Size(5, 5));
}


position::~position(void)
{
}


vector<Mat> position::getPlates(Mat videoFrame){	
	position::sourceFrame = videoFrame;

	vector<Rect> locPosition = LocatePosition();
	return TurnPosition(locPosition);
}

//定位车牌
vector<Rect> position::LocatePosition()
{
	vector<Rect> platePosition;
	 
	//灰度化图像
	cv::cvtColor(sourceFrame, grayFrame, CV_RGB2GRAY);
	//高斯滤波
	cv::GaussianBlur(grayFrame,grayFrame, Size(3,3),0);
	//竖向边缘检测（X轴方向二阶微分），输出的为二值图
	cv::Sobel(grayFrame,grayFrame,grayFrame.depth(),2,0);
	//二值化
	cv::threshold(grayFrame,grayFrame,80,255,CV_THRESH_BINARY);
	//形态学运算(2次膨胀，2次腐蚀)
	cv::dilate(grayFrame, grayFrame,shapeKernal);
	cv::dilate(grayFrame, grayFrame,shapeKernal);
	cv::erode(grayFrame, grayFrame,shapeKernal);
	cv::erode(grayFrame, grayFrame,shapeKernal);
	imshow("aaa", grayFrame);
	moveWindow("aaa",0,0);
	//找到轮廓
	cv::findContours(grayFrame,contourAll,hierarchy,CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	//填充车牌	
	Rect plateRect;
	int platItemCount = contourAll.size();
	for(int i=0;i<platItemCount;i++)
    {
		plateRect = boundingRect(contourAll[i]);
		if(plateRect.area() < 50 || plateRect.width < 5 || plateRect.height < 5) 
			continue; //面积小于50，长或宽小于5像素认为不是矩形
		if(plateRect.width < plateRect.height || plateRect.width > plateRect.height * 4)
			continue; //国内车牌宽高多为440*140（比3.14），小为220*140（1.57），则宽度和高度的比值为 1 到 5 之间认为可能为车牌	
		rectangle(grayFrame,Point(plateRect.x,plateRect.y), Point(plateRect.x + plateRect.width, plateRect.y + plateRect.height), Scalar(255));
		drawContours(grayFrame,contourAll,i,Scalar(255),CV_FILLED);  
    }
	//横向连通==========================距离小于10像素认为属于同一个矩形

	imshow("ccc", grayFrame);
	moveWindow("ccc",1000,0);

	return platePosition;
}

//矫正角度
vector<Mat> position::TurnPosition(vector<Rect>)
{
	vector<Mat> plates;

	return plates;
}