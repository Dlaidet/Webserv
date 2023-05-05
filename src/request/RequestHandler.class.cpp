/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandler.class.cpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tlafont <tlafont@student.42angouleme.      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/25 15:58:19 by tlafont           #+#    #+#             */
/*   Updated: 2023/05/03 18:18:00 by fleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "request/RequestHandler.class.hpp"

/*
*  @brief   Set request location and requested file from header body.
*  @param   void
*  @return  void
*/
void RequestHandler::parsePOSTBody() {
    if (resolveMethod(this->_request_method) &&
        !this->_request_body.empty()) {
        std::string		path;
        std::string		body;
        std::size_t		position;
        std::size_t		start;
        std::size_t		end;
        std::size_t		size_n;
        std::size_t		last_pos;

        body = this->_request_body;
        position = body.find("name=\"");
        start = position + 6;
        end = body.find(";", position) - 1;
        size_n = end - start;
        path = body.substr(start, size_n);
        last_pos = body.substr(start, size_n).find_last_of("/");
        this->_request_location = path.substr(0, last_pos + 1);
        this->_request_file = path.substr(last_pos + 1);
    }
}

/*
*  @brief   Main constructor. Join the request and the config together.
*  @param   void
*  @return  void
*/
RequestHandler::RequestHandler(Request * req, Config *conf)
{
	this->_request[0] = req;
	this->_status_code = req->getStatus();
	this->_request_method = req->getMethod();
	this->_request_location = req->getUri().first;
	this->_request_file = req->getUri().second;
	this->_query_string = req->getCgi();
	this->_request_body = req->getBody();
  this->parsePOSTBody();
	this->_files_root = conf->getRoot();
	this->_index_file = conf->getIndex();
	this->_locations = conf->getLocations();
	this->_cgi_list = conf->getCgi();
	this->_errorPages = conf->getErrorPages();
	this->_auto_index = conf->getAutoIndex();
	this->_server_name = conf->getServerName();
}

/*
*  @brief   Destructor.
*			Destroy all member objects
*  @param   void
*  @return  void
*/
RequestHandler::~RequestHandler()
{}

/*
*  @brief   Main file reader.
*			Return the content of any file.
*  @param   void
*  @return  std::string file content
*/
std::string RequestHandler::readContent(std::string & path)
{
	std::ofstream		file;
	std::stringstream	buffer;

	file.open(path.c_str(), std::ifstream::in);
	if (file.is_open() == false)
		this->setStatusCode(404);
	else
		buffer << file.rdbuf();
	file.close();
	
	return buffer.str();
}

/*
*  @brief   Set the Mime content type for the response
*  @param   std::string path, the file path
*  @return  void
*/
void RequestHandler::setContentType(std::string path)
{
	std::string type = path.substr(path.rfind(".") + 1, path.size() - path.rfind("."));
	if (type == "html")
		_content_type = "text/html";
	else if (type == "css")
		_content_type = "text/css";
	else if (type == "js")
		_content_type = "text/javascript";
	else if (type == "svg" || type == "xml")
		_content_type = "image/svg+xml";
	else if (type == "jpeg" || type == "jpg")
		_content_type = "image/jpeg";
	else if (type == "png")
		_content_type = "image/png";
	else if (type == "bmp")
		_content_type = "image/bmp";
	else if (type == "ico")
		_content_type = "image/x-icon";
	else
		_content_type = "text/plain";
}

/*
*  @brief   get the corresponding file of the status code
*			if the file doesn't open, fall back on default error.html
*  @param   void
*  @return  std::string file path
*/
std::string RequestHandler::getErrorPagePath()
{
	std::ofstream		file;
	// Set the path for page error
   	std::string path = this->_files_root + "/";
	std::map<std::string, std::string>::iterator it;
	
	// Add error page name depend on status code
	for (it = _errorPages.begin(); it != _errorPages.end(); ++it)
	{
		int error = std::atoi(it->first.substr(it->first.size() - 3).c_str());
		if (error == this->getStatusCode())
			path += it->second;
	}
	// Open Error file
	file.open(path.c_str(), std::ifstream::in);
	// in case of fail set Default error page
	if (file.is_open() == false) 
		path = this->_files_root + "/error.html";
	file.close();
	return path;
}

