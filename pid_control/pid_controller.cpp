#include "ros/ros.h"
#include "ros/console.h"
#include "sensor_msgs/Imu.h"
#include "geometry_msgs/Twist.h"


double clamp(double num, double min_value, double max_value)
{
    return std::max(std::min(num, max_value), min_value);
}

double map(double x,double max_value, double min_value , double min , double max)
{
    return min + (min - max) * (x - min_value) / (min_value - max_value);
}

class myController
{
    public:

        double i = 0 ;
        double oldp = 0;
        double oldi = 0;
        double p = 0;
        double d = 0;

        myController(void)
        {
            ros::NodeHandle nh;
            ros::Rate loop_rate(100);

            imu_sub = nh.subscribe("imu",5,&myController::call_back,this);
            vel_pub = nh.advertise<geometry_msgs::Twist>("cmd_vel",10,false);
        }

        void call_back(const sensor_msgs::Imu::ConstPtr& msg)
        {
            double angle = msg->orientation.x + 0.5;
            //ROS_DEBUG("angle = %", angle);
            std::cout << angle << std::endl;
            geometry_msgs::Twist res;
            res.angular.z = -map(pwm(angle+0.5),-250,250,-15,+15);
            std::cout << pwm(angle+0.5) << std::endl;
            vel_pub.publish(res);
        }

    private:
        ros::Subscriber imu_sub;
        ros::Publisher vel_pub;

        double pwm(float angle, float kp = 1000, float ki = 1 , float kd = 10)
        {
            p = angle * kp;
            i = i + (p * 0.05);
            i = i + ((i - oldi) * 2);
            i = clamp (i , -250 , +250);
            d = p - oldp;

            oldp = p;
            oldi = i;

            return clamp(p + i * ki + d * kd,-250,+250);
        }

};

int main(int argc, char **argv)
{
    ros::init(argc,argv,"controller");
    
    myController ctr;

    ros::spin();

}