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
            {
                check = true;   
                break;
            }
        }
        snlist.close();
    }
    else
    {
        std::cout << "No file found \n";
    }
    return check;
}

void service::getTest(const HttpRequestPtr& req,
        std::function<void (const HttpResponsePtr &)> &&callback) const
{
    LOG_DEBUG;   

    Json::Value requestJson, reponseJson,  configJson;
    
    reponseJson["ok"] = " ok ";
    

    std::ifstream snlist ("snlist.txt");
    std::string line, requestS{""};

    int i {0}, y{0};

    if(snlist.is_open())
    {
        while(std::getline (snlist, line))
        {

            auto client = HttpClient::newHttpClient("http://atv.changhongnetwork.net:8080");
            auto requestH = HttpRequest::newHttpFormPostRequest();
            //LOG_DEBUG;
            requestH->setMethod(drogon::Post);
            requestH->setPath("/update_iptv_en/main.html");
            requestH->addHeader("Content-Type","application/x-www-form-urlencoded");
            requestH->setBody("json={"
                                "\"appType\":\"Android\","
                                "\"appVersion\":\"3.0\","     
                                "\"body\": {"
                                "\"OUI\":\"CHOTT\","
                                "\"acc\":\"640\","
                                "\"androidSdk\":\"28\","
                                "\"connectMode\":\"lan\","
                                "\"firmwareVersion\":\"2.9006\","
                                "\"hardwareVersion\":\"61.02.03.03\","
                                "\"ip\":\"0.0.0.0\","
                                "\"mac\":\"B4:60:77:03:95:91\","
                                "\"model\":\"Hi3798MV200\","
                                "\"netMode\":\"dhcp\","
                                "\"stbid\":\""+line+"\","
                                "\"type\":\"Sversioninfo\"   },"
                                "\"businessType\":\"update_check\"}");
                                
            auto a = client->sendRequest(requestH, 2);
            
            if(a.first != ReqResult::Ok)
            {
                LOG_DEBUG;
                reponseJson["response"] = "Problemas durante comunicação até servidor MOTV";
                LOG_DEBUG;
                std::string stg {a.second->getBody()};
                std::cout << stg;
            }
            else
            {
                LOG_DEBUG;
                Json::Reader reader;
                reader.parse(std::string{a.second->getBody()}, requestJson);
                //LOG_DEBUG << "End";
            }
            std::cout << requestJson.toStyledString() << std::endl;
            LOG_DEBUG;
            if(requestJson["status"].asInt() == 1000)
            {
                i++;
                LOG_DEBUG;
                std::cout << line << std::endl;
                reponseJson[std::to_string(i)] = line;
            }    
            y++;
            std::cout << y << " " << i << std::endl;
        }
        snlist.close();       
    }
    else
    {
        std::cout << "No file found \n";
    }

    

    auto resp=HttpResponse::newHttpJsonResponse(reponseJson);
    callback(resp);
}
