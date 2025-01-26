#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#pragma comment(lib, "Ws2_32.lib")
#include <winsock2.h>
#include <string>
#include <time.h>
#include "my_http.h"
#include <vector>
#include<set>

#include <filesystem>
namespace fs = std::filesystem;
#include <iostream>
#include <cstdlib>

using namespace std;


//need to change structure to more comprihensive one

struct SocketState
{
	SOCKET id;			// Socket handle
	int	recv;			// Receiving?
	int	send;			// Sending?
	int sendSubType;	// Sending sub-type
	string buffer;   //make it bigger change it to dynamic allocator
	HttpRequest* requestStruct = nullptr;
	HttpResponse* responseStruct  = nullptr;
	time_t lastRecived;
};

bool addSocket(SOCKET id, int what, SocketState* sockets);
void removeSocket(int index, SocketState* sockets);
void acceptConnection(int index, SocketState* sockets);
void receiveMessage(int index, SocketState* sockets);
void sendMessage(int index, SocketState* sockets);
int parseContentLength(string& buffer, int headersLength);
void prepareResponse(int index, SocketState* sockets);
void AddRegularHeaders(HttpResponse* response);
void OptionsResponsefiller(HttpRequest* request, HttpResponse* response);
void GETResponsefiller(HttpRequest* request, HttpResponse* response, bool IFHEAD);
void CheckLangValid(HttpRequest* request);
void NotAllowMethodHandler(HttpResponse* response);
void NotFoundResourceHandler(HttpResponse* response);
string makePostHTMLbody(const string& filePath, const string& body);
void sendMessage(int index, SocketState* sockets); 
void ErrorResponse(HttpResponse* response, int statusCode, const string& message);
void HEADResponsefiller(HttpRequest* request, HttpResponse* response);
bool checkIfPathAllowd(HttpRequest* request);
void POSTesponsefiller(HttpRequest* request, HttpResponse* response);
bool content_type_validation(const string& file_path);
string trim(const string& str);
void PUTesponsefiller(HttpRequest* request, HttpResponse* response);
string makePUTHTMLbody(const string& filePath, const string& body);
void DELETEesponsefiller(HttpRequest* request, HttpResponse* response);
string makeDELETEHTMLbody(const string& filePath, const string& body);
void TRACEresponsefiller(HttpRequest* request, HttpResponse* response);

//mybe think about different classification 
const int PORT = 80;
const int MAX_SOCKETS = 60;
const int INITIAL_BUFFER_SIZE = 2048;
const int MAX_TIME_FOR_SOCKET = 120;
const int EMPTY = 0;// if sockets[i].recv = EMPTY WE CAN PUT THERE NEW CLIENT SOCKET 
const int LISTEN = 1;// if sockets[i].recv = LISTEN IT IS THE SERVER SOCKET 
const int RECEIVE = 2;//SERVER NEED TO READ THE CONTENT OF THE MESSAGE 
const int PENDING = 3;
const int IDLE = 4;//THERE NOTHING TO SEND
const int SEND = 5;// SERVER NEED TO SEND MESSAGE 

//commands 
// change to get, put, etc.
const int RN_ENDING = 2;
const int RNRN_ENDING = 4; 


int socketsCount = 0;

