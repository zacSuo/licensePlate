#include <iostream>

#include "segment.h"
#include "position.h"

using namespace std;

void main(){
	//定位车牌
	position position;
	Mat sourceFrame=imread("car.jpg");
	position.getPlates(sourceFrame);

	////分割字符
	//segment segment;
	//Mat image=imread("13.bmp");
	//vector<Rect> wordsRect= segment.SeperateWords(image);
	//for(int i=0;i< wordsRect.size();i++)
	//{
	//	cout << wordsRect[i].x << "		"<<wordsRect[i].y << "	"<<wordsRect[i].width<<"	"<<wordsRect[i].height<<endl;
	//}
	waitKey();
}