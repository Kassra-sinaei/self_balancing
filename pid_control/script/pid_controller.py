#!/usr/bin/env python

import rospy
import numpy as np
from sensor_msgs.msg import Imu
from geometry_msgs.msg import Twist
from geometry_msgs.msg import Vector3


def clamp(num, min_value, max_value):
   return max(min(num, max_value), min_value)

def callback(message):
    angle = message.orientation.x
    print (angle+0.5)
    res = Twist()
    #res.linear.x = 1
    res.angular.z = -clamp(pwm(angle+0.5),-20,+20)
    publisher = rospy.Publisher('/cmd_vel',Twist, queue_size=10)
    publisher.publish(res)
    loop_rate.sleep()

def pwm(angle, kp = 1000, ki = 1 , kd = 10):
    global i
    global oldi
    global oldp
    
    p = angle * kp
    i = i + (p * 0.05)
    i = i + ((i - oldi) * 2)
    i = clamp(i,-250,+250)
    d = p - oldp
    res = p + i * ki + d * kd

    oldp = p
    oldi = i

    return res

if __name__ == "__main__":
    oldi = 0
    oldp = 0
    i = 0

    rospy.init_node('controller' , anonymous=True)
    loop_rate = rospy.Rate(100)
    while not rospy.is_shutdown():
        rospy.Subscriber('imu', Imu , callback)
        rospy.spin()