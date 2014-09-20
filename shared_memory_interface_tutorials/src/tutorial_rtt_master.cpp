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
// #include <chrono>

#define NUM_SAMPLES 1000  // the number of samples over which to calculate the latency statistics

bool firstRound; // keep track of first round so that we can ignore it
int currCount;
int rcvdCount;
// std::chrono::high_resolution_clock::time_point sendTime;
ros::Time sendTime;

void rttRxCallback(std_msgs::Int64& msg)
{
  rcvdCount = msg.data;
  // ROS_INFO("Master: rttRxCallback: rcvdCount = %i, currCount = %i", rcvdCount, currCount);

  if (!firstRound && rcvdCount == currCount)
  {
    // Compute the time since the sequence number was sent.
    
    double rtt = (ros::Time::now() - sendTime).toSec();

    // std::chrono::nanoseconds timeSpan = std::chrono::duration_cast<std::chrono::nanoseconds>(
    //   std::chrono::high_resolution_clock::now() - sendTime);
    // double rtt = 1e9 / timeSpan.count();
    ROS_INFO("RTT: %f us", rtt * 1e6);
  }
  firstRound = false;
}

int main(int argc, char **argv)
{
  ros::init(argc, argv, "master");
  ros::NodeHandle n;

  firstRound = true;
  rcvdCount = currCount = 0;  // set rcvdCount equal to count to trigger send

  shared_memory_interface::Subscriber<std_msgs::Int64> sub;
  sub.subscribe("/rtt_rx", boost::bind(&rttRxCallback, _1));

  shared_memory_interface::Publisher<std_msgs::Int64> pub;
  pub.advertise("/rtt_tx");

  ros::Rate loop_rate(10);

  std_msgs::Int64 msg;

  double data[NUM_SAMPLES];

  while (ros::ok())
  {
    // ROS_INFO("Master: begin loop cycle.");
    if (rcvdCount == currCount)
    {
      msg.data = ++currCount;
      // ROS_INFO("Master: publishing %i", msg.data);
      // sendTime = std::chrono::high_resolution_clock::now();

      sendTime = ros::Time::now();
      if (!pub.publish(msg))
      {
        ROS_ERROR("Master: Failed to publish message. Aborting.");
        break;
      }      
    }
    
    loop_rate.sleep();
  }

  ros::spin();
  return 0;
}
