#include <QCoreApplication>
#include "json.hpp"
#include <fstream>
#include <iostream>
using json = nlohmann::json;
bool GetFileJson(const std::string& file, json& json_data)
{
    std::ifstream jfile(file);
    if (!jfile)
    {
        std::cerr << "No " <<file <<" such config file!\n";
        return false;
    }

    jfile >> json_data;
    if(!json_data.is_object())
    {
        std::cerr << file  <<   "is not json object!\n";
        jfile.close();
        return false;
    }
    return true;
}

bool CheckNode(const json& json_new, const json& json_org, json& json_diff)
{
   if (json_new.find("attrs") == json_new.end() )
   {
       return true;
   }
   json json_attrs_new = json_new["attrs"];
   json json_attrs_org = json_org["attrs"];
   auto iter = json_attrs_new.begin();
   json json_addattrs;
   json json_delattrs;
   json json_updateattrs;
   bool ret = true;
   while(iter != json_attrs_new.end())
   {
       std::string key = iter.key();
       std::string value = iter.key();
       if (json_attrs_org.find(key) != json_attrs_org.end())
       {
           std::string value_org = json_attrs_org[key];
           if (value_org == value)
           {

           }
           else
           {
               json json_attr;
               json_attr["key"] = key;
               json_attr["value"] = value;
               json_updateattrs.push_back(json_attr);
               ret = false;
           }
       }
       else
       {
           json json_attr;
           json_attr["key"] = key;
           json_attr["value"] = value;
           json_addattrs.push_back(json_attr);
           ret = false;
       }

       iter++;
   }

   auto iter_org = json_attrs_org.begin();
   while(iter_org != json_attrs_org.end())
   {
       std::string key = iter_org.key();
       std::string value = iter_org.key();
       if (json_attrs_new.find(key) == json_attrs_new.end())
       {
           json json_attr;
           json_attr["key"] = key;
           json_attr["value"] = value;
           json_delattrs.push_back(json_attr);
           ret = false;
       }
       iter_org++;
   }
   json_diff["addAttrs"] = json_addattrs;
   json_diff["updateAttrs"] = json_updateattrs;
   json_diff["deleteAttrs"] = json_delattrs;

   return ret;
}

json CompareJson(const json& json_new, const json& json_org)
{
    json json_ret;
    json json_nodes_new = json_new["nodes"];
    json json_nodes_org = json_org["nodes"];

    auto iter = json_nodes_new.begin();
    json json_addnode;
    json json_delnode;
    json json_updatenode;
    while(iter != json_nodes_new.end())
    {
        std::string key = iter.key();
        std::cout << "key: " << key << std::endl;
        if (json_nodes_org.find(key) == json_nodes_org.end())
        {
            json json_node ;
            json_node["key"] = key;
            json_node["vaule"] = iter.value();
            json_delnode.push_back(json_node);
        }
        else
        {
            json json_node_new  = json_nodes_new[key];
            json json_node_org = json_nodes_org[key];
            json json_diff;
            bool ret  = CheckNode(json_node_new, json_node_org, json_diff);
            if (ret)
            {

            }
            else
            {
                json json_node ;
                json_node["key"] = key;
                json_node["vaule"] = json_diff;
                json_updatenode.push_back(json_node);
            }
        }
        iter ++;
    }

    auto iter_org = json_nodes_org.begin();
    while(iter_org != json_nodes_org.end())
    {
        std::string key = iter_org.key();
        std::string value = iter_org.key();
        if (json_nodes_new.find(key) == json_nodes_new.end())
        {
            json json_node;
            json_node["key"] = key;
            json_node["value"] = value;
            json_updatenode.push_back(json_node);
        }
        iter_org++;
    }

    json_ret["addNodes"] = json_addnode;
    json_ret["updateNodes"] = json_updatenode;
    json_ret["deleteNodes"] = json_delnode ;

    return json_ret;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    std::string  new_file = "C:\\Users\\foursea\\Desktop\\exportCfg-new.json";
    json json_new;
    GetFileJson(new_file, json_new);

    std::string  org_file = "C:\\Users\\foursea\\Desktop\\exportCfg.json";
    json json_org;
    GetFileJson(org_file, json_org);

    json json_diff = CompareJson(json_new,json_org);
    std::cout << json_diff.dump(2);
    std::ofstream("diff.json") << json_diff.dump(2);
    return a.exec();
}
