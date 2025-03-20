{
	signal: [
		{ name: "time", wave: "xPP|PPPPPPPPPPPP|PP", },
		{ name: "master", wave: "137|1.|45..8.42.|41", data: ["ENQ", "cmd      ", "SOH", "(optional args)", "length", "STX", "data", "ETX",] },
		{ name: "slave (success)", wave: "1..|.41.........|..", data: ["ACK",] },
		{ name: "slave (handshake fail)", wave: "1..|.491........|..", data: ["NAK", "ERR",] },	]
}
