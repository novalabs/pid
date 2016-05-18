/* COPYRIGHT (c) 2016 Nova Labs SRL
 *
 * All rights reserved. All use of this software and documentation is
 * subject to the License Agreement located in the file LICENSE.
 */

#pragma once

#include <Core/MW/Publisher.hpp>
#include <Core/MW/Subscriber.hpp>
#include <Core/MW/CoreNode.hpp>
#include <Core/MW/Callback.hpp>

#include <Configuration.hpp>

#include <common_msgs/Float32.hpp>
#include <pid/PIDNodeConfiguration.hpp>
#include <pid/PID.hpp>

namespace pid {
   class PIDNode:
      public Core::MW::CoreNode,
                       PID
   {
public:
      PIDNode(
         const char*                    name,
         Core::MW::Thread::PriorityEnum priority = Core::MW::Thread::PriorityEnum::NORMAL
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
         Core::MW::Node*             node
      );

      static bool
      measure_callback(
         const common_msgs::Float32& msg,
         Core::MW::Node*             node
      );


private:
      Core::MW::Subscriber<common_msgs::Float32, 5> _setpoint_subscriber;
      Core::MW::Subscriber<common_msgs::Float32, 5> _measure_subscriber;
      Core::MW::Publisher<common_msgs::Float32>     _output_publisher;
      float _setpoint;
      Core::MW::Time _setpoint_timestamp;
      float          _measure;
   };
}
