#include <iostream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <string>
#include<vector>
#include <unordered_map>
#include <algorithm>
#include <ctime>
using namespace std;
typedef struct A
{

    string site;              //节点id
    int NodeLeftWidth;        //该节点剩余带宽
    int isopened;             //节点是否打开
    long bandwidth = 0;
   

}node;

typedef struct B
{
    vector<string>name_array;
    string name;              //用户名称
    int Time=0;                //需要带宽时刻
    string WidthNeed;             //需要带宽大小

}demand;

typedef struct C
{
    string site;   //延迟节点
    string name;   //延迟客户
    vector<pair<string, int>>one_qos;
    string qosnum;//

}qos;

struct hash_pair {
    template <class T1, class T2>
    size_t operator()(const pair<T1, T2>& p) const
    {
        auto hash1 = hash<T1>{}(p.first);
        auto hash2 = hash<T2>{}(p.second);
        return hash1 ^ hash2;
    }
};



demand demand_buf;
qos qos_buf;
node node_buf;


int limited_qos = 400;
int widthcost;//总节点消耗带宽
int widthneeded=0;//总节点需求带宽
vector<string>demand_name_array;//需要带宽列表
vector<string>server_name_array;//用户列表
//边缘节点带宽
//unordered_map<string, node> node_map;
vector<pair<string,int>>site_bandwidth;
vector<pair<int, int>>pair_karman;//一个节点常用带宽卡尔曼
unordered_map<pair<string,string>,int,hash_pair>all_qos;//每个接口延迟
vector<pair<pair<int,string>,int>>all_demand;//每个时刻需求
unordered_map<string,int>popular_node;     //受欢迎的节点

void InitSite(string buf)
{

    int j = 0;
    node_buf.site = "";
    node_buf.bandwidth = 0;

    //cout << buf[0] << endl;

    while (buf[j] != ',')
    {
        node_buf.site += buf[j];
        j++;
    }
    j++;
    while (buf[j] >= 48 && buf[j] <= 57)
    {

        node_buf.bandwidth = node_buf.bandwidth * 10 + (buf[j] - '0');
        j++;
    }
    site_bandwidth.push_back(make_pair(node_buf.site,node_buf.bandwidth));
    //site_bandwidth[site_name] = bandwidth;

    //if (node_buf.name[0].isal)
   
    //else return 1;
}


void InitQos(string buf, vector<string>server_name_array)
{

    int j = 0;
    qos_buf.site = "";
    
    while (buf[j] != ',')
    {
        qos_buf.site+= buf[j];
        j++;
    }
    
    //cout << server_name_array.size();
    int count = 0;
    j++;


    //cout << server_name_array.size() <<endl;
    //cout << buf[j];
    while (count < (server_name_array.size()))//解决count越界

    {
        while (buf[j] != ','&&j<buf.size())
        {
            if(buf[j])
            qos_buf.qosnum += buf[j];
            j++;
        }
        all_qos.insert({{ qos_buf.site,server_name_array[count]},stoi(qos_buf.qosnum)});
        
        if (stoi(qos_buf.qosnum) < 400)
        {
            if (!popular_node.count(qos_buf.site))
            {
                //cout << qos_buf.site << endl;
                popular_node.insert({ qos_buf.site ,1 });
                //cout << popular_node["qos_buf.site"] << endl;
            }
            else
            {
                popular_node[qos_buf.site]+=1;
            }
            
        }
        
        
        //cout <<qos_buf.site<< popular_node["qos_buf.site"]<<endl;
        //cout<<qos_buf.site<<server_name_array[count] << " " << qos_buf.qosnum << endl;
        
        j++;
        qos_buf.qosnum = "";
        count++;
    }    
    j++;
    count = 0;
    
    //cout<<site_name<<" "<<demand_name_array[count]<<" "<<stoi(qosnum)<<endl;
    
    //count++;
    //cout<<count<<" "<<demand_name_array.size()<<endl;
    
}