/*
*  @brief   run the get method
*			check first if the path is root / to get the index page
*			check then what it the content type (html, txt, binary)
*				this->setContentType(path);
*			if the content is html, read the file content with readHTML(path);
*			check if there is an error code and get the corresponding page.
*				4**.html or 5**.html file, fall back on the error.html page if problem.
*			empty the body if 304 Not modified.
*  @param   void
*  @return  void
*/
void RequestHandler::runGETMethod()
{
	std::string		 path;

	if (this->getRequestURI().find('.') == std::string::npos &&
        this->getRequestURI()[this->getRequestURI().size() - 1] == '/')
		path = this->_files_root + this->getRequestURI() + this->_index_file;
	else
		path = this->_files_root + this->getRequestURI();
	this->setContentType(path);
	this->_body = readContent(path);
}

/*
*  @brief   run the delete method, which delete the file requested
*  @param   void
*  @return  void
*/
void RequestHandler::runDELETEMethod()
{
	std::ofstream		file;
	std::string path = this->_files_root + this->getRequestURI();
	file.open(path.c_str(), std::ifstream::in);
	if (file.is_open())
	{
		file.close();
		if (std::remove(path.c_str()) == 0)
			this->setStatusCode(202);
		else
			this->setStatusCode(403);
	}
	else
	{
		file.close();
		this->setStatusCode(403);
	}
}


/*
*  @brief   run the POST method if not a form for cgi, upload a file.
*  @param   void
*  @return  void
*/
void RequestHandler::runPOSTMethod()
{
	std::string		path;
	std::string		body;
	std::ofstream	output;
	std::size_t		start;
	std::size_t		size_n;

	body = this->_request_body;
    path = this->getRoot() + this->_request_location + this->_request_file;

    start = body.find("\r\n\r\n");
    body = body.substr(start + 4);

    size_n = body.rfind("\r\n-");
    body = body.substr(0, size_n);
    output.open(path.c_str(), std::ofstream::binary);
    if (output.fail())
        this->setStatusCode(500);
    else
    {
        this->setStatusCode(202);
        this->setContentType(path);
        output.write(body.c_str(), body.size());
        output.close();
    }
}

/*
*  @brief : This function checks whether a given HTTP request method is allowed or not.
*  @param allowed_methods : A string containing the allowed HTTP methods separated by a space.
*  @param request_method : The HTTP method of the current request being processed.
*  @return  std::string location
*  @return True if the request method is allowed, False otherwise.
*/
bool checkIfMethod(std::string allowed_methods, std::string request_method)
{
	// Convert the allowed_methods string into a vector of strings
	std::stringstream ss(allowed_methods);
	std::vector<std::string> methods;
	std::string method;
	while (std::getline(ss, method, ' '))
		methods.push_back(method);
	// Check if the request method is present in the vector of allowed methods
	for (std::vector<std::string>::size_type i = 0; i < methods.size(); ++i)
		if (methods[i] == request_method)
			return true;
	return false;
}

/*
*  @brief   check if the RequestURI is redirected, the conf for each location will check if there is a rewrite option.
*  @param   void
*  @return  bool, true if method is allowed, else false
*/
bool RequestHandler::checkIfRedirection(void)
{
	// Define iterators for the outer and inner maps
	std::map<std::basic_string<char>, std::map<std::basic_string<char>, std::basic_string<char> > >::const_iterator it;
	std::map<std::basic_string<char>, std::basic_string<char> >::const_iterator in_it;

	// Iterate through each entry in the outer map
	for (it = _locations.begin(); it != _locations.end(); ++it)
	{
		// Iterate through each entry in the inner map
		for (in_it = it->second.begin(); in_it != it->second.end(); ++in_it)
		{
			// Check if the request URI contains the outer map's key and the inner map's key is "rewrite"
			if ((this->getRequestURI().find(it->first) != std::string::npos) && in_it->first == "rewrite")
			{
				// If the inner map's value can be split into two strings separated by a space...
				std::istringstream iss(in_it->second);
				std::string str1, str2;
				if (std::getline(iss, str1, ' ') && std::getline(iss, str2, ' '))
				{
					// ...and the first string is empty or the second string is not empty...
					if (str1.empty() || !str2.empty())
						// ...then return false.
						return false;
				}
				// Set the redirect location to the first string from the inner map's value and return true
				this->_redirect_location = str1;
				return true;
			}
		}
	}
	// If no redirection is found, return false
	return false;
}

