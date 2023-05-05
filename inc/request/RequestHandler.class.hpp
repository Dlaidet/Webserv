/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestHandler.class.hpp                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mdaadoun <mdaadoun@student.42angouleme.    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/04/25 15:58:25 by mdaadoun          #+#    #+#             */
/*   Updated: 2023/05/03 18:18:03 by fleblanc         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HANDLE_REQUEST_HPP
#define HANDLE_REQUEST_HPP

# include <ctime>
# include <cstring>
# include <dirent.h>
# include "parsing/Parsing.class.hpp"
# include "request/Request.class.hpp"
# include "cgi/CgiHandler.class.hpp"

class Request;
class CgiHandler;

class RequestHandler {
public:
    // constructor/destructor
    RequestHandler(Request * req, Config * conf);
    ~RequestHandler(void);

    // setters
    void setStatusCode(int code);
    void setContentType(std::string path);

    // response getters
    int                                 getStatusCode() const;
    std::string                         getStatusCodeString();
    std::string                         getContentType() const;
    std::string                         getBody() const;
    std::string                         getMethod() const;
    std::string                         getRoot() const;
    std::string                         getRequestLocation() const;
    std::string                         getRedirectLocation() const;
    std::string                         getRequestFile() const;
    std::string                         getRequestURI() const;
    std::string                         getCgiInterpreter() const;
    std::map<std::string, std::string>  getQueryString() const;
    std::string                         getServerName() const;

    void                                setBody(std::string newbody);

    // main
    void                                run(void);

    // utils
    bool                                checkIfMethodIsAllowed();
    std::string                         getErrorPagePath();
    void                                parsePOSTBody();

    // Readings
    std::string                         readContent(std::string & path);

    // CGI
    bool                                checkIfCGI(void);

    // Methods
    void                                runGETMethod(void);
    void                                runPOSTMethod(void);
    void                                runDELETEMethod(void);

    m_METHOD                            resolveMethod(std::string & method);

    bool                                checkIfRedirection(void);

    std::string                         directoryListing(void);
    bool                                listDirectory(const char* path, std::ofstream& outfile);

private:
    Request                                                     *_request[1];
    // CONFIG data
    std::string													_auto_index;
    std::string													_index_file;
    std::string													_files_root;
    std::string													_server_name;
    std::map<std::string, std::string>                          _errorPages;
    std::map<std::string, std::string>                          _query_string;
    std::string                                                 _cgi_interpreter;
    std::map<std::basic_string<char>, std::map<std::basic_string<char>, std::basic_string<char> > > _locations;
    std::map<std::basic_string<char>, std::map<std::basic_string<char>, std::basic_string<char> > >	_cgi_list;

    // REQUEST/RESPONSE data
    std::string                                                 _request_IfModifiedSince;
    std::map<int, std::string>                                  _status_code_registry;
    int                                                         _status_code;
    std::string                                                 _request_method;
    std::string                                                 _request_location;
    std::string                                                 _redirect_location;
    std::string                                                 _request_body;
    std::string                                                 _request_body_full;
    std::string                                                 _request_file;
    std::string                                                 _content_type;
    std::string                                                 _body; //body for response
};

#endif
