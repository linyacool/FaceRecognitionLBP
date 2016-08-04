
// lbpDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "lbp.h"
#include "lbpDlg.h"
#include "afxdialogex.h"

#include <fstream>
#include <afxpriv.h>
#include<opencv2/opencv.hpp> 
#include <stdio.h>
#include <io.h>
#include<iostream>
#include<cstring>
#include <string>
#include<windows.h>
using namespace std;
using namespace cv;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
//每个人的样本图片数量
const int SAMPLE_NUM = 3;

//是否需要检测图像中的人脸区域
bool NEED_DETECT=true;
//视频开关
bool Close_Capture;
//线程
struct ThreadInfo
{
	int num;
	//string s;
};

uchar table[256];
int hist[300][10][20][60];
int file_index,pic_index,area_index;
//int Chi[100][100];
double avghist[300][20][60];
//记录路径
CString picpath[300];
CString VideoPath;

//识别
double hist_r[16][256];


double weight[300][SAMPLE_NUM][20];
double avgWeight[300][20];

//分类器
String face_cascade_name = "haarcascade_frontalface_alt.xml";
String eyes_cascade_name = "haarcascade_eye_tree_eyeglasses.xml";
CascadeClassifier face_cascade;
CascadeClassifier eyes_cascade;

CWnd* CWnd_pic_A;
CWnd* CWnd_pic_B;
// ClbpDlg 对话框
/**************************函数*******************************/
void clearHist()
{
	file_index=-1;
	pic_index=0;
	area_index=0;
	for(int n=0;n<300;++n)
		for(int i=0;i<10;++i)
			for(int j=0;j<20;++j)
				for(int k=0;k<60;++k)
					hist[n][i][j][k]=0;

	for(int n=0;n<300;++n)
		for(int j=0;j<20;++j)
			for(int k=0;k<60;++k)
				avghist[n][j][k]=0;
}
//计算跳变次数
int getHopCount(int i)
{
	if(i>256 || i<0) return 0;
	int a[8] = { 0 };
	int cnt = 0;
	int k = 7;
	while (i)
	{
		a[k] = i & 1;
		i = i >> 1;
		--k;
	}
	for (k = 0; k < 7; k++)
	{
		if (a[k] != a[k + 1])
		{
			++cnt;
		}
	}
	if (a[0] != a[7])
	{
		++cnt;
	}
	return cnt;
}
//80*80区域编号
//1  2  3  4
//5  6  7  8
//9  10 11 12
//13 14 15 16
void getWeight(Mat img)
{
	int i;
	double ans;
	int x,y;
	double h[260];

	for(i=0;i<257;++i)
		h[i]=0;
	ans=0;
	for(x=0;x<=19;++x)
	{
		for(y=0;y<=19;++y)
		{
			++h[img.at<uchar>(x,y)];
		}
	}
	for(i=0;i<257;++i)
	{
		if(h[i]!=0)
		{
			ans += log10(h[i]/400.0) * h[i]/400.0;
		}
	}
	weight[file_index][pic_index][0]=ans*(-1.0);


	for(i=0;i<257;++i)
		h[i]=0;
	ans=0;
	for(x=20;x<=39;++x)
	{
		for(y=0;y<=19;++y)
		{
			++h[img.at<uchar>(x,y)];
		}
	}
	for(i=0;i<257;++i)
	{
		if(h[i]!=0)
		{
			ans += log10(h[i]/400.0) * h[i]/400.0;
		}
	}
	weight[file_index][pic_index][1]=ans*(-1.0);

	for(i=0;i<257;++i)
		h[i]=0;
	ans=0;
	for(x=40;x<=59;++x)
	{
		for(y=0;y<=19;++y)
		{
			++h[img.at<uchar>(x,y)];
		}
	}
	for(i=0;i<257;++i)
	{
		if(h[i]!=0)
		{
			ans += log10(h[i]/400.0) * h[i]/400.0;
		}
	}
	weight[file_index][pic_index][2]=ans*(-1.0);



	for(i=0;i<257;++i)
		h[i]=0;
	ans=0;
	for(x=60;x<=79;++x)
	{
		for(y=0;y<=19;++y)
		{
			++h[img.at<uchar>(x,y)];
		}
	}
	for(i=0;i<257;++i)
	{
		if(h[i]!=0)
		{
			ans += log10(h[i]/400.0) * h[i]/400.0;
		}
	}
	weight[file_index][pic_index][3]=ans*(-1.0);



	for(i=0;i<257;++i)
		h[i]=0;
	ans=0;
	for(x=0;x<=19;++x)
	{
		for(y=20;y<=39;++y)
		{
			++h[img.at<uchar>(x,y)];
		}
	}
	for(i=0;i<257;++i)
	{
		if(h[i]!=0)
		{
			ans += log10(h[i]/400.0) * h[i]/400.0;
		}
	}
	weight[file_index][pic_index][4]=ans*(-1.0);



	for(i=0;i<257;++i)
		h[i]=0;
	ans=0;
	for(x=20;x<=39;++x)
	{
		for(y=20;y<=39;++y)
		{
			++h[img.at<uchar>(x,y)];
		}
	}
	for(i=0;i<257;++i)
	{
		if(h[i]!=0)
		{
			ans += log10(h[i]/400.0) * h[i]/400.0;
		}
	}
	weight[file_index][pic_index][5]=ans*(-1.0);

	for(i=0;i<257;++i)
		h[i]=0;
	ans=0;
	for(x=40;x<=59;++x)
	{
		for(y=20;y<=39;++y)
		{
			++h[img.at<uchar>(x,y)];
		}
	}
	for(i=0;i<257;++i)
	{
		if(h[i]!=0)
		{
			ans += log10(h[i]/400.0) * h[i]/400.0;
		}
	}
	weight[file_index][pic_index][6]=ans*(-1.0);


	for(i=0;i<257;++i)
		h[i]=0;
	ans=0;
	for(x=60;x<=79;++x)
	{
		for(y=20;y<=39;++y)
		{
			++h[img.at<uchar>(x,y)];
		}
	}
	for(i=0;i<257;++i)
	{
		if(h[i]!=0)
		{
			ans += log10(h[i]/400.0) * h[i]/400.0;
		}
	}
	weight[file_index][pic_index][7]=ans*(-1.0);


	for(i=0;i<257;++i)
		h[i]=0;
	ans=0;
	for(x=0;x<=19;++x)
	{
		for(y=40;y<=59;++y)
		{
			++h[img.at<uchar>(x,y)];
		}
	}
	for(i=0;i<257;++i)
	{
		if(h[i]!=0)
		{
			ans += log10(h[i]/400.0) * h[i]/400.0;
		}
	}
	weight[file_index][pic_index][8]=ans*(-1.0);



	for(i=0;i<257;++i)
		h[i]=0;
	ans=0;
	for(x=20;x<=39;++x)
	{
		for(y=40;y<=59;++y)
		{
			++h[img.at<uchar>(x,y)];
		}
	}
	for(i=0;i<257;++i)
	{
		if(h[i]!=0)
		{
			ans += log10(h[i]/400.0) * h[i]/400.0;
		}
	}
	weight[file_index][pic_index][9]=ans*(-1.0);


	for(i=0;i<257;++i)
		h[i]=0;
	ans=0;
	for(x=40;x<=59;++x)
	{
		for(y=40;y<=59;++y)
		{
			++h[img.at<uchar>(x,y)];
		}
	}
	for(i=0;i<257;++i)
	{
		if(h[i]!=0)
		{
			ans += log10(h[i]/400.0) * h[i]/400.0;
		}
	}
	weight[file_index][pic_index][10]=ans*(-1.0);

	for(i=0;i<257;++i)
		h[i]=0;
	ans=0;
	for(x=60;x<=79;++x)
	{
		for(y=40;y<=59;++y)
		{
			++h[img.at<uchar>(x,y)];
		}
	}
	for(i=0;i<257;++i)
	{
		if(h[i]!=0)
		{
			ans += log10(h[i]/400.0) * h[i]/400.0;
		}
	}
	weight[file_index][pic_index][11]=ans*(-1.0);

	for(i=0;i<257;++i)
		h[i]=0;
	ans=0;
	for(x=0;x<=19;++x)
	{
		for(y=60;y<=79;++y)
		{
			++h[img.at<uchar>(x,y)];
		}
	}
	for(i=0;i<257;++i)
	{
		if(h[i]!=0)
		{
			ans += log10(h[i]/400.0) * h[i]/400.0;
		}
	}
	weight[file_index][pic_index][12]=ans*(-1.0);

	for(i=0;i<257;++i)
		h[i]=0;
	ans=0;
	for(x=20;x<=39;++x)
	{
		for(y=60;y<=79;++y)
		{
			++h[img.at<uchar>(x,y)];
		}
	}
	for(i=0;i<257;++i)
	{
		if(h[i]!=0)
		{
			ans += log10(h[i]/400.0) * h[i]/400.0;
		}
	}
	weight[file_index][pic_index][13]=ans*(-1.0);

	for(i=0;i<257;++i)
		h[i]=0;
	ans=0;
	for(x=40;x<=59;++x)
	{
		for(y=60;y<=79;++y)
		{
			++h[img.at<uchar>(x,y)];
		}
	}
	for(i=0;i<257;++i)
	{
		if(h[i]!=0)
		{
			ans += log10(h[i]/400.0) * h[i]/400.0;
		}
	}
	weight[file_index][pic_index][14]=ans*(-1.0);

	for(i=0;i<257;++i)
		h[i]=0;
	ans=0;
	for(x=60;x<=79;++x)
	{
		for(y=60;y<=79;++y)
		{
			++h[img.at<uchar>(x,y)];
		}
	}
	for(i=0;i<257;++i)
	{
		if(h[i]!=0)
		{
			ans += log10(h[i]/400.0) * h[i]/400.0;
		}
	}
	weight[file_index][pic_index][15]=ans*(-1.0);

	double sum=0;
	for(i=0;i<16;++i)
		sum+=weight[file_index][pic_index][i];
	for(i=0;i<16;++i)
	{
		weight[file_index][pic_index][i]=weight[file_index][pic_index][i]/sum;
	}
	//file<<endl;
	//file.close();
	/*
	double sum=0;
	for(i=0;i<16;++i)
		sum+=weight[pic_index][i];
	
	ofstream file;
	file.open("weight.txt");
	for(i=0;i<16;++i)
	{
		weight[pic_index][i]=weight[pic_index][i]/sum;
		file<<i<<" "<<weight[i]<<" ";
	}
	file.close();
	*/

}

