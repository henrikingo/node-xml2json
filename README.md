Experimental Notice
===================

Note: You have checked out an experimental branch. This thing will do the
same as the master branch, but instead of doing the xml translation as a 
node.js module, we want to move it into the mongodb connector. The node.js
code will then simply feed xml into the mongodb connector.

Installation
============

We use a lightweight Node.js middleware layer to expose a
HTTP REST interface of data in MongoDB. See install/README.txt on how to 
install Node.js and MongoDB.

This repository has the needed Node plugins already included. Several plugins
are binary and need to be compiled for your OS and HW. When you have
installed node, do this:

    cd crest/
    npm rebuild

Using
=====

When everything is installed, start the http rest server:

   bin/crest

POST
----

You can now upload data, let's try JSON first:

   POST:
   curl -i -d '{ "a" : "b", "n" : 5 }' -H "Content-Type: application/json" http://localhost:3500/test/example
   
Take note of the Location: header, as it contains the _id you can use to query 
the object:

    HTTP/1.1 201 Created
    Location: /test/example/52d83cbbd88cb41f0c000001
    Content-Type: application/json

If _application/xml_ is given as content-type, the xml is automatically converted 
to json and inserted as a normal MongoDB document:
   
    curl -i -d @"sample_data/pubs.trinity.xml" -H "Content-Type: application/xml" http://localhost:3500/test/example

GET
---

Now we can query for the data we just inserted:

   GET (all):
   curl -i http://localhost:3500/test/example

   GET (single by id):
   curl -i http://localhost:3500/test/example/52d83cbbd88cb41f0c000001

To do more interesting queries, we would create indexes and query for keys:

   mongo
   mongo> db.example.ensureIndex( { "n" : 1 } )

We can query it over HTTP:

   GET: http://localhost:3500/test/example?query={ "n" : 5 }

Note: You need to urlencode the above. curl won't work, but just copy it into a 
      browser and enjoy!

Note: Currently, any data that comes in as xml is strings. Even if you'd think 
      of it as a number, you need to quote it to make it a json string. 
      Plan to fix: 
                   1) If using a schema, I'm hoping libxml spits out correct types?
                   2) add a separate middleware filter that will do 
                      if v == Number(v) and convert if yes.
                   3) allow user supplied middleware, for example convert OSM
                      x and y coordinates to proper geoJSON.

Credits
=======

The XML to JSON conversion is done by: https://github.com/henrikingo/xml2json

For more info on crest and restify which provide the cool http plumbing, see:
https://github.com/cordazar/crest
http://mcavage.me/node-restify/

Authors, Copyrights and an Important Notice
===========================================

Crest author:
Ricard Aspeljung.

Xml2json integration to Crest:
Henrik Ingo, MongoDB Inc

Xml2json originally authored by:
Stefan Goessner

See each individual plugin for their respective authors
 *
Note: This software is published as open source for purposes of sharing 
MongoDB examples and use cases. This is not an official product of MongoDB Inc
and there's no support available.

