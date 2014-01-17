/*
 * Copyright (C) 2010 Javeline BV
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option) any later
 * version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this library; if not, write to the Free Software Foundation, Inc., 51
 * Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

var canvasFactory = require('../index.js')
function oldtestshapes(ctx)
{
    ctx.fillStyle = "rgb(200,0,0)";
    ctx.fillRect (10, 10, 55, 50);

    ctx.fillStyle = "rgba(0, 0, 200, 0.5)";
    ctx.fillRect (30, 30, 55, 50);
    
    ctx.moveTo(20,40);
    ctx.lineTo(260,280);
    ctx.lineTo(20,280);
    ctx.closePath();
	
    ctx.moveTo(10,10);
    ctx.lineTo(300,300);
    ctx.lineTo(10,300);
    ctx.closePath();
    

    ctx.fill();
    ctx.stroke();
}

function drawtocontext(ctx)
{
	ctx.fillStyle = "rgb(200,200,200)";
	ctx.fillRect(0,0,300,300);
	
	ctx.strokeStyle= "rgb(0,0,0)";
	for (var i = 0;i<10;i++)
	{
		ctx.save();
		
		var color = Math.floor(200+Math.random()*55);
		
		ctx.fillStyle = "rgba(255,255,"+color+",0.4)";
		
		ctx.translate(50+Math.random()*200, Math.random()*200);
		var sx = Math.random()+ 0.5;
		var sy = sx;
		if (Math.random()<0.5) sx = -sx;
		ctx.scale(sx,sy);
		//ctx.lineWidth = 4;
		ctx.beginPath();  
		ctx.moveTo(75,25);  
		ctx.quadraticCurveTo(25,25,25,62.5);  
		ctx.quadraticCurveTo(25,100,50,100);  
		ctx.quadraticCurveTo(50,120,30,125);  
		ctx.quadraticCurveTo(60,120,65,100);  
		ctx.quadraticCurveTo(125,100,125,62.5);  
		ctx.quadraticCurveTo(125,25,75,25);  
		
		ctx.fill();
		ctx.stroke();  
		ctx.restore();
	};
}
/*
function draw() 
{
    var canvas = document.getElementById("canvas");
    var ctx = canvas.getContext("2d");
    drawtocontext(ctx);
}
*/
  
var http = require('http');
http.createServer(function (req, res) {
  var ctx = canvasFactory(300,300, "argb");
  drawtocontext(ctx);
  var buf = ctx.pngBuffer();
  var buf2 = ctx.jpgBuffer();
  res.writeHead(200, {'Content-Type': 'text/html'});
  res.end('<img alt="Embedded Image" src="data:image/png;base64,'+buf.toBase64()+'"><br>'
   +'<img alt="Embedded Image" src="data:image/png;base64,'+buf2.toBase64()+'">'
   );
}).listen(8124, "127.0.0.1");
console.log('Server running at http://127.0.0.1:8124/');



