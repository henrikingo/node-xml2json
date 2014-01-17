try
{
var canvasFactory = require('../index.js')
var http = require('http');
var voronoiFactory = require('./rhill-voronoi-core-min.js');

http.createServer(function (req, res) 
{
xsize = 1920;
ysize = 1200;
  var ctx = canvasFactory(xsize,ysize, "argb");
  var voronoi = voronoiFactory();
  var bbox =  {xl:0,xr:xsize,yt:0,yb:ysize};
  
  for (var i =0 ;i<340;i++)
  {
	var x = Math.random()*xsize;
	var y = Math.random()*ysize;
	voronoi.addSites([{x:x,y:y}]);
  };
  var diagram = voronoi.compute(bbox);
		
	ctx.beginPath();
	ctx.rect(0,0,xsize,ysize);
	ctx.fillStyle = '#fff';
	ctx.fill();
	ctx.strokeStyle = 'black';
	ctx.stroke();
	// voronoi
	ctx.strokeStyle='rgba(255,255,255,0.5)';
	ctx.lineWidth = 4;
	// edges
	var edges = diagram.edges;
	var nEdges = edges.length;
	
	var sites = diagram.sites;
	var nSites = sites.length;
	for (var iSite=nSites-1; iSite>=0; iSite-=1) 
	{
		site = sites[iSite];
			ctx.rect(site.x-0.5,site.y-0.5,1,1);

	
				
//		ctx.stroke();
		var cell = diagram.cells[diagram.sites[iSite].id];
		if (cell !== undefined) 
		{
			var halfedges = cell.halfedges;
			var nHalfedges = halfedges.length;
			if (nHalfedges < 3) {return;}
			var minx = xsize;
			var miny = ysize;
			var maxx = 0;
			var maxy = 0;
			
			var v = halfedges[0].getStartpoint();
			ctx.beginPath();
			ctx.moveTo(v.x,v.y);
			
			for (var iHalfedge=0; iHalfedge<nHalfedges; iHalfedge++) 
			{
				v = halfedges[iHalfedge].getEndpoint();
				ctx.lineTo(v.x,v.y);
				if (v.x< minx) minx = v.x;
				if (v.y< miny) miny = v.y;
				if (v.x> maxx) maxx = v.x;
				if (v.y> maxy) maxy = v.y;
			}
			var C = Math.floor(Math.random()*128 + 127).toString();
			
			
			var midx = (maxx+minx)/2;
			var midy = (maxy+miny)/2;
			var R = 0;
			
			for (var iHalfedge=0; iHalfedge<nHalfedges; iHalfedge++) 
			{
				v = halfedges[iHalfedge].getEndpoint();
				var dx = v.x - site.x;
				var dy = v.y - site.y;
				var newR = Math.sqrt(dx*dx + dy*dy);
				if (newR >R) R = newR;
			}
			midx = site.x;
			midy = site.y;
			var radgrad = ctx.createRadialGradient(midx+R*0.3,midy-R*0.3,0,midx,midy,R);
			radgrad.addColorStop(0, "#09760b");
			radgrad.addColorStop(1.0, "black");
		
		
			ctx.fillStyle = radgrad;
			ctx.fill();
			
			var radgrad2 = ctx.createRadialGradient(midx-R*0.5,midy+R*0.5,R*0.1,midx,midy,R);
			radgrad2.addColorStop(0, "rgba(255,255,255,0.5)");
			radgrad2.addColorStop(0.04, "rgba(255,255,255,0.3)");
			radgrad2.addColorStop(0.05, "rgba(255,255,255,0)");
		
		
			ctx.fillStyle = radgrad2;
			ctx.fill();
			
			var lingrad = ctx.createLinearGradient(minx, site.y, minx+100, site.y-20);
			lingrad.addColorStop(0.0, "rgba(255,255,255,0.5)");
			lingrad.addColorStop(0.2, "rgba(255,255,255,0.2)");
			lingrad.addColorStop(1.0, "rgba(255,255,255,0)");
			ctx.fillStyle = lingrad;
			ctx.fill();
			
		}

	}
		
	if (nEdges) 
	{
			var edge, v;
			ctx.beginPath();
			for (var iEdge=nEdges-1; iEdge>=0; iEdge-=1) {
				edge = edges[iEdge];
				v = edge.va;
				ctx.moveTo(v.x,v.y);
				v = edge.vb;
				ctx.lineTo(v.x,v.y);
				}
			ctx.stroke();
			}			
			
  ctx.font = "bold 15px arial";
  ctx.strokeStyle="white";
  ctx.fillStyle="rgb(0,0,0)";
  ctx.lineWidth=4;
  ctx.strokeText("o3-canvas voronoi bubbles",20, 20);
  ctx.fillText("o3-canvas voronoi bubbles",20, 20);
  
  var buf = ctx.pngBuffer();
  res.writeHead(200, {'Content-Type': 'text/html'});
  res.end('<img alt="Embedded Image" src="data:image/png;base64,'+buf.toBase64()+'">' );
}).listen(4000, "127.0.0.1");
console.log('Server running at http://127.0.0.1:4000/');



}
catch (e)
{
console.log(e.message);
}