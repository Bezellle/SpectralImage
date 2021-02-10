#include "RefData.h"

RefData::RefData():Name("unknown")
{
	Ref.reserve(10);
	//flag = 0;
}

RefData::RefData(std::string name):Name(name)
{
	loadRef("1ref.dat");
}

const std::string RefData::getName() { return Name; }
void RefData::setName(const std::string& name) { Name = name; }

void RefData::addRef(const cv::Vec<ushort, 248>& spectrum)
{
	//Ref.push_back(spectrum);
	Ref.emplace_back(spectrum);
}

void RefData::callSyg()
{
	if (Ref.size() > 0) {
		float sum = 0;
		for (int i = 0; i < 248; i++)
		{
			for (int k = 0; k < Ref.size(); k++)
				sum += (float)Ref[k][i];
			Sygnature[i] = (sum / (float)Ref.size());
			sum = 0;
		}
		Flag = SygMade;
	}
	else { std::cout << "Brak danych referencyjnych" << std::endl; }
}

void RefData::deriv()
{
	if (Flag != 0) {
		for (int i = 0; i < 247; i++)
			Sygnature[i] = (Sygnature[i + 1] - Sygnature[i]) / Resolution;
		Sygnature[247] = Sygnature[246];
		Flag = DerivativeMade;
	}
	else { std::cout << "Brak sygnatury" << std::endl; }
}

void RefData::deriv(cv::Vec<ushort, 248>& Syg, const int row)
{
	for (int i = 0; i < 247; i++)
		NormRef[row][i] = (Syg[i + 1] - Syg[i]) / Resolution;
	NormRef[247] = NormRef[246];
}

void RefData::snv()
{
	if (Flag != 0) {
		float mean = 0, stdVar = 0;
		for (int i = 0; i < 248; i++)
			mean += Sygnature[i];
		mean = mean / 248;

		for (int i = 0; i < 248; i++)
			stdVar += (Sygnature[i] - mean) * (Sygnature[i] - mean);
		stdVar = sqrt(stdVar / 248);

		for (int i = 0; i < 248; i++)
			Sygnature[i] = (Sygnature[i] - mean) / stdVar;

		Flag =NormalizedSyg;
	}
	else { std::cout << "Brak sygnatury" << std::endl; }
}

void RefData::snvR(cv::Vec<ushort, 248>& Syg, const int row)
{
	float mean = 0, stdVar = 0;
	for (int i = 0; i < 248; i++)
		mean += Syg[i];
	mean = mean / 248;

	for (int i = 0; i < 248; i++)
		stdVar += (Syg[i] - mean) * (Syg[i] - mean);
	stdVar = sqrt(stdVar / 248);

	for (int i = 0; i < 248; i++)
		NormRef[row][i] = (Syg[i] - mean) / stdVar;
}

void RefData::saveRef()
{
	//std::string name1 = Name;
	//if (Flag > 0) {
	//	std::ofstream save(name1+="_syg.txt");			//zapisanie przebiegu sygnatury do .txt

	//	for (ushort i = 0; i < 248; i++)
	//		save << Sygnature[i] << std::endl;
	//	save.close();
	//}

	//std::ofstream base("BaseClass.txt",std::ios::app);		//dopisanie nazwy do bazy 
	//if (base.is_open()) {
	//	base << std::endl << Name;
	//}
	//else { std::cout << "Error Error" << std::endl; }
	//base.close();

	std::string name = Name;
	//name += 'tt';

	std::ofstream output(name+="_snv.dat", std::ios::out | std::ios::binary);
	//ushort x = (ushort)Ref.size();		//zapis sygnatur
	float x = (float)Ref.size();			//zapis po normalizacji
	/*float x = Ref.size();*/
	if (output)
	{
		//std::cout << "otwarto" << std::endl;
		//output.write((char*)&x,sizeof(ushort));			//zapisanie rozmiaru zbioru referencyjnego
		output.write((char*)&x, sizeof(float));

		for (int i = 0; i < Ref.size(); i++)
		{
			//output.write((char*)&Ref[i][0],248*sizeof(ushort));			//zapisywanie po 1 wartoœci 
			output.write((char*)&NormRef[i][0],248*sizeof(float));
		}

		std::cout << "zapisano" << std::endl;
	}
	else {
		std::cout << "fail" << std::endl;
	}
			
	output.close();
}

void RefData::loadRef(std::string name)
{
	if (name.compare("1ref.dat")!=0)
	{
		name += ".dat";
	}

	std::cout << name << std::endl;

	std::ifstream input(name, std::ios::binary);
	ushort size=0;
	if (input)
	{
		for (int i = 0; i < 4; i++)
			name.pop_back();
		Name = name;
		//std::cout << "otwarto plik ";
		input.read((char*)&size,sizeof(ushort));
		Ref.resize(size);
		
		for (int i = 0; i < size-1; i++)
		{
			input.read((char*)&Ref[i][0], 248 * sizeof(ushort));
			//for (int k = 0; k < 248; k++)
			//	input.read((char*)&Ref[i][k], sizeof(ushort));			//wczytywanie po 1 wartoœci ???
			//input.read((char*)&Ref[i], sizeof cv::Vec<ushort,248>);
		}
		std::cout << " Wczytano Dane referencyjne" << std::endl;
	}
	else {std::cout << "Nie znaleziono pliku" << std::endl;}

	/*for (ushort i = 0; i < 248; i++)
		std::cout<<Ref[1][i]<<" ";
	std::cout << std::endl;*/

	input.close();
}

void RefData::saveSyg(std::string name)
{
	
	name += "_syg.csv";
	std::ofstream SygFile;
	SygFile.open(name);
	for (int i = 0; i < 248; i++)
		SygFile << Sygnature[i] << "\n";
	SygFile.close();

}