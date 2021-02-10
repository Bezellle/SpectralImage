#include "SpectralData.h"

using namespace std;
using namespace cv;

SpectralData::SpectralData() :Pixel(312), Spectra(248), Line(1)
{
	Refs.reserve(2);
	Refs.push_back(new RefData);
	RefsPosition = Refs.size()-1;
	GnuplotPipe = _popen("C:\\gnuplot\\bin\\gnuplot.exe", "w");
}
SpectralData::~SpectralData()
{
	for (int i = 0; i < Refs.size(); i++)
		delete Refs[i];
}

void SpectralData::load(int num)
{
	string name=std::to_string(num)+".bin";

	ifstream input(name, ios::binary);

	if (input) {
		input.read((char*)&Pixel, sizeof(ushort));			//X - kolumny - kolejne piksele w lini
		input.read((char*)&Spectra, sizeof(ushort));			//Y - rzedy -  Wartoœci spektralne dla kolejnych kanalow
		input.read((char*)&Line, sizeof(ushort));			//Z - kolejne zarejestrowane linie 

		if (Spectra > 248) cout << "error too many chanels" << endl;
		char* buff = (char*)malloc(Pixel * Line * Spectra * sizeof(ushort));	//alokacja pamiêci pod bufor 

		input.read(buff, Pixel * Line * Spectra * sizeof(ushort));				//wczytanie danych do bufora
		
		cv::Mat_<cv::Vec<ushort, 248>> hs(Pixel, Line, (cv::Vec<ushort, 248>*)buff, Line * Spectra * sizeof(ushort));
		DataCube = hs.clone();
		input.close();

		std::vector<cv::Mat> vec;
		cv::split(hs, vec);
		Pic = vec;
		free(buff);
		//std::cout << "Poprawnie wczytano plik " << endl;
	}
	else { std::cout << "Wyst¹pi³ b³¹d przy próbie otwarcia pliku" << std::endl; }
}

void SpectralData::onMouseStatic(int event, int x, int y, int flags, void* param)
{
	SpectralData* thiz = static_cast<SpectralData*>(param);
	thiz->onMouse(event,x,y,flags);
}

void SpectralData::onMouse(int event, int x, int y, int flags)										////////////////// OBSLUGA MYSZKI /////////////////////
{
	
	if (event == EVENT_LBUTTONDOWN /*&& flags == EVENT_FLAG_CTRLKEY*/)																		////////////////LButton CLICK
	{

		//cout << "Left button of the mouse is clicked - position (" << x << ", " << y << ")" << endl;
		//if (Refs.size() == 0) { std::cout << "Na Ref Data added" << endl; }
		//else {
		Refs[RefsPosition]->P1 = Point(x, y);
		std::cout << Refs[RefsPosition]->P1.x << " " << Refs[RefsPosition]->P1.y << std::endl;
			Refs[Refs.size() - 1]->addRef(DataCube(y, x));

			//cout<<Ref1.Ref.size()<<endl;
			std::cout << Refs[RefsPosition]->Ref.size() << std::endl;
			Refs[RefsPosition]->callSyg();
		//}
	}
	else if (event == EVENT_MOUSEMOVE && flags == (EVENT_FLAG_CTRLKEY + EVENT_FLAG_LBUTTON))
	{
		Refs[RefsPosition]->P2 = Point(x, y);
		//std::cout << p2.x << " " << p2.y << std::endl;
		cv::Mat track;
		Pic[50].convertTo(track, CV_8UC1);
		
		rectangle(track, Refs[RefsPosition]->P1, Refs[RefsPosition]->P2, Scalar(0,0));
		imshow("test", track);
		//imshow("test", tmp);
	}
	else if (event == EVENT_RBUTTONDOWN)																/////////////////RButton
	{
		Refs[RefsPosition]->callSyg();
		//snv();
		ShowPlot(Refs[RefsPosition]->Sygnature,GnuplotPipe);
		Refs[RefsPosition]->saveRef();
		Refs[RefsPosition]->loadRef("1ref.dat");
		for (int i = 0; i < 248; i++)
			std::cout << Refs[RefsPosition]->Ref[1][i]<< " ";
		std::cout << endl;
	}
	else if (event == EVENT_MBUTTONDOWN)																/////////////////Mbutton
	{
		Refs.emplace_back(new RefData);
		std::cout << "Dodano nowa klase referencyjna" << std::endl;
		//reRead(GnuplotPipe, DataCube(x, y));
	}
	else if (event == EVENT_MOUSEMOVE)																	///////////////////Move
	{
		ShowPlot(DataCube(y,x),GnuplotPipe);
		//reRead(GnuplotPipe,x,y);
		//cout << "x: " << x << ", y: " << y << endl;
	}

}

