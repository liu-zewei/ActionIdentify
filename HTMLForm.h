#ifndef HTMLFORM_H
#define HTMLFORM_H

#include <string>
#include "curl/curl.h"
#include "HttpClient.h"

class HTMLForm
{
public:
    HTMLForm();
    ~HTMLForm();

    void Add(const std::string& name, const std::string& content);
    void Add(const std::string& name, int content);
    //void Add(const std::string& name, const std::string& content, const std::string& contentType);

private:
    friend class HttpClient;
    struct curl_httppost* post;
    struct curl_httppost* last;
};

#endif //HTMLFORM_H