void main()
{
	struct SocketState sockets[MAX_SOCKETS] = { 0 };
	WSAData wsaData;

	if (NO_ERROR != WSAStartup(MAKEWORD(2, 2), &wsaData))
	{
		cout << "Server: Error at WSAStartup()\n";
		return;
	}


	SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);


	if (INVALID_SOCKET == listenSocket)
	{
	//    cout << "Time Server: Error at socket(): " << WSAGetLastError() << endl;
		WSACleanup();
		return;
	}

	// Create a sockaddr_in object called serverService represnts the server address. 
	sockaddr_in serverService;
	serverService.sin_family = AF_INET;
	serverService.sin_addr.s_addr = INADDR_ANY;
	serverService.sin_port = htons(PORT);
	//bind the socket with the port address
	if (SOCKET_ERROR == bind(listenSocket, (SOCKADDR*)&serverService, sizeof(serverService)))
	{
		//cout << "Time Server: Error at bind(): " << WSAGetLastError() << endl;
		closesocket(listenSocket);
		WSACleanup();
		return;
	}

	// Listen on the Socket for incoming connections.
	// create a queue with a size of 5 
	if (SOCKET_ERROR == listen(listenSocket, 5))
	{
	 //cout << "Time Server: Error at listen(): " << WSAGetLastError() << endl;
		closesocket(listenSocket);
		WSACleanup();
		return;
	}

	//adding the server socket to socket array 
	//it will be changed to more efficient then array
	addSocket(listenSocket, LISTEN,sockets);
	cout << "Server:server is listening at port 80" << endl;
	// Accept connections and handles them one by one.
	time_t currTime;
	
	while (true)
	{
		
		fd_set waitRecv;
		FD_ZERO(&waitRecv);
		for (int i = 0; i < MAX_SOCKETS; i++)
		{
			time(&currTime);
			time_t timeToClose = currTime - MAX_TIME_FOR_SOCKET;
			//adding the socket that listening or recived data
			//the sockets with the status RECIEIVE are sockets that containing a msg from the client's
			if (i != 0) 
			{
				if ((sockets[i].recv == LISTEN) || (sockets[i].recv == RECEIVE))
				{
					if (sockets[i].lastRecived < timeToClose)
					{
						removeSocket(i, sockets);
					}
					else
					{
						FD_SET(sockets[i].id, &waitRecv);
					}
					
				}
			}
			else
			{
				FD_SET(sockets[i].id, &waitRecv);
			}
			
		}

		fd_set waitSend;
		FD_ZERO(&waitSend);
		for (int i = 0; i < MAX_SOCKETS; i++)
		{
			//adding the socket that need to be sending data too 

			if (sockets[i].send == SEND)
				FD_SET(sockets[i].id, &waitSend);
		}

		//
		// Wait for interesting event.
		// Note: First argument is ignored. The fourth is for exceptions.
		// And as written above the last is a timeout, hence we are blocked if nothing happens.
		//
		int nfd;
		nfd = select(0, &waitRecv, &waitSend, NULL, NULL);
		if (nfd == SOCKET_ERROR)
		{
			cout << "Time Server: Error at select(): " << WSAGetLastError() << endl;
			WSACleanup();
			return;
		}

		for (int i = 0; i < MAX_SOCKETS && nfd > 0; i++)
		{
			if (FD_ISSET(sockets[i].id, &waitRecv))
			{
				nfd--;
				switch (sockets[i].recv)
				{
				case LISTEN:
					acceptConnection(i,sockets);
					break;

				case RECEIVE:
					receiveMessage(i,sockets);
					break;
				}
			}

			if (sockets[i].send == PENDING)
			{
				prepareResponse(i, sockets);
			}
		}


		for (int i = 0; i < MAX_SOCKETS && nfd > 0; i++)
		{
			
			if (FD_ISSET(sockets[i].id, &waitSend))
			{
				nfd--;
				if (sockets[i].send == SEND)
				{

					sendMessage(i,sockets);
				}
			}
		}
	}

	// Closing connections and Winsock.
	//cout << "Time Server: Closing Connection.\n";
	closesocket(listenSocket);
	WSACleanup();
}

//add later 

//void createTempDirectory() {
//	if (!filesystem::exists(ROOT_DIRECTORY)) {
//		if (filesystem::create_directory(ROOT_DIRECTORY)) {
//			cout << "Temporary directory created: " << ROOT_DIRECTORY << endl;
//		}
//		else {
//			cerr << "Failed to create temporary directory!" << endl;
//			exit(EXIT_FAILURE);
//		}
//	}
//	else {
//		cout << "Temporary directory already exists: " << ROOT_DIRECTORY << endl;
//	}
//}


