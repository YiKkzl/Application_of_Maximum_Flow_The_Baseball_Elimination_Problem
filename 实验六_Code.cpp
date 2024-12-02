#include <bits/stdc++.h>
#include <windows.h>
#include <chrono>

//#pragma GCC optimize(3)

//#define ll long long
#define endl '\n'
#define intmax 2147483647 //2e9
using namespace std;

#define test cout<<endl;for(int ijk=0;ijk<n-1;ijk++) cout<<a[ijk]<<" ";cout<<a[n-1]<<endl;//用于测试小规模数组是否正确排序

const int MAXN = 1000005;

struct team{
    string name;
    int w;
    int l;
    int r;
    bool ez=0;
};

struct edge{
    int from;
    int to;
    int cap;
    int flow;
};

class Dinic{
public:

    vector<edge> edges;
    vector<vector<int>> adj;//保存边序号
    vector<int>level;
    int n, m;
    int source, sink;

    Dinic(int n,int source,int sink):n(n),source(source),sink(sink){
        adj.resize(n);
        level.resize(n);
    }

    void add_edge(int from, int to, int cap){
        edges.push_back({from,to,cap,0});
        edges.push_back({to,from,0,0});//添加反向边 方便扩容/计算层次
        adj[from].push_back(edges.size() - 2);
        adj[to].push_back(edges.size() - 1);
    }

    bool bfs(){//BFS分层
        queue<int> q;
        q.push(source);
        fill(level.begin(),level.end(),-1);//初始化
        level[source] = 0;//源点为首

        while(!q.empty()){
            int u = q.front();
            q.pop();

            for(int id:adj[u]){
                if(edges[id].cap - edges[id].flow < 1) continue;//已阻塞（residual=0）
                if(level[edges[id].to] != -1) continue;//已访问
                level[edges[id].to] = level[u] + 1;//标记指向点
                q.push(edges[id].to);//准备下层
            }
        }
        if(level[sink] == -1) return 0;//无法到达汇点
        return 1;//可达
    }

    int dfs(int u,int nowflow){//寻找可达路
        if(nowflow == 0) return 0;
        if(u == sink) return nowflow;//已达汇点

        for(int cid = 0; cid < adj[u].size() ;cid++){//当前边id
            int id = adj[u][cid];
            int to = edges[id].to;
            if(level[u] + 1 != level[to] || edges[id].cap - edges[id].flow < 1) continue;//只走 可流入 的 下一层 的 点
            int tr = dfs(to, min(nowflow, edges[id].cap - edges[id].flow));//true flow 向后搜索
            if(tr==0) continue;//无可达

            //推流
            edges[id].flow += tr;
            if(id%2==0) edges[id+1].flow -= tr;//反向边 add_edge中 先保存from（正02468）再保存to（反13579）
            else edges[id-1].flow -= tr;//这两行可以通过[id^1]来实现 可读性较差
            
            return tr;//返回已推流量
        }
        return 0;//推流失败
    }

    int max_flow(){
        int flow=0;
        while(bfs()){//每次推流后重新分层
            int nowflow;
            while(nowflow = dfs(source, intmax)){
                flow += nowflow;
            }
        }
        return flow;
    }
};



void zdl(){
    int n;cin>>n;
    team t[n+5];
    int a[n+5][n+5];//避免与team的r冲突
    for(int i=0;i<n;i++){
        cin>>t[i].name>>t[i].w>>t[i].l>>t[i].r;
        for(int j=0;j<n;j++) cin>>a[i][j];
        a[i][i]=0;
    }
    cout<<"-------------------------"<<endl;

    int maxj=0;
    auto statime=chrono::high_resolution_clock::now();
    //////////简单
    for(int i=0;i<n;i++) if(t[i].w>t[maxj].w) maxj=i;
    for(int i=0;i<n;i++){
        if(t[i].w+t[i].r < t[maxj].w){
            t[i].ez=1;
            cout<<"第"<<i<<"队"<<t[i].name<<"被简单淘汰"<<endl;
        }
    }
    cout<<"-------------------------"<<endl;
    //////////复杂
    for(int x=0;x<n;x++){
        if(t[x].ez) continue;//已淘汰
        int maxWins = t[x].w + t[x].r;
        int source = 0, sink = 1 + (n-1)*(n-2)/2 + n - 1;
        Dinic dinic(sink + 1, source, sink);
        
        int Gai=1;//gameindex
        for(int i=0;i<n;i++){
            if(i==x) continue;
            for (int j=i+1;j<n;j++){
                if(j==x) continue;
                dinic.add_edge(source,Gai,a[i][j]);//源点到比赛点
                //比赛点到队伍点
                dinic.add_edge(Gai, 1 + (n-1)*(n-2)/2 + i ,intmax);
                dinic.add_edge(Gai, 1 + (n-1)*(n-2)/2 + j ,intmax);
                Gai++;//from 1 to (n-1)*(n-2) / 2
            }
        }
        
        for(int i=0;i<n;i++){
            if(i==x) continue;
            int cap = maxWins - t[i].w;
            if(cap<0){
                t[x].ez = 1;//实际上不ez 但用于最后输出
                break;
            }
            dinic.add_edge(1 + (n-1)*(n-2)/2 + i, sink, cap);//比赛点到汇点
        }
        
        if(!t[x].ez){
            int totalCapacity = 0;
            for(int i=0;i<n;i++){
                if(i==x) continue;
                for(int j=i+1;j<n;j++){
                    if(j==x) continue;
                    totalCapacity+=a[i][j];//所有的比赛容量
                }
            }
            if(dinic.max_flow() < totalCapacity){
                t[x].ez=1;//实际上不ez 但用于最后输出
                cout<<"第"<<x<<"队"<<t[x].name<<"被复杂淘汰"<<endl;
            }
        }
    }
    cout<<"-------------------------"<<endl;

    for(int i=0;i<n;i++){
        if(!t[i].ez){
            cout<<"第"<<i<<"队"<<t[i].name<<"有机会夺冠！"<<endl;
        }
    }
    cout<<"-------------------------"<<endl;

    
    

    auto endtime=chrono::high_resolution_clock::now();


    auto thistime=chrono::duration_cast<chrono::microseconds>(endtime - statime);
    cout<<"所用时间为"<<fixed<<setprecision(3)<<thistime.count()<<"微秒\n";

    return;
}


int main(){
    //ios::sync_with_stdio(0);cin.tie(0);cout.tie(0);
    zdl();

    
  
    
    return 0;
}

