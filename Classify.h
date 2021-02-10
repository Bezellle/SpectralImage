#pragma once
#include "RefData.h"
#include <array>
#include <math.h>

#define PI 3.1415

class Classify :
	public RefData
{
public:
	Classify();
	template<typename N>
	Classify(const std::vector<cv::Vec<ushort, 248>>& refs);
	Classify(const std::vector<RefData*> refs);

	~Classify();

	std::vector<RefData*> Refs;
	std::vector<ushort> Dims;
	//template<typename N>
	std::vector<cv::Vec<float, 248>> Ref2;
	//int** ErrorMat;
	std::array<std::array<int, 3>, 3> ErrorMat;
	cv::Mat Result;
	float Dcomp,Dprod,Kappa1,Kappa2,D1,D2;

	
	void printResult(cv::Mat_<cv::Vec<ushort, 248>>& DataCube, int norm, int clas);
	void dimReduction(int size);						
	int distance(const cv::Vec<float,248>& spectrum);
	int SAM(const cv::Vec<float, 248>& spectrum);
	void testClassify(int norm=0, int clasy=0);
	cv::Vec<float, 248> derivetiv(cv::Vec<ushort, 248> org);
	cv::Vec<float, 248> SSNV(cv::Vec<ushort, 248> org);
};

