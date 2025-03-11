{
	signal: [
		{ name: "time", wave: "xPPP|PP|PP|PPP|PP", },
		{ name: "master", wave: "1371|............", data: ["ENQ", "cmd",] },
		{ name: "(success) slave", wave: "1...|41|38|32.|31", data: ["ACK", "SOH", "args      ", "STX", "data      ", "ETX",] },
		{ name: "(fail) slave", wave: "1...|491|........", data: ["NAK", "err",] },
	]
}