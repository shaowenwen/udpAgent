#include <evpp/udp/udp_server.h>
#include <evpp/udp/udp_message.h>
#include <evpp/event_loop.h>
#include <evpp/event_loop_thread_pool.h>
#include <appimp.h>

zlog_category_t* g_server_cat;
ConfigPara g_conf;

void recbjczmsg(const string request)
{
    string SIM;
    string path;
    string body;
    size_t p0 = request.find("A P ");
    if (p0 != string::npos) {
        size_t p1 = request.find(":");
        SIM = request.substr(p1 + 1,13);
        path = request.substr(p0,4);
        size_t p3 = request.find("ffffffff ");
        if (p3 != string::npos) {
            p3 += 9;
            body = request.substr(p3, 128);//95
        }
        else
        {
            body="";
        }
    }
//  A P 6407:8613426062002 ffffffff 5b2b559d|0|3ccf72|b2484a|0|156|0|0|0|1,1;s7XDxbnY,R1BTtqjOuw==,tqjOu9PQ0Kc=,QUNDudg=,v9WztQ==,06rUy8H3y666xTotNjQzNQ==|¾©B7Y568
    zlog_info(g_server_cat,"request:%s ", request.c_str());
//  cout << " \n---------------------------------- \n  "\
//          "Handler receives \n"
//          "SIM:"<< SIM.c_str() << endl << \
//          "path:"<< path.c_str() << endl<< \
//          "body:" << body.c_str()<<endl;
  appManager *appMan = appManager::instance();
  RecHead headers;
  char *reqinfo;
  if(strcmp("A P ",(char *)path.c_str()) == 0)
  {
     PackTraj packtraj;
     vector<string> v_vert = appMan->split(body, "|"); //可按多个字符来分隔;
     for(vector<string>::size_type i = 0; i != v_vert.size(); ++i)
       cout << v_vert[i] << " "<< endl;

//     vector<string> v_comma = appMan->split(v[9], ","); //可按多个字符来分隔;
//     for(vector<string>::size_type i = 0; i != v_comma.size(); ++i)
//       cout << v_comma[i] << " "<< endl;

//     if(strcmp("40000000",(char *)v_vert[7].c_str()) == 0)
//     {
//         zlog_info(g_server_cat,"gps invaild :%s", reqinfo);
//         return;
//     }
     Recssl ssl;
     string encrypt_string =  ssl.encrypto("njitsbjbjcz");
     //hearders
     sprintf((char *)headers.szUserName ,"%s","njitsbjbjcz");
     sprintf((char *)headers.szUserPwd ,"%s",(char*)encrypt_string.substr(0,32).c_str());
     sprintf((char *)headers.szCommand ,"%s","track");
     sprintf((char *)headers.szIMEI ,"%s",SIM.c_str());

     //body
     sprintf((char *)packtraj.latitude ,"%d%s%d",hex_to_decimal((char *)v_vert[2].c_str(),6)/100000,".",hex_to_decimal((char *)v_vert[2].c_str(),6)%100000);
     sprintf((char *)packtraj.longitude ,"%d%s%d",hex_to_decimal((char *)v_vert[3].c_str(),6)/100000,".",hex_to_decimal((char *)v_vert[3].c_str(),6)%100000);
     sprintf((char *)packtraj.altitude ,"%d",hex_to_decimal((char *)v_vert[4].c_str(),v_vert[4].size()));
     sprintf((char *)packtraj.direction_angle ,"%d",hex_to_decimal((char *)v_vert[5].c_str(),v_vert[5].size()));
     int m_speed = hex_to_decimal((char *)v_vert[6].c_str(),v_vert[6].size());
     sprintf((char *)packtraj.speed ,"%d",int(m_speed* 0.036));

     time_t tick = (time_t)(hex_to_decimal((char *)v_vert[0].c_str(),8));
     struct tm tm;
     char bjtime[100];
     tm = *localtime(&tick);
     strftime(bjtime, sizeof(bjtime), "%Y-%m-%d %H:%M:%S", &tm);
     sprintf((char *)packtraj.stamp ,"%s",bjtime);
//     sprintf((char *)packtraj.visitor ,"%s",(char *)v_vert[8].c_str());
//     sprintf((char *)packtraj.plate ,"%s",(char *)v_vert[10].c_str());

//     cout <<"latitude:"<< (char *)packtraj.latitude  <<endl
//          <<"longitude:"<<packtraj.longitude<<endl
//          <<"speed:"<<packtraj.speed<<endl
//          <<"altitude:"<<packtraj.altitude<<endl
//          <<"direction_angle:"<<packtraj.direction_angle <<endl
//          <<"stamp:"<<packtraj.stamp<<endl;

     reqinfo = appMan->pkg->PkgPostoinToJson(headers,packtraj);

     zlog_info(g_server_cat,"reqinfo :%s", reqinfo);
     if(g_conf.GetKafkaServerused() == 1)
     {
         if(appMan->pushKafkaData(reqinfo) == 0)
         {
             zlog_info(g_server_cat,"pushKafkaData,success\n");
         }
         else
         {
             zlog_error(g_server_cat,"pushKafkaData,failed");
         }
     }

  }
}

int main(int argc, char* argv[]) {

    //app init
    appManager  *appMan = appManager::instance();
    appMan->init();
    //udp
    int port1 = g_conf.GetUdpPort1();
    int port2 = g_conf.GetUdpPort2();
    std::vector<int> ports = { port1, port2 };
    int thread_num = g_conf.GetUdpThreadNum();

    evpp::udp::Server server;

    server.SetThreadDispatchPolicy(evpp::ThreadDispatchPolicy::kIPAddressHashing);
    server.SetMessageHandler([](evpp::EventLoop* loop, evpp::udp::MessagePtr& msg) {
        recbjczmsg(msg->data());
    });
    server.Init(ports);
    server.Start();

    evpp::EventLoop loop;
    std::shared_ptr<evpp::EventLoopThreadPool> tpool(new evpp::EventLoopThreadPool(&loop, thread_num));
    tpool->Start(true);
    server.SetEventLoopThreadPool(tpool);
    loop.Run();
    server.Stop(true);
    tpool->Stop(true);
    return 0;
}
