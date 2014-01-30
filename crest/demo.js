var printResult = function(op, err, result) { 
    if ( err ) {
        console.log( "\n" + op + " failed with error: " + err );
        console.log( result );
    }
    else {
        console.log( "\n" + op + " succeeded: " );
        console.log( result );
    }
};




var MongoClient = require("mongodb").MongoClient;
MongoClient.connect("mongodb://", function (err, db) {
var collection = db.collection("dublindemo");
collection.drop();





var doc = { person :
            { firstname : "Henrik",
              lastname  : "Ingo",
              shoesize  : 44 
            }
          };
collection.insert( doc, function (err, docs) {
    printResult( "insert", err, doc );





var query = { "person.firstname" : "Henrik" };

collection.findOne( query, function(err, doc){
    printResult( "query", err, doc );





















process.exit();
        }); // find
    }); // insert
}); // connect








