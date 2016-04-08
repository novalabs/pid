#include <Module.hpp>

#include <pid/PIDNode.hpp>
#include <Core/Utils/Math/Constants.hpp>
#include <Core/Utils/Math/Conversions.hpp>

namespace pid {
	PIDNode::PIDNode(
			const char*                    name,
			Core::MW::Thread::PriorityEnum priority
	) :
		CoreNode::CoreNode(name, priority),
		_setpoint(0),
		_measure(0)
	{
		_workingAreaSize = 512;
	}

	PIDNode::~PIDNode()
	{
		teardown();
	}

	bool
	PIDNode::onConfigure()
	{
		config(configuration.kp, configuration.ti, configuration.td, configuration.ts, configuration.min, configuration.max);

		return true;
	}

	bool
	PIDNode::onPrepareMW()
	{
		_setpoint_subscriber.set_callback(PIDNode::setpoint_callback);
		_measure_subscriber.set_callback(PIDNode::measure_callback);

		this->subscribe(_setpoint_subscriber, configuration.setpoint_topic);
		this->subscribe(_measure_subscriber, configuration.measure_topic);

		this->advertise(_output_publisher, configuration.output_topic);

		return true;
	}

	bool
	PIDNode::onLoop()
	{
		if (!this->spin(Configuration::SUBSCRIBER_SPIN_TIME)) {
			Module::led.toggle();
		}

		if ((this->_setpoint_timestamp + configuration.timeout) > Core::MW::Time::now()) {
			common_msgs::Float32* msgp;

			if (_output_publisher.alloc(msgp)) {
				msgp->value = configuration.idle;
				_output_publisher.publish(*msgp);
			}
		}

		return true;
	} // PID::onLoop

	bool
	PIDNode::setpoint_callback(
			const common_msgs::Float32& msg,
			Core::MW::Node*             node
	)
	{
		PIDNode* _this = static_cast<PIDNode*>(node);

		_this->_setpoint = msg.value;
		_this->_setpoint_timestamp = Core::MW::Time::now();
		_this->set(_this->_setpoint);

		return true;
	}

	bool
	PIDNode::measure_callback(
			const common_msgs::Float32& msg,
			Node*                       node
	)
	{
		PIDNode* _this = static_cast<PIDNode*>(node);
		common_msgs::Float32* msgp;

		_this->_measure = msg.value;

		if (_this->_output_publisher.alloc(msgp)) {
			msgp->value = _this->update(_this->_measure);
			_this->_output_publisher.publish(*msgp);
		}

		return true;
	}
}