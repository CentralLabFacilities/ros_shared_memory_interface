/*
 * Software License Agreement (BSD License)
 *
 *  Copyright (c) 2014, Joshua James
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above
 *     copyright notice, this list of conditions and the following
 *     disclaimer in the documentation and/or other materials provided
 *     with the distribution.
 *
 *  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 *  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 *  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 *  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 *  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 *  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 *  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 *  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 *  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 *  POSSIBILITY OF SUCH DAMAGE.
 */

#include "ros/ros.h"
#include "shared_memory_interface/shared_memory_publisher.hpp"
#include "shared_memory_interface/shared_memory_subscriber.hpp"
#include <std_msgs/Int64.h>


int rcvdCount;
bool rcvdMsg;


void rttTxCallback(std_msgs::Int64& msg)
{
	ROS_INFO("Slave: rttTxCallback called.");
    rcvdCount = msg.data;
    rcvdMsg = true;
}

int main(int argc, char **argv)
{
  ros::init(argc, argv, "slave");
  ros::NodeHandle n;

  rcvdCount = 0;
  rcvdMsg = false;

  shared_memory_interface::Subscriber<std_msgs::Int64> sub;
  sub.subscribe("/rtt_tx", boost::bind(&rttTxCallback, _1));

  // shared_memory_interface::Publisher<std_msgs::Int64> pub;
  // pub.advertise("/rtt_rx");

  ros::Rate loop_rate(1000);
  int count = 0;

  std_msgs::Int64 msg;

  while (ros::ok())
  {
  	// ROS_INFO("Slave: begin loop cycle.");
    if (rcvdMsg)
    {
      msg.data = rcvdCount;
      ROS_INFO("Slave: publishing %i", msg.data);
      // if (!pub.publish(msg))
      // {
      //   ROS_ERROR("Slave: Failed to publish message. Aborting.");
      //   break;
      // }
    }
    loop_rate.sleep();
  }

  ros::spin();
  return 0;
}
