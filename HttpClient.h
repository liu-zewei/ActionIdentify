#ifndef HTTPCLIENT_H
#define HTTPCLIENT_H

#include "HTMLForm.h"
#include <string>

class HttpClient
{
public:
    HttpClient(const std::string& host, int port = 80);
    virtual ~HttpClient();

    int Get(const std::string& path, std::string& rlt);
    int Download(const std::string& path, const std::string& fileName);
    int Post(const std::string& path, std::string& rlt, const std::string& fields = "");
    int Post(const std::string& path, std::string& rlt, const HTMLForm &form, const std::string& fields = "");

private:
    std::string host_;
    int port_;
    std::string url_;
};

#endif //HTTPCLIENT_H