bool addSocket(SOCKET id, int role, SocketState* sockets)
{
	time_t start_time;
	for (int i = 0; i < MAX_SOCKETS; i++)
	{
		if (sockets[i].recv == EMPTY)
		{
			unsigned long flag = 1;
			if (ioctlsocket(id, FIONBIO, &flag) != 0)
			{
				//	cout << "Time Server: Error at ioctlsocket(): " << WSAGetLastError() << endl;
			}
			sockets[i].id = id;
			sockets[i].recv = role;
			sockets[i].send = IDLE;
			sockets[i].buffer;
			sockets[i].lastRecived = time(&start_time);
			socketsCount++;
			return (true);
		}
	}
	return (false);
}

void removeSocket(int index, SocketState* sockets)
{
	sockets[index].recv = EMPTY;
	sockets[index].send = EMPTY;
	socketsCount--;
}

void acceptConnection(int index, SocketState* sockets)
{
	SOCKET id = sockets[index].id;
	struct sockaddr_in from;		// Address of sending partner
	int fromLen = sizeof(from);

	SOCKET msgSocket = accept(id, (struct sockaddr*)&from, &fromLen);
	if (INVALID_SOCKET == msgSocket)
	{
		cout << "Time Server: Error at accept(): " << WSAGetLastError() << endl;
		return;
	}
	

	if (addSocket(msgSocket, RECEIVE, sockets) == false)
	{
		cout << "\t\tToo many connections, dropped!\n";
		closesocket(id);
	}
	return;
}

int parseContentLength(string& buffer,int headersLength)
{
	// Look for the "Content-Length:" header in the first `headerLength` bytes
	size_t start_of_content_length = buffer.find("Content-Length:");
	if (start_of_content_length == std::string::npos) {
		return 0; // "Content-Length:" header not found
	}
	start_of_content_length += strlen("Content-Length: ");

	//skiping un wanted \t or spaces 

	while (start_of_content_length < headersLength &&
		(buffer[start_of_content_length] == ' ' || buffer[start_of_content_length] == '\t')) {
		++start_of_content_length;
	}
	string content_length_str = buffer.substr(start_of_content_length, headersLength);
	int contentLength = stoi(content_length_str);
	if (contentLength < 0)
	{
		return -1;
	}
	// Extract the integer value

	return contentLength;
}


void receiveMessage(int index, SocketState* sockets)
{
	time_t currTime;
	SOCKET msgSocket = sockets[index].id;
	//string request;
	//figure out len of the massege before
	char tempBuffer[INITIAL_BUFFER_SIZE] = { 0 }; // Temporary buffer for reading data
	


	int bytesRecv = recv(msgSocket, tempBuffer, sizeof(tempBuffer), 0);
	tempBuffer[bytesRecv] = '\0';
	sockets[index].lastRecived= time(&currTime);
	if (tempBuffer == "")

		return;
	if (bytesRecv <= 0)
	{
		// שגיאה או חיבור סגור
		closesocket(msgSocket);
		removeSocket(index, sockets);
		return;
	}
	if (SOCKET_ERROR == bytesRecv)
	{
		cout << "http server: error at recv(): " << WSAGetLastError() << endl;
		closesocket(msgSocket);
		removeSocket(index,sockets);
		return;
	}
	
	sockets[index].buffer.append(tempBuffer, bytesRecv);
	HttpRequest* request = nullptr;

	while (!sockets[index].buffer.empty())
	{
		// Step 1: Find the end of headers
		size_t requestLineEnd = sockets[index].buffer.find("\r\n");
		size_t headersEnd = sockets[index].buffer.find("\r\n\r\n");

		if (headersEnd == string::npos || requestLineEnd == string::npos)//npos is not found indicator
		{
			// Headers are incomplete; wait for more data
			return;
		}

		// Step 2: Extract headers and calculate Content-Length
		int requestLineLength = requestLineEnd + RN_ENDING; //Include \r\n
		int headersLength = headersEnd + RNRN_ENDING; // Include \r\n\r\n

		int contentLength = parseContentLength(sockets[index].buffer, headersLength);

		if (contentLength < 0)
		{
			// Invalid Content-Length; close connection
			closesocket(msgSocket);
			removeSocket(index, sockets);
			return;
		}

		// Step 3: Check if the full message (headers + body) is available
		int totalMessageLength = headersLength + contentLength;// mybe need to delete+ RN_ENDING to put \r\n in the end 
		if (sockets[index].buffer.size() < static_cast<size_t>(totalMessageLength))
		{
			// Full message not yet received; wait for more data
			return;
		}

		//check if need to close socket using connection 

		// Step 4: Separate headers and body
		string requestLine = sockets[index].buffer.substr(0, requestLineLength);
		string headers = sockets[index].buffer.substr(requestLineLength, headersLength);
		string body = sockets[index].buffer.substr(headersLength, contentLength);

		// Step 5: Process the message

		if (request == nullptr)
		{
			request = new HttpRequest();
		}

		parseHttpRequest(requestLine, headers, body, request);
		// Step 6: Remove processed message from the buffer
		sockets[index].buffer.erase(0, totalMessageLength);
		break;
	}

	sockets[index].requestStruct = request;

	//change to get post etc 
		//ADD ERROR OR SOMETHING AND IF NOT 
		//adding if sockets[index].requestStruct.headers[connection]= "disconnect" (or other exit word)
		//close the socket using the function closesocket()
	
	if (sockets[index].requestStruct)
	{
		sockets[index].send = PENDING;
	}
	
}

