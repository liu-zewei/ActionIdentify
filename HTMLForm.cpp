#include "HTMLForm.h"
#include <sstream>

HTMLForm::HTMLForm()
    : post(NULL)
    , last(NULL)
{}

HTMLForm::~HTMLForm()
{
    curl_formfree(post);
}

void HTMLForm::Add(const std::string &name, const std::string &content)
{
    curl_formadd(&post, &last, CURLFORM_COPYNAME, name.c_str(), CURLFORM_COPYCONTENTS, content.c_str(), CURLFORM_END);
}

void HTMLForm::Add(const std::string& name, int content)
{
    std::stringstream stream;
    stream << content;
    curl_formadd(&post, &last, CURLFORM_COPYNAME, name.c_str(), CURLFORM_COPYCONTENTS, stream.str().c_str(), CURLFORM_END);
}