void InitDemand(string buf, vector<string>demand_name_array)
{

    int j = 0;
    demand_buf.Time +=5;

    
    while (buf[j] != ',')
    {
        
        //demand_buf.Time += buf[j];
        j++;
    }
    //cout << demand_buf.Time<<endl;
    //demand_buf.Time = "";
    
    //cout << server_name_array.size();
    int count = 0;
    j++;


    //cout << server_name_array.size() <<endl;
    //cout << buf[j];
    while (count < (demand_name_array.size()))//解决count越界

    {
        while (buf[j] != ',' && j < buf.size())
        {
            if (buf[j])
                demand_buf.WidthNeed += buf[j];
            j++;
        }
        all_demand.push_back({ {demand_buf.Time,demand_name_array[count]},stoi(demand_buf.WidthNeed) });
        //cout << demand_buf.Time << " " << demand_name_array[count] << " " << qos_buf.qosnum << demand_buf.WidthNeed<<endl;
        //cout << server_name_array[count]<< endl;
        widthneeded += stoi(demand_buf.WidthNeed);
        j++;
        demand_buf.WidthNeed = "";
        count++;
    }
    j++;
    count = 0;

    //cout<<site_name<<" "<<demand_name_array[count]<<" "<<stoi(qosnum)<<endl;

    //count++;
    //cout<<count<<" "<<demand_name_array.size()<<endl;

}


void firt_line_list(int j,string msg,vector<string>lis)
{
    while (msg[j] != ',' && j < msg.size())
    {
        qos_buf.name += msg[j];
        j++;
    }
    lis.push_back(qos_buf.name);
    //cout << demand_buf.name<< endl;
    qos_buf.name = "";
    j++;
}



bool cmp_BestNode(const pair<string, int>& a, const pair<string, int>& b)
{

    return a.second < b.second;
}

bool clost_node(const pair<string,long>& a, const pair<string,long>& b)// 是否满足分配cnt流量
{
    return a.second < b.second;
}



