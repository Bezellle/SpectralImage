#pragma once
#include <iostream>		
#include <fstream>			//fstream
#include <opencv2/imgcodecs.hpp> 
#include <opencv2\opencv.hpp>

	class SpectralData
	{
	public:
		SpectralData();
		~SpectralData();


	public:
		int X,Y,Z;
		int*** DataCube;
		int*** ScaledArray;
		int** Picture;
		int* Line;

		//ifstream input;

	public:
		void load();
		int scaleSpectrum(int unscaleInput);
		void scaleArray();
		void makePicture();
		//void on_trackbar(int, void*);

		
	};
