#include <string>
#include <sstream>
#include <windows.h>
#include <iphlpapi.h>
#include <stdio.h>
#include "md5.h"

#pragma comment(lib, "iphlpapi.lib")

using namespace std;

string get_volume_serial(){
	DWORD dwSerial;
	stringstream ss;

	if (!GetVolumeInformation(TEXT("C:\\"), NULL, 0, &dwSerial, NULL, NULL, NULL, 0)) {
		ss << "Error: " << GetLastError();
	}
	else {
		ss << dwSerial;
	}
	return ss.str();
}

//and the motherboard id for example that wasn't an option? well .. same shit 4982 ppl had the same ID aha I see
//Well, boys. Lets get to work xD
string getHWID()
{
	return md5(get_volume_serial());
}

