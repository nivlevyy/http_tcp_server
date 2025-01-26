# http_tcp_server
made by:niv levy

Custom HTTP 1.1 Web Server:
This project is a custom implementation of an HTTP/1.1 web server built from scratch in C++.
It demonstrates how to process HTTP requests and responses, manage multiple client connections, and serve web content dynamically.
The server is designed to support multiple HTTP methods, including GET, POST, PUT, DELETE, OPTIONS, HEAD, and TRACE.

Key Features:

Protocol Support:
Full support for the HTTP/1.1 protocol with proper parsing of request lines, headers, and bodies.
Correct construction of HTTP responses with status codes, headers, and content.
Dynamic File Serving:

Serves HTML, plain text, JSON, and various image formats.
Validates file paths and types to ensure secure and accurate content delivery.
Multi-Client Handling:

Utilizes the select function for efficient I/O multiplexing, allowing the server to handle multiple simultaneous connections.
Content Management:

Handles operations like file creation, updating, and deletion using POST, PUT, and DELETE methods.
Supports query parameters for language-based content serving (e.g., English, French, Hebrew).

Error Handling:

Responds with appropriate HTTP status codes and detailed error messages for invalid requests or server errors.

Customizable Responses:
Returns HTML responses for various operations like file creation, updates, and deletions, with dynamically generated messages.

Files in the Project:
my_http.h: Defines core data structures and utility functions for handling HTTP requests and responses​.
my_http.cpp: Contains the implementation of functions for parsing HTTP requests, generating responses, and managing content types​.
server.cpp: Implements the main server logic, including connection management, request processing, and response handling using Winsock​.
page.html: A simple web page available in three languages (English, French, Hebrew) showcasing the server's ability to serve localized content​.

Usage:
This server is a demonstration of basic web server capabilities, ideal for learning about network programming and HTTP protocol fundamentals.
It can be extended to support more headers, file types, and advanced features like persistent connections or HTTPS.

Note:
there is a test json file for a postman text collection for your convenience, the file name is http_server_test.postman_collection.json
The server require run in C++17 at least.
my_http.h and my_http.cpp files belong to my_tcp_http_server.sln and server.cpp code.

there are in the submit 3 folders en, he and fr.
You should copy the folders to C:/temp with all the folders content.
(C:/temp/en, C:/temp/he, C:/temp/fr) with page.html inside each folder.