int main()
{   
    //存了节点和带宽
    string buf;
    string msg;
    ifstream infile("G:\\data\\site_bandwidth_new.csv", ios::in);
    //存下第一行无用数据
    getline(infile, msg);
    int k=0;
    while (!infile.eof())
    {//从第二行开始记录
        getline(infile, buf);
        if (buf.empty())
        {
            continue;
        }
        InitSite(buf);
        //if (InitSite(buf)==0)
        //    break;
        
        //k++;
    }
    infile.clear();

    //存qos
    ifstream infile_qos("G:\\data\\qos.csv", ios::in);
    if (!infile_qos)
    {
        cout << "打开文件失败！" << endl;
        exit(1);
    }
    msg = "";
    buf = "";
    getline(infile_qos, msg);
    int j = find(msg.begin(), msg.end(), ',') - msg.begin();//记录第一个'，'
    j++;//??
    //cout << msg<<msg.size() << endl;
    while (j < msg.size())
    {

        while (msg[j] != ',' && j < msg.size())
        {
            qos_buf.name += msg[j];
            j++;
        }
        server_name_array.push_back(qos_buf.name);
        //popular_node({ qos_buf.site ,0 });
        //cout << demand_buf.name<< endl;
        qos_buf.name = "";
        j++;


    }
    while (!infile_qos.eof())
    {//从第二行开始记录

        int l = 0;
        getline(infile_qos, buf);
        if (buf.empty())
        {
            //cout << 1;
            continue;
        }
        InitQos(buf, server_name_array);
        //cout << buf << endl;


        //if ((buf[0] >= 48 && buf[0] <= 57) || (buf[0] >= 65 && buf[0] <= 90) || (buf[0] >= 97 && buf[0] <= 122))
        //InitQos(buf, demand_name_array);
        //else break;
    }
    infile_qos.clear();
    


    //存客户需求
    
    ifstream infile_demand("G:\\data\\demand.csv", ios::in);
    if (!infile_demand)
    {
        cout << "打开文件失败！" << endl;
        exit(1);
    }
    msg = "";
    buf = "";
    getline(infile_demand, msg);
    j = find(msg.begin(), msg.end(), ',') - msg.begin();//记录第一个'，'
    //string demand_buf.name = "";
    //cout <<j<< msg << endl;
    //string demand_buf.name = "";
    j++;//??
    //cout << msg<<msg.size() << endl;
    while (j <msg.size())
    {

        while (msg[j] != ',' && j < msg.size())
        {
            demand_buf.name += msg[j];
            j++;
        }
        demand_name_array.push_back(demand_buf.name);
        //cout << demand_buf.name<< endl;
        demand_buf.name = "";
        j++;
    }
    //cout << 1;
    while (!infile_demand.eof())
    {//从第二行开始记录

        
        int l = 0;
        getline(infile_demand,buf);
        if (buf.empty())
        {
            //cout << 1;
            continue;
        }
        //cout << buf << endl;
        
     
        //if ((buf[0] >= 48 && buf[0] <= 57) || (buf[0] >= 65 && buf[0] <= 90) || (buf[0] >= 97 && buf[0] <= 122))
        InitDemand(buf, demand_name_array);
        //else break;
        
        
    }
    infile_demand.clear();
    
    
    
    
    /*
    for (auto ch : all_qos)
    {
        cout << all_qos.at <<ch.second<< endl;
    }
    */
    cout << site_bandwidth.size();
    vector<pair<string, int>>nums;
    vector<pair<string, int>>used_opened_nums;
    used_opened_nums= site_bandwidth;
    sort(site_bandwidth.begin(), site_bandwidth.end(), cmp_BestNode);
    
    vector<int>Time_Widthneed;
    vector<int>opend_times(site_bandwidth.size());
    for (int i = 0; i < demand_buf.Time / 5; i++)
    {
        nums= site_bandwidth;
        for (int j = 0; j < server_name_array.size();j++)
        {
            
            //int q = p;
            string name = "";
            pair<string, int>f;
            int site=0;
            while(all_demand[i* server_name_array.size()+j].second>0) 
            {
                int p = 1000000;

                for (int k=0;k< used_opened_nums.size(); k++)
                {
                    
                    f = { used_opened_nums[k].first ,fabs(used_opened_nums[k].second - all_demand[i * server_name_array.size() + j].second)};
                    if (f.second<p&&(all_qos[{site_bandwidth[k].first, server_name_array[j]}] < 400))
                    {
                        name = f.first;
                        p = f.second;
                        site = k;
                        //cout << used_opened_nums[k].first <<endl;
                        
                    }
                        
                }
                cout <<"剩余需求" << all_demand[i * server_name_array.size() + j].second << endl;
                opend_times[site] += 1;//目标节点开启计数
                
                
                
                if ((all_demand[i * server_name_array.size() + j].second) - nums[site].second > 0)
                {
                    all_demand[i * server_name_array.size() + j].second -= nums[site].second;
                    used_opened_nums[site].second = nums[site].second+ (opend_times[site]- 1 )* used_opened_nums[site].second/ opend_times[site];
                    nums[site].second = 0;
                }
                else
                {
                    
                    nums[site].second -= all_demand[i * server_name_array.size() + j].second;
                    used_opened_nums[site].second = all_demand[i * server_name_array.size() + j].second + (opend_times[site]- 1 ) * used_opened_nums[site].second / opend_times[site];
                    all_demand[i * server_name_array.size() + j].second = 0;
                }
                
                cout << used_opened_nums[site].first << used_opened_nums[site].second <<endl;

                if (all_demand[i * server_name_array.size() + j].second == 0)
                {
                    cout << "yes" << endl;
                }
                





                /*
                //cout << all_demand[i * server_name_array.size() + j].second << endl;
                
                //cout << nums.size();
                for (int k = 0; k < site_bandwidth.size(); k++)
                {
                    //cout << all_qos[{site_bandwidth[k].first, server_name_array[j]}] << endl;
                    cout << k<< endl;
                    
                    if (all_qos[{site_bandwidth[k].first,server_name_array[j]}]>400)
                    {
                        cout <<1<< site_bandwidth[k].first<<" " << server_name_array[j];
                    }
                    
                }
                //cout << endl;
                */
                
                
            }
        }
    }
    
    /*
    while (time_pas <= demand_buf.Time)
    {
        while (Time_Widthneed[j]> 0)
        {
            Time_Widthneed[j]-=all_demand[i].second;
            int a_need = all_demand[i].second;
            for (int k = 0; k < popular_node.size(); k++)
            {
                if (a_need > 0)
                    a_need -= ifSuitnodeForTheFirst();
                else
                    break;
            }
            i++;
            
        }
        //widthneeded -= all_demand[i].second;
        time_pas += 5;
        j++;

    }
    */
    
    






    return 0;
}


