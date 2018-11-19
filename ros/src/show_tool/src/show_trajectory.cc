#include "ros/ros.h"
#include "car_msgs/localization.h"
#include "car_msgs/trajectory.h"
#include <visualization_msgs/Marker.h>
#include <visualization_msgs/MarkerArray.h>
#include <geometry_msgs/Point.h>
#include <Eigen/Dense>

//localization
ros::Publisher localization_markerPub;
visualization_msgs::Marker localization_marker_list;
double time_old = 0;
#define LONCALIZATION_PATH_LENGTH 300
//refrenceline
ros::Publisher refrenceline_markerPub;
visualization_msgs::Marker refrenceline_marker_list;
//trajectory
ros::Publisher trajectory_markerPub;
visualization_msgs::Marker trajectory_marker_list;
car_msgs::trajectory trajectory_msg_list;
void localization_topic_callback(const car_msgs::localization::ConstPtr &localization_msg){
    double time_now = ros::Time::now().toSec();
    if(time_now - time_old > 0.01){
        time_old = time_now;
        //显示已走轨迹
        localization_marker_list.header.frame_id = "map";
        localization_marker_list.header.stamp = ros::Time::now();
        localization_marker_list.ns = "my_namespace";
        localization_marker_list.id = 2;
        localization_marker_list.type = visualization_msgs::Marker::SPHERE_LIST;
        localization_marker_list.action = visualization_msgs::Marker::ADD;

        localization_marker_list.scale.x = 0.1;
        localization_marker_list.scale.y = 0.1;
        localization_marker_list.scale.z = 0.1;
        localization_marker_list.color.a = 1.0; // Don't forget to set the alpha!
        localization_marker_list.color.r = 0.0;
        localization_marker_list.color.g = 1.0;
        localization_marker_list.color.b = 0.0;

        if(localization_marker_list.points.size() > LONCALIZATION_PATH_LENGTH)
            localization_marker_list.points.erase(localization_marker_list.points.begin());
        localization_marker_list.points.push_back(localization_msg->position);

        localization_markerPub.publish(localization_marker_list);

        //显示路径
        if(refrenceline_marker_list.points.size() > 0){
            refrenceline_marker_list.header.frame_id = "map";
            refrenceline_marker_list.header.stamp = ros::Time::now();
            refrenceline_marker_list.ns = "my_namespace";
            refrenceline_marker_list.id = 3;
            refrenceline_marker_list.type = visualization_msgs::Marker::SPHERE_LIST;
            refrenceline_marker_list.action = visualization_msgs::Marker::ADD;

            refrenceline_marker_list.scale.x = 0.2;
            refrenceline_marker_list.scale.y = 0.2;
            refrenceline_marker_list.scale.z = 0.2;
            refrenceline_marker_list.color.a = 1.0; // Don't forget to set the alpha!
            refrenceline_marker_list.color.r = 1.0;
            refrenceline_marker_list.color.g = 0.0;
            refrenceline_marker_list.color.b = 0.0;
            refrenceline_markerPub.publish(refrenceline_marker_list);
        }
        //显示目标轨迹
        if(trajectory_msg_list.total_path_length > 0){
            for(int i = 0;i < trajectory_msg_list.total_path_length;i++){
                trajectory_marker_list.header.frame_id = "map";
                trajectory_marker_list.header.stamp = ros::Time::now();
                trajectory_marker_list.ns = "my_namespace";
                trajectory_marker_list.id = i;
                trajectory_marker_list.type = visualization_msgs::Marker::ARROW;
                trajectory_marker_list.action = visualization_msgs::Marker::ADD;

                trajectory_marker_list.pose.position.x = trajectory_msg_list.trajectory_path[i].x;
                trajectory_marker_list.pose.position.y = trajectory_msg_list.trajectory_path[i].y;
                trajectory_marker_list.pose.position.z = trajectory_msg_list.trajectory_path[i].z;


                Eigen::Matrix3d R;
                R = Eigen::AngleAxisd(trajectory_msg_list.trajectory_path[i].theta, Eigen::Vector3d::UnitZ())
                    * Eigen::AngleAxisd(0, Eigen::Vector3d::UnitY())
                    * Eigen::AngleAxisd(0, Eigen::Vector3d::UnitX());
                //RotationMatrix to Quaterniond
                Eigen::Quaterniond q; 
                q = R;    

                trajectory_marker_list.pose.orientation.x = q.x();
                trajectory_marker_list.pose.orientation.y = q.y();
                trajectory_marker_list.pose.orientation.z = q.z();
                trajectory_marker_list.pose.orientation.w = q.w();

                trajectory_marker_list.scale.x = 0.3;
                trajectory_marker_list.scale.y = 0.08;
                trajectory_marker_list.scale.z = 0.08;
                trajectory_marker_list.color.a = 1.0; // Don't forget to set the alpha!
                trajectory_marker_list.color.r = 0.0;
                trajectory_marker_list.color.g = 0.0;
                trajectory_marker_list.color.b = 1.0;
                trajectory_markerPub.publish(trajectory_marker_list);
            }
        }
    }
}

void refrenceline_topic_callback(const car_msgs::trajectory::ConstPtr &refrenceline_msg){
    if(refrenceline_marker_list.points.size() < refrenceline_msg->total_path_length ){
        for(int i = 0;i < refrenceline_msg->total_path_length;i++){
            geometry_msgs::Point point_temp;
            point_temp.x = refrenceline_msg->trajectory_path[i].x;
            point_temp.y = refrenceline_msg->trajectory_path[i].y;
            point_temp.z = refrenceline_msg->trajectory_path[i].z;
            refrenceline_marker_list.points.push_back(point_temp);
        }
    }
}

void trajectory_topic_callback(const car_msgs::trajectory::ConstPtr &trajectory_msg){
    trajectory_msg_list = (*trajectory_msg);
    
    // trajectory_marker_list.points.clear();
    // for(int i = 0;i < trajectory_msg->total_path_length;i++){
    //     geometry_msgs::Point point_temp;
    //     point_temp.x = trajectory_msg->trajectory_path[i].x;
    //     point_temp.y = trajectory_msg->trajectory_path[i].y;
    //     point_temp.z = trajectory_msg->trajectory_path[i].z;
    //     trajectory_marker_list.points.push_back(point_temp);
    // }
}

int main(int argc, char **argv){
    ros::init(argc, argv, "showline");
    ros::NodeHandle n;

    localization_markerPub = n.advertise<visualization_msgs::Marker>("localization_Marker", 1000);
    ros::Subscriber localization_publisher = n.subscribe("localization_topic", 1, localization_topic_callback);

    refrenceline_markerPub = n.advertise<visualization_msgs::Marker>("refrenceline_Marker", 1000);
    ros::Subscriber refrenceline_publisher = n.subscribe("refrenceline_topic", 1, refrenceline_topic_callback);
    
    trajectory_markerPub = n.advertise<visualization_msgs::Marker>("trajectory_Marker", 1000);
    ros::Subscriber trajectory_publisher = n.subscribe("trajectory_topic", 1, trajectory_topic_callback);

    ros::spin();
    return 0;
}