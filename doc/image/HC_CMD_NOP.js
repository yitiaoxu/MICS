{
	signal: [
		{ name: "time", wave: "xPPP|PPP", },
		{ name: "master", wave: "1371|...", data: ["ENQ", "0x01",] },
		{ name: "(success) slave", wave: "1...|41.", data: ["ACK",] },
		{ name: "(fail) slave", wave: "1...|491", data: ["NAK", "err",] },
	]
}