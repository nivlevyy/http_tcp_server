# http_tcp_server
<h1 align="center">
  <a><img src="https://github.com/nivlevyy/http_tcp_server/blob/main/image/server_logo.png" width="300"></a>
<br>
## Custom HTTP 1.1 Web Server
<br>
</h1>

A custom implementation of an HTTP/1.1 web server built from scratch in C++. This project showcases HTTP protocol handling, multi-client management, and dynamic content serving.

---

## Key Features

### Protocol Support
- Full HTTP/1.1 support with proper parsing of request lines, headers, and bodies.
- Construction of HTTP responses with status codes, headers, and dynamic content.

### Dynamic File Serving
- Serves HTML, plain text, JSON, and image formats.
- Validates file paths and content types for secure delivery.

### Multi-Client Handling
- Utilizes the `select` function for efficient I/O multiplexing.
- Handles multiple simultaneous client connections.

### Content Management
- **File Creation (`POST`)**: Creates a new file at the specified path with the provided content. The `Content-Type` and `Content-Length` headers are validated to ensure the data's integrity and format.
  - If the file does not exist, it is created, and a `201 Created` status is returned.
  - Example: Uploading a JSON file or plain text to the server.

- **File Updates (`PUT`)**: Updates an existing file or creates it if it doesn't exist. Ensures the provided data matches the expected content type.
  - Example: Overwriting an image or text file with new data.
  - Returns `200 OK` for successful updates or creates the file with `201 Created` if it did not exist.

- **File Deletion (`DELETE`)**: Deletes the specified file if it exists. Validates the file path to ensure it is within the allowed directory structure.
  - Example: Removing a specific resource from the server.
  - Returns `200 OK` for successful deletions or `404 Not Found` if the file does not exist.

- **Language-Based Content (`GET`)**: Serves localized files based on the `lang` query parameter (e.g., `en`, `fr`, `he`). If the language is not specified or invalid, defaults to English.
  - Example: Fetching localized versions of `page.html`.

- **Other Methods**:
  - **`HEAD`**: Retrieves the headers of a resource without the body content.
  - **`OPTIONS`**: Returns supported HTTP methods for a given resource.
  - **`TRACE`**: Echoes the received request for debugging purposes.

### Error Handling
- Responds with appropriate HTTP status codes and detailed error messages for invalid requests.

### Customizable Responses
- Generates HTML responses dynamically for operations like file creation, updates, and deletions.

---

## Project Structure

- **`my_http.h`**: Defines core data structures and utility functions for HTTP request and response handling.
- **`my_http.cpp`**: Implements functions for parsing HTTP requests, generating responses, and content type management.
- **`server.cpp`**: Main server logic, including connection management, request processing, and response handling using Winsock.
- **`page.html`**: Sample web page available in English, French, and Hebrew, demonstrating server localization capabilities.

---

## Getting Started

1. **Setup the Content Directory**:
   Copy the provided language folders (`en`, `he`, `fr`) with their respective `page.html` files to `C:/temp`:
   ```
   C:/temp/en/page.html
   C:/temp/he/page.html
   C:/temp/fr/page.html
   ```

2. **Clone the Repository**:
   ```bash
   git clone https://github.com/nivlevyy/http_tcp_server.git
   ```

3. **Compile the Server**:
   Ensure you have a C++ compiler (e.g., GCC, MSVC) that supports C++17 or higher:
   ```bash
   g++ -o server server.cpp my_http.cpp -lws2_32
   ```

4. **Run the Server**:
   Execute the compiled program to start the server on port 80:
   ```bash
   ./server
   ```

5. **Test the Server**:
   Use Postman with the provided `http_server_test.postman_collection.json` to test HTTP methods.

---

## Usage

This server is ideal for learning HTTP fundamentals, network programming, and multi-client management. It can be extended to support advanced features like persistent connections or HTTPS.

---

## Notes

- Requires at least C++17.
- Language folders (`en`, `he`, `fr`) must be placed in `C:/temp`.
- A Postman test collection (`http_server_test.postman_collection.json`) is included for convenience.

---

## Author

- **Niv Levy**