void prepareResponse(int index, SocketState* sockets)
{
	HttpResponse* response = new HttpResponse();
	HttpRequest* request = sockets[index].requestStruct;

	//GET need to check the query then prepre the right html file and send it in the body 
	//check if header is valid or using  NotAllowMethodHandler()
	//check if mendatory headers exists or headers missing code response
	//***if not build to client proper response through cases ***

	if (request->method == "OPTIONS")
	{
		//checking if the request is fine
		OptionsResponsefiller(request,response);
	}
	else if(request->method == "GET")
	{
		//checking if the request is fine
		GETResponsefiller(request,response,false);
	}
	else if (request->method == "HEAD")
	{
		//checking if the request is fine
		HEADResponsefiller(request, response);

	}
	else if (request->method == "POST")
	{
		//checking if the request is fine
		POSTesponsefiller(request, response);
	}
	else if (request->method == "PUT")
	{
		//checking if the request is fine
		PUTesponsefiller(request, response);
	}
	else if (request->method == "DELETE")
	{
		//checking if the request is fine
		DELETEesponsefiller(request, response);
	}
	else if (request->method == "TRACE")
	{
		//checking if the request is fine
		TRACEresponsefiller(request, response);
	}
	else 
	{
		NotAllowMethodHandler(response);
	}

	sockets[index].responseStruct = response;
	if (request->headers["connection"] == "close")
	{
		response->connection_close = true;
	}
	//function to convert to string or something else
	sockets[index].buffer = buildResponse(*response);
	sockets[index].send= SEND; // Move to SEND state

	delete request;
	sockets[index].requestStruct = nullptr;
	//build the message according to httpresponse
	//then shove it to sockets[index].buffer
}

void AddRegularHeaders(HttpResponse* response)
{
	time_t t;
	time(&t);
	tm* utc_time = gmtime(&t);
	string timeFormatted(80, '\0');
	strftime(&timeFormatted[0], timeFormatted.size(), "%a, %d %b %Y %H:%M:%S GMT", utc_time);
	timeFormatted.resize(strlen(timeFormatted.c_str()));
	timeFormatted = string(ctime(&t));
	if (!timeFormatted.empty() && timeFormatted.back() == '\n') {
		timeFormatted.erase(timeFormatted.size() - 1); // Remove the last character if it's '\n'
	}
	response->headers.push_back({ "Server","My HTTP Web Server" });
	response->headers.push_back({ "Date", timeFormatted });
}
void Errorhandaling(HttpResponse* response, const exception& e)
{
	cout << "Error handling request: " << e.what() << endl;
	response->status_code = 500;
	response->status_message = "Internal Server Error";
	response->headers.push_back({ "Content-Type", "text/html" });
	response->body = "<html><body><h1>500 Internal Server Error</h1></body></html>";

}
void OptionsResponsefiller(HttpRequest* request, HttpResponse* response)
{
	fileType contentType = TEXT_HTML;
	string resourcePath(request->path.empty() || request->path == "/" ? DEFAULT_RESOURCE : request->path);
	ifstream file;
	CheckLangValid(request);
	string filePath = ROOT_DIRECTORY + "/" + request->query["lang"] + resourcePath;
	
	if (fs::exists(filePath))
	{
		response->status_code = 200;
		response->status_message = "OK";
		response->headers.push_back({ "Content-Type","text/html" });
		response->headers.push_back({ "Allow-methods", "OPTIONS, GET, HEAD, POST, PUT, DELETE, TRACE" });
		AddRegularHeaders(response);
		response->headers.push_back({ "Content-Length", "0" });
		file.close();
	}
	else
	{
		response->status_code = 204;
		response->status_message = "No Content";
		response->headers.push_back({ "Content-Type","text/html" });
		response->headers.push_back({ "Allow-methods", "OPTIONS, POST, PUT, TRACE" });
		AddRegularHeaders(response);
		response->headers.push_back({ "Content-Length", "0" });
	}
	
	
}

