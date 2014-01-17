/**
 * Copyright 2013 Ricard Aspeljung. All Rights Reserved.
 *
 * rest.js
 * crest
 */

var MongoClient = require("mongodb").MongoClient,
  BSON = require("mongodb").BSONPure,
  server = module.parent.exports.server,
  config = module.parent.exports.config,
  debug = module.parent.exports.debug,
  restify = module.parent.exports.restify,
  xml2json = module.parent.exports.xml2json,
  util = require("./util").util;

debug("rest.js is loaded");


/**
 * Query
 */
function handleGet(req, res, next) {
  debug("GET-request recieved");
  console.log(req.query);
  var query;
  // Providing an id overwrites giving a query in the URL
  if (req.params.id) {
    query = {
      '_id': new BSON.ObjectID(req.params.id)
    };
  } else {
    query = req.query.query ? util.parseJSON(req.query.query, next, restify) : {};
  }
  var options = req.params.options || {};

  var test = ['limit', 'sort', 'fields', 'skip', 'hint', 'explain', 'snapshot', 'timeout'];

  var v;
  for (v in req.query) {
    if (test.indexOf(v) !== -1) {
      options[v] = req.query[v];
    }
  }

  if (req.body.toString().length > 0) {
    var body = req.body.split(",");
    if (body[0]) {
      query = util.parseJSON(body[0], next);
    }
    if (body[1]) {
      options = util.parseJSON(body[1], next);
    }
  }

  MongoClient.connect(util.connectionURL(req.params.db, config), function (err, db) {
    db.collection(req.params.collection, function (err, collection) {
      collection.find(query, options, function (err, cursor) {
        cursor.toArray(function (err, docs) {
          var result = [];
          if (req.params.id) {
            if (docs.length > 0) {
              result = util.flavorize(docs[0], "out");
              res.json(result, {'content-type': 'application/json; charset=utf-8'});
            } else {
              res.json(404);
            }
          } else {
            docs.forEach(function (doc) {
              result.push(util.flavorize(doc, "out"));
            });
            res.json(result, {'content-type': 'application/json; charset=utf-8'});
          }
          db.close();
        });
      });
    });
  });
}

server.get('/:db/:collection/:id?', handleGet);
server.get('/:db/:collection', handleGet);


/**
 * Insert
 *
 * Supports JSON and XML, use correct Content-type header.
 */
server.post('/:db/:collection', function (req, res) {
  debug("POST-request recieved");
  debug( "db=" + req.params.db + " , collection=" + req.params.collection );
  debug( "content-type= " + req.headers["content-type"] );

  var jsObj = Array.isArray(req.params) ? util.cleanParams(req.params[0]) : util.cleanParams(req.params);

  // xml has been converted to json by prior middleware handlers, but content-type remains the original
  if (req.headers["content-type"] == "application/json" || req.headers["content-type"] == "application/xml") { 
    MongoClient.connect(util.connectionURL(req.params.db, config), function (err, db) {
      debug( "MongoClient.connect callback( err=" + err + " , db = " + db + " )" );
      var collection = db.collection(req.params.collection);
      collection.insert( jsObj, function (err, docs) {
        res.header('Location', '/' + req.params.db + '/' + req.params.collection + '/' + docs[0]._id.toHexString());
        res.set('content-type', 'application/json; charset=utf-8');
        res.json(201, {"ok": 1});
        db.close();
      });
    });
  } else {
    res.set('content-type', 'application/json; charset=utf-8');
    res.json(200, {"ok": 0, "message" : "We only support application/json and application/xml"});
  }
});

/**
 * Update
 */
server.put('/:db/:collection/:id', function (req, res) {
  debug("PUT-request recieved");
  var spec = {
    '_id': new BSON.ObjectID(req.params.id)
  };
  MongoClient.connect(util.connectionURL(req.params.db, config), function (err, db) {
    db.collection(req.params.collection, function (err, collection) {
      collection.update(spec, util.cleanParams(req.params), true, function (err, docs) {
        res.set('content-type', 'application/json; charset=utf-8');
        res.json({"ok": 1});
      });
    });
  });
});

/**
 * Delete
 */
server.del('/:db/:collection/:id', function (req, res) {
  debug("DELETE-request recieved");
  var spec = {
    '_id': new BSON.ObjectID(req.params.id)
  };
  MongoClient.connect(util.connectionURL(req.params.db, config), function (err, db) {
    db.collection(req.params.collection, function (err, collection) {
      collection.remove(spec, function (err, docs) {
        res.set('content-type', 'application/json; charset=utf-8');
        res.json({"ok": 1});
        db.close();
      });
    });
  });
});


