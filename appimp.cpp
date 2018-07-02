#include "appimp.h"

extern zlog_category_t* g_server_cat;
extern ConfigPara g_conf;

appManager* appManager::appMan = new appManager();

appManager::appManager()
{

}

void appManager::zlog_InitStep()
{
    int ret = zlog_init("../conf/RecData_log.conf");
    if (ret != 0)
    {
        printf("Initlize zlog faield\n");
        exit(EXIT_FAILURE);
    }

    g_server_cat = zlog_get_category("server_cat_bj");
    if (!g_server_cat)
    {
       printf("zlog_get_category failed\n");
        exit(EXIT_FAILURE);
    }
     g_conf.ReadOptionsParam("../conf/aoptions.xml");
     m_planetoftpmap = g_conf.GetNASConfig();

}


void appManager::kafka_InitStep()
{
    kafkaPar.brokers   = g_conf.GetKafkaServerIP();
    kafkaPar.topic  = g_conf.GetKafkaTopic();

    char *topic = (char *) kafkaPar.topic.c_str();
    char *brokers = (char *) kafkaPar.brokers.c_str();
    int partition = 0;

    int ret = 0;
    ret = kp.init(topic, brokers, partition);
    if(ret != 0){zlog_error(g_server_cat,"Error: kp.init(): ret=%d;\n", ret);}

}


void appManager::init()
{
    zlog_InitStep();
    kafka_InitStep();

}

appManager* appManager::instance() {
    return appManager::appMan;
}


int appManager::pushKafkaData(char * recmsg)
{
    int ret = kp.sendMessage(recmsg, strlen(recmsg)); //向kafka服务器发送消息
    if(ret != 0)
    {
        zlog_error(g_server_cat,"Error: kp.sendMessage(): ret=%d;\n", ret);
        return ret;
    }
    return ret;

}

bool appManager::verifyHeaders(char * recMsg,RecHead &headers)
{
    cJSON* root = cJSON_Parse(recMsg);
    if (root == NULL)
    {
        zlog_error(g_server_cat," parse verifyHeaders json failed \n");
        return false;
    }
    else
    {
          //取包头数据
          memset(&head,0,sizeof(RecHead));
          if (!pkg->JSONToPkgHead(root, head))
          {
             zlog_error(g_server_cat," get Headers json data failed \n");
             return false;
          }

          memcpy(&headers,&head,sizeof(RecHead));
          string sz_username = (char*)head.szUserName;
          LaneToFTPIterator tempit;
          tempit = m_planetoftpmap->find(sz_username);
          if (tempit == m_planetoftpmap->end())
          {
              zlog_error(g_server_cat," config no proxyAddress auth failed \n");
              return false;
          }

          Recssl ssl;
          string encrypt_string;
          encrypt_string = ssl.encrypto((char*)head.szUserName);
          zlog_debug(g_server_cat,"\nencrypt_string:%s,\nhead.szUserName:%s\n",(unsigned char*)encrypt_string.substr(0,32).c_str(),head.szUserPwd);
          zlog_debug(g_server_cat,"head.szUserName:%s\n",head.szUserName);
          if(strcmp((char*)encrypt_string.substr(0,32).c_str(),(char*)head.szUserPwd) == 0)
          {
              zlog_debug(g_server_cat,"encrypt_string and head.szUserName compare success \n");
              return true;
          }
          return false;
    }

}



std::vector<std::string> appManager::split(const  std::string& s, const std::string& delim)
{
    std::vector<std::string> elems;
    size_t pos = 0;
    size_t len = s.length();
    size_t delim_len = delim.length();
    if (delim_len == 0) return elems;
    while (pos < len)
    {
        int find_pos = s.find(delim, pos);
        if (find_pos < 0)
        {
            elems.push_back(s.substr(pos, len - pos));
            break;
        }
        elems.push_back(s.substr(pos, find_pos - pos));
        pos = find_pos + delim_len;
    }
    return elems;
}





