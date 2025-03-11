{
	signal: [
		{ name: "time", wave: "xPPP||PPP|PPP|PP", },
		{ name: "master", wave: "1375|1...|32.|31", data: ["ENQ", "cmd", "args      ", "STX", "data      ", "ETX",] },
		{ name: "(success) slave", wave: "1....|41.|......", data: ["ACK",] },
		{ name: "(fail) slave", wave: "1....|491|......", data: ["NAK", "err",] },
	]
}