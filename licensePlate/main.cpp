#include <iostream>

#include "segment.h"

using namespace std;

void main(){

	segment segment;
	Mat image=imread("13.bmp");
	vector<Rect> wordsRect= segment.SeperateWords(image);
	for(int i=0;i< wordsRect.size();i++)
	{
		cout << wordsRect[i].x << "		"<<wordsRect[i].y << "	"<<wordsRect[i].width<<"	"<<wordsRect[i].height<<endl;
	}
	waitKey();
}