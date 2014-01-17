var util = require('util');
var dgram = require('dgram');

var server = dgram.createSocket("udp4");

server.on("message", function (msg, rinfo) {
  util.puts(msg+"\n");
});

server.on("listening", function () {
  util.puts("Log server listening " + server.address().address);
})

server.bind(3333);