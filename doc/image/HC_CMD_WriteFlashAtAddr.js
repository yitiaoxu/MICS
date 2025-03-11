{
	signal: [
		{ name: "time", wave: "xPPPPPPPPP|PP|PPP|PPP", },
		{ name: "master", wave: "1375...5.1|...|32.|31", data: ["ENQ", "0x2A", "addr", "len-1", "STX", "data      ", "ETX",] },
		{ name: "(success) slave", wave: "1.........|41.|......", data: ["ACK",] },
		{ name: "(fail) slave", wave: "1.........|491|......", data: ["NAK", "err",] },
	]
}