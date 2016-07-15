/* COPYRIGHT (c) 2016 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#pragma once

#include <core/mw/Publisher.hpp>
#include <core/mw/Subscriber.hpp>
#include <core/mw/CoreNode.hpp>
#include <core/mw/Callback.hpp>

#include <Configuration.hpp>

#include <common_msgs/Float32.hpp>
#include <pid/PIDNodeConfiguration.hpp>
#include <pid/PID.hpp>

namespace pid {
class PIDNode:
   public core::mw::CoreNode,
                    PID
{
public:
   PIDNode(
      const char*                name,
      core::os::Thread::Priority priority = core::os::Thread::PriorityEnum::NORMAL
   );
   virtual
   ~PIDNode();

public:
   PIDNodeConfiguration configuration;

private:
   bool
   onConfigure();

   bool
   onPrepareMW();

   bool
   onLoop();

   static bool
   setpoint_callback(
      const common_msgs::Float32& msg,
      core::mw::Node*             node
   );

   static bool
   measure_callback(
      const common_msgs::Float32& msg,
      core::mw::Node*             node
   );


private:
   core::mw::Subscriber<common_msgs::Float32, 5> _setpoint_subscriber;
   core::mw::Subscriber<common_msgs::Float32, 5> _measure_subscriber;
   core::mw::Publisher<common_msgs::Float32>     _output_publisher;
   float _setpoint;
   core::os::Time _setpoint_timestamp;
   float          _measure;
};
}
