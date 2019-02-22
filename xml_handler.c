#include <iostream>
#include <ostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <cctype>
#include <cstdio>
#include <cstdlib>

#include <libxml/parser.h>
#include <libxml/xinclude.h>
#include <libxml/tree.h>
#include <libxml/xmlIO.h>
#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>
#include <libxml/xmlreader.h>

#include "filesys.h"
#include "bufferList.h"
#include "strings.h"

#if 0
static const char *result = "<list><people>a</people><people>b</people></list>";
static const char *cur = NULL;
static int rlen;

static int
sqlMatch(const char * URI) {
	if ((URI != NULL) && (!strncmp(URI, "sql:", 4))) {
		return(1);
	}
	return(0);
}

static void *
sqlOpen(const char * URI) {
	if ((URI == NULL) || (strncmp(URI, "sql:", 4))) {
		return(NULL);
	}
	cur = result;
	rlen = strlen(result);
	return((void *) cur);
}

static int
sqlClose(void * context) {
	if (context == NULL) {
		return(-1);
	}
	cur = NULL;
	rlen = 0;
	return(0);
}

static int
sqlRead(void * context, char * buffer, int len) {
	const char *ptr = (const char *) context;

	if ((context == NULL) || (buffer == NULL) || (len < 0)) {
		 return(-1);
	}

	if (len > rlen) {
		len = rlen;
	}
	memcpy(buffer, ptr, len);
	rlen -= len;
	return(len);
}

int registerXML(void) {
	if (xmlRegisterInputCallbacks(sqlMatch, sqlOpen, sqlRead, sqlClose) < 0) {
		return -1;
	}
	return 0;
}

int readXMLfile(std::string path, class buffer *Buffer) {

	return 0;
}

#endif

int handleNewButton(FILE *fd, class buffer *Buffer, char button, int step) {
	char *data;
	int item = 0;
	while((data = readLine(fd)) != NULL) {
		if(!strncmp(data, "</button>", 9)) {
			return 1;
		} else if(!strncmp(data, "<next>", 6)) {
			item++;
		} else if (removeEndWhiteSpace(data) != NULL) {
			Buffer->addItemToBuffer(data, button, step, item);
		}
	}
	return 0;
}

int readIniFile(std::string path, class buffer *Buffer) {
	FILE *fd;
	char *data;
	int stepVal, readButtons = 0;

	if((fd = openFileRead((char *)path.c_str())) != NULL) {
		while((data = readLine(fd)) != NULL) {
			if(!strncmp(data, "<button=", 8)) {
				if(!parseInt((char *)&data[9], &stepVal)) {
					handleNewButton(fd, Buffer, data[8], stepVal);
				}
			} else if(!strncmp(data, "<start>", 7)) {
				std::cout << "enter basic init here" << std::endl;
			}
		}
	}
	closeFile(fd);
	
	return readButtons;
}


