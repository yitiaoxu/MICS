{
	signal: [
		{ name: "time", wave: "xPP||PP", },
		{ name: "master", wave: "1371|..", data: ["ENQ", "cmd",] },
		{ name: "(handshake success) slave", wave: "1..|41.", data: ["ACK",] },
		{ name: "(handshake fail) slave", wave: "1..|491", data: ["NAK", "err",] },
	]
}