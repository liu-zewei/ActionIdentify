#include "HttpClient.h"
#include "curl/curl.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdio.h>

static size_t WriteData(void* ptr, size_t size, size_t nmemb, void* stream)
{
    long totalSize = size * nmemb;
    std::string *pStr = static_cast<std::string *>(stream);
    if( pStr ){
        pStr->append((char *)ptr, ((char *)ptr) + totalSize);
    }
    return totalSize;
}

static size_t DownloadCallback(void* ptr, size_t size, size_t nmemb, void* stream)
{
    long totalSize = size *nmemb;
    std::ofstream* pOfs = static_cast<std::ofstream *>(stream);
    if( pOfs ){
        pOfs->write(ptr, totalSize);
    }
    return totalSize;
}

static size_t ReadData(void* ptr, size_t size, size_t nmemb, void* stream)
{
    //TODO
}

HttpClient::HttpClient(const std::string &host, int port)
    : host_(host)
    , port_(port)
    , url_("http://")
{
    url_ += host_;
    if ( port_ != 80 ){
        std::stringstream stream;
        stream << ":" << port << "/";
        url_ += stream.str();
    }
}

HttpClient::~HttpClient()
{
}

int HttpClient::Get(const std::string& path, std::string& rlt)
{
    CURL *curl_handle;
    CURLcode res;
    if ( path.empty() ) { return 1; }
    std::string url = url_ + path;

    curl_handle = curl_easy_init();
    if ( !curl_handle ){ return 1; }

    curl_easy_setopt(curl_handle, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteData);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &rlt);
    res = curl_easy_perform(curl_handle);
    if ( res != CURLE_OK ){
        //"Curl Perform Failed!"
    }
    curl_easy_cleanup(curl_handle);
    return (int)res;
}

int HttpClient::Download(const std::string& path, const std::string& fileName)
{
    CURL *curl_handle;
    CURLcode res;
    if ( path.empty() || fileName.empty() ) { return 1; }
    std::string url = url_ + path;

    std::ofstream ofs;
    ofs.open(fileName, std::ofstream::out | std::ofstream::trunc);
    if (!ofs.is_open()){ return ; }

    curl_handle = curl_easy_init();
    if ( !curl_handle ){ return 1; }

    curl_easy_setopt(curl_handle, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, DownloadCallback);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &ofs);
    res = curl_easy_perform(curl_handle);
    if ( res != CURLE_OK ){
        //"Curl Perform Failed!"
    }
    curl_easy_cleanup(curl_handle);
    ofs.close();
    return (int)res;
}

int HttpClient::Post(const std::string& path, std::string& rlt, const std::string& fields)
{
    CURL *curl_handle;
    CURLcode res;
    if ( path.empty() ) { return 1; }
    std::string url = url_ + path;

    curl_handle = curl_easy_init();
    if ( !curl_handle ){ return 1; }

    curl_easy_setopt(curl_handle, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, fields.c_str());
    curl_easy_setopt(curl_handle, CURLOPT_POST, 1L);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteData);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &rlt);
    res = curl_easy_perform(curl_handle);
    if ( res != CURLE_OK ){
        //"Curl Perform Failed!"
    }
    curl_easy_cleanup(curl_handle);
    return (int)res;
}

int HttpClient::Post(const std::string& path, std::string& rlt, const HTMLForm &form, const std::string& fields)
{
    CURL *curl_handle;
    CURLcode res;
    if ( path.empty() ) { return 1; }
    std::string url = url_ + path;

    curl_handle = curl_easy_init();
    if ( !curl_handle ){ return 1; }

    curl_easy_setopt(curl_handle, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, fields.c_str());
    curl_easy_setopt(curl_handle, CURLOPT_HTTPPOST, form.post);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteData);
    curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, &rlt);
    res = curl_easy_perform(curl_handle);
    if ( res != CURLE_OK ){
        //"Curl Perform Failed!"
    }
    curl_easy_cleanup(curl_handle);
    return (int)res;
}
