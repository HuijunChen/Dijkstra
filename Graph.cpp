// Graph.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include<stdlib.h>
#include<fstream>
#include<string>
#include<iostream>
using namespace std;

const int MAX_VERTEX_NUM=200;
const int maxint=99999;


//结构定义
typedef struct ArcCell{
	char way[10];
	float cost;
	float time;
	float weight;
}ArcCell,AdjMatrix[MAX_VERTEX_NUM][MAX_VERTEX_NUM];

typedef struct VexNode{
	char country[50];
	char city[50];
	float longitude;
	float latitude;
}VexNode;

typedef struct{
	VexNode vexs[MAX_VERTEX_NUM];
	AdjMatrix arcs;
	int vexnum,arcnum;
}MGraph;



//函数声明
int  LocateVex(char *m);
void CreateGraph(FILE *fp1,FILE *fp2);
void ShortestPath_Dijkstra(int v0,int *dist,int *prev);
void SearchPath(int *prev,int v0,int u);
void FindShortestPath(int v0,int u);

//全局变量MGraph G
MGraph G;

int _tmain(int argc, _TCHAR* argv[])
{

	//打开文件，读取顶点和边的个数
	FILE *fp1=fopen("cities.csv","r"),*fp2=fopen("routes.csv","r");
	if(!fp1||!fp2){cout<<"cannot open the file!\n";return 0;}

	//创建图，初始化顶点的个数为199，边的条数为1978
	G.vexnum = 199;
	G.arcnum = 1978;
	CreateGraph(fp1,fp2);

	//选定起点和终点
	char begin[50]="Kampala",aim[50]="Lisbon";

	//char begin[]="Kampala",aim[]="Lisbon";	
	cout<<"这里权重设定为时间*0.8，开销*0.2"<<endl;
	printf("默认起点为%s，终点为%s，按'c'修改起点与终点，按其他键继续\n\n",begin,aim);
	char c=getchar();;
	if(c=='c')
	{
		bool RightBegin=false,RightAim=false;
		while(!RightBegin)
		{
			printf("请输入起点城市：");
			scanf("%s",begin);

			for(int i=1;i<=G.arcnum;i++)
				if(strcmp(begin,G.vexs[i].city)==0)
				{
					RightBegin=true;
					break;
				}
			if(RightBegin==false)
				printf("输入错误,请重新输入！\n\n");
		}
		while(!RightAim)
		{
			printf("\n请输入终点城市：");
			scanf("%s",aim);


			for(int i=1;i<=G.arcnum;i++)
				if(strcmp(aim,G.vexs[i].city)==0)
				{
					RightAim=true;
					break;
				}
			if(RightAim==false)
				printf("输入错误,请重新输入！\n\n");
		}
	}
	int v0=LocateVex(begin),u=LocateVex(aim);
	
	//找到最短路径
	FindShortestPath(v0,u);
	cout<<endl<<"链接生成成功！请查看OutputMap.htm！"<<endl;

	//关闭文件
	fclose(fp1);fclose(fp2);
	system("pause");
	return 0;
}


//创建图
void CreateGraph(FILE *fp1,FILE *fp2){

	//初始化两点间路径长度（权值）
	for(int i=1;i<=MAX_VERTEX_NUM;i++)
		for(int j=1;j<=MAX_VERTEX_NUM;j++)
			G.arcs[i][j].weight=maxint;

	//读入顶点信息

	for(int i=1;i<=G.vexnum;i++){

		char s[100],*t1=NULL,*t2=NULL,*t3=NULL,*t4=NULL;
		fgets(s,100,fp1);

		t3=strtok(s,",");strcpy(G.vexs[i].country,t3);
		t4=strtok(NULL,",");strcpy(G.vexs[i].city,t4);
		t1=strtok(NULL,",");
		t2=strtok(NULL,",");
		
		G.vexs[i].latitude=atof(t1);
		G.vexs[i].longitude=atof(t2);
		
	}

	//初始化权值
	for(int i=1;i<=MAX_VERTEX_NUM;i++)
		for(int j=1;j<=MAX_VERTEX_NUM;j++)
			G.arcs[i][j].weight=maxint;
			
	//读入边信息
	for(int i=1;i<G.arcnum;i++){
		char s[1024],*m,*n,*t;
		fgets(s,1024,fp2);
		int a,b;
		m=strtok(s,",");
		n=strtok(NULL,",");

		a=LocateVex(m);
		b=LocateVex(n);
		if(a!=-1 && b!=-1){
		t=strtok(NULL,","); strcpy(G.arcs[a][b].way,t);
		G.arcs[a][b].time=atof(strtok(NULL,","));
		G.arcs[a][b].cost=atof(strtok(NULL,","));

		//这里权重设定为时间*0.8，开销*0.2
		G.arcs[a][b].weight = G.arcs[a][b].time * 0.8+G.arcs[a][b].cost * 0.2;

		}
	}
}


//通过顶点的信息找到顶点编号
int  LocateVex(char *m){
	int i;
	for(i=1;i<=G.arcnum;i++){
		if(strcmp(m,G.vexs[i].city)==0)
			return i;
	}
	return -1;
}


//调用迪杰斯特拉函数并输出结果
void FindShortestPath(int v0,int u){
	int dist[MAX_VERTEX_NUM];//表示当前点到源点的最短路径长度
	int prev[MAX_VERTEX_NUM];// 记录当前点的前一个结点

	ShortestPath_Dijkstra(v0,dist,prev);
	SearchPath(prev,v0,u);

}


