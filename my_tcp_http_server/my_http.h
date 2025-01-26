#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
using namespace std;
#include <string>
#include <map>
#include <vector>
#include <sstream> 
#include <fstream>
//#include <filesystem>
#include <iostream>
#include <cstdlib>

// this is my implemetation to http 1.1 protocol
//for this task i took the nassesary headrs only ,can be modifid to full scale server that support more headers
//and file types 
const string ROOT_DIRECTORY = "C://temp"; // default working directory
const string DEFAULT_RESOURCE = "/page.html"; // default index

enum fileType {
	TEXT_HTML,
	TEXT_PLAIN,
	APPLICATION_JSON,
	IMAGE_JPEG,
	IMAGE_PNG,
	IMAGE_GIF,
	NOT_SUPPORTED
};

//******request******
/*
HTTP 1.1 - REQUEST :

Request line : <Method> <Path> <HTTP - Version>\r\n
Headers :
<Header 1 - Name> : <Value>\r\n
<Header 2 - Name>: <Value>\r\n
...
<Header n - Name>: <Value>\r\n
entity body :
<Data>
*/

#define MAX_METHOD 8
#define MAX_PATH 128
#define MAX_VERSION 16
#define MAX_HEADERS_REQUEST 7
#define MAX_HEADER_NAME 32
#define MAX_HEADER_VALUE 96
#define MAX_BODY_REQUEST 512

struct HttpRequest 
{
    string method;                           // HTTP method (e.g., GET, POST)
    string path;                             // Path (e.g., /index.html)
    map<string, string> query;               // Query parameters as key-value pairs
    string version;                          // HTTP version (e.g., HTTP/1.1)
    map<string,string> headers;              // Headers as key-value pairs
    string body;                             // Request body
};


int parseHttpRequest(const string& requestLine,const string& headers, const string& body,HttpRequest* request);


//******response******

/*HTTP 1.1 - RESPONSE:
Status line : <HTTP - Version> <Status - Code> <Reason - Phrase>\r\n
Headers :
<Header 1 - Name> : <Value>\r\n
<Header 2 - Name>: <Value>\r\n
.
.
.
<Header n - Name>: <Value>\r\n
entity body :
<Data>*/

//I CAN ADD MORE IF I WANT 
#define MAX_STATUS_MESSAGE 64
#define MAX_HEADERS_RESPONSE 7
#define MAX_BODY_RESPONSE 75000


struct HttpResponse
{
    string version="HTTP/1.1";                      // HTTP version (e.g., HTTP/1.1)
    int status_code;                                // Status code (e.g., 200)
    string status_message;						    // Status message (e.g., OK)
	vector<pair<string, string>> headers;                // Headers as key-value pairs
    string body;
	bool connection_close = false;												// Response body (e.g., HTML or image data)
};

string read_file(ifstream& file);

//optional headers for response:
// Headers:
	//Content - Type: Specifies the MIME type of the response body(e.g., text / html, application / json).
	//Example : Content - Type : text / html; charset = UTF - 8
    // 
	//Content - Length: Indicates the size(in bytes) of the response body.
	//Example : Content - Length : 1024
    // 
	//Date : Provides the date and time the response was generated.
	//Example : Date: Fri, 03 Jan 2025 12 : 00 : 00 GMT
	//Connection : Indicates whether the server will close the connection or keep it open for further requests.
	//Example : Connection: keep - alive
	// 
	//Last - Modified : Indicates the last time the resource was modified.
	//Example : Last - Modified : Fri, 01 Jan 2025 10 : 00 : 00 GMT
	// (for put)
	// 
	//Server : Identifies the server software handling the request.
	//Example : Server: MyCustomServer / 1.0
	// 
	//Location : Used in redirection responses to indicate the new URL.
	//Example : Location: https://example.com/new-page

string buildResponse(HttpResponse& response);
string get_content_type(const string& file_path, fileType& fType);
bool isBinaryData(fileType type);

#define OK 200
#define Created 201
#define Accepted 202
#define NO_CONTENT 204
#define Moved_Permanently 300
#define NOT_FOUND 404
#define REQUEST_TIMEOUT 408
#define INTERN_SERVER_ERRNO 500