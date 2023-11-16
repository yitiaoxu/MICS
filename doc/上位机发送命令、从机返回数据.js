{
	signal: [
		{ name: "time", wave: "xPP|PPPPPPPPPPPP|PP", },
		{ name: "master", wave: "137|1...........|..", data: ["ENQ", "cmd      ",] },
		{ name: "slave (success)", wave: "1..|.4145..8.42.|41", data: ["ACK", "SOH", "(optional args)", "length", "STX", "data", "ETX",] },
		{ name: "slave (handshake fail)", wave: "1..|.491........|..", data: ["NAK", "ERR",] },
	]
}