//迪杰斯特拉算法计算最小路径
void ShortestPath_Dijkstra(int v0,int *dist,int *prev){

	bool s[MAX_VERTEX_NUM];//判断是否已存入该点到S集合中

	for(int i=1;i<=G.vexnum;i++){
		dist[i]=G.arcs[v0][i].weight;
		s[i]=0;
		if(dist[i]==maxint) prev[i]=0;
		else prev[i]=v0;
	}
	dist[v0]=0;
	s[v0]=1;

	//依次将点放入集合s中
	for(int i=2;i<=G.vexnum;i++){
		int t=maxint;
		int u=v0;  //u保存当前邻接点中距离最小的点的号码

		//找到未进入s的点j的dist[j]最小值
		for(int j=1;j<=G.vexnum;j++){
			if((!s[j]) && dist[j]<maxint){
				u=j;
				t=dist[j];
			}
		}
		s[u]=1;

		//修改数组dist
		for(int j=1;j<=G.vexnum;j++){
			if((!s[j]) && G.arcs[u][j].weight<maxint){
				int newdist=dist[u]+G.arcs[u][j].weight;
				if(newdist < dist[j]){ dist[j]=newdist;prev[j]=u;}
			}
		}
	}

}

//查找v0到u的路径
void SearchPath(int *prev,int v0,int u){

	int queue[MAX_VERTEX_NUM];
	int i=1;
	queue[i]=u;
	i++;
	int tmp=prev[u];
	while(tmp!=v0){
		queue[i]=tmp;
		i++;
		tmp=prev[tmp];
	}
	queue[i]=v0;

	//设置链接,把结果用谷歌地图表示
	FILE *fp=fopen("OutputMap.htm","w");
	if(!fp)return;

	fprintf(fp,"<HTML><HEAD><TITLE>Shortest path from %s to %s</TITLE></HEAD><script type='text/javascript' src='http://maps.google.com/maps/api/js?sensor=false'></script><script>function initialize() { var myOptions = { zoom: 3, center: new google.maps.LatLng(0, 0), mapTypeId: google.maps.MapTypeId.ROADMAP};var map=new google.maps.Map(document.getElementById('map'), myOptions);\n\n",G.vexs[v0].country,G.vexs[u].country);

	int count=0,t=0;

	fprintf(fp,"var marker%d = new google.maps.Marker({ position: new google.maps.LatLng(%f, %f), map: map, title:'%s, %s'});\n",
		count,G.vexs[queue[i]].latitude,G.vexs[queue[i]].longitude,G.vexs[queue[i]].city,G.vexs[queue[i]].country);
	count++;


	for(int j=i-1;j>1;j--){
		fprintf(fp,"var marker%d = new google.maps.Marker({ position: new google.maps.LatLng(%f, %f), map: map, title:'%s, %s'});\n",
			count,G.vexs[queue[j]].latitude,G.vexs[queue[j]].longitude,G.vexs[queue[j]].city,G.vexs[queue[j]].country);
		count++;

		fprintf(fp,"var contentString%d = \042%s, %s --> %s, %s (%s - %f hours - $%f)\042; var path%d = new google.maps.Polyline({ path: [new google.maps.LatLng(%f, %f), new google.maps.LatLng(%f, %f)], strokeColor: '#0000FF', strokeOpacity: 1.0, strokeWeight: 2}); path%d.setMap(map); google.maps.event.addListener(path%d, 'click', function(event) { alert(contentString%d); });\n",
			t,G.vexs[queue[j+1]].city,G.vexs[queue[j+1]].country,G.vexs[queue[j]].city,G.vexs[queue[j]].country,G.arcs[queue[j+1]][queue[j]].way,G.arcs[queue[j+1]][queue[j]].time,
			G.arcs[queue[j+1]][queue[j]].cost,t,G.vexs[queue[j+1]].latitude,G.vexs[queue[j+1]].longitude,G.vexs[queue[j]].latitude,G.vexs[queue[j]].longitude,t,t,t);

		fprintf(fp,"var marker%d = new google.maps.Marker({ position: new google.maps.LatLng(%f, %f), map: map, title:'%s, %s'});\n",
			count,G.vexs[queue[j]].latitude,G.vexs[queue[j]].longitude,G.vexs[queue[j]].city,G.vexs[queue[j]].country);
		count++;  t++;
	}

	int j=1;
	fprintf(fp,"var marker%d = new google.maps.Marker({ position: new google.maps.LatLng(%f, %f), map: map, title:'%s, %s'});\n",
		count,G.vexs[queue[j]].latitude,G.vexs[queue[j]].longitude,G.vexs[queue[j]].city,G.vexs[queue[j]].country);
	fprintf(fp,"var contentString%d = \042%s, %s --> %s, %s (%s - %f hours - $%f)\042; var path%d = new google.maps.Polyline({ path: [new google.maps.LatLng(%f, %f), new google.maps.LatLng(%f, %f)], strokeColor: '#0000FF', strokeOpacity: 1.0, strokeWeight: 2}); path%d.setMap(map); google.maps.event.addListener(path%d, 'click', function(event) { alert(contentString%d); });\n",
		t,G.vexs[queue[j+1]].city,G.vexs[queue[j+1]].country,G.vexs[queue[j]].city,G.vexs[queue[j]].country,G.arcs[queue[j+1]][queue[j]].way,G.arcs[queue[j+1]][queue[j]].time,
		G.arcs[queue[j+1]][queue[j]].cost,t,G.vexs[queue[j+1]].latitude,G.vexs[queue[j+1]].longitude,G.vexs[queue[j]].latitude,G.vexs[queue[j]].longitude,t,t,t);


	char str[]="} google.maps.event.addDomListener(window, 'load', initialize); </script></HEAD><BODY><div id='map' style='width:100%;height:100%;'></div></BODY></HTML>";
	fprintf(fp,"%s\n\n",str);

	fclose(fp);
}

