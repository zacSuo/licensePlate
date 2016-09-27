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
	Mat grayFrame, edgeFrame;
	//灰度化图像
	cv::cvtColor(sourceFrame, grayFrame, CV_RGB2GRAY);
	//高斯滤波
	cv::GaussianBlur(grayFrame,grayFrame, Size(3,3),0);	
	//竖向边缘检测（X轴方向二阶微分），输出的为二值图
	cv::Sobel(grayFrame,edgeFrame,grayFrame.depth(),2,0);
	////二值化
	cv::threshold(edgeFrame,edgeFrame,80,255,CV_THRESH_BINARY);
	//形态学运算(2次膨胀，2次腐蚀)
	cv::dilate(edgeFrame, edgeFrame,shapeKernal);
	cv::dilate(edgeFrame, edgeFrame,shapeKernal);
	cv::erode(edgeFrame, edgeFrame,shapeKernal);
	cv::erode(edgeFrame, edgeFrame,shapeKernal);
	//通过车牌先验信息，初次找到潜在车牌
	cv::findContours(edgeFrame,contourAll,hierarchy,CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	Rect plateRect;
	vector<Rect> edgeRect;
	int platItemCount = contourAll.size();
	for(int i=0;i<platItemCount;i++)
    {
		plateRect = boundingRect(contourAll[i]);
		if(plateRect.area() < 50 || plateRect.width < 5 || plateRect.height < 5) 
			continue; //面积小于50，长或宽小于5像素认为不是矩形
		if(plateRect.width < plateRect.height || plateRect.width > plateRect.height * 4)
			continue; //国内车牌宽高多为440*140（比3.14），小为220*140（1.57），则宽度和高度的比值为 1 到 5 之间认为可能为车牌			
		edgeRect.push_back(plateRect);
		//drawContours(edgeFrame,contourAll,i,Scalar(255),CV_FILLED);  
    }
	//横向连通==========================距离小于10像素认为属于同一个矩形

	//颜色过滤 ============================蓝底白字，黄底黑字，黑底白字，白底黑字

	//横向跳变过滤
	vector<int> jumpRect = HorizonJumpCheck(grayFrame,edgeRect);
	for(int i=0;i<jumpRect.size();i++){
		platePosition.push_back(edgeRect[jumpRect[i]]);
		rectangle(sourceFrame,Point(platePosition[i].x,platePosition[i].y), Point(platePosition[i].x + platePosition[i].width, platePosition[i].y + platePosition[i].height), Scalar(0,0,255), 2);
	}

	imshow("ddd", sourceFrame);
	moveWindow("ddd",1000,0);

	return platePosition;
}


//横向跳变检测
vector<int> position::HorizonJumpCheck(Mat grayFrame,vector<Rect> edgeRect){
		//横向跳变过滤
	int platItemCount = edgeRect.size();
	Mat roiImg;
	uchar lastPixelValue, * tmpRow;
	vector<int> jumpRect, jumpRowCount;
	map<int,int> jumpCount;
	int itemCount, itemRowCount, roiWidth, roiHeight;
	bool thisBigFlag, changeFlag;
	for(int i=0;i<platItemCount;i++)
	{//统计所有矩形
		jumpRowCount.clear();
		jumpCount.clear ();
		roiImg = grayFrame(edgeRect[i]);
		roiWidth = roiImg.cols;
		roiHeight = roiImg.rows;
		for(int k=0;k<roiHeight;k++)
		{//每行统计
			itemCount = 0;
			itemRowCount = 0;
			tmpRow = roiImg.ptr<uchar>(i);
			lastPixelValue = tmpRow[0];
			for(int j=1;j<roiWidth;j++)
			{//统计当前变化
				if(abs(lastPixelValue - tmpRow[j])<5){ //像素变化小于5像素认为，没变状态
					changeFlag = false;
				}else{//大小变化
					changeFlag = true;
					itemRowCount ++;
				}
				lastPixelValue = tmpRow[j];
				if(changeFlag && itemRowCount > 3){//3个像素内的突变认为是噪声
					changeFlag = false;
					itemRowCount = 0;
					itemCount++;
				}
			}
			jumpRowCount.push_back(itemCount);
		}
		for(int j=0;j<jumpRowCount.size();j++)
		{//统计每行跳变次数
			if(jumpCount.find(jumpRowCount[j]) == jumpCount.end())
				jumpCount.insert(pair<int,int>(jumpRowCount[j],1));
			else jumpCount[jumpRowCount[j]]++;
		}
		itemRowCount = 0;
		for(map<int,int>::iterator itemStep = jumpCount.begin();itemStep != jumpCount.end(); itemStep++){
			//(,itemStep->second)
			if(itemStep->first > 12 && itemStep->first < 20) //车牌跳变合理次数【12 - 20】
				itemRowCount += itemStep->second;
		}
		if(itemRowCount >= roiHeight / 4.0) //跳变有效区域超过高度的1/4，认为是车牌
			jumpRect.push_back(i);
	}

	return jumpRect;	
}

//矫正角度
vector<Mat> position::TurnPosition(vector<Rect>)
{
	vector<Mat> plates;

	return plates;
}