void SpectralData::makePicture(int chanel)
{
	cv::Mat tmp;
	Pic[chanel].convertTo(tmp, CV_8UC1);
	int Channel = chanel;
	namedWindow("test",WINDOW_AUTOSIZE);
	GnuplotPipe = _popen("C:\\gnuplot\\bin\\gnuplot.exe", "w");
	ShowPlot(DataCube(100,100),GnuplotPipe);

	setMouseCallback("test",onMouseStatic, (void*) this);										//MouseCallback static
	createTrackbar("Channel: ", "test", &Channel, Spectra-1, onTrackbarStatic, (void*)this);			
	cv::imshow("test", tmp);
	cv::waitKey();
	_pclose(GnuplotPipe);
}

void SpectralData::updateRef()
{
	/*int rectSize = abs(Refs[RefsPosition]->P2.x - Refs[RefsPosition]->P1.x) * abs(Refs[RefsPosition]->P2.y - Refs[RefsPosition]->P1.y);
	Refs.reserve(rectSize);*/
	int p1x, p2x, p1y, p2y;

	if (Refs[RefsPosition]->P1.x < Refs[RefsPosition]->P2.x) {					//sprawdzenie kolejnoœci punktów i uporz¹dkowanie mniejszy/wiekszy
		p1x = Refs[RefsPosition]->P1.x;
		p2x = Refs[RefsPosition]->P2.x;
	}
	else {
		p2x = Refs[RefsPosition]->P1.x;
		p1x = Refs[RefsPosition]->P2.x;
	}

	if (Refs[RefsPosition]->P1.y < Refs[RefsPosition]->P2.y) {
		p1y = Refs[RefsPosition]->P1.y;
		p2y = Refs[RefsPosition]->P2.y;
	}
	else {
		p2y = Refs[RefsPosition]->P1.y;
		p1y = Refs[RefsPosition]->P2.y;
	}

	for (int i = p1x; i < p2x; i++)					//Dodanie kwadratu do referencji
	{
		for (int k = p1y; k < p2y; k++)
			Refs[RefsPosition]->addRef(DataCube(k, i));
	}
	
}

void SpectralData::reRead(FILE* gnuplotpipe, int x, int y)
{
	ofstream zapis("dane.txt");			//zapisanie przebiegu do pliku .txt w celu wyswietlenia w gnuplocie
	for (ushort i = 0; i < 248; i++)
		zapis << 900 + i * Resolution << " " << DataCube(y,x)[i]<< endl;
	zapis.close();

	fprintf(gnuplotpipe, "replot\n");
	fflush(gnuplotpipe);
}

template<typename T>
void SpectralData::ShowPlot(cv::Vec<T, 248> spec, FILE* gnuplotpipe)
{
	if (!gnuplotpipe) cout << "error" << endl;
	
	ofstream zapis("dane.txt");			//zapisanie przebiegu do pliku .txt w celu wyswietlenia w gnuplocie

	for (ushort i = 0; i < 248; i++)
		zapis << 900 + i * Resolution << " " << spec[i] << endl;
	zapis.close();

	//fprintf(gnuplotpipe, "set yrange [-10:10]\n");
	fprintf(gnuplotpipe, "plot 'dane.txt' with lines\n");
	fflush(gnuplotpipe);
}


void SpectralData::onTrackbarStatic(int chanel, void* data)
{
	SpectralData* ref = static_cast<SpectralData*>(data);
	ref->onTrack(chanel);
	
	/*cv::Mat dst=*((Mat*)data);
	cv::Mat track;
	dst.convertTo(track, CV_8UC1);
	imshow("test", track);*/
}

void SpectralData::onTrack(int chanel)
{
	cv::Mat track;
	Pic[chanel].convertTo(track, CV_8UC1);
	imshow("test", track); 
}

int SpectralData::setPosition(const unsigned int& pos)
{
	if (pos > Refs.size() - 1) { return 0; }
	else RefsPosition = pos;
	return 1;
}

unsigned int SpectralData::getPosition() { return RefsPosition; }
