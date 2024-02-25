system = Module("system"
print = system.out.write_line
dbus = Module("dbus"
assert = @(x) x || throw Throwable("Assertion failed."

dbus.main(@
	connection = dbus.Connection(dbus.BusType.SESSION
	call = @(message)
		try
			reply = connection.send_with_reply(message
			result = reply(
			result.get_type() == dbus.MESSAGE_TYPE_ERROR && throw Throwable(result.get().__string(
			xs = result.get(
			print(xs
			assert(xs.size() > 0
			xs[0]
		finally
			message.release(
			reply && reply.release(
			result && result.release(
	assert(call(dbus.Message(
		"org.freedesktop.DBus"
		"/org/freedesktop/DBus"
		"org.freedesktop.DBus.Introspectable"
		"Introspect"
	)).size() > 0
	method = @(name) dbus.Message(
		"org.freedesktop.DBus"
		"/org/freedesktop/DBus"
		"org.freedesktop.DBus"
		name
	assert(!call(method("NameHasOwner").string("xemmai.XEMMAI"
	assert(call(method("RequestName").string("xemmai.XEMMAI").uint32(0)) == 1
	assert(call(method("NameHasOwner").string("xemmai.XEMMAI"
	assert(call(method("ReleaseName").string("xemmai.XEMMAI")) == 1
	assert(!call(method("NameHasOwner").string("xemmai.XEMMAI"
