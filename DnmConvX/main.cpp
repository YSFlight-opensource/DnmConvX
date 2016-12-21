// main.cpp: define el punto de entrada de la aplicación de consola.
//
#define INITGUID

#include<windows.h>
// #include<string>	// string
// #include<sstream>	// istringstream
// #include<iostream>	// cout endl getchar()
// #include<iomanip>	// setprecision() hex setw() setfill() right
// #include<fstream>
// #include<vector>
// #include<queue>
// #include<map>		// map make_pair()
// #include<cerrno>	// throw(errno)
// #include<math.h>	// sqrtf()
// #include<algorithm>	// for_each()
// #include<valarray>	// valarray
// #include<memory>	// shared_ptr
#include"DnmConvX.h"
//// Load a .X File using the DirectXFile API
//#include "dxfile.h"
//#include "rmxfguid.h"
//#include "rmxftmpl.h"
//
//#pragma comment (lib, "d3dxof.lib")
/// to do _HAS_ITERATOR_DEBUGGING false
///#define _HAS_ITERATOR_DEBUGGING false

using namespace std;

//
// Global variables
//
#define TITLE -2
#define HELP -1
//#define ERROR 1

LPCTSTR g_pjname="DnmConvX";	// Global variable: Project name
UINT g_lineCount=0;	// track error read on input file

//
// Prototype function
// 
// Print console help usage
i32 printConsole(i32 option);
#include<list>
template<class BDIter>
void alg(BDIter,BDIter,std::bidirectional_iterator_tag){
	std::cout << "alg() called for bidirectional iterator\n";
}
template <class RAIter>
void alg(RAIter,RAIter,std::random_access_iterator_tag){
	std::cout << "alg() called for random-access iterator\n";
}
template<class Iter>
void alg(Iter first,Iter last){
	alg(first,last,typename std::iterator_traits<Iter>::iterator_category());
}
//
// Main entry
// 
i32 main(i32 argc, char *argv[]){
	printConsole(TITLE);
	std::vector<int> v;
	alg(v.begin(), v.end());

	std::list<int> l;
	alg(l.begin(), l.end());


// 	vector<int>i(10,3);
// 	for(decltype(i)::iterator it=i.begin();i!=it.end();++it){
// 		cout<<i<<' ';
// 	}
//	return 0;///delete
	CDnmConvX dcx;

//	system("PAUSE");
//	return 10;

	if(argc==1)
		return printConsole(HELP);
	else{
		u16 lastError=dcx.inputDnmFile(argv[1]);
		if(lastError==E_DnmRead){
			printConsole(E_DnmRead);
			return lastError;
		}
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

