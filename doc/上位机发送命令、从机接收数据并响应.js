{
	signal: [
		{ name: "time", wave: "xPP|PPPPPPPPPPPP|PPPPP", },
		{ name: "master", wave: "137|1.|45..8.42.|41...", data: ["ENQ", "cmd      ", "SOH", "(optional args)", "length", "STX", "data", "ETX",] },
		{ name: "slave (success)", wave: "1..|.41.........|..41.", data: ["ACK", "ACK",] },
		{ name: "slave (handshake fail)", wave: "1..|.491........|.....", data: ["NAK", "ERR",] },
		{ name: "slave (protocol error)", wave: "1..|.41.........|..491", data: ["ACK", "NAK", "ERR",] },
	]
}