/*
*  @brief   get route and check with config locations method permissions
*			loc string is used to remove the trailing / in path to fix the checking
*  @param   void
*  @return  bool, true if method is allowed, else false
*/
bool RequestHandler::checkIfMethodIsAllowed()
{
	// Define iterators for the outer and inner maps of the _locations map
	std::map<std::basic_string<char>, std::map<std::basic_string<char>, std::basic_string<char> > >::const_iterator it;
	std::map<std::basic_string<char>, std::basic_string<char> >::const_iterator inner_it;

	// Iterate through each entry in the outer map of the _locations map
	for(it = _locations.begin(); it != _locations.end(); ++it)
	{
		// Get the location requested by the client and remove trailing slash if it exists
		std::string loc = this->getRequestLocation();
		if (!loc.empty() && loc[loc.length() - 1] == '/') 
			loc.erase(loc.length() - 1);
		// Check if the requested location matches the current outer map key
		if (this->getRequestLocation() == it->first || loc == it->first)
		{
			// If a match is found, iterate through the inner map of the current outer map entry
			for(inner_it = it->second.begin(); inner_it != it->second.end(); ++inner_it)
			{
				// Check if the current inner map key is "allow_methods"
				if (inner_it->first == "allow_methods")
					// If it is, check if the requested HTTP method is allowed
					if (checkIfMethod(inner_it->second, this->getMethod()))
						// If the method is allowed, return true
						return true;
			}
		}
	}
	// If no match was found, return false
	return false;
}


/*
*  @brief   This function checks whether a given request file is a CGI script based on its file extension
*			and the CGI configuration stored in the _cgi_list member variable.
*  @param   void
*  @return  bool after check
*/
bool	RequestHandler::checkIfCGI()
{
	// Get an iterator to loop through the CGI configurations stored in the _cgi_list member variable.
	std::map<std::string, std::map<std::string, std::string> >::iterator it;
	// Get the file path of the requested resource.
	std::string file = this->_request_file;
	// Check if the file path is not empty.
	if (file.size())
	{
		// Find the position of the last occurrence of the '.' character in the file path.
		size_t	dot = file.rfind(".");
		// Check if a '.' character was found.
		if (dot != file.npos)
		{
			// Get the file extension of the requested resource.
			std::string type = file.substr(dot, file.size() - dot);
			// Loop through each CGI configuration stored in the _cgi_list member variable.
			for (it = _cgi_list.begin(); it != _cgi_list.end(); ++it)
			{
				// Get the key of the current CGI configuration.
				std::string key = it->first;
				// Get an iterator to loop through the key-value pairs of the current CGI configuration.
				std::map<std::string, std::string>::iterator inner_it;
				// Initialize a boolean variable to keep track of whether the current configuration is a CGI script.
				bool	cgi = false;
				// Loop through each key-value pair of the current CGI configuration.
				for (inner_it = it->second.begin(); inner_it != it->second.end(); ++inner_it)
				{
					// If the current key is "file" and its value matches the file extension of the requested resource,
					// mark the current configuration as a CGI script.
					if (inner_it->first == "file" && inner_it->second == type)
						cgi = true;
				}
				// If the current configuration is a CGI script, check if it applies to the requested resource.
				if (cgi) 
				{
					// Loop through each key-value pair of the current CGI configuration again.
					for (inner_it = it->second.begin(); inner_it != it->second.end(); ++inner_it) 
					{
						// If the current key is "directory", check if its value matches the location of the requested resource.
						if (inner_it->first == "directory") 
						{
							std::string test = this->_request_location;
							// Remove the leading '/' character from the requested location if it exists.
							if (test.size() > 2 && test.at(0) == '/') 
								test.erase(0, 1);
							// Remove the trailing '/' character from the requested location if it exists.
							if (test.size() > 2 && test.at(test.size() - 1) == '/') 
								test.erase(test.length() - 1, 1);
							// If the requested location does not match the value of the "directory" key,
							// return false indicating that this CGI configuration does not apply to the requested resource.
							if (test != inner_it->second)
								return false;
						// If the current key is "interpreter", set the _cgi_interpreter member variable to its value.
						}
						else if (inner_it->first == "interpreter")
							this->_cgi_interpreter = inner_it->second;
					}
					// If the current configuration applies to the requested resource, return true indicating that
                    // If the file exist, try to open it, if it exist, return true
                    std::ofstream		fs;
                    std::string path = this->_files_root + this->_request_location + this->_request_file;
                    fs.open(path.c_str(), std::ifstream::in);
                    if (fs.is_open()) {
                        fs.close();
                        return true;
                    }
				}
			}
		}
	}
	return false;
}

