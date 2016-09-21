#include "segment.h"
#include <numeric>
#include <fstream>

segment::segment(void)
{
}


segment::~segment(void)
{
}

vector<Rect> segment::SeperateWords(Mat sourceFrame){
	//处理图像
	imagePrepare(sourceFrame);
	//计算最小边界
	int upLoc,downLoc,leftLoc,rightLoc, speratValue;
	getPlateBoundaryUpDown(upLoc, downLoc);
	getPlateBoundaryLeftRight(leftLoc,rightLoc);
	vector<Rect> rectList;
	for(int i=0;i< wordsBoundList.size();i+=2)
	{
		Rect rectItem(wordsBoundList[i],upLoc,wordsBoundList[i+1] - wordsBoundList[i], downLoc - upLoc);
		rectList.push_back(rectItem);
		rectangle(sourceFrame,rectItem, Scalar(255),2);
	}
	imshow("imag6",sourceFrame);	

	return rectList;
}

//车牌预处理
void segment::imagePrepare(Mat sourceFrame){
	//灰度化图像
	cv::cvtColor(sourceFrame, plateInfo, CV_RGB2GRAY);
	//高斯滤波
	cv::GaussianBlur(plateInfo,plateInfo, Size(3,3),0);
	//边缘检测，输出的为二值图
	cv::Canny(plateInfo,plateInfo,100,250);
	//找到轮廓
	cv::findContours(plateInfo,contourAll,hierarchy,CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	//填充车牌
	int platItemCount = contourAll.size();
	Rect wordArea;
	vector<Point> contoursAppr;

	for(int i=0;i<platItemCount;i++)  
    {  
		approxPolyDP(Mat(contourAll[i]),contoursAppr,5,true);
		wordArea = boundingRect(contoursAppr);
		if(wordArea.height <= 10) continue;		
//		cv::polylines(m4, contourAll[i], false,Scalar(255), 1);
//		rectangle(m4,Point(wordArea.x,wordArea.y), Point(wordArea.x + wordArea.width, wordArea.y + wordArea.height), Scalar(255));
		drawContours(plateInfo,contourAll,i,Scalar(255),CV_FILLED);  
    }
	
	//显示图像
	imshow("imag4",plateInfo);	
	
	platWidth = plateInfo.cols;
	platHeight = plateInfo.rows;
}

//寻找数组的边界值，返回值是偏移量
int getUpDownBoundIdx(const ushort *moveCount,const bool moveDown, const int len,const int checkValue)
{//找到连续N（5）个小于均值1/3认为到达边界
	int resultIdx = 0, flagCount = 0,boundCount=5;
	for(int i=1;i<len;i++)
	{
		//Down为Y轴正向
		if(moveDown) moveCount++;
		else moveCount--;
		//统计边界值
		if(*(moveCount) < checkValue)
			flagCount++;
		else
			flagCount=0;
		if(flagCount >= boundCount)
		{//边界
			resultIdx = i;
			break;
		}
	}
	return resultIdx - (flagCount - 1);
}

//寻找数组的边界值，返回值是偏移量
int getLeftRightBoundIdx(const ushort *moveCount,const bool moveRight, const int len,const int checkValue)
{//找到连续N（10）个值大于底线认为到达有效区域
	int resultIdx = 0, flagCount = 0,boundCount=10;
	for(int i=1;i<len;i++)
	{
		//Right为X轴正向
		if(moveRight) moveCount++;
		else moveCount--;
		//统计边界值
		if(*(moveCount) > checkValue)
			flagCount++;
		else
			flagCount=0;
		if(flagCount >= boundCount)
		{//边界
			resultIdx = i;
			break;
		}
	}
	return resultIdx - (flagCount - 1);
}

//获取车牌上下边界
void segment::getPlateBoundaryUpDown(int &upLoc, int &downLoc){
	uchar * pMatInfo;
	ushort *moveCount = new ushort[platHeight];
	for(int i=0;i<platHeight;i++) 
		moveCount[i] = 0;
	for (int i = 0; i < platHeight; i++)
	{//图像压缩到一维（Y轴方向）
		pMatInfo = plateInfo.ptr<uchar>(i);
		for(int j=0;j<platWidth;j++){
			moveCount[i] += ((int)pMatInfo[j] == 255?1:0);
		}
	}
	int idx = platHeight/2, midAreaSum = moveCount[idx], midAreaMean, meanCount = 5;
	for(int i= 1 ;i<= meanCount;i++)
	{//中线位置区域N（5*2+1）像素求均值
		midAreaSum += moveCount[idx+i] + moveCount[idx-i];
	}
	midAreaMean = midAreaSum / (2 * meanCount + 1);
	//原理：找到连续N（5）个小于均值1/3认为到达边界
	midAreaMean /= 3.0;
	upLoc = getUpDownBoundIdx(moveCount+idx-meanCount,false,idx-meanCount,midAreaMean);
	upLoc = idx-meanCount - upLoc;
	downLoc = getUpDownBoundIdx(moveCount+idx+meanCount,true,platHeight-idx-meanCount, midAreaMean);
	downLoc = idx+meanCount + downLoc;
	//释放空间
	delete(moveCount);
}

//获取车牌左右边界
void segment::getPlateBoundaryLeftRight(int &left, int &right){
	uchar * pMatInfo;
	ushort *moveCount = new ushort[platWidth];
	for(int i=0;i<platWidth;i++) 
		moveCount[i] = 0;
	for (int i = 0; i < platHeight; i++)
	{//图像压缩到一维（X轴方向）
		pMatInfo = plateInfo.ptr<uchar>(i);
		for(int j=0;j<platWidth;j++){
			moveCount[j] += ((int)pMatInfo[j] == 255?1:0);
		}
	}
	map<ushort,ushort> verticalCount;
	for( int i=0;i<platWidth;i++)
	{//统计结果再压缩到垂直方向（Y轴方向）
		if(verticalCount.find(moveCount[i]) == verticalCount.end())
			verticalCount.insert(pair<int,int>(moveCount[i],1));
		else
			verticalCount[moveCount[i]]++;
	}
	//找N(20)像素内的大于M（5）的最小值点（原理：分割字母的空格占用的纵向空间最多，其有效像素最少）
	//N：图片上N(20)个像素的高度作为分割点，M：除去空格外的相对多像素（铆钉）（由于空格通常会是最多像素点）
	//用最大值（会是空格）会导致部分位置（铆钉）影响，使得纵向无法通过空格（像素很少），而是需要通过某一个小值（铆钉）进行分割
	//其实也可以直接用一个经验值进行分割
	int mostPixel = platWidth, speratValue = 0;
	for(map<ushort,ushort>::iterator itemStep = verticalCount.begin();itemStep != verticalCount.end(); itemStep++)
	{//map默认按Key值升序排列
		if(itemStep->first >= 20) break;
		if(itemStep -> second < mostPixel && itemStep -> second > 5){
			mostPixel = itemStep->second;
			speratValue = itemStep->first;
		}
	}
	//求解左右最小值（这一步可以不做，因为求文字边界时通过去噪声可以过滤掉边界）
	left = getLeftRightBoundIdx(moveCount,true, platWidth/5.0, speratValue);
	right = getLeftRightBoundIdx(moveCount + platWidth,false, platWidth/5.0, speratValue);
	right = platWidth - right;
	//分解字符
	getWordsBoundary(moveCount,speratValue, left - 1, right + 1);
	delete(moveCount);
}


void segment::getWordsBoundary(const ushort * moveCount,int minValue, int start,int end)
{
	bool findStartFlag = false;//左边找起点，右边找终点
	const ushort * tmpCount = moveCount + start;
	for(int i=start;i<=end;i++,tmpCount++)
	{//找左右两端
		if(!findStartFlag && *tmpCount > minValue 
		 || findStartFlag && *tmpCount <= minValue)
		{
			wordsBoundList.push_back(i);
			findStartFlag = !findStartFlag;
		}
	}
	if(wordsBoundList.size() %2 > 0) //末尾加入
		wordsBoundList.push_back(end);

	int wordCount = wordsBoundList.size();
	for(int i=0;i<wordCount;i+=2)
	{//去掉噪声点（噪声：长度小于N[7]像素）
		if(wordsBoundList[i+1] - wordsBoundList[i] < 7)
		{
			vector<int>::iterator stepItem = wordsBoundList.begin()+i;
			wordsBoundList.erase(stepItem);
			stepItem = wordsBoundList.begin()+i;
			wordsBoundList.erase(stepItem);
			i-=2;
			wordCount-=2;
		}
	}
}