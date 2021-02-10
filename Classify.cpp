#include "Classify.h"

Classify::Classify()
{
	Dims.resize(248);
	for (int i = 0; i < 248; i++)
		Dims[i] = i;
}

template<typename N>
Classify::Classify(const std::vector<cv::Vec<ushort,248>>& refs):Classify()
{
	Ref2 = refs;
	ErrorMat = new int[3];
}


Classify::Classify(const std::vector<RefData*> refs) :Classify()
{
	Refs = refs;
	//*RefDatx.Ref = &ref.Ref;
}

Classify::~Classify()
{
	/*for (int i = 0; i < Refs.size(); i++)
		delete Refs[i];*/
}


void Classify::dimReduction(int size)
{
	float tab[248];
	float* tmp = new float[size];
	memset(tab, 0, 248 * sizeof(float));
	memset(tmp, 0, size * sizeof(float));
	if (Refs.size() > 1) {
		Dims.resize(size);
		//ushort* chanel=new ushort[size];
		///*for (int i = 0; i < size; i++)
		//	chanel[i] = 0;*/
		//memset(chanel, 0, size * sizeof(ushort));


		for (int i = 10; i <248 ; i++)
			tab[i] = abs(Refs[0]->Sygnature[i] - Refs[1]->Sygnature[i]);

		for (int i = 0; i < size; i++)
		{
			for (int n = 0; n < 248; n++)
			{
				if (tab[n] > tmp[i]) {
					Dims[i] = n;
					tmp[i] = tab[n];
				}
			}
			//std::cout << Dims[i] << " " << tab[Dims[i]] << std::endl;
			tab[Dims[i]] = 0;
		}
	}
	else { std::cout << "Zbyt malo obszarow referencyjnych" << std::endl; }

	//delete tmp;

	std::string name = "reduce88.csv";
	std::ofstream ReduceFile;
	ReduceFile.open(name);
	for (int i = 0; i < size; i++)
		ReduceFile << Dims[i] << "\n";
	ReduceFile.close();

}

int Classify::distance(const cv::Vec<float, 248>& spectrum)
{
	float dist1 = 0, dist2 = 0;

	for (int i = 0; i < Dims.size(); i++)
	{
		dist1 += abs(Refs[0]->Sygnature[Dims[i]] - spectrum[Dims[i]]);
		dist2 += abs(Refs[1]->Sygnature[Dims[i]] - spectrum[Dims[i]]);
	}
	   
	float res = dist1 / dist2;
	if (res == 0)
		return 0;
	if (res>1.2)
	{
		return 2;
	}
	else if(res<0.95)
	{
		return 1;
	}
	else {
		return 0;
	}

}

int Classify::SAM(const cv::Vec<float, 248>& spectrum)
{
	float skal1=0, skal2=0, a_len=0, b_len=0, c_len=0, alfa=0, beta=0;

	for (int i = 0; i < Dims.size(); i++)
	{
		skal1 += Refs[0]->Sygnature[Dims[i]] * spectrum[Dims[i]];			//iloczyn skalarny pomiedzy spektrum a syg1 i syg2
		skal2 += Refs[1]->Sygnature[Dims[i]] * spectrum[Dims[i]];

		a_len += pow(Refs[0]->Sygnature[Dims[i]], 2);
		b_len += pow(Refs[1]->Sygnature[Dims[i]], 2);
		c_len += pow(spectrum[Dims[i]], 2);
	}
	
	alfa = skal1 / sqrt(a_len) * sqrt(c_len);
	beta = skal2 / sqrt(b_len) * sqrt(c_len);

	if (alfa > 0.8 && alfa>beta)
		return 1;
	else if(beta > 0.8)
		return 2;
	else
		return 0;
}

