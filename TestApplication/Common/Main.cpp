#include <iostream>
#include "Loader/TGAImage.h"
#include "Common/Utilities.h"

using namespace Utilities;
using namespace std;

int main(int argc, char* argv[])
{
	if (argc != 3)
	{
		cout << endl;
		cout << "Syntax: TGAResizer original.tga half.tga" << endl;
		cout << endl;
	}
	else
	{
		TGAImage* image = new TGAImage();

		cout << "Reading " << argv[1] << "..." << endl;

		if (image->Load(string(argv[1])) == TGA_OK)
		{
			cout << "Reading done." << endl;
			cout << "Original size: " << NumberToString(image->GetWidth()) << "x" << NumberToString(image->GetHeight()) << endl;
			cout << "Resizing to: " << NumberToString(image->GetWidth() / 2) << "x" << NumberToString(image->GetHeight() / 2) << "..." << endl;

			image->Resize();

			cout << "Done." << endl;
			cout << "Saving " << argv[2] << "..." << endl;

			image->Save(string(argv[2]));

			cout << "Done." << endl;
		}
		else
			cout << "Image reading error." << endl;

		delete image;
	}

	return 0;
}