void handleFavicon(HttpResponse* response) {
	response->status_code = 204; // No Content
	response->status_message = "No Content";
	response->headers.push_back({ "Content-Type", "image/x-icon" });
	response->headers.push_back({ "Content-Length", "0" });
	response->body = ""; // אין תוכן להחזיר
}

void GETResponsefiller(HttpRequest* request, HttpResponse* response, bool IFHEAD)
{
	fileType contentType = TEXT_HTML;
	string resourcePath(request->path.empty()|| request->path=="/" ? DEFAULT_RESOURCE : request->path);
	ifstream file;

	//check lang valid if not set to english 
	if (resourcePath == "/favicon.ico") {
		handleFavicon(response);
		return;
	}

	CheckLangValid(request);
	
	string filePath = ROOT_DIRECTORY  +"/"+ request->query["lang"] + resourcePath; // need to create temp directory in c\\//
	// what file type do we need to get from the path
	string desieredContentType = get_content_type(resourcePath, contentType);

	try
	{
		file.open(filePath, isBinaryData(contentType) ? ios::binary : ios::in);

		if (file.is_open() == false)//hendling not exist file
		{
			NotFoundResourceHandler(response);
			return;
		}

		string file_Content(read_file(file));
		size_t file_Length = file_Content.length();

		response->status_code = 200;
		response->status_message = "OK";
		response->headers.push_back({ "Content-Type", desieredContentType });
		AddRegularHeaders(response); // Add Server and Date headers
		response->headers.push_back({ "Content-Length",to_string(file_Length) });
		if (!IFHEAD)
		{
			response->body = file_Content;
		}
	}
	catch (const exception& e) 
	{
		Errorhandaling(response, e);
	}

	file.close();
}

void HEADResponsefiller(HttpRequest* request, HttpResponse* response)
{
	GETResponsefiller(request, response,true);
}

