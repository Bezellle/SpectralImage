#include <iostream>
#include <fstream>   //ifstream
#include <opencv2/imgcodecs.hpp> 
#include <opencv2\opencv.hpp>

#include "SpectralData.h"

using namespace std;

int main(int argc, char** argv)
{
	SpectralData Spectra;
	//Spectra.load();
	Spectra.scaleArray();
	Spectra.makePicture();
	//~Spectra;
	
	return 0;
}