void clearWeight()
{
	for(int k=0;k<300;++k)
	{
		for(int i=0;i<SAMPLE_NUM;++i)
		{
			for(int j=0;j<256;++j)
			{
				weight[k][i][j]=0;
			}
		}
	}
}
// 降维数组 由256->59  
void lbp59table(uchar *table)
{
	memset(table, 0, 256);
	uchar temp = 1;
	for (int i = 0; i < 256; i++)
	{
		if (getHopCount(i) <= 2)    // 跳变次数<=2 的为非0值  
		{
			table[i] = temp;
			temp++;
		}
	}
}
void uniformLBP(Mat &image, Mat &result, uchar *table)
{
	for (int y = 1; y < image.rows - 1; y++)
	{
		for (int x = 1; x < image.cols - 1; x++)
		{
			uchar neighbor[8] = { 0 };
			neighbor[0] = image.at<uchar>(y - 1, x - 1);
			neighbor[1] = image.at<uchar>(y - 1, x);
			neighbor[2] = image.at<uchar>(y - 1, x + 1);
			neighbor[3] = image.at<uchar>(y, x + 1);
			neighbor[4] = image.at<uchar>(y + 1, x + 1);
			neighbor[5] = image.at<uchar>(y + 1, x);
			neighbor[6] = image.at<uchar>(y + 1, x - 1);
			neighbor[7] = image.at<uchar>(y, x - 1);
			uchar center = image.at<uchar>(y, x);
			uchar temp = 0;
			for (int k = 0; k < 8; k++)
			{
				temp += (neighbor[k] >= center)* (1 << k);  // 计算LBP的值  
			}
			result.at<uchar>(y, x) = table[temp];   //  降为59维空间  
			//result.at<uchar>(y, x)=temp;
		}
	}
}
//各区域计算直方图
void calcHist(Mat image)
{
	int x,y;
	for(x=1;x<=19;++x)
	{
		for(y=1;y<=19;++y)
		{
			hist[file_index][pic_index][area_index][image.at<uchar>(x,y)]++;
		}
	}
	++area_index;
	for(x=20;x<=39;++x)
	{
		for(y=1;y<=19;++y)
		{
			hist[file_index][pic_index][area_index][image.at<uchar>(x,y)]++;
		}
	}
	++area_index;
	for(x=40;x<=59;++x)
	{
		for(y=1;y<=19;++y)
		{
			hist[file_index][pic_index][area_index][image.at<uchar>(x,y)]++;
		}
	}
	++area_index;
	for(x=60;x<=79;++x)
	{
		for(y=1;y<=19;++y)
		{
			hist[file_index][pic_index][area_index][image.at<uchar>(x,y)]++;
		}
	}
	++area_index;
	for(x=1;x<=19;++x)
	{
		for(y=20;y<=39;++y)
		{
			hist[file_index][pic_index][area_index][image.at<uchar>(x,y)]++;
		}
	}
	++area_index;
	for(x=20;x<=39;++x)
	{
		for(y=20;y<=39;++y)
		{
			hist[file_index][pic_index][area_index][image.at<uchar>(x,y)]++;
		}
	}
	++area_index;
	for(x=40;x<=59;++x)
	{
		for(y=20;y<=39;++y)
		{
			hist[file_index][pic_index][area_index][image.at<uchar>(x,y)]++;
		}
	}
	++area_index;
	for(x=60;x<=79;++x)
	{
		for(y=20;y<=39;++y)
		{
			hist[file_index][pic_index][area_index][image.at<uchar>(x,y)]++;
		}
	}
	++area_index;
	for(x=1;x<=19;++x)
	{
		for(y=40;y<=59;++y)
		{
			hist[file_index][pic_index][area_index][image.at<uchar>(x,y)]++;
		}
	}
	++area_index;
	for(x=20;x<=39;++x)
	{
		for(y=40;y<=59;++y)
		{
			hist[file_index][pic_index][area_index][image.at<uchar>(x,y)]++;
		}
	}
	++area_index;
	for(x=40;x<=59;++x)
	{
		for(y=40;y<=59;++y)
		{
			hist[file_index][pic_index][area_index][image.at<uchar>(x,y)]++;
		}
	}
	++area_index;
	for(x=60;x<=79;++x)
	{
		for(y=40;y<=59;++y)
		{
			hist[file_index][pic_index][area_index][image.at<uchar>(x,y)]++;
		}
	}
	++area_index;
	for(x=1;x<=19;++x)
	{
		for(y=60;y<=79;++y)
		{
			hist[file_index][pic_index][area_index][image.at<uchar>(x,y)]++;
		}
	}
	++area_index;
	for(x=20;x<=39;++x)
	{
		for(y=60;y<=79;++y)
		{
			hist[file_index][pic_index][area_index][image.at<uchar>(x,y)]++;
		}
	}
	++area_index;
	for(x=40;x<=59;++x)
	{
		for(y=60;y<=79;++y)
		{
			hist[file_index][pic_index][area_index][image.at<uchar>(x,y)]++;
		}
	}
	++area_index;
	for(x=60;x<=79;++x)
	{
		for(y=60;y<=79;++y)
		{
			hist[file_index][pic_index][area_index][image.at<uchar>(x,y)]++;
		}
	}
	area_index=0;
	++pic_index;
}
//计算识别直方图
void calcHist_recognition(Mat image)
{
	for(int i=0;i<16;++i)
		for(int j=0;j<256;++j)
			hist_r[i][j]=0;
	int x,y;
	for(x=1;x<=19;++x)
	{
		for(y=1;y<=19;++y)
		{
			hist_r[0][image.at<uchar>(x,y)]++;
		}
	}
	for(x=20;x<=39;++x)
	{
		for(y=1;y<=19;++y)
		{
			hist_r[1][image.at<uchar>(x,y)]++;
		}
	}
	for(x=40;x<=59;++x)
	{
		for(y=1;y<=19;++y)
		{
			hist_r[2][image.at<uchar>(x,y)]++;
		}
	}
	for(x=60;x<=79;++x)
	{
		for(y=1;y<=19;++y)
		{
			hist_r[3][image.at<uchar>(x,y)]++;
		}
	}
	for(x=1;x<=19;++x)
	{
		for(y=20;y<=39;++y)
		{
			hist_r[4][image.at<uchar>(x,y)]++;
		}
	}
	for(x=20;x<=39;++x)
	{
		for(y=20;y<=39;++y)
		{
			hist_r[5][image.at<uchar>(x,y)]++;
		}
	}
	for(x=40;x<=59;++x)
	{
		for(y=20;y<=39;++y)
		{
			hist_r[6][image.at<uchar>(x,y)]++;
		}
	}
	for(x=60;x<=79;++x)
	{
		for(y=20;y<=39;++y)
		{
			hist_r[7][image.at<uchar>(x,y)]++;
		}
	}
	for(x=1;x<=19;++x)
	{
		for(y=40;y<=59;++y)
		{
			hist_r[8][image.at<uchar>(x,y)]++;
		}
	}
	for(x=20;x<=39;++x)
	{
		for(y=40;y<=59;++y)
		{
			hist_r[9][image.at<uchar>(x,y)]++;
		}
	}
	for(x=40;x<=59;++x)
	{
		for(y=40;y<=59;++y)
		{
			hist_r[10][image.at<uchar>(x,y)]++;
		}
	}
	for(x=60;x<=79;++x)
	{
		for(y=40;y<=59;++y)
		{
			hist_r[11][image.at<uchar>(x,y)]++;
		}
	}
	for(x=1;x<=19;++x)
	{
		for(y=60;y<=79;++y)
		{
			hist_r[12][image.at<uchar>(x,y)]++;
		}
	}
	for(x=20;x<=39;++x)
	{
		for(y=60;y<=79;++y)
		{
			hist_r[13][image.at<uchar>(x,y)]++;
		}
	}
	for(x=40;x<=59;++x)
	{
		for(y=60;y<=79;++y)
		{
			hist_r[14][image.at<uchar>(x,y)]++;
		}
	}
	for(x=60;x<=79;++x)
	{
		for(y=60;y<=79;++y)
		{
			hist_r[15][image.at<uchar>(x,y)]++;
		}
	}
}
//计算每个人的均值向量
void calcAvg()
{
	double sum=0;
	for(int fp=0;fp<file_index;++fp)
	{
		for(int ap=0;ap<16;++ap)
		{
			for(int i=0;i<60;++i)
			{
				sum=0;
				for(int pp=0;pp<SAMPLE_NUM;++pp)
				{
					sum+=hist[fp][pp][ap][i];
				}
				sum=sum/SAMPLE_NUM;
				avghist[fp][ap][i]=sum;
			}

		}
	}
	ofstream file;
	file.open("weight.txt");
	for(int fp=0;fp<file_index;++fp)
	{
		for(int i=0;i<16;++i)
		{
			sum=0;
			for(int pp=0;pp<SAMPLE_NUM;++pp)
			{
				sum+=weight[fp][pp][i];
			}
			avgWeight[fp][i] = sum/SAMPLE_NUM;
			file<<avgWeight[fp][i]<<" ";
		}
		file<<endl;
	}
	file.close();
}
//改变图片格式，是否需要针对人脸数据库检测人脸区域在这里
//修改适应数据库
void readfile(CString tp)
{
	CFileFind fileFinder;
	CString filePath = tp + _T("//*.*");
	bool isFirstPic=true;
	BOOL bFinished = fileFinder.FindFile(filePath);
	//int filenum=0;
	while(bFinished)  //每次循环对应一个类别目录
	{
		bFinished = fileFinder.FindNextFile();
		if(fileFinder.IsDirectory() && !fileFinder.IsDots())  //若是目录则递归调用此方法
		{
			++file_index;	
			pic_index=0;
			isFirstPic=true;
			readfile(fileFinder.GetFilePath());	
		}
		else
		{
			if(file_index>299)
			{
				//AfxMessageBox(_T("文件数量过多"));
				return;
			}
			if(pic_index >= SAMPLE_NUM)
			{
				fileFinder.Close();
				return;
			}
				  
			//获取文件类型
			CString fileName = fileFinder.GetFileName();
			int dotPos=fileName.ReverseFind('.');
			CString fileExt=fileName.Right(fileName.GetLength()-dotPos);

			CString filename = fileFinder.GetFileName();
			//&& (filename.Find(_T("01"))!=-1 || filename.Find(_T("03"))!=-1 || filename.Find(_T("04"))!=-1 || filename.Find(_T("02"))!=-1)
			///////图片格式
			///////图片格式
			///////图片格式
			///////图片格式
			///////图片格式
			if(fileExt == _T(".bmp"))
			{
				string str=CStringA(fileFinder.GetFilePath());
				//记录一张图片路径
				if(isFirstPic)
				{
					picpath[file_index]=fileFinder.GetFilePath();
					isFirstPic=false;
				}
				Mat frame=imread(str);
				//resize(frame,frame,Size(80,80),0,0,CV_INTER_LINEAR);
				
/*****************检测人脸部分***********************/
				
				if(NEED_DETECT)
				{
					std::vector<Rect> faces;
					Mat frame_gray;
					cvtColor( frame, frame_gray, CV_BGR2GRAY );
					equalizeHist( frame_gray, frame_gray );
					//-- Detect faces
					face_cascade.detectMultiScale( frame_gray, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(30, 30) );
					//Mat test(frame,faces[0]);
					if(faces.size()==0)
					{
						//AfxMessageBox(_T("没有检测到人脸"));
						continue;
						//return;
					}
					double maxW=-1;
					size_t maxI=0;
					for( size_t i = 0; i < faces.size(); i++ )
					{
						if(faces[i].height>maxW)
						{
							maxW=faces[i].height;
							maxI=i;
						}
					}
					Mat rect_img=frame(faces[maxI]);

					Mat new_img;
					resize(rect_img,new_img,Size(80,80),0,0,CV_INTER_LINEAR);
					//ShowMatImgToWnd(CWnd_pic_B,test);

					//imwrite("img.jpg",new_img);
					//imshow("test",new_img);
					//waitKey(0);
					Mat result;
					result.create(Size(new_img.cols,new_img.rows),new_img.type());
					uniformLBP(new_img, result, table);
					getWeight(new_img);
					calcHist(result);
				}
				else
				{
					/***********************不需要检测人脸部分**********************/
					Mat result;
					result.create(Size(frame.cols,frame.rows),frame.type());
					uniformLBP(frame, result, table);
					getWeight(frame);
					calcHist(result);
				}
			}

		}
	}
	//++file_index;
	fileFinder.Close();
}
//计算卡方值，是否以熵值作为不同区域的加权系数设置
double calcChiavg(int _index)
{
	double area[16];
	for(int i=0;i<16;++i)
		area[i]=0;
	double ans=0;
	double x,y;
	for(int i=0;i<16;++i)
	{
		for(int j=0;j<60;++j)
		{
			if(hist_r[i][j] > 0.1 || avghist[_index][i][j] > 0.1)
			{
				x=hist_r[i][j];
				y=avghist[_index][i][j];
				area[i] += (x-y)*(x-y)/(x+y);
			}
		}
	}
	//如果需要加权，在这里改
	for(int i=0;i<16;++i)
	{
		//ans+=avgWeight[_index][i] * area[i]; // 加权
		ans+=area[i];     //不加权
	}
	return ans;
}
//各种识别数据存入文本文件中，文件在当前目录
void write_file()
{
	ofstream file;
	file.open("train.txt");
	for(int fi=0;fi<file_index;++fi)
	{
		file<<fi<<endl;
		for(int ai=0;ai<16;++ai)
		{
			for(int i=0;i<60;++i)
			{
				file<<avghist[fi][ai][i]<<" ";
			}
			file<<endl;
		}
	}
	file.close();
	file.open("path.txt");
	for(int i=0;i<file_index;++i)
	{
		string str=CStringA(picpath[i]);
		file<<str<<endl;
	}
	file.close();
}
//实现将任意图像固定到对话框的Picture Control控件上
void ShowMatImgToWnd(CWnd* pWnd, cv::Mat img)
{
	if(img.empty())  
		return;  

	CRect drect;       
	pWnd->GetClientRect(&drect);

	CClientDC dc(pWnd);
	HDC hDC =dc.GetSafeHdc();

	//内存中的图像数据拷贝到屏幕上
	BYTE *bitBuffer		   = NULL;
	BITMAPINFO *bitMapinfo = NULL;

	int ichannels =img.channels();
	if( ichannels == 1)
	{
		bitBuffer  = new BYTE[40+4*256]; 
	}
	else if( ichannels == 3)
	{
		bitBuffer  = new BYTE[sizeof(BITMAPINFO)]; 
	}
	else
	{
		return;
	}


	if(bitBuffer == NULL)
	{	
		return;
	}


	bitMapinfo = (BITMAPINFO *)bitBuffer;
	bitMapinfo->bmiHeader.biSize			= sizeof(BITMAPINFOHEADER);
	bitMapinfo->bmiHeader.biHeight		    = -img.rows;  //如果高度为正的，位图的起始位置在左下角。如果高度为负，起始位置在左上角。
	bitMapinfo->bmiHeader.biWidth		    = img.cols;
	bitMapinfo->bmiHeader.biPlanes			= 1;      // 目标设备的级别，必须为1	
	bitMapinfo->bmiHeader.biBitCount		= ichannels *8;     // 每个像素所需的位数，必须是1(双色), 4(16色)，8(256色)或24(真彩色)之一
	bitMapinfo->bmiHeader.biCompression	    = BI_RGB; //位图压缩类型，必须是 0(不压缩), 1(BI_RLE8压缩类型)或2(BI_RLE4压缩类型)之一
	bitMapinfo->bmiHeader.biSizeImage		= 0;      // 位图的大小，以字节为单位
	bitMapinfo->bmiHeader.biXPelsPerMeter	= 0;	  // 位图水平分辨率，每米像素数
	bitMapinfo->bmiHeader.biYPelsPerMeter	= 0;	  // 位图垂直分辨率，每米像素数
	bitMapinfo->bmiHeader.biClrUsed			= 0;	  // 位图实际使用的颜色表中的颜色数
	bitMapinfo->bmiHeader.biClrImportant	= 0;	  // 位图显示过程中重要的颜色数

	if(ichannels == 1)
	{
		for(int i=0; i<256; i++)
		{	//颜色的取值范围 (0-255)
			bitMapinfo->bmiColors[i].rgbBlue  =bitMapinfo->bmiColors[i].rgbGreen =bitMapinfo->bmiColors[i].rgbRed   =(BYTE) i;
		}

		bitMapinfo->bmiHeader.biClrUsed			= 256;	  // 位图实际使用的颜色表中的颜色数
	}
	SetStretchBltMode(hDC, COLORONCOLOR);


	StretchDIBits(hDC,
		0,
		0,
		drect.right,		//显示窗口宽度
		drect.bottom,		//显示窗口高度
		0,
		0,
		img.cols,		   //图像宽度
		img.rows,		   //图像高度
		img.data,			
		bitMapinfo,			
		DIB_RGB_COLORS, 
		SRCCOPY
		);

	delete []bitBuffer;

}
//视频检测和识别
void detectAndDisplay( Mat frame )
{
	std::vector<Rect> faces;
	Mat frame_gray;

	cvtColor( frame, frame_gray, CV_BGR2GRAY );
	equalizeHist( frame_gray, frame_gray );
	//-- Detect faces
	face_cascade.detectMultiScale( frame_gray, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(30, 30) );

	if(faces.size()==1)
	{
		//Mat test(frame,faces[0]);
		Mat rect_img=frame(faces[0]);

		Mat new_img;
		resize(rect_img,new_img,Size(80,80),0,0,CV_INTER_LINEAR);
		//ShowMatImgToWnd(CWnd_pic_B,test);

		//imwrite("img.jpg",new_img);
		//imshow("test",new_img);
		//waitKey(0);
		Mat result;
		result.create(Size(new_img.cols,new_img.rows),new_img.type());
		uniformLBP(new_img,result,table);
		calcHist_recognition(result);

		//getWeight(new_img);
		double temp=99999;
		int  ans=0;
		for(int fi=0;fi<file_index;++fi)
		{
			double ret=calcChiavg(fi);
			if(ret<temp)
			{
				temp=ret;
				ans=fi;
			}
		}
		string str=CStringA(picpath[ans]);
		Mat res=imread(str);
		if (!res.data)
		{
			AfxMessageBox(_T("路径不对"));
			return;
		}
		ShowMatImgToWnd(CWnd_pic_B,res);
	}

	for( size_t i = 0; i < faces.size(); i++ )
	{
		Point center( int(faces[i].x + faces[i].width*0.5), int(faces[i].y + faces[i].height*0.5) );
		Point t1(faces[i].x,faces[i].y);
		Point t2(faces[i].x+faces[i].width,faces[i].y+faces[i].height);
		ellipse( frame, center, Size( int(faces[i].width*0.5), int(faces[i].height*0.5)), 0, 0, 360, Scalar( 0, 255, 255 ), 2, 8, 0 );
		//rectangle(frame,t1,t2,Scalar(255,0,255));
		/*
		Mat faceROI = frame_gray( faces[i] );
		std::vector<Rect> eyes;

		//-- In each face, detect eyes
		eyes_cascade.detectMultiScale( faceROI, eyes, 1.1, 2, 0 |CV_HAAR_SCALE_IMAGE, Size(30, 30) );

		for( size_t j = 0; j < eyes.size(); j++ )
		{
		Point center( int(faces[i].x + eyes[j].x + eyes[j].width*0.5), int(faces[i].y + eyes[j].y + eyes[j].height*0.5) ); 
		int radius = cvRound( (eyes[j].width + eyes[i].height)*0.25 );
		circle( frame, center, radius, Scalar( 255, 0, 0 ), 3, 8, 0 );
		}
		*/
	} 
	//-- Show what you got
	//imshow( window_name, frame );
	ShowMatImgToWnd(CWnd_pic_A,frame);
}
//摄像头检测识别线程
UINT  Capture_Thread(LPVOID p)
{
	//int n=((ThreadInfo*)(p))->num;
	Mat frame;
	if( !face_cascade.load( face_cascade_name ) ){ AfxMessageBox(_T("Error loading\n")); return 0; };
	if( !eyes_cascade.load( eyes_cascade_name ) ){ AfxMessageBox(_T("Error loading\n")); return 0; };
	CvCapture* capture = cvCaptureFromCAM(0);
	if(capture)	// 摄像头读取文件开关
	{
		while( true )
		{
			frame = cvQueryFrame( capture );	// 摄像头读取文件开关
			//capture >> frame;
			//-- 3. Apply the classifier to the frame
			if( !frame.empty() )
			{ 
				detectAndDisplay( frame );
				//识别部分

			}
			else
			{
				AfxMessageBox(_T(" --(!) No captured frame -- Break!"));
				return 0;
			}

			if(Close_Capture)
			{
				cvReleaseCapture(&capture);
				frame.release();
				break;
			}
		}
	}
	return 0;
}
//视频播放检测识别线程
UINT  Video_Thread(LPVOID p)
{
	Mat frame;
	if( !face_cascade.load( face_cascade_name ) ){ AfxMessageBox(_T("Error loading\n")); return 0; };
	//if( !eyes_cascade.load( eyes_cascade_name ) ){ AfxMessageBox(_T("Error loading\n")); return 0; };
	//CvCapture* capture = cvCaptureFromCAM(0);
	//my_videopath.GetWindowTextW(path);
	string str=CStringA(VideoPath);
	VideoCapture capture(str);

	if(capture.isOpened())
	{
		while( true )
		{
			//frame = cvQueryFrame( capture );	// 摄像头读取文件开关
			capture >> frame;
			//-- 3. Apply the classifier to the frame
			if( !frame.empty() )
			{ 
				detectAndDisplay( frame );
				//识别部分
			}
			else
			{
				frame.release();
				AfxMessageBox(_T("播放结束"));
				return 0;
			}

			if(Close_Capture)
			{
				//cvReleaseCapture(&capture);
				frame.release();
				break;
			}
		}
		
	}
	return 0;
}
//统计识别正确率用
void recog(CString tp)
{
	CFileFind fileFinder;
	CString filePath = tp + _T("//*.*");
	BOOL bFinished = fileFinder.FindFile(filePath);
	while(bFinished)  //每次循环对应一个类别目录
	{
		bFinished = fileFinder.FindNextFile();
		if(fileFinder.IsDirectory() && !fileFinder.IsDots())  //若是目录则递归调用此方法
		{	
			recog(fileFinder.GetFilePath());	
		}
		else
		{
			//获取文件类型
			CString fileName = fileFinder.GetFileName();
			//fileFinder.GetFileTitle

			if(fileName.Find(_T("-004"))!=-1)
			{
				string str=CStringA(fileFinder.GetFilePath());
				Mat frame = imread(str);
				if (!frame.data)
				{
					AfxMessageBox(_T("读取文件失败"));
					return;
				}

				std::vector<Rect> faces;
				Mat frame_gray;
				cvtColor( frame, frame_gray, CV_BGR2GRAY );
				equalizeHist( frame_gray, frame_gray );
				//-- Detect faces
				face_cascade.detectMultiScale( frame_gray, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(30, 30) );
				//Mat test(frame,faces[0]);
				if(faces.size()==0)
				{
					//AfxMessageBox(_T("没有检测到人脸"));
					return;
				}
				Mat rect_img=frame(faces[0]);

				Mat new_img;
				resize(rect_img,new_img,Size(80,80),0,0,CV_INTER_LINEAR);

				Mat result;
				result.create(Size(new_img.cols, new_img.rows), new_img.type());
				uniformLBP(new_img, result, table);
				calcHist_recognition(result);
				//getWeight(new_img);
				getWeight(result);
				double temp=99999;
				int ans=-1;
				for(int fi=0;fi<file_index;++fi)
				{
					double ret=calcChiavg(fi);
					if(ret<temp)
					{
						temp=ret;
						ans=fi;
					}
				}
				CString Path=picpath[ans];
				if(Path != fileFinder.GetFilePath())
				{
					AfxMessageBox(Path+fileFinder.GetFilePath());
				}

			}
		}
	}
	fileFinder.Close();
}
//读识别文件，要使用需要修改
//修改适应视频库
void ReadVideoFile(CString tp)
{
	CFileFind fileFinder;
	CString filePath = tp + _T("//*.*");
	//bool isFirstPic=true;
	BOOL bFinished = fileFinder.FindFile(filePath);
	int filenum=-1;
	Mat frame;
	while(bFinished)  //每次循环对应一个类别目录
	{
		bFinished = fileFinder.FindNextFile();
		if(fileFinder.IsDots())  //若是目录则递归调用此方法
		{
			continue;
		}
		//获取文件类型
		CString fileName = fileFinder.GetFileName();
		int dotPos=fileName.ReverseFind('.');
		CString fileExt=fileName.Right(fileName.GetLength()-dotPos);
		CString filename = fileFinder.GetFileName();
			//&& (filename.Find(_T("01"))!=-1 || filename.Find(_T("03"))!=-1 || filename.Find(_T("04"))!=-1 || filename.Find(_T("02"))!=-1)
		if((fileExt == _T(".mp4") || fileExt == _T(".avi")))
		{
			++file_index;
			pic_index=0;
			string str=CStringA(fileFinder.GetFilePath());
			VideoCapture capture(str);
			int frame_num=0;
			//int sample=0;
			//isFirstPic=true;
			if(capture.isOpened())
			{
				while( true )
				{
					capture >> frame;
					++frame_num;
					if(frame.empty()) 
						break;
					if((frame_num==1 || frame_num%10==0) && !frame.empty())
					{
						if(pic_index >= SAMPLE_NUM) 
						{
							capture.release();
							break;
						}
						/*
						if(isFirstPic)
						{
							filenum++;
							isFirstPic=false;
							switch(filenum)
							{
							case 0:
								{
									imwrite("1.jpg",frame);
									picpath[filenum]=CString("1.jpg");
									break;
								}
							case 1:
								{
									imwrite("2.jpg",frame);
									picpath[filenum]=CString("2.jpg");
									break;
								}
							case 2:
								{
									imwrite("3.jpg",frame);
									picpath[filenum]=CString("3.jpg");
									break;
								}
							case 3:
								{
									imwrite("4.jpg",frame);
									picpath[filenum]=CString("4.jpg");
									break;
								}
							case 4:
								{
									imwrite("5.jpg",frame);
									picpath[filenum]=CString("5.jpg");
									break;
								}
							case 5:
								{
									imwrite("6.jpg",frame);
									picpath[filenum]=CString("6.jpg");
									break;
								}
							}
						}
						*/
						std::vector<Rect> faces;
						Mat frame_gray;
						cvtColor( frame, frame_gray, CV_BGR2GRAY );
						//equalizeHist( frame_gray, frame_gray );
						face_cascade.detectMultiScale( frame_gray, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(30, 30) );
						if(faces.size() != 1)
						{
							--frame_num;
							//--sample;
							//imshow("nanana",frame_gray);
							//waitKey(0);
							continue;
						}
						Mat rect_img=frame_gray(faces[0]);

						Mat new_img;
						resize(rect_img,new_img,Size(80,80),0,0,CV_INTER_LINEAR);
						//路径
						//路径
						//路径
						CString s1= CString("F:\\test\\");
						CString s2,s3;
						s2.Format(_T("%d"),file_index);
						s3.Format(_T("%d"),pic_index);
						s1=s1+s2;
						//最好先建立该目录，用于存储视频中的几帧图像
						if (!PathIsDirectory(s1))
						{
							::CreateDirectory(s1, NULL);
						}
						CString s4=CString("\\");
						CString s5=CString(".bmp");
						s1=s1+s4+s3+s5;
						string ss=CStringA(s1);
						imwrite(ss,new_img);
						pic_index++;
						//imshow("nanana",new_img);
						//waitKey(0);

						/*
						Mat result;
						result.create(Size(new_img.cols,new_img.rows),new_img.type());
						uniformLBP(new_img, result, table);
						area_index=0;
						calcHist(result);
						*/
					}

				}
			}
		}
	}
	++file_index;
	fileFinder.Close();
}
/*
void tiqutezheng()
{
	//int a=picmat[0][0].at<uchar>(1,1);
	Mat result;
	result.create(Size(80,80),picmat[0][0].type());
	for(int i=0;i<file_index;++i)
	{
		for(int j=0;j<pic_index;++j)
		{
			uniformLBP(picmat[file_index][pic_index], result, table);
			area_index=0;
			calcHist1(result,i,j);
			result.release();
			//--pic_index;
		}
	}
	
}
*/
/**************************我自己的函数*******************************/



