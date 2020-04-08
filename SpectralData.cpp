#include "SpectralData.h"

using namespace std;
using namespace cv;

SpectralData::SpectralData()
{
	int X = 312;
	int Y = 248;
	int Z = 0;
	DataCube = NULL;
}
SpectralData::~SpectralData()
{
	DataCube = NULL;
	ScaledArray = NULL;
}

void SpectralData::load()
{
	ifstream input;
	input.open("3D.dat");		//ifstream input

	input >> X;			//X,Y - rozdzielczoœæ obrazu
	input >> Y;			
	input >> Z;			//Z - kolejne linie w sekwencji

	DataCube = new int** [X];
	for (int i = 0; i < X; i++)
	{
		DataCube[i] = new int* [Y];
		for (int j = 0; j < Y; j++)
			DataCube[i][j] = new int[Z];
	}
		
	for (int k = 0; k < Z; k++)
	{
		for (int m = 0; m < X; m++)
		{
			for (int l = 0; l < Y; l++)
				input >> DataCube[m][l][k];
		}
	}

	//for (int i = 0; i < X; i++)		//wyœwietlanie wybranej lini
	//{
	//	for (int l = 0; l < Y; l++)
	//		cout << DataCube[i][l][44] << " ";
	//	cout << endl;
	//}

	input.close();
}

int SpectralData::scaleSpectrum(int unscaleInput)
{
	double x= (255 - 1) * (unscaleInput) / 4095;
	return (int)round(x);
}

void SpectralData::scaleArray()
{
	ScaledArray = new int** [X];			//inicjalizacja zmiennej
	for (int i = 0; i < X; i++)
	{
		ScaledArray[i] = new int* [Y];
		for (int j = 0; j < Y; j++)
			ScaledArray[i][j] = new int[Z];
	}

	for (int k = 0; k < Z; k++)
	{
		for (int m = 0; m < X; m++)
		{
			for (int l = 0; l < Y; l++)
				ScaledArray[m][l][k]= scaleSpectrum(DataCube[m][l][k]);
		}
	}
}

void SpectralData::makePicture()
{
	//int alpha = 0;

	ifstream input("short.bin",ios::binary);

	short int a = 0;

	input.read((char*)&X, sizeof a);			//dokoñczyæ 
	input.read((char*)&Y, sizeof a);
	input.read((char*)&Z, sizeof a);

	cout << X << " " << Y << " " << Z << endl;

	vector<vector<vector<short int>>>Array3D(X, vector<vector<short int>>(Y, vector<short int>(Z)));
	vector<vector<short>> Array2D(X, vector<short>(Y));


	for (int i = 0; i < Z;i++)
	{
		for (int j = 0; j < Y; j++)
		{
			for (int k = 0; k < X; k++)
				input.read((char*)&Array3D[k][j][i],sizeof(Array3D[0][0][0]));
		}
	}

	input.close();

	//input.read((char*)&Line[0], Line.size() * sizeof(Line[0]));

	for (int i = 0; i < X; i++)
	{
		for (int j = 0; j < Y; j++)
			Array2D[i][j] = Array3D[i][j][1];
	}

	Mat image(X ,Y , CV_16U, &Array2D);
	namedWindow("Obraz",WINDOW_AUTOSIZE);
	imshow("Obraz", image);
	waitKey(-1);
	
	
}

