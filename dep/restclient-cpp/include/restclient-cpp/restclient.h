/**
 * @file restclient.h
 * @brief libcurl wrapper for REST calls
 * @author Daniel Schauenberg <d@unwiredcouch.com>
 * @version
 * @date 2010-10-11
 */

#ifndef INCLUDE_RESTCLIENT_H_
#define INCLUDE_RESTCLIENT_H_

#include <curl/curl.h>
#include "meta.h"

#include <string>
#include <map>
#include <cstdlib>
#include <algorithm>
#include <functional>
#include <cctype>

class RestClient
{
  public:
    /**
     * public data definitions
     */
    typedef std::map<std::string, std::string> headermap;
    
    /** struct used to hold cookies */
    struct cookie
    {
        std::string name;
        std::string value;
        long expires;
        std::string path;
        std::string domain;

        cookie()
        {
            name = std::string();
            value = std::string();
            expires = 0;
            path = std::string();
            domain = std::string();
        }
    };

    typedef std::map<std::string, cookie> cookiesmap;

    /** response struct for queries */
    typedef struct
    {
      int code;
      std::string body;
      headermap headers;
      cookiesmap cookies;
    } response;

    /** struct used for uploading data */
    typedef struct
    {
      const char* data;
      size_t length;
    } upload_object;

    /** public methods */
    // Auth
    static void clearAuth();
    static void setAuth(const std::string& user,const std::string& password);
    // HTTP GET
    static response get(const std::string& url);
    // HTTP POST
    static response post(const std::string& url, const std::string& ctype,
                         const std::string& data);
    // HTTP PUT
    static response put(const std::string& url, const std::string& ctype,
                        const std::string& data);
    // HTTP DELETE
    static response del(const std::string& url);

    static inline void setCookies(std::string cookies)
    {
        _cookies = cookies;
    }

    static inline void setReferer(std::string referer)
    {
        _referer = referer;
    }

    // trim from start
    static inline std::string &ltrim(std::string &s) {
        s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
        return s;
    }

    // trim from end
    static inline std::string &rtrim(std::string &s) {
        s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
        return s;
    }

    // trim from both ends
    static inline std::string &trim(std::string &s) {
        return ltrim(rtrim(s));
    }

  private:
    // writedata callback function
    static size_t write_callback(void *ptr, size_t size, size_t nmemb,
                                 void *userdata);

    // header callback function
    static size_t header_callback(void *ptr, size_t size, size_t nmemb,
				  void *userdata);
    // read callback function
    static size_t read_callback(void *ptr, size_t size, size_t nmemb,
                                void *userdata);
    static const char* user_agent;
    static std::string user_pass;

    static std::string _referer;
    static std::string _cookies;

};

#endif  // INCLUDE_RESTCLIENT_H_
