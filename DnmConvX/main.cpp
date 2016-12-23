// main.cpp:
//
#include<windows.h>
#include"DnmConvX.h"

using namespace std;

//
// Global variables
//
#define TITLE -2
#define HELP -1
//#define ERROR 1

LPCTSTR g_pjname="DnmConvX";	// Global variable: Project name

//
// Prototype function
// 
// Print console help usage
i32 printConsole(i32 option);
//
// Main entry
// 
i32 main(i32 argc, char *argv[]){
	printConsole(TITLE);
	CDnmConvX dcx;
	if(argc==1)
		return printConsole(HELP);
	else{
		u16 lastError=dcx.inputDnmFile(argv[1]);
		if(lastError==E_DnmRead) return printConsole(lastError);
		string ini(argv[0]);
		size_t bg=ini.find_last_of('\\')+1;
		ini=ini.substr(bg,ini.find_last_of('.')-bg);
		ini+=".ini";
		if(lastError<E_MemoryError)lastError=dcx.inputIniFile(ini.c_str());
		ini=argv[1];
		ini=ini.substr(0,ini.find_last_of('.'));
		ini+=".ini";
		if(lastError<E_MemoryError)lastError=dcx.inputIniFile(ini.c_str());
		if(lastError<E_MemoryError)lastError=dcx.outputToXFile();
		return lastError;
	}
}
// Print console help usage
i32 printConsole(i32 option){
	if(option==TITLE)
	{
		cout<<g_pjname<<" version 1.0\n"
			<< "Convert YS Flight DNM file format to DirectX file.\n\n";
	}

	if(option==HELP)
	{
		cout<<"Usage:\n"
			<<g_pjname<<".exe <input file> [output file]\n\n"
			<<endl
			<<"  <input file> Specify the input file path.\n"
			<<"Example:\n\n"
			<<g_pjname<<".exe airplane.dnm\n\n"
			<<"              It will output to airplane.x\n";
		return true;
	}

	if(ERROR<=option)
	{
		cout<< "Invalid command line? please check the usage help\n\n"
			<< "Try typping:\n\t\t "<<g_pjname<<".exe /H\n\n"
			<< "Or\n\n"
			<< "Try typping:\n\t\t "<<g_pjname<<".exe <input file>\n\n";
		return option;
	}
	return 0;
} // end of "Print console help usage"
;