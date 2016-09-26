#include "position.h"


position::position()
{
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


	return platePosition;
}

//矫正角度
vector<Mat> position::TurnPosition(vector<Rect>)
{
	vector<Mat> plates;

	return plates;
}