ClbpDlg::ClbpDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(ClbpDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void ClbpDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_MFCEDITBROWSE1, my_filepath);
	DDX_Control(pDX, IDC_MFCEDITBROWSE2, my_picpath);
	DDX_Control(pDX, IDC_MFCEDITBROWSE3, my_videopath);
}

BEGIN_MESSAGE_MAP(ClbpDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_EXIT, &ClbpDlg::OnBnClickedExit)
	ON_BN_CLICKED(IDC_TRAIN, &ClbpDlg::OnBnClickedTrain)
	ON_BN_CLICKED(IDC_RECOGNITION, &ClbpDlg::OnBnClickedRecognition)
	ON_EN_CHANGE(IDC_MFCEDITBROWSE2, &ClbpDlg::OnEnChangeMfceditbrowse2)
	ON_BN_CLICKED(IDC_CAMERA, &ClbpDlg::OnBnClickedCamera)
	ON_BN_CLICKED(IDC_STOP, &ClbpDlg::OnBnClickedStop)
	//ON_BN_CLICKED(IDC_READ_RES, &ClbpDlg::OnBnClickedREAD_RES)
	ON_BN_CLICKED(IDC_VIDEO, &ClbpDlg::OnBnClickedVideo)
	ON_BN_CLICKED(IDC_READ_RES, &ClbpDlg::OnBnClickedReadRes)
	ON_EN_CHANGE(IDC_MFCEDITBROWSE1, &ClbpDlg::OnEnChangeMfceditbrowse1)
	ON_BN_CLICKED(IDC_BUTTON2, &ClbpDlg::OnBnClickedButton2)
