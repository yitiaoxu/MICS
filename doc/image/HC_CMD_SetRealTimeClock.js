{
	signal: [
		{ name: "time", wave: "xP......|..." },
		{ name: "master", wave: "1375...1|...", data: ["ENQ", "0x09", "timestamp"] },
		{ name: "(success) slave", wave: "1.......|41.", data: ["ACK"] },
		{ name: "(fail) slave", wave: "1.......|491", data: ["NAK", "err"] },
	]
}