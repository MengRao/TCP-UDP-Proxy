#pragma once

#include <fstream>

#ifdef WIN32
#include <tchar.h>

template<class T>
struct fstream_fix
	: public T
{
	fstream_fix(){};

	template<class T1>
	fstream_fix(T1 v1){
		setlocale(LC_CTYPE, "");
		T::open(v1);
		setlocale(LC_CTYPE, 0);
	}

	template<class T1,class T2>
	fstream_fix(T1 v1,T2 v2){
		setlocale(LC_CTYPE, "");
		T::open(v1,v2);
		setlocale(LC_CTYPE, 0);
	}


	template<class T1>
	void open(T1 v1){
		setlocale(LC_CTYPE, "");
		T::open(v1);
		setlocale(LC_CTYPE, 0);
	}

	template<class T1,class T2>
	void open(T1 v1,T2 v2){
		setlocale(LC_CTYPE, "");
		T::open(v1,v2);
		setlocale(LC_CTYPE, 0);
	}
};

#define ifstream fstream_fix<std::ifstream>
#define ofstream fstream_fix<std::ofstream>

#endif