END_MESSAGE_MAP()


// ClbpDlg 消息处理程序

BOOL ClbpDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	//ShowWindow(SW_MINIMIZE);
	ShowWindow(SW_SHOW);

	// TODO: 在此添加额外的初始化代码

	CWnd_pic_A = GetDlgItem(IDC_PIC_A);
	CWnd_pic_B = GetDlgItem(IDC_PIC_B);
	clearHist();
	lbp59table(table);
	Close_Capture=false;

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void ClbpDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR ClbpDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void ClbpDlg::OnBnClickedExit()
{
	// TODO: 在此添加控件通知处理程序代码
	exit(0);
}


void ClbpDlg::OnBnClickedTrain()
{
	// TODO: 在此添加控件通知处理程序代码
	if( !face_cascade.load( face_cascade_name ) ){ AfxMessageBox(_T("Error loading\n")); return; };
	file_index=-1;
	GetDlgItem(IDC_TRAIN)->EnableWindow(FALSE);
	CString Cstr;
	my_filepath.GetWindowTextW(Cstr);
	if(Cstr.GetLength()==0)
	{
		AfxMessageBox(_T("请选择目录"));
		GetDlgItem(IDC_TRAIN)->EnableWindow(TRUE);
		return;
	}
	readfile(Cstr);
	++file_index;
	calcAvg();
	write_file();
	AfxMessageBox(_T("训练完成！"));
	GetDlgItem(IDC_TRAIN)->EnableWindow(TRUE);

}

