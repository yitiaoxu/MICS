{
	signal: [
		{ name: "time", wave: "xP..|........" },
		{ name: "master", wave: "1371|...|....", data: ["ENQ", "cmd"] },
		{ name: "(success) slave", wave: "1...|41.|3231", data: ["ACK", "STX", "stat", "ETX"] },
		{ name: "(fail) slave", wave: "1...|491|....", data: ["NAK", "err"] },
	]
}