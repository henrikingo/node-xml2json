try
{
var canvasFactory = require('../index.js')
var http = require('http');
var voronoiFactory = require('./rhill-voronoi-core-min.js');

http.createServer(function (req, res) 
{
	if (req.url != "/") return;
	xsize = 100;
	ysize = 100;
    var ctx1 = canvasFactory(xsize,ysize, "argb");
    var ctx2 = canvasFactory(xsize,ysize, "argb");
 
			
	  ctx1.font = "bold 15px arial";
	  ctx1.strokeStyle="white";
	  ctx1.fillStyle="rgb(0,0,0)";
	  ctx1.lineWidth=4;
	  ctx1.strokeText("o3-canvas voronoi bubbles",20, 20);
	  ctx1.fillText("o3-canvas voronoi bubbles",20, 20);
	  ctx2.scale(0.5);
	  var buf = ctx1.pngBuffer();
	  var buf2 = ctx2.pngBuffer();
	  res.writeHead(200, {'Content-Type': 'text/html'});
	  res.end('<img alt="Embedded Image" src="data:image/png;base64,'+buf.toBase64()+'">' );
}).listen(4000, "127.0.0.1");
console.log('Server running at http://127.0.0.1:4000/');


}
catch (e)
{
console.log(e.message);
}