//修改使用
void ClbpDlg::OnBnClickedRecognition()
{
	// TODO: 在此添加控件通知处理程序代码
	//统计识别正确率
	/*
	kk=0;
	rright=0;
	CString Cstr;
	my_filepath.GetWindowTextW(Cstr);
	recog(Cstr);	
	Cstr.Format(_T("%d"),rright);
	AfxMessageBox(Cstr);
	*/

	/**********************单张识别部分*********************/
	CString Cstr;
	my_picpath.GetWindowTextW(Cstr);
	string str=CStringA(Cstr);
	Mat frame=imread(str);
	//ShowMatImgToWnd(CWnd_pic_A,image);
	if (!frame.data)
	{
		AfxMessageBox(_T("读取文件失败"));
		return;
	}

/*********************检测人脸部分*****************************/
	if(NEED_DETECT)
	{
		std::vector<Rect> faces;
		Mat frame_gray;
		cvtColor( frame, frame_gray, CV_BGR2GRAY );
		equalizeHist( frame_gray, frame_gray );
		//-- Detect faces
		face_cascade.detectMultiScale( frame_gray, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(30, 30) );
		if(faces.size()==0)
		{
			AfxMessageBox(_T("没有检测到人脸"));
			return;
		}
		for( size_t i = 0; i < faces.size(); i++ )
		{
			Point center( int(faces[i].x + faces[i].width*0.5), int(faces[i].y + faces[i].height*0.5) );
			Point t1(faces[i].x,faces[i].y);
			Point t2(faces[i].x+faces[i].width,faces[i].y+faces[i].height);
			ellipse( frame, center, Size( int(faces[i].width*0.5), int(faces[i].height*0.5)), 0, 0, 360, Scalar( 0, 255, 255 ), 2, 8, 0 );
			//rectangle(frame,t1,t2,Scalar(255,0,255));
		}
		ShowMatImgToWnd(CWnd_pic_A,frame);
		Mat rect_img=frame(faces[0]);
		Mat new_img;
		resize(rect_img,new_img,Size(80,80),0,0,CV_INTER_LINEAR);
		Mat result;
		result.create(Size(new_img.cols, new_img.rows), new_img.type());
		uniformLBP(new_img, result, table);
		calcHist_recognition(result);
	}
	else
	{
		ShowMatImgToWnd(CWnd_pic_A,frame);
		Mat result;
		result.create(Size(frame.cols, frame.rows), frame.type());
		uniformLBP(frame, result, table);
		calcHist_recognition(result);
	}
/********************************************************************/
	double temp=99999;
	int ans=-1;
	for(int fi=0;fi<file_index;++fi)
	{
		double ret=calcChiavg(fi);
		if(ret<temp)
		{
			temp=ret;
			ans=fi;
		}
	}
	str=CStringA(picpath[ans]);
	Mat res=imread(str);
	if (!res.data)
	{
		AfxMessageBox(_T("未找到对应图片"));
		return;
	}
	ShowMatImgToWnd(CWnd_pic_B,res);
}


