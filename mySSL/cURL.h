#pragma once

#include <regex>
#include <Windows.h>

typedef enum class Curl { // You can find this by going to the cURL source. I removed the unneeded ones though.
    CURLE_OK = 0,
} Curl;

const char* HostLink = "localhost:3000";

typedef void CURL;

auto (*curl_original)(CURL* cURL, uintptr_t opt, ...)->Curl;

Curl CurlDetour(CURL* Curl, uintptr_t opt, va_list info)
{
    switch (opt)
    {
    case 10117: // PROXY
        return curl_original(Curl, opt, "");
        break;

    case 10064: // Where we acutally "SSL Bypass".
        return curl_original(Curl, opt, 0);
        break;
    case 10081: // Verify Host
        return curl_original(Curl, opt, 0);
        break;
    case 10230: // PINNEDPUBLICKEY
        return Curl::CURLE_OK;
        break;
    case 10002: // URL
        std::string url = info;
        std::regex EG("(.*).ol.epicgames.com");
        /* HYBRID
        if (std::regex_search(info, std::regex("/fortnite/api/cloudstorage/system"))) {
            return curl_original(Curl, opt, std::regex_replace(info, EG, HostLink).c_str());
        }
        else if (std::regex_search(info, std::regex("/fortnite/api/v2/versioncheck/"))) {
            return curl_original(Curl, opt, std::regex_replace(info, EG, HostLink).c_str());
        }
        else if (std::regex_search(info, std::regex("/fortnite/api/game/v2/profile"))) {
            return curl_original(Curl, opt, std::regex_replace(info, EG, HostLink).c_str());
        }
        else if (std::regex_search(info, std::regex("/content/api/pages/fortnite-game"))) {
            return curl_original(Curl, opt, std::regex_replace(info, EG, HostLink).c_str());
        }
        else if (std::regex_search(info, std::regex("/affiliate/api/public/affiliates/slug"))) {
            return curl_original(Curl, opt, std::regex_replace(info, EG, HostLink).c_str());
        }
        else if (std::regex_search(info, std::regex("/socialban/api/public/v1"))) {
            return curl_original(Curl, opt, std::regex_replace(info, EG, HostLink).c_str());
        } */

        // PRIVATE
        if (std::regex_search(info, EG)) {
            return curl_original(Curl, opt, std::regex_replace(info, EG, HostLink).c_str());
            break;
        }
        break;
    }
    return curl_original(Curl, opt, info);
}