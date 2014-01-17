/**
 * Copyright 2013 Ricard Aspeljung.
 * Copyright 2014 MongoDB Inc. (Author: Henrik Ingo.)
 *
 * Note: This software is published as open source for purposes of sharing 
 * MongoDB examples and use cases. This is not an official product of MongoDB Inc
 * and there's no support available.
 *
 * Crest, with support for inserting XML data as well as JSON.
 */

var fs = require("fs"),
  mongodb = require("mongodb"),
  restify = module.exports.restify = require("restify");

var DEBUGPREFIX = "DEBUG: ";

var config = {
  "db": {
    "port": 27017,
    "host": "localhost"
  },
  "server": {
    "port": 3500,
    "address": "0.0.0.0"
  },
  "flavor": "mongodb",
  "debug": true
};

var debug = module.exports.debug = function (str) {
  if (config.debug) {
    console.log(DEBUGPREFIX + str);
  }
};

// uses debug() from above
var xml2json = module.exports.xml2json = require("xml2json");


try {
  config = JSON.parse(fs.readFileSync(process.cwd() + "/config.json"));
} catch (e) {
  debug("Error loading config.json file: " + e );
  debug("Fall back to default config.");
}

module.exports.config = config;

var server = restify.createServer({
  name: "crest"
});

server.acceptable = ['application/json', 'application/xml'];
server.use(restify.acceptParser(server.acceptable));
server.use(xml2json.rawBodyParser);  /* If given xml or other non-json input, copy it to a readable place */
server.use(restify.bodyParser());
server.use(xml2json.xmlToJsonParams); /* Translate application/xml input into json, after which code paths are identical for both cases */
server.use(restify.fullResponse());
server.use(restify.queryParser());
server.use(restify.jsonp());
module.exports.server = server;

require('./lib/rest');

server.listen(config.server.port, function () {
  console.log("%s listening at %s", server.name, server.url);
});
