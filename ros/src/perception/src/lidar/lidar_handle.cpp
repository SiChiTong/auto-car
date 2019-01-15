#include "perception/lidar_handle.h"

#define DATATYPE float  
#define POINT_D 0.5
#define MAX_PNUM 5

LidarHandle::LidarHandle(YAML::Node yaml_conf){

}

void LidarHandle::process(const sensor_msgs::PointCloud2& msg,
                        car_msgs::base_obstacle_list& obstacle_list){
    if(msg.header.seq<1) return;
    pcl::PCLPointCloud2 pcl_pc2;
    pcl_conversions::toPCL(msg, pcl_pc2);
    pcl::PointCloud<pcl::PointXYZ>::Ptr temp_cloud(new pcl::PointCloud<pcl::PointXYZ>);
    pcl::fromPCLPointCloud2(pcl_pc2,*temp_cloud);
    int len = temp_cloud->points.size();
    //
    cout<<"convert finnished!"<<endl;
    XY cur,start,last;
    int point_count = 0;
    obstacle_list.list.clear();
    car_msgs::base_obstacle obst;
    for(int i=0;i<len;i++){
        cur.first = temp_cloud->points[i].x;
        cur.second = temp_cloud->points[i].y;
        if((cur.first-last.first)*(cur.first-last.first)+(cur.second-last.second)*(cur.second-last.second)>POINT_D
        ||point_count>MAX_PNUM||point_count == 0){
            //创造障碍物
            if(creat_obstacle(start, last, point_count, obst)){
                obstacle_list.list.push_back(obst);
                cout<<obst.header.seq<<"."
                <<"x:"<<obst.xa[0]
                <<"y:"<<obst.ya[0]
                <<"r:"<<obst.a
                <<endl;
            }
            point_count = 1;
            //
            start = cur;
            last = cur;

        }else{
            last = cur;
            point_count ++;
        }

    }

}



bool LidarHandle::readonce(const sensor_msgs::PointCloud2& msg, XY* pos){ //读一个完整的坐标点
    // static int head_seq = 0;
    // static int count =0;
    // static unsigned char* fream_header;
    // DATATYPE* data;
    // if(head_seq!= msg.header.seq){
    //     count = 0;
    //     fream_header = &msg.data;
    // }
    // head_seq = msg.header.seq;
    // data = (DATATYPE*)fream_header;
    // pos->first = *(data++);
    // pos->second = *(data++);
    // fream_header += msg.point_step;
    // count++;
    // if(count<width)
    //     return 1;
    // else
    //     return 0;
}

bool LidarHandle::creat_obstacle(XY start,XY end,
                    int point_count,
                    car_msgs::base_obstacle& obst){
    static int count = 0;
    if(point_count==0){
        count = 0;
        return 0;
    }
    else
        count ++;
    obst.header.seq = count;
    obst.type = 0;
    obst.xa.resize(4);
    obst.ya.resize(4);
    obst.theta_a.resize(4);
    obst.xa[0] = (start.first + end.first)/2;
    obst.ya[0] = (start.second + end.second)/2;
    if(point_count==1)
        obst.a = 0.01;
    else
        obst.a = sqrt((start.first - end.first)*(start.first - end.first) 
            + (start.second - end.second)*(start.second - end.second));
    obst.b = obst.a;
}