#pragma once
#include <drogon/HttpController.h>
#include <drogon/drogon.h>
#include <fstream>
using namespace drogon;
namespace OTA
{
  class service:public drogon::HttpController<service>
  {
    public:
      METHOD_LIST_BEGIN
        METHOD_ADD(service::getUpdate,"/Changhong",Post);
        METHOD_ADD(service::getTest,"/test",Post);        
      METHOD_LIST_END    
        void getUpdate(const HttpRequestPtr& req,std::function<void (const HttpResponsePtr &)> &&callback) const;
        void getTest(const HttpRequestPtr& req,std::function<void (const HttpResponsePtr &)> &&callback) const;
        
  };
}
double returnFirmwareVersion(Json::Value &json);
bool checkSnList(std::string &stbid);