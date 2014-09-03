#ifndef _AUTOGAME_UTILS_H_
#define _AUTOGAME_UTILS_H_

#include <tinyxml2.h>
#include <string>

namespace autogame
{
    std::string html_xml(std::string html);

    tinyxml2::XMLElement* findByName(tinyxml2::XMLElement* root, std::string name);
    tinyxml2::XMLElement* findByID(tinyxml2::XMLElement* root, std::string ID);
    tinyxml2::XMLElement* findByClass(tinyxml2::XMLElement* root, std::string cls);

    bool is_number(const std::string& s);
}

#endif