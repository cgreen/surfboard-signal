// Gets the signal data page from a Motorola Surfboard cable modem

#include <iostream>
#include <iterator>
#include <string>

#include <tidy/buffio.h>
#include <tidy/tidy.h>

#include <xercesc/dom/DOM.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>

#include "HttpClient.h"

using namespace std;
using namespace boost;
using namespace xercesc;

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

	// Ugly code copied from a TidyLib example

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

	try
	{
		XMLPlatformUtils::Initialize();
	}
	catch (const XMLException& exception)
	{
		char *message = XMLString::transcode(exception.getMessage());
		cout << "Error during initialization: " << endl << message << endl;
		XMLString::release(&message);
		return 1;
	}

	byte *xmlStream = new byte[output.size];
	memcpy(xmlStream, output.bp, output.size);
	MemBufInputSource inputSource(xmlStream, output.size, "tidylib output", true);

	XercesDOMParser *parser = new XercesDOMParser();
	ErrorHandler *errorHandler = (ErrorHandler*) new HandlerBase();
	parser->setErrorHandler(errorHandler);
	parser->setLoadExternalDTD(false);

	try
	{
		parser->parse(inputSource);
	}
	catch (const SAXParseException &exception)
	{
		char *message = XMLString::transcode(exception.getMessage());
		cout << "Error during parsing: " << endl << message << endl;
		XMLString::release(&message);
		return 1;
	}
	catch (const XMLException &exception)
	{
		char *message = XMLString::transcode(exception.getMessage());
		cout << "Error during parsing: " << endl << message << endl;
		XMLString::release(&message);
		return 1;
	}
	catch (const DOMException &exception)
	{
		char *message = XMLString::transcode(exception.getMessage());
		cout << "Error during parsing: " << endl << message << endl;
		XMLString::release(&message);
		return 1;
	}

	DOMDocument *domDoc = parser->getDocument();
	DOMElement *docRoot = domDoc->getDocumentElement();

	XMLCh *rowTag = XMLString::transcode("tr");
	DOMNodeList *nodeList = docRoot->getElementsByTagName(rowTag);
	XMLString::release(&rowTag);

	DOMNode *rowNode = nodeList->item(3);
	DOMNodeList *children = rowNode->getChildNodes();

	int numChildren = children->getLength();

	for (int i = 0; i < numChildren; i++)
	{
		DOMNode *child = children->item(i);

		if (child->getNodeType() == DOMNode::TEXT_NODE)
		{
			DOMText *data = dynamic_cast<DOMText*>(child);
			char *name = XMLString::transcode(child->getNodeName());
			//char *text = XMLString::transcode(child->getNodeValue());
			char *text = XMLString::transcode(data->getWholeText());
			cout << "node " << i << " name: " << name;
			cout << " value: " << text;
			delete name;
			delete text;
		}
	}

/*
	char *nodeContent = XMLString::transcode(rowNode->getTextContent());
	cout << endl << nodeContent << endl;
	delete nodeContent;
*/
	

	delete parser;

	XMLPlatformUtils::Terminate();

	tidyBufFree(&output);
	tidyBufFree(&errorBuffer);
	tidyRelease(doc);

	return rc;
}

