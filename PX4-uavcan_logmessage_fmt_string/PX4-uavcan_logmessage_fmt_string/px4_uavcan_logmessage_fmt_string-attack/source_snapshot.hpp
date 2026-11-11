// PX4-Autopilot - src/drivers/uavcan/logmessage.hpp
//
// Vulnerable form, confirmed at v1.14.0, v1.15.0, v1.16.0, v1.17.0,
// release/1.17 and v1.18.0-rc1.
//
// Fixed form on main at ccf6adae010c7b128ad5ef126c63b6cbe715900e
// (pull request #29492).

// ---------------------------------------------------------------------------
// UavcanLogMessage::logmessage_sub_cb (v1.17.0)
// ---------------------------------------------------------------------------

		void logmessage_sub_cb(const uavcan::ReceivedDataStructure<uavcan::protocol::debug::LogMessage> &msg)
		{
			int px4_level = _PX4_LOG_LEVEL_INFO;

			switch (msg.level.value) {
			/* ... level mapping omitted ... */
			}

			char module_name_buffer[80];
			snprintf(module_name_buffer, sizeof(module_name_buffer), "uavcan:%d:%s", msg.getSrcNodeID().get(), msg.source.c_str());
			px4_log_modulename(px4_level, module_name_buffer, msg.text.c_str());   // line 88
		}

// ---------------------------------------------------------------------------
// main at ccf6adae (fixed)
// ---------------------------------------------------------------------------

			px4_log_modulename(px4_level, module_name_buffer, "%s", msg.text.c_str());   // line 88