void ClbpDlg::OnEnChangeMfceditbrowse2()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
	CString Cstr;
	my_picpath.GetWindowTextW(Cstr);
	string str=CStringA(Cstr);
	Mat image_A=imread(str);
	if(!image_A.empty())
	{
		ShowMatImgToWnd(CWnd_pic_A,image_A);
	}
}


void ClbpDlg::OnBnClickedCamera()
{
	// TODO: 在此添加控件通知处理程序代码
	GetDlgItem(IDC_CAMERA)->EnableWindow(FALSE);
	GetDlgItem(IDC_VIDEO)->EnableWindow(FALSE);
	ThreadInfo info;
	info.num=999;
	AfxBeginThread((AFX_THREADPROC)Capture_Thread, &info, THREAD_PRIORITY_NORMAL, 0, 0, NULL);
	Close_Capture=false;
}


void ClbpDlg::OnBnClickedStop()
{
	// TODO: 在此添加控件通知处理程序代码
	GetDlgItem(IDC_CAMERA)->EnableWindow(TRUE);
	GetDlgItem(IDC_VIDEO)->EnableWindow(TRUE);
	Close_Capture=true;
	Sleep(1000);
	//Mat im=imread("F:\\lena.png");
	//ShowMatImgToWnd(CWnd_pic_A,im);

}



