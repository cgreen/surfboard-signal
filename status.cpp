// Gets the signal data page from a Motorola Surfboard cable modem

#include <iostream>
#include <iterator>
#include <string>

#include <buffio.h>
#include <tidy.h>

#include "HttpClient.h"

using namespace boost;
using namespace std;

#define MODEM_ADDRESS "192.168.100.1"
#define MODEM_PORT "80"
#define MODEM_URI "/cmSignalData.htm"


int main(int argc, char* argv[])
{
	HttpClient httpClient(MODEM_ADDRESS, MODEM_PORT);

	TidyDoc doc = tidyCreate();

	string getBuffer = httpClient.Get(MODEM_URI);
	size_t endOfHeader = getBuffer.find("<!DOCTYPE");
	if (endOfHeader != string::npos)
		getBuffer = getBuffer.substr(endOfHeader);

	TidyBuffer output = {0};
	TidyBuffer errorBuffer = {0};
	int rc = -1;
	Bool ok;

	ok = tidyOptSetBool(doc, TidyXhtmlOut, yes);
	if (ok)
		rc = tidySetErrorBuffer(doc, &errorBuffer);
	if (rc >= 0)
		rc = tidyParseString(doc, getBuffer.c_str());
	if (rc >= 0)
		rc = tidyCleanAndRepair(doc);
	if (rc >= 0)
		rc = tidyRunDiagnostics(doc);
	if (rc > 1)
		rc = (tidyOptSetBool(doc, TidyForceOutput, yes) ? rc : -1);
	if (rc >= 0)
		rc = tidySaveBuffer(doc, &output);

	if (rc >= 0)
	{
		if (rc > 0)
			printf("\nDiagnostics:\n\n%s", errorBuffer.bp);
		printf("\nAnd here is the result:\n\n%s", output.bp );
	}
	else
		printf("A server error (%d) has occured.\n", rc);

	tidyBufFree(&output);
	tidyBufFree(&errorBuffer);
	tidyRelease(doc);

	return rc;
}

