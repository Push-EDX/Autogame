#include <Utils.hpp>
#include <buffio.h>
#include <vector>
#include <sstream>

namespace autogame
{
    std::string html_xml(std::string html) {
        TidyDoc tidyDoc = tidyCreate();
        TidyBuffer tidyOutputBuffer = { 0 };

        bool configSuccess = tidyOptSetBool(tidyDoc, TidyXmlOut, yes)
            && tidyOptSetBool(tidyDoc, TidyQuiet, yes)
            && tidyOptSetBool(tidyDoc, TidyQuoteNbsp, no)
            && tidyOptSetBool(tidyDoc, TidyXmlDecl, yes) //XML declaration on top of the content
            && tidyOptSetBool(tidyDoc, TidyForceOutput, yes)
            && tidyOptSetValue(tidyDoc, TidyInCharEncoding, "utf8") // Output from here should be UTF-8
            && tidyOptSetValue(tidyDoc, TidyOutCharEncoding, "utf8") // Output from CURL is UTF-8
            && tidyOptSetBool(tidyDoc, TidyNumEntities, yes)
            && tidyOptSetBool(tidyDoc, TidyShowWarnings, no)
            && tidyOptSetInt(tidyDoc, TidyDoctypeMode, TidyDoctypeOmit); //Exclude DOCTYPE

        int tidyResponseCode = -1;

        if (configSuccess) {
            std::vector<unsigned char> bytes(html.begin(), html.end());

            TidyBuffer buf;
            tidyBufInit(&buf);

            for (size_t i = 0; i < bytes.size(); i++) {
                tidyBufAppend(&buf, &bytes[i], 1);
            }

            tidyResponseCode = tidyParseBuffer(tidyDoc, &buf);
        }

        if (tidyResponseCode >= 0)
            tidyResponseCode = tidyCleanAndRepair(tidyDoc);

        if (tidyResponseCode >= 0)
            tidyResponseCode = tidySaveBuffer(tidyDoc, &tidyOutputBuffer);

        if (tidyResponseCode < 0) {
            throw ("Tidy encountered an error while parsing an HTML response. Tidy response code: " + tidyResponseCode);
        }

        std::string tidyResult = (char*)tidyOutputBuffer.bp;

        tidyBufFree(&tidyOutputBuffer);
        tidyRelease(tidyDoc);

        return tidyResult;
    }



    tinyxml2::XMLElement* findByID(tinyxml2::XMLElement* root, std::string ID)
    {
        if (!root)
            return NULL;

        if (root->Attribute("id"))
        {
            if (strncmp(root->Attribute("id"), ID.c_str(), ID.length()) == 0)
            {
                return root;
            }
        }

        if (root->FirstChildElement())
        {
            tinyxml2::XMLElement* e = findByID(root->FirstChildElement(), ID);
            if (e)
                return e;
        }

        if (root->NextSiblingElement())
        {
            tinyxml2::XMLElement* e = findByID(root->NextSiblingElement(), ID);
            if (e)
                return e;
        }

        return NULL;
    }

    tinyxml2::XMLElement* findByName(tinyxml2::XMLElement* root, std::string name)
    {
        if (!root)
            return NULL;

        if (root->Attribute("name"))
        {
            if (strncmp(root->Attribute("name"), name.c_str(), name.length()) == 0)
            {
                return root;
            }
        }

        if (root->FirstChildElement())
        {
            tinyxml2::XMLElement* e = findByName(root->FirstChildElement(), name);
            if (e)
                return e;
        }

        if (root->NextSiblingElement())
        {
            tinyxml2::XMLElement* e = findByName(root->NextSiblingElement(), name);
            if (e)
                return e;
        }

        return NULL;
    }

    tinyxml2::XMLElement* findByClass(tinyxml2::XMLElement* root, std::string cls)
    {
        if (!root)
            return NULL;

        const char* attr = root->Attribute("class");
        if (attr)
        {
            std::string _cls(attr);
            if (_cls.find(cls) != std::string::npos)
            {
                std::istringstream iss(_cls);
                std::string s;

                while (std::getline(iss, s, ' '))
                {
                    if (s.compare(cls) == 0)
                    {
                        return root;
                    }
                }
            }
        }

        if (root->FirstChildElement())
        {
            tinyxml2::XMLElement* e = findByClass(root->FirstChildElement(), cls);
            if (e)
                return e->ToElement();
        }

        if (root->NextSiblingElement())
        {
            tinyxml2::XMLElement* e = findByClass(root->NextSiblingElement(), cls);
            if (e)
                return e->ToElement();
        }

        return NULL;
    }

    bool is_digit(int c)
    {
        return c <= '9' && c >= '0';
    }


    bool is_number(const std::string& s)
    {
        std::string::const_iterator it = s.begin();
        while (it != s.end() && is_digit(*it)) ++it;
        return !s.empty() && it == s.end();
    }
}
