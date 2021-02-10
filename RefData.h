#pragma once
#include <opencv2\opencv.hpp>  //vec
#include <iostream>	
#include <string>   //std::string <<
#include <fstream>  //ifstrem ofstream
#include <math.h>

class RefData
{
public:
	RefData();
	RefData(std::string name);
	
	const float Resolution = (1700-930)/ 248;
	const int SygMade = 1;
	const int NormalizedSyg = 2;
	const int DerivativeMade = 3;

	int Flag = 0;
	std::string Name;
	cv::Point P1, P2;
	std::vector<cv::Vec<ushort, 248>> Ref;
	std::vector<cv::Vec<float, 248>> NormRef;
	cv::Vec<float, 248> Sygnature;
	cv::Vec<float, 248> Normalized;
	
	const std::string getName();
	void setName(const std::string& name);

	void addRef(const cv::Vec<ushort,248>& spectrum);
	void callSyg();
	void deriv();
	void deriv(cv::Vec<ushort, 248>& Syg, const int row);
	void snv();
	void snvR(cv::Vec<ushort, 248>& Syg, const int row);
	
	void saveRef();
	void loadRef(std::string name);

	void saveSyg(std::string name);
};

