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
vector<pair<string,int>>site_bandwidth;//边缘节点带宽
unordered_map<pair<string,string>,int,hash_pair>all_qos;//每个接口延迟
vector<pair<pair<int,string>,int>>all_demand;//每个时刻需求
unordered_map<string,int>popular_node;     //受欢迎的节点

void InitSite(string buf)
{

    int j = 0;
    node_buf.site = "";
    node_buf.bandwidth = 0;
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

    int count = 0;
    j++;
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
                popular_node.insert({ qos_buf.site ,1 });
            }
            else
            {
                popular_node[qos_buf.site]+=1;
            }
        }       
        j++;
        qos_buf.qosnum = "";
        count++;
    }    
    j++;
    count = 0;   
}

void InitDemand(string buf, vector<string>demand_name_array)
{
    int j = 0;
    demand_buf.Time +=5;  
    while (buf[j] != ',')
    {     
        j++;
    }
    int count = 0;
    j++;

    while (count < (demand_name_array.size()))//解决count越界
    {
        while (buf[j] != ',' && j < buf.size())
        {
            if (buf[j])
                demand_buf.WidthNeed += buf[j];
            j++;
        }
        all_demand.push_back({ {demand_buf.Time,demand_name_array[count]},stoi(demand_buf.WidthNeed) });
        widthneeded += stoi(demand_buf.WidthNeed);
        j++;
        demand_buf.WidthNeed = "";
        count++;
    }
    j++;
    count = 0;
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

int main()
{   
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
    }
    infile.clear();

    ifstream infile_limit("G:\\data\\config.ini",ios::in);
    if (!infile_limit)
    {
        cout << "打开文件失败！" << endl;
        exit(1);
    }
    msg = "";
    getline(infile_limit, msg);
    getline(infile_limit, msg);
    int j = find(msg.begin(), msg.end(), '=') - msg.begin();//记录第一个'，'
    int qos = 0;
    j++;
    while (msg[j] >= 48 && msg[j] <= 57)
    {
        qos = qos * 10 + msg[j] - 48;
        j++;
    }
    //cout << qos;
    cin.clear();

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
    j = find(msg.begin(), msg.end(), ',') - msg.begin();//记录第一个'，'
    j++;
    while (j < msg.size())
    {

        while (msg[j] != ',' && j < msg.size())
        {
            qos_buf.name += msg[j];
            j++;
        }
        server_name_array.push_back(qos_buf.name);
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
    j++;
    while (j <msg.size())
    {

        while (msg[j] != ',' && j < msg.size())
        {
            demand_buf.name += msg[j];
            j++;
        }
        demand_name_array.push_back(demand_buf.name);
        demand_buf.name = "";
        j++;
    }
    while (!infile_demand.eof())
    {//从第二行开始记录

        
        int l = 0;
        getline(infile_demand,buf);
        if (buf.empty())
        {
            //cout << 1;
            continue;
        }
        InitDemand(buf, demand_name_array);
        
    }
    infile_demand.clear();

    ofstream out("G:\\data\\output_data.txt", ios::app);
    if (!out)
    {
        cout << "Unable to open otfile";
        exit(1);
    }

    //cout << site_bandwidth.size();
    vector<pair<string, int>>nums;
    vector<pair<string, int>>used_opened_nums;
    used_opened_nums= site_bandwidth;
    vector<int>Time_Widthneed;
    vector<int>opend_times(site_bandwidth.size());
    for (int i = 0; i < demand_buf.Time / 5; i++)
    {
        nums= site_bandwidth;
        for (int j = 0; j < server_name_array.size(); j++)
        {

            //int q = p;
            string name = "";
            pair<string, int>f;
            int site = 0;
            int morethanonce = 0;
            out << server_name_array[j] << ":" ;
            while (all_demand[i * server_name_array.size() + j].second > 0)
            {
                int p = 1000000;
                if (morethanonce == 1)
                {
                    out << ",";
                }
                for (int k = 0; k < used_opened_nums.size(); k++)
                {
                    f = { used_opened_nums[k].first ,fabs(used_opened_nums[k].second-(site_bandwidth[k].second-nums[k].second) - all_demand[i * server_name_array.size() + j].second)};
                    if (f.second < p && (all_qos[{site_bandwidth[k].first, server_name_array[j]}] < qos) && nums[k].second != 0)
                    {
                        name = f.first;
                        p = f.second;
                        site = k;
                    }

                }
                //cout << "剩余需求" << all_demand[i * server_name_array.size() + j].second << endl;
                opend_times[site] = 1;//目标节点开启计数

                if ((all_demand[i * server_name_array.size() + j].second) - nums[site].second > 0)
                {
                    out << "<" << nums[site].first<<"," << nums[site].second << ">";
                    all_demand[i * server_name_array.size() + j].second -= nums[site].second;
                    //used_opened_nums[site].second = (nums[site].second + (opend_times[site] - 1) * used_opened_nums[site].second)/ opend_times[site];
                    nums[site].second = 0;
                }
                else if((all_demand[i * server_name_array.size() + j].second) - nums[site].second <= 0)
                {
                    out << "<"<< nums[site].first <<"," << all_demand[i * server_name_array.size() + j].second << ">";
                    nums[site].second -= all_demand[i * server_name_array.size() + j].second;
                    //used_opened_nums[site].second = (all_demand[i * server_name_array.size() + j].second + (opend_times[site] - 1) * used_opened_nums[site].second)/ opend_times[site];
                    all_demand[i * server_name_array.size() + j].second = 0;
                }
                //cout << used_opened_nums[site].first <<"used_open" << used_opened_nums[site].second << "nums_site_second" << site_bandwidth[site].second-nums[site].second << endl;

                if (all_demand[i * server_name_array.size() + j].second != 0)
                {
                    morethanonce=1;
                }
            }
            out << endl;
            
        }
        //cout << "下一轮" << endl;
        for (int v = 0; v < used_opened_nums.size(); v++)
        {
            if (opend_times[v] == 1)
            {
                used_opened_nums[v].second = (i * used_opened_nums[v].second + site_bandwidth[v].second - nums[v].second) / (i + 1);
                opend_times[v] = 0;
            }
        }
    }
    out.close();
    return 0;
}


