{
	signal: [
		{ name: "time", wave: "xPPP|PP|PPP|PP", },
		{ name: "master", wave: "1371|.........", data: ["ENQ", "0x2E", ] },
		{ name: "(success) slave", wave: "1...|41|32.|31", data: ["ACK", "STX", "deviceID      ", "ETX",] },
		{ name: "(fail) slave", wave: "1...|491|.....", data: ["NAK", "err",] },
	]
}