void ClbpDlg::OnBnClickedVideo()
{
	// TODO: 在此添加控件通知处理程序代码
	GetDlgItem(IDC_VIDEO)->EnableWindow(FALSE);
	GetDlgItem(IDC_CAMERA)->EnableWindow(FALSE);
	ThreadInfo info;
	info.num=666;
	//if( !face_cascade.load( face_cascade_name ) ){ AfxMessageBox(_T("Error loading\n")); return; };
	//if( !eyes_cascade.load( eyes_cascade_name ) ){ AfxMessageBox(_T("Error loading\n")); return; };
	my_videopath.GetWindowTextW(VideoPath);
	//AfxMessageBox(path);
	AfxBeginThread((AFX_THREADPROC)Video_Thread,&info, THREAD_PRIORITY_NORMAL, 0, 0, NULL);
	Close_Capture=false;
	//string path=CStringA(VideoPath);
	//Mat image=imread(path);
	//detectAndDisplay( image );

}


void ClbpDlg::OnBnClickedReadRes()
{
	// TODO: 在此添加控件通知处理程序代码
	if( !face_cascade.load( face_cascade_name ) ){ AfxMessageBox(_T("Error loading\n")); return; };
	GetDlgItem(IDC_READ_RES)->EnableWindow(FALSE);
	ifstream file;
	file.open("train.txt");
	int fi;
	//int avghist[300][20][60];
	while(file>>fi)
	{
		for(int ai=0;ai<16;++ai)
		{
			for(int i=0;i<60;++i)
			{
				file>>avghist[fi][ai][i];
			}
		}
	}
	file_index=fi+1;
	file.close();
	file.open("path.txt");
	int index=0;
	string str;
	while(file>>str)
	{
		picpath[index++]=str.c_str();
	}
	AfxMessageBox(_T("读取完成"));
	GetDlgItem(IDC_READ_RES)->EnableWindow(TRUE);
}


void ClbpDlg::OnEnChangeMfceditbrowse1()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。

	// TODO:  在此添加控件通知处理程序代码
}

void ClbpDlg::OnBnClickedButton2()
{
	// TODO: 在此添加控件通知处理程序代码
	if( !face_cascade.load( face_cascade_name ) ){ AfxMessageBox(_T("Error loading\n")); return; };
	file_index=-1;
	GetDlgItem(IDC_BUTTON2)->EnableWindow(FALSE);
	CString Cstr;
	my_filepath.GetWindowTextW(Cstr);
	if(Cstr.GetLength()==0)
	{
		AfxMessageBox(_T("请选择目录"));
		GetDlgItem(IDC_TRAIN)->EnableWindow(TRUE);
		return;
	}
	ReadVideoFile(Cstr);
	CString path=CString("F:\\test");
	file_index=-1;
	pic_index=0;
	bool need_det=NEED_DETECT;
	NEED_DETECT=false;
	readfile(path);
	NEED_DETECT=need_det;
	++file_index;
	calcAvg();
	write_file();
	AfxMessageBox(_T("训练完成！"));
	GetDlgItem(IDC_BUTTON2)->EnableWindow(TRUE);
}
