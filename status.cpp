// Gets the signal data page from a Motorola Surfboard cable modem

#include <iostream>
#include <iterator>
#include <string>

#include "HttpClient.h"

using namespace boost;
using namespace std;

#define MODEM_ADDRESS "192.168.100.1"
#define MODEM_PORT "80"
#define MODEM_URI "/cmSignalData.htm"


int main(int argc, char* argv[])
{
	HttpClient httpClient(MODEM_ADDRESS, MODEM_PORT);
	cout << httpClient.Get(MODEM_URI);

	return 0;
}

