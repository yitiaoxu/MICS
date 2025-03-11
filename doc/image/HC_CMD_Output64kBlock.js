{
	signal: [
		{ name: "time", wave: "xPPP|PP|PPPPPP|PP", },
		{ name: "master", wave: "1371|............", data: ["ENQ", "0x22", ] },
		{ name: "(success) slave", wave: "1...|41|38.32.|31", data: ["ACK", "SOH", "len-1", "STX", "data      ", "ETX",] },
		{ name: "(fail) slave", wave: "1...|491|........", data: ["NAK", "err",] },
	]
}