/*
*  @brief List the files in a directory and write them to an output file stream
*  @param   void
*  @return  void
*/
bool RequestHandler::listDirectory(const char* path, std::ofstream& outfile)
{
	DIR* dir = opendir(path);
	if (dir)
	{
		struct dirent* entry;
		while ((entry = readdir(dir)) != NULL)
		{
			std::string name(entry->d_name);
			if (name != "." && name != ".." && name != ".directory_listing.html")
				outfile << "<li>" << name << "</li>" << std::endl;
		}
		closedir(dir);
		return true;
	}
	return false;
}

/*
*  @brief Generate an HTML page that lists the contents of a directory
*  @param   void
*  @return  void
*/
std::string RequestHandler::directoryListing(void)
{
	std::string filepath = this->_files_root + "/.directory_listing.html";
	std::ofstream outfile(filepath.c_str());
	if (outfile.is_open())
	{
		outfile << "<html><head><title>Directory Listing</title></head><body>" << std::endl;
		outfile << "<h1>Directory Listing</h1>" << std::endl;
		outfile << "<ul>" << std::endl;
		std::string requested_path = this->_files_root + this->getRequestURI();
		bool valid = this->listDirectory(requested_path.c_str(), outfile);
		outfile << "</ul>" << std::endl;
		outfile << "</body></html>" << std::endl;
		outfile.close();
		if (valid)
			this->_status_code = 200;
	}
	return filepath;
}

/*
*  @brief	run the request method to each possibilities
*			check first if the method is allowed to the given location
*			check if it is a cgi script
*			deal with static files
*  @param   void
*  @return  void
*/
void RequestHandler::run(void)
{
	if (this->getStatusCode() >= 200 && this->getStatusCode() < 300)
	{
		if (this->checkIfMethodIsAllowed())
		{
			if (this->checkIfCGI())
			{
				CgiHandler *cgi = new CgiHandler(*this);
				this->_request[0]->setCgiHandler(*cgi);
				cgi->executeCgi();
			}
			else
			{
				switch (RequestHandler::resolveMethod(this->_request_method))
				{
					case GET:
						this->runGETMethod();
						break;
					case POST:
						if (!this->_request_body.empty())
							this->runPOSTMethod();
						break;
					case DELETE:
						this->runDELETEMethod();
						break;
					default:
						break;
				}
			}
		}
		else
			this->setStatusCode(403);
	}

	if (this->getStatusCode() != 200 && this->getStatusCode() != 202)
	{
		std::string path;
		// Checks before to generate a directory listing
		if (this->checkIfMethodIsAllowed() &&
			this->getStatusCode() != 400 &&
			RequestHandler::resolveMethod(this->_request_method) == GET &&
			this->_auto_index == "on" &&
			this->getRequestFile().empty())
		{
				path = this->directoryListing();
		}
		// Return the path to the error page
		if (this->getStatusCode() != 200 && this->getStatusCode() != 202)
			path = getErrorPagePath();
		this->setContentType(path);
		this->_body = readContent(path);
	}

	if (this->checkIfRedirection())
	{
		this->runGETMethod();
		this->setStatusCode(302);
	}
}

/*
*  @brief   convert a string to the method code
*  @param   std::string &met, the method as a string
*  @return  m_METHOD, the correct code.
*/
m_METHOD RequestHandler::resolveMethod(std::string &met)
{
	if (met == "GET")
		return GET;
	else if (met == "HEAD")
		return HEAD;
	else if (met == "POST")
		return POST;
	else if (met == "DELETE")
		return DELETE;
	else
		return ERROR;
}

/*
*  @brief return the location requested as a string
*  @param   void
*  @return  std::string
*/
std::string RequestHandler::getRequestLocation() const
{
	return this->_request_location;
}

/*
*  @brief   return the path of the redirection as a string
*  @param   void
*  @return  std::string
*/
std::string RequestHandler::getRedirectLocation(void) const
{
	return this->_redirect_location;
}

/*
*  @brief   return the server name
*  @param   void
*  @return  std::string
*/
std::string RequestHandler::getServerName(void) const
{
	return this->_server_name;
}

/*
*  @brief   return the request file as a string
*  @param   void
*  @return  std::string
*/
std::string RequestHandler::getRequestFile(void) const
{
	return this->_request_file;
}