//check posting other things like text etc
void POSTesponsefiller(HttpRequest* request, HttpResponse* response)
{
	fileType contentType = TEXT_HTML;
	const set<string> supportedContentTypes =
	{
		"application/json",
		"text/plain",
		"image/png",
		"image/jpeg"
	};

	if (request->path.empty())
	{
		ErrorResponse(response, 400, "Path cannot be empty.");
		return;
	}
	if (!checkIfPathAllowd(request)) 
	{
		ErrorResponse(response, 403, "Path not allowed.");
		return;
		
	}
	if (request->headers.find("Content-Length") == request->headers.end() || atoi(request->headers["Content-Length"].c_str()) <= 0)
	{
		ErrorResponse(response, 411, "Missing or invalid Content-Length.");
		return;
	}
	if (request->body.empty()) // do data type supported function 
	{
		ErrorResponse(response, 400, "Empty-Content");
		return;
	}
	auto contentTypeHeader = request->headers.find("Content-Type");
	if (contentTypeHeader == request->headers.end())
	{
		ErrorResponse(response, 415, "Missing Content-Type header.");
		return;
	}
	string theRealContentType = contentTypeHeader->second;
	theRealContentType = trim(theRealContentType);
	if (supportedContentTypes.find(theRealContentType) == supportedContentTypes.end())
	{
		ErrorResponse(response, 415, "Unsupported Content-Type: " + contentType);
		return;
	}

	string filePath = ROOT_DIRECTORY + request->path;
	string desieredContentType = get_content_type(request->path, contentType);
	if (!fs::exists(filePath)) 
	{
		ofstream file(filePath, isBinaryData(contentType) ? ios::binary : ios::out);

		if (!file.is_open()) {
			ErrorResponse(response, 500, "Failed to create the file.");
			return;
		}

		// Step 5: Write the request body to the file
		if (theRealContentType == "application/json" || theRealContentType == "text/plain") {

			file << request->body;
		}
		else if (theRealContentType == "image/png" || theRealContentType == "image/jpeg")
		{
			file.write(request->body.data(), request->body.size());
		}

		file.close();

		if (file.fail())
		{
			ErrorResponse(response, 500, "Error while writing to the file.");
			return;
		}

		// Step 6: Respond with 201 Created
		response->status_code = 201;
		response->status_message = "Created";
		response->headers.push_back({ "Content-Type","text/html" });
		AddRegularHeaders(response); // Add Date and Server headers
		response->body = makePostHTMLbody(request->path, request->body);
		size_t body_length = response->body.length();
		response->headers.push_back({ "Content-Length",to_string(body_length) });
		// No body for a 201 response
	}
	else
	{
		ErrorResponse(response, 409, "File already exists at the specified path." + filePath);
		return;
	}

}
bool content_type_validation(const string& file_path)
{
	const set<string> supportedContentTypes =
	{
		"html",
		"htm",
		"txt",
		"json",
		"jpg",
		"jpeg",
		"png",
		"gif"
	};
	string extension = file_path.substr(file_path.find_last_of('.') + 1);
	for (const auto contype : supportedContentTypes)
	{
		if(contype ==extension)
		{
			return true;
		}
	}
	return false;

}
string trim(const string& str)
{
	size_t start = str.find_first_not_of(" \t\r\n");
	if (start == string::npos) return ""; // No content left after trimming
	size_t end = str.find_last_not_of(" \t\r\n");
	return str.substr(start, end - start + 1);
}
bool checkIfPathAllowd(HttpRequest* request)
{
	const set<string> allowedPrefixes = { "/en/", "/fr/", "/he/" };
	size_t path_size = request->path.length();
	// Check if the path starts with any of the allowed prefixes
	for (const auto& prefix : allowedPrefixes)
	{
		if (request->path.find(prefix) == 0 && path_size>prefix.length())// Ensure it matches at the start
		{
			return content_type_validation(request->path);// Path is allowed
		}
	}

	return false; // Path is not allowed
}
//check puting other things like text etc
void PUTesponsefiller(HttpRequest* request, HttpResponse* response)
{
	
	fileType contentType = TEXT_HTML;
	const set<string> supportedContentTypes =
	{
		"application/json",
		"text/plain",
		"image/png",
		"image/jpeg"
	};

	if (request->path.empty())
	{
		ErrorResponse(response, 400, "Path cannot be empty.");
		return;
	}
	if (!checkIfPathAllowd(request))
	{
		ErrorResponse(response, 403, "Path not allowed.");
		return;
	}
	///
	if (request->headers.find("Content-Length") == request->headers.end() || atoi(request->headers["Content-Length"].c_str()) <= 0)
	{
		ErrorResponse(response, 411, "Missing or invalid Content-Length.");
		return;
	}
	if (request->body.empty()) // do data type supported function 
	{
		ErrorResponse(response, 400, "Empty-Content");
		return;
	}
	
	auto contentTypeHeader = request->headers.find("Content-Type");
	if (contentTypeHeader == request->headers.end())
	{
		ErrorResponse(response, 415, "Missing Content-Type header.");
		return;
	}
	//change below to check if its the same type
	string theRealContentType = contentTypeHeader->second;
	theRealContentType = trim(theRealContentType);
	if (supportedContentTypes.find(theRealContentType) == supportedContentTypes.end())
	{
		ErrorResponse(response, 415, "Unsupported Content-Type: " + theRealContentType);
		return;
	}

	string filePath = ROOT_DIRECTORY + request->path;
	string desieredContentType = get_content_type(request->path, contentType);

	if (fs::exists(filePath))
	{
		ofstream file(filePath, isBinaryData(contentType) ? ios::binary : ios::out);
		if (!file.is_open()) {
			ErrorResponse(response, 500, "Failed to create the file.");
			return;
		}

		
		if (theRealContentType == "application/json" || theRealContentType == "text/plain") {

			file << request->body;
		}
		else if (theRealContentType == "image/png" || theRealContentType == "image/jpeg")
		{
			file.write(request->body.data(), request->body.size());
		}

		file.close();

		if (file.fail())
		{
			ErrorResponse(response, 500, "Error while writing to the file.");
			return;
		}
		// Step 6: Respond with 201 Created
		response->status_code = 200;
		response->status_message = "OK";
		response->headers.push_back({ "Content-Type","text/html" });
		AddRegularHeaders(response); // Add Date and Server headers
		response->body = makePUTHTMLbody(request->path, request->body);
		size_t body_length = response->body.length();
		response->headers.push_back({ "Content-Length",to_string(body_length) });
		
	}
	else
	{
		
	 POSTesponsefiller(request, response);
	 return;
		
	}

}

