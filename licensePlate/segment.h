#pragma once
#include <iostream>
#include <core/core.hpp>
#include <highgui/highgui.hpp>
#include <imgproc/imgproc.hpp>

using namespace std;
using namespace cv;

//分割车牌字符
class segment
{
public:
	segment(void);
	~segment(void);

	//分割字符
	vector<Rect> SeperateWords(Mat);

private:
	//车牌预处理
	void imagePrepare(Mat);
	//获取车牌边界
	void getPlateBoundaryUpDown(int&,int&);
	void getPlateBoundaryLeftRight(int&,int&);
	//获取字符边界
	void getWordsBoundary(const ushort*,int, int,int);

	//车牌图片
	Mat plateInfo, platBinary;
	//车牌
	int platWidth, platHeight;
	//图像中的轮廓
	vector<vector<Point> > contourAll;
	vector<Vec4i> hierarchy;
	//文字分割
	vector<int> wordsBoundList;
};

