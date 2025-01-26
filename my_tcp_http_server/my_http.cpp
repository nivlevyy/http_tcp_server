#include "my_http.h"


//fix it due to new because we can use stl

int parseHttpRequest(const string& requestLine, const string& headers, const string& body, HttpRequest* request)
{
    // Parse Request Line: "<Method> <Path> <HTTP-Version>"

    istringstream requestLineStream(requestLine);
    if (!(requestLineStream >> request->method >> request->path >> request->version)) {
        return -1; // Invalid request line format
    }

    // Parse Query Parameters (if present)
    size_t queryStart = request->path.find('?');
    if (queryStart != string::npos) {
        string queryStr = request->path.substr(queryStart + 1);
        request->path = request->path.substr(0, queryStart); // Trim path and shove into the path atribute

        istringstream queryStream(queryStr);
        string pair;
        while (getline(queryStream, pair, '&')) {
            size_t eqPos = pair.find('=');
            if (eqPos != string::npos) {
                string key = pair.substr(0, eqPos);
                string value = pair.substr(eqPos + 1);
                request->query[key] = value;
            }
            else {
                request->query[pair] = ""; // Handle keys with no value
            }
        }
    }

    // Parse Headers: "<Header-Name>: <Value>"
    istringstream headersStream(headers);
    string headerLine;
    while (getline(headersStream, headerLine)) {
        size_t colonPos = headerLine.find(':');
        if (colonPos != string::npos) {
            string name = headerLine.substr(0, colonPos);
            string value = headerLine.substr(colonPos + 1);

            // Trim whitespace
            name.erase(0, name.find_first_not_of(" \t"));
            name.erase(name.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);

            request->headers[name] = value;
        }
    }

    // Parse Body
    request->body = body;

    return 0; // Success
}

//after checking everything is alright
//done optionsresponse build

bool isBinaryData(fileType type)
{
    switch (type)
    {
    case TEXT_PLAIN:
    case TEXT_HTML:
    case APPLICATION_JSON:
        return false;
        break;
    case IMAGE_JPEG:
    case IMAGE_PNG:
    case IMAGE_GIF:
        return true;
    case NOT_SUPPORTED:
        return true;
        break;
    default:
        return true;
        break;
    }
}



string get_content_type(const string& file_path, fileType& fType)
{
    string extension = file_path.substr(file_path.find_last_of('.') + 1);
    if (extension == "html" || extension == "htm")
    {
        fType = TEXT_HTML;
        return "text/html";
    }
    else if (extension == "txt")
    {
        fType = TEXT_PLAIN;
        return "text/plain";
    }
    else if (extension == "json")
    {
        fType = APPLICATION_JSON;
        return "application/json";
    }
    else if (extension == "jpg" || extension == "jpeg")
    {
        fType = IMAGE_JPEG;
        return "image/jpeg";
    }
    else if (extension == "png")
    {
        fType = IMAGE_PNG;
        return "image/png";
    }
    else if (extension == "gif")
    {
        fType = IMAGE_GIF;
        return "image/gif";
    }
    else
    {
        fType = NOT_SUPPORTED;
        return "application/octet-stream"; // Default content type for unknown types
    }
}

string read_file(ifstream& file)
{
    stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

string buildResponse(HttpResponse& response)
{

   ostringstream rawResponse;

    // Status line
    rawResponse << response.version << " " << response.status_code << " " << response.status_message << "\r\n";

    // Headers
    for (const auto& header : response.headers)
    {
        rawResponse << header.first << ": " << header.second << "\r\n";
    }

    // Blank line between headers and body
    rawResponse << "\r\n"; //it will become "\r\n""\r\n"

    // Body (if any)
    rawResponse << response.body;

    return rawResponse.str();
}



















//void contentTypeHandler(HttpRequest* requst, fileType& fType)
//{
//    fileType type = NOT_SUPPORTED;
//    auto it = headers.find("Content-Type");
//    if (it != headers.end())
//    {
//        if (it->second == "text/html")
//            type = TEXT_HTML;
//        else if (it->second == "text/plain")
//            type = TEXT_PLAIN;
//        else if (it->second == "application/json")
//            type = APPLICATION_JSON;
//        else if (it->second == "image/jpeg")
//            type = IMAGE_JPEG;
//        else if (it->second == "image/png")
//            type = IMAGE_PNG;
//        else if (it->second == "image/gif")
//            type = IMAGE_GIF;
//        else
//            type = NOT_SUPPORTED;
//    }
//    fType = type;
//}