/*
*  @brief return the root path of the server files
*  @param   void
*  @return  std::string
*/
std::string RequestHandler::getRoot(void) const
{
	return this->_files_root;
}

/*
*  @brief return the query string as a map key:value
*  @param   void
*  @return  std::map<std::string, std::string>
*/
std::map<std::string, std::string> RequestHandler::getQueryString(void) const
{
	return this->_query_string;
}

/*
*  @brief return the requested method as string
*  @param   void
*  @return  std::string
*/
std::string RequestHandler::getMethod(void) const
{
	return this->_request_method;
}

/*
*  @brief return the uri requested as string
*  @param   void
*  @return  std::string
*/
std::string RequestHandler::getRequestURI(void) const
{
	return this->_request_location + this->_request_file;
}

/*
*  @brief change the status code
*  @param   int, the new status code
*  @return  void
*/
void RequestHandler::setStatusCode(int sc)
{
	this->_status_code = sc;
}

/*
*  @brief return the status code
*  @param   void
*  @return  int
*/
int RequestHandler::getStatusCode(void) const
{
	return this->_status_code;
}

/*
*  @brief return the path of the cgi interpreter
*  @param   void
*  @return  std::string
*/
std::string RequestHandler::getCgiInterpreter(void) const
{
	return this->_cgi_interpreter;
}

/*
*  @brief return the status code and its message as a string
*  @param   void
*  @return  std::string
*/
std::string RequestHandler::getStatusCodeString(void)
{
	this->_status_code_registry[100] = "100 Continue";
	this->_status_code_registry[101] = "101 Switching Protocols";
	this->_status_code_registry[200] = "200 OK";
	this->_status_code_registry[201] = "201 Created";
	this->_status_code_registry[202] = "202 Accepted";
	this->_status_code_registry[203] = "203 Non-Authoritative Information";
	this->_status_code_registry[204] = "204 No Content";
	this->_status_code_registry[205] = "205 Reset Content";
	this->_status_code_registry[206] = "206 Partial Content";
	this->_status_code_registry[300] = "300 Multiple Choices";
	this->_status_code_registry[301] = "301 Moved Permanently";
	this->_status_code_registry[302] = "302 Found";
	this->_status_code_registry[303] = "303 See Other";
	this->_status_code_registry[304] = "304 Not Modified";
	this->_status_code_registry[305] = "305 Use Proxy";
	this->_status_code_registry[307] = "307 Temporary Redirect";
	this->_status_code_registry[400] = "400 Bad Request";
	this->_status_code_registry[401] = "401 Unauthorized";
	this->_status_code_registry[402] = "402 Payment Required";
	this->_status_code_registry[403] = "403 Forbidden";
	this->_status_code_registry[404] = "404 Not Found";
	this->_status_code_registry[405] = "405 Method Not Allowed";
	this->_status_code_registry[406] = "406 Not Acceptable";
	this->_status_code_registry[407] = "407 Proxy Authentication Required";
	this->_status_code_registry[408] = "408 Request Time-out";
	this->_status_code_registry[409] = "409 Conflict";
	this->_status_code_registry[410] = "410 Gone";
	this->_status_code_registry[411] = "411 Length Required";
	this->_status_code_registry[412] = "412 Precondition Failed";
	this->_status_code_registry[413] = "413 Request Entity Too Large";
	this->_status_code_registry[414] = "414 Request-URI Too Large";
	this->_status_code_registry[415] = "415 Unsupported Media Type";
	this->_status_code_registry[416] = "416 Requested range not satisfiable";
	this->_status_code_registry[417] = "417 Expectation Failed";
	this->_status_code_registry[500] = "500 Internal Server Error";
	this->_status_code_registry[501] = "501 Not Implemented";
	this->_status_code_registry[502] = "502 Bad Gateway";
	this->_status_code_registry[503] = "503 Service Unavailable";
	this->_status_code_registry[504] = "504 Gateway Time-out";
	this->_status_code_registry[505] = "505 HTTP Version not supported";

	std::string sc = this->_status_code_registry[this->_status_code];
	return (sc);
}

/*
*  @brief return content type
*  @param   void
*  @return  std::string
*/
std::string RequestHandler::getContentType(void) const
{
	return this->_content_type;
}

/*
*  @brief 	return body
*  @param   void
*  @return  std::string
*/
std::string RequestHandler::getBody(void) const
{
	return this->_body;
}

void RequestHandler::setBody(std::string newbody)
{
	this->_body = newbody;
}
