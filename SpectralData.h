#pragma once
#include <iostream>		
#include <fstream>			//fstream
//#include <opencv2/imgcodecs.hpp> 
#include <opencv2\opencv.hpp>  //cv::Mat,Mat_,vec, imshow, etc.
#include "opencv2/highgui/highgui.hpp"  
#include "RefData.h"

class SpectralData : 
	public RefData
	{
	public:
		SpectralData();
		~SpectralData();

	public:
		int Pixel, Line, Spectra;
		
		//RefData Ref1;
		
	//protected:
		cv::Mat_<cv::Vec<ushort, 248>> DataCube;
		std::vector<cv::Mat> Pic;
		cv::Mat tmp;
		RefData Ref1;
		std::vector<RefData*> Refs;
		
		int setPosition(const unsigned int& pos);
		unsigned int getPosition();

	//protected:
		FILE* GnuplotPipe;
		int RefsPosition;

	public:
		void load(int num);
		static void onMouseStatic(int event, int x, int y, int flags, void* userdata);
		void onMouse(int event, int x, int y, int flags);
		static void onTrackbarStatic(int chanel, void* data);
		void onTrack(int chanel);
		void makePicture(int chanel);
		void updateRef();
		template<typename T>
		void ShowPlot(cv::Vec<T, 248> spec, FILE* gnuplotpipe);
		void reRead(FILE* gnuplotpipe, int x, int y);
				
	};