void DELETEesponsefiller(HttpRequest* request, HttpResponse* response)
{
	if (request->path.empty())
	{
		ErrorResponse(response, 400, "Path cannot be empty.");
		return;
	}

	if (!checkIfPathAllowd(request))
	{
		ErrorResponse(response, 403, "Path not allowed.");
		return;
	}

	string filePath = ROOT_DIRECTORY + request->path;

	// Check if the file exists
	if (!fs::exists(filePath))
	{
		ErrorResponse(response, 404, "File not found.");
		return;
	}

	// Try to delete the file
	if (fs::remove(filePath))
	{
		response->status_code = 200;
		response->status_message = "OK";
		response->headers.push_back({ "Content-Type", "text/html" });
		AddRegularHeaders(response);
		response->body = makeDELETEHTMLbody(request->path, request->body);
		size_t body_length = response->body.length();
		response->headers.push_back({ "Content-Length",to_string(body_length) });
		
	}
	else
	{
		ErrorResponse(response, 500, "Failed to delete the file.");
	}
}

void TRACEresponsefiller(HttpRequest* request, HttpResponse* response)
{
		ostringstream echoBody;

		echoBody << request->method << " " << request->path << " " << request->version << "\r\n";

		for (const auto& header : request->headers)
		{
			echoBody << header.first << ": " << header.second << "\r\n";
		}

		// Include an empty line to separate headers from body
		echoBody << "\r\n";

		// Echo back the body if present
		echoBody << request->body;

		string echoContent = echoBody.str();

		response->status_code = 200;
		response->status_message = "OK";
		response->headers.push_back({ "Content-Type", "message/http" });
		AddRegularHeaders(response);
		response->headers.push_back({ "Content-Length", to_string(echoContent.length()) });
		response->body = echoContent;
}

void ErrorResponse(HttpResponse* response, int statusCode, const string& message)
{
	response->status_code = statusCode;
	response->status_message = message;
	response->headers.push_back({ "Content-Type", "application/json" });
	AddRegularHeaders(response); // Add Server and Date headers
	string body = "{ \"error\": { \"code\": " + to_string(statusCode) +
		", \"message\": \"" + message + "\" } }";
	response->headers.push_back({ "Content-Length", to_string(body.length()) });
	response->body = body;
}


void CheckLangValid(HttpRequest* request)
{
	if (!request->query.empty()) {
		if (request->query["lang"] != "fr" && request->query["lang"] != "he")
		{
			request->query["lang"] = "en";
		}
	}
	else
	{
		request->query["lang"] = "en";
	}


}

