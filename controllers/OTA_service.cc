#include "OTA_service.h"
using namespace OTA;

void service::getUpdate(const HttpRequestPtr& req,
        std::function<void (const HttpResponsePtr &)> &&callback) const
{
    LOG_DEBUG;   

    Json::Value requestJson, reponseJson,  configJson;
    Json::Reader reader;
    
    std::string defaultResponse = "{ \"message\":\"version is latest, not need update\",\"status\": 1003}";
    std::string requestString = req->getParameter("json");
    
    std::ifstream configFile ("json.json");
    double firmwareVersionSTB, firmwareVersionConfig;

    reader.parse(configFile, configJson);
    reader.parse(requestString, requestJson);
    reader.parse(defaultResponse, reponseJson);
    
    const Json::Value &tempJson = requestJson["body"];
    std::string stbID = tempJson["stbid"].asString();

    if (checkSnList(stbID))
    {
        LOG_DEBUG << "SN in list";
        firmwareVersionConfig = returnFirmwareVersion(configJson);
        firmwareVersionSTB = std::stod(tempJson["firmwareVersion"].asString());

        if( firmwareVersionSTB < firmwareVersionConfig )
        {
            LOG_DEBUG << "Atualiza";
            reponseJson = configJson;
        }

    }

    auto resp=HttpResponse::newHttpJsonResponse(reponseJson);
    callback(resp);
}

//Return firmware version in config file
double returnFirmwareVersion(Json::Value &json)
{   
    double version;
    Json::Value& body = json["body"];
    Json::Value& updateModel = body["updateModel"];
    Json::Value& info = updateModel[0]["info"];
    version = std::stod(info[0]["newSwVersion"].asString());
    
    return version;
}

//Check if SN given is in the list
bool checkSnList(std::string &stbid)
{
    bool check = false;
    std::ifstream snlist ("snlist.txt");
    std::string line;
    
    if(snlist.is_open())
    {
        while(std::getline (snlist, line))
        {
            if(line == stbid)
                check = true;   
        }
        snlist.close();
    }
    else
    {
        std::cout << "No file found \n";
    }
    return check;
}