void Classify::testClassify(int norm, int clasy)
{
	ErrorMat[0].fill(0);
	ErrorMat[1].fill(0);

	for (int i = 0; i < Refs.size(); i++)
	{
		Refs[i]->NormRef.resize(Refs[i]->Ref.size());
	}
	int tmp;

	if (norm == 1)					//SVN dla zbioru
	{
		for (int i = 0; i < Refs.size(); i++)
		{
			for (int k = 0; k < Refs[i]->Ref.size(); k++)
			{
				Refs[i]->snvR(Refs[i]->Ref[k],k);
			}
		}
	}
	else if (norm == 2)
	{
		//for (int i = 0; i < Refs.size(); i++)			//standaryzacja tylko na probe
		//{
		//	for (int k = 0; k < Refs[i]->Ref.size(); k++)
		//	{
		//		Refs[i]->snvR(Refs[i]->Ref[k], k);
		//	}
		//}

		for (int i = 2; i < Refs.size(); i++)						//pochodna dla zbioru 
		{
			for (int k = 0; k < Refs[i]->Ref.size(); k++)
			{
				Refs[i]->deriv(Refs[i]->Ref[k], k);
			}
		}
	}
	else if(norm == 0)
	{
		for (int i = 2; i < Refs.size(); i++)
		{
			for (int k = 0; k < Refs[i]->Ref.size(); k++)
			{
				Refs[i]->NormRef[k]=Refs[i]->Ref[k];
			}
		}
	}

	if (clasy == 0)
	{
		for (int i = 2; i < Refs.size(); i++)
		{
			for (int k = 0; k < Refs[i]->Ref.size(); k++)
			{
				tmp = distance(Refs[i]->NormRef[k]);
				ErrorMat[i-2][tmp] += 1;
			}
		}
	}
	else if (clasy == 1)
	{
		for (int i = 2; i < Refs.size(); i++)
		{
			for (int k = 0; k < Refs[i]->Ref.size(); k++)							//////////Klasyfikacja i ErrorMat
			{
				tmp = SAM(Refs[i]->NormRef[k]);
				ErrorMat[i-2][tmp] += 1;
			}
		}
	}
	else
	{
		std::cout << "Wrong Classyfier type" << std::endl;
	}

	for (int i = 0; i < Refs.size(); i++)
		std::cout << "size: " << i << "  " << Refs[i]->Ref.size() << std::endl;

	std::cout << "\n ---------- \n ERROR MAT:\n";
	for (int i = 0; i < 3; i++)
	{
		for (int k = 0; k < 3; k++)
			std::cout << ErrorMat[i][k] << " ";
		std::cout << std::endl;
	}

	/////////////// Wyniki ///////////////////
	std::cout << "\n ---------- \n INDEX:\n";
	float positive=0, all=0;											/////
	positive = (ErrorMat[0][1] + ErrorMat[1][2]);///(Refs[0]->Ref.size()+ Refs[1]->Ref.size());
	all = Refs[2]->Ref.size() + Refs[3]->Ref.size();			
	Dcomp = positive / all;
	std::cout << "Dcomp= " << Dcomp << std::endl;

	D1 = ErrorMat[0][1];
	D1 = D1/ Refs[2]->Ref.size();
	D2 = ErrorMat[1][2];
	D2 = D2 / Refs[3]->Ref.size();

	std::cout << "D1 = " << D1 << " " << "D2= " << D2 << std::endl;

	tmp = Refs[2]->Ref.size() * (ErrorMat[0][1] + ErrorMat[1][1]);
	Kappa1 = positive * all - tmp;
	Kappa1 = Kappa1 / ((all *all) - tmp);
	std::cout << "Kappa1= " << Kappa1 << std::endl;

	tmp = Refs[3]->Ref.size() * (ErrorMat[0][2] + ErrorMat[1][2]);
	Kappa2 = positive * all - tmp;
	Kappa2 = Kappa2 / ((all * all) - tmp);
	std::cout << "Kappa2= " << Kappa2 << std::endl;
}

void Classify::printResult(cv::Mat_<cv::Vec<ushort, 248>>& DataCube, int norm, int clas)
{
	int tmp;
	Result.create(DataCube.size(), CV_8UC3);

	for (int y = 0; y < Result.rows; y++)
	{
		for (int x = 0; x < Result.cols; x++)
		{
			if (DataCube(y, x)[17] < 45)
				Result.at<cv::Vec3b>(y, x) = cv::Vec3b(0,0,0);
			else
			{
				//Result.at<cv::Vec3b>(y, x) = cv::Vec3b(250, 250, 250);
				if (norm == 0)
				{
					if(clas==1)
						tmp = SAM(DataCube(y, x));
					else
						tmp = SAM(DataCube(y, x));
					if (tmp == 0)
						Result.at<cv::Vec3b>(y, x) = cv::Vec3b(255, 255, 255);
					else if (tmp == 1)
						Result.at<cv::Vec3b>(y, x) = cv::Vec3b(200, 10, 69);
					else
						Result.at<cv::Vec3b>(y, x) = cv::Vec3b(69, 69, 200);
				}
				else if (norm == 1)
				{
					if (clas == 1)
						tmp = SAM(SSNV(DataCube(y, x)));
					else
						tmp = distance(SSNV(DataCube(y, x)));
					
					if (tmp == 0)
						Result.at<cv::Vec3b>(y, x) = cv::Vec3b(255, 255, 255);
					else if (tmp == 1)
						Result.at<cv::Vec3b>(y, x) = cv::Vec3b(200, 10, 69);
					else
						Result.at<cv::Vec3b>(y, x) = cv::Vec3b(69, 69, 200);
				}
				else if (norm == 2)
				{
					if (clas == 1)
						tmp = SAM(derivetiv(DataCube(y, x)));
					else
						tmp = distance(derivetiv(DataCube(y, x)));
					
					if (tmp == 0)
						Result.at<cv::Vec3b>(y, x) = cv::Vec3b(255, 255, 255);
					else if (tmp == 1)
						Result.at<cv::Vec3b>(y, x) = cv::Vec3b(200, 10, 69);
					else
						Result.at<cv::Vec3b>(y, x) = cv::Vec3b(69, 69, 200);
				}
			}
				
		}
	}

	cv::namedWindow("Result", cv::WINDOW_AUTOSIZE);
	cv::imshow("Result", Result);
	cv::waitKey();
}

cv::Vec<float, 248> Classify::derivetiv(cv::Vec<ushort, 248> org)
{
	cv::Vec<float, 248> pochodna;

	for (int i = 0; i < 247; i++)
		pochodna[i] = (org[i + 1] - org[i]) / Resolution;
	pochodna[247] = pochodna[246];

	return pochodna;
}

cv::Vec<float, 248> Classify::SSNV(cv::Vec<ushort, 248> org)
{
	cv::Vec<float, 248> ss;
	float mean = 0, stdVar = 0;
	for (int i = 0; i < 248; i++)
		mean += org[i];
	mean = mean / 248;

	for (int i = 0; i < 248; i++)
		stdVar += (org[i] - mean) * (org[i] - mean);
	stdVar = sqrt(stdVar / 248);

	for (int i = 0; i < 248; i++)
		ss[i] = (org[i] - mean) / stdVar;


	return ss;
}