void NotAllowMethodHandler(HttpResponse* response)
{
	response->status_code = 405;
	response->status_message = "Method Not Allowed";
	response->headers.push_back({ "Content-Type","text/html" });
	response->headers.push_back({ "Allow-methods", "OPTIONS, GET, HEAD, POST, PUT, DELETE, TRACE" });
	response->headers.push_back({ "Connection","close" });
	AddRegularHeaders(response);
	response->headers.push_back({ "Content-Length", "0" });
}
void NotFoundResourceHandler(HttpResponse* response)
{
	response->status_code = 404;
	response->status_message = "Resource Not Found";
	response->headers.push_back({ "Content-Type","text/html" });
	response->headers.push_back({ "Connection","close" });
	AddRegularHeaders(response);
	response->headers.push_back({ "Content-Length", "152" });
	response->body = "<html><head><title>Error!</title></head><body><h1>404 Not Found</h1><p>The request could not be completed because the file not found.</p></body></html>";


}


string makePostHTMLbody(const string& filePath, const string& body)
{
	std::ostringstream htmlBody;
	// Send HTML content to the stream
	htmlBody << "<!DOCTYPE html>\n";
	htmlBody << "<html>\n" << "<head>\n";
	htmlBody << "<title>File Created</title>\n";
	htmlBody << "</head>\n" << "<body>\n";
	htmlBody << "<h1>File Created Successfully</h1>\n";
	htmlBody << "<p>The file has been created successfully at <a>" << filePath << "</a>.</p>\n";
	htmlBody << "<p>The Content in the file is \"<a>" << body << " \"</a>.</p>\n";
	htmlBody << "</body>\n" << "</html>\n";

	return htmlBody.str();
}
string makePUTHTMLbody(const string& filePath, const string& body)
{
	std::ostringstream htmlBody;
	// Send HTML content to the stream
	htmlBody << "<!DOCTYPE html>\n";
	htmlBody << "<html>\n" << "<head>\n";
	htmlBody << "<title>File Changed</title>\n";
	htmlBody << "</head>\n" << "<body>\n";
	htmlBody << "<h1>File Changed Successfully</h1>\n";
	htmlBody << "<p>The file has been changed successfully at <a>" << filePath << "</a>.</p>\n";
	htmlBody << "<p>The Content in the new file is \"<a>" << body << " \"</a>.</p>\n";
	htmlBody << "</body>\n" << "</html>\n";

	return htmlBody.str();
}
string makeDELETEHTMLbody(const string& filePath, const string& body)
{
	std::ostringstream htmlBody;
	// Send HTML content to the stream
	htmlBody << "<!DOCTYPE html>\n";
	htmlBody << "<html>\n" << "<head>\n";
	htmlBody << "<title>File Delted</title>\n";
	htmlBody << "</head>\n" << "<body>\n";
	htmlBody << "<h1>File Delted Successfully</h1>\n";
	htmlBody << "<p>The file has been deleted successfully at <a>" << filePath << "</a>.</p>\n";
	htmlBody << "</body>\n" << "</html>\n";

	return htmlBody.str();
}


void sendMessage(int index, SocketState* sockets)
{
	int bytesSent = 0;
	SOCKET msgSocket = sockets[index].id;
	string response_string_formated = sockets[index].buffer;
	
	bytesSent = send(msgSocket, response_string_formated.c_str(), response_string_formated.length(), 0);

	if (SOCKET_ERROR == bytesSent)
	{
		cout << " Server: Error at send(): " << WSAGetLastError() << endl;
		return;
	}
	cout << " [INFO]    Response sent on socket " << msgSocket << ":\n"
		<< "           - Bytes Sent      : " << bytesSent << "/" << response_string_formated.length() << "\n"
		<< "           - Response Content:\n\n" << response_string_formated << "\n\n" <<  endl;
	
	if (sockets[index].responseStruct->connection_close)
	{
		// שגיאה או חיבור סגור
		closesocket(msgSocket);
		removeSocket(index, sockets);
		return;
	}
	delete sockets[index].responseStruct;
	sockets[index].buffer.clear();
	sockets[index].send = IDLE;
}


