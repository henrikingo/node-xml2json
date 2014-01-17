node-libxml
===========

node-libxml is a W3C-DOM XML Library for NodeJS with XPath and namespace support. It is implemented using the C based LibXML2 and the Ajax.org O3 component system. This is the only W3C-DOM standards based XML api for NodeJS we are aware of so your code should work in both the browser and nodejs. This project is used in production in many NodeJS based backend projects at Ajax.org.

Installation
------------

To use this library simply install it using [npm](http://npmjs.org/):

    npm install libxml
    
Example
-------

```javascript
var xml = require("libxml");
var xmlData = "\
<?xml version=\"1.0\"?>\n\
<catalog>\n\
   <book id=\"bk101\" available=\"true\">\n\
      <author>Gambardella, Matthew</author>\n\
      <title>XML Developer's Guide</title>\n\
      <genre>Computer</genre>\n\
      <price>44.95</price>\n\
      <publish_date>2000-10-01</publish_date>\n\
      <description>An in-depth look at creating applications \n\
      with XML.</description>\n\
   </book>\n\
</catalog>";

var doc = xml.parseFromString(xmlData);
console.log("parsed document:\n" + doc.xml);

var xpathNodeList = elem.selectNodes("descendant-or-self::node()[@available='true']");
console.log("first element of the xpath querry: " + xpathNodeList[0].getAttribute("id"));
```

Continuous Integration status
-----------------------------

This project is tested with [Travis CI](http://travis-ci.org)
[![Build Status](https://secure.travis-ci.org/ajaxorg/node-libxml.png)](http://travis-ci.org/ajaxorg/node-libxml)

Contributing
------------

If you want to contribute to the source, please look at the main [o3 repository](http://github.com/ajaxorg/o3).
