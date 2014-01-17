try
{
var canvasFactory = require('../index.js')
var http = require('http');


function RenderEndpoint(ctx, x,y, starttime, time)
{
	if (time> starttime)
	{
		ctx.lineCap = "square";
		ctx.strokeStyle = "rgb(200,0,0)";
		ctx.beginPath();
		ctx.moveTo(x-5,y);
		ctx.lineTo(x, y-5);
		ctx.lineTo(x+5, y);
		ctx.lineTo(x, y+5);	
		ctx.lineTo(x-5, y);
		ctx.fill();
		ctx.stroke();
	};
	
}

function RenderConnection(ctx,x1,y1, x2, y2, starttime, time)
{
	
	ctx.strokeStyle='rgba(255,255,255,0.5)';
	ctx.lineWidth = 4;
	if (time> starttime)
	{
		var dx = x2-x1;
		var dy = y2-y1;
		var L = Math.sqrt(dx*dx+ dy*dy);
		if (L>0)
		{
			var mtime = Math.min(1.0, (time-starttime)/(L/80));
			
			ctx.lineCap = "square";
			ctx.strokeStyle = "rgb(200,0,0)";
			dx*=mtime;
			dy*=mtime;
			ctx.beginPath();
			ctx.moveTo(x1, y1);
			ctx.lineTo(x1+ dx, y1+dy);
			ctx.stroke();
			if (mtime>=1.0) return true;
		};
	};
	return false;
};

http.createServer(function (req, res) 
{
	xsize = 1920/2;
	ysize = 1200/2;
    var ctx = canvasFactory(xsize,ysize, "argb");
    
	ctx.beginPath();
	ctx.rect(0,0,xsize,ysize);
	ctx.fillStyle = 'black';
	ctx.fill();
	ctx.strokeStyle = 'black';
	ctx.stroke();
	// voronoi
	ctx.strokeStyle='rgba(255,255,255,0.5)';
	ctx.lineWidth = 4;
	// edges
	 res.writeHead(200, {'Content-Type': 'text/html'});
	var output = "";
	var Connections = [[100,100,150,100,0,false]];
	var EndPoints = [];
	
	for (var i = 0;i<20;i++)
	{
		ctx.beginPath();
		ctx.rect(0,0,xsize,ysize);
		ctx.fillStyle = 'black';
		ctx.fill();

		console.log("frame "+ i);
		time = i*2;
		for (var j = 0;j<EndPoints.length;j++)
		{
			RenderEndpoint(ctx, EndPoints[j][0], EndPoints[j][1], EndPoints[j][2], time);
		};
		for (var j = 0;j<Connections.length;j++)
		{
			//console.log(Connections[j]);
			if (RenderConnection(ctx,Connections[j][0],Connections[j][1], Connections[j][2], Connections[j][3], Connections[j][4], time) && Connections[j][5] == false)
			{
				Connections[j][5] = true;
				var J = Math.random();
				if (J>0.5)
				{
					EndPoints.push([Connections[j][2],Connections[j][3],time]);
				}
				
				{
					var count = 1+ Math.floor(Math.random()*2);
					for (var q = 0;q<count;q++)
					{
						var J2 = Math.floor(Math.random()*7.9999);
						var dx = 40;
						var dy = 0;
						if (J2 == 1) {dx = 0;dy = -40};
						if (J2 == 2) {dx = -40;dy = 40};
						if (J2 == 3) {dx = 0;dy = 40};
						if (J2 == 4) {dx = -40;dy = -40};
						if (J2 == 5) {dx = -40;dy = 40};
						if (J2 == 6) {dx = 40;dy = -40};
						if (J2 == 7) {dx = 40;dy = 40};
						Connections.push([Connections[j][2],Connections[j][3], Connections[j][2]+dx,Connections[j][3]+dy,time,false]);
					};
				};
			}
		};
			
			
	  ctx.font = "bold 15px arial";
	  ctx.strokeStyle="white";
	  ctx.fillStyle="rgb(0,0,0)";
	  ctx.lineWidth=4;
	  ctx.strokeText("o3-canvas circuit anim",20, 20);
	  ctx.fillText("o3-canvas circuit anim",20, 20);
	  
	  };
	  var buf = ctx.pngBuffer();
	  output += '<img alt="Embedded Image" src="data:image/png;base64,'+buf.toBase64()+'"><br/>' 
	  res.end(output);
}).listen(4000, "127.0.0.1");
console.log('Server running at http://127.0.0.1:4000/');



}
catch (e)
{
console.log(e.message);
}