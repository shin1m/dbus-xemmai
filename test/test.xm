system = Module("system"
print = system.out.write_line
dbus = Module("dbus"
assert = @(x) x || throw Throwable("Assertion failed."

dbus.main(@
	connection = dbus.Connection(dbus.BusType.SESSION
	message = dbus.Message(
		"org.freedesktop.DBus"
		"/org/freedesktop/DBus"
		"org.freedesktop.DBus.Introspectable"
		"Introspect"
	try
		reply = connection.send_with_reply(message
		result = reply(
		result.get_type() == dbus.MESSAGE_TYPE_ERROR && throw Throwable(result.get().__string(
		a = result.get(
		print(a
		assert(a.size() > 0
		assert(a[0].size() > 0
	finally
		message.release(
		reply !== null && reply.release(
		result !== null && result.release(
