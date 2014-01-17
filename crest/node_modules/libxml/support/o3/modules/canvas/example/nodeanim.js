//var createContext = require('../index.js')
var createContext = require('../lib/o3-canvas')
 
var color  = "rgb(200,200,200)";
function drawAnim1(ctx)
{
	ctx.fillStyle = color;
	ctx.fillRect(0,0,300,300);
	
	ctx.fillStyle = "rgba(0,0,200,0.5)";
	ctx.strokeStyle= "rgba(0,0,0,0.5)";
	var n = Date.now()/1000;
	var xs = 20, ys = 20, cx = xs/2, cy=ys/2, xd = 300/xs, yd = 300/ys;
	for(var x = -cx, xt = 0; x < cx; x++, xt+=xd){
		for(var y = -cy, yt = 0; y < cy; y++, yt+=yd){
		    var d = Math.sin(Math.sqrt(x*x+y*y)/2-n)
            ctx.fillStyle = "rgba(0,"+(xt>255?255:xt)+","+parseInt(d*128+128)+",0.5)";
		    var sz = d*cx*0.6+cx*0.6;
			ctx.fillRect (xt-sz, yt-sz, 2*sz, 2*sz);
    		ctx.strokeRect (xt-sz, yt-sz, 2*sz, 2*sz);
		}
	}
}
function drawAnim2(ctx)
{
    ctx.fillStyle = color;
	ctx.fillRect(0,0,300,300);
	
	ctx.fillStyle = "rgba(0,0,200,0.5)";
	ctx.strokeStyle= "rgba(100,100,100,0.5)";
	var n = Date.now()/1000;
	var xs = 10, ys = 10, cx = xs/2, cy=ys/2, xd = 300/xs, yd = 300/ys;
	for(var xt = 0; xt<300; xt+=xd){
		for(var yt = 0; yt<300; yt+=yd){
		    ctx.moveTo(xt,0);ctx.lineTo(xt,300);
		    ctx.moveTo(0,yt);ctx.lineTo(300,yt);
		}
	}
	ctx.stroke();
	ctx.strokeWidth = 5;
	ctx.strokeStyle = "rgba(0,0,0,0.5)";
    ctx.fillStyle= "rgba(0,174,239,0.8)";
    ctx.beginPath();
    ctx.moveTo(0,300);
    for(var xt = 0; xt<300; xt+=1)
        ctx.lineTo(xt, 100+10*Math.sin(xt/32+n+Math.sin(xt/64+n))*5+Math.sin(xt/48));
    ctx.lineTo(300,300);
    ctx.lineTo(0,300);
    ctx.fill();
    ctx.stroke();
    ctx.beginPath();
    ctx.fillStyle = "rgba(242,0,144,0.7)"; n = n+2;
    for(var xt = 0; xt<300; xt+=1)
        ctx.lineTo(xt, 150+10*Math.sin(xt/32+n+Math.sin(xt/64+n))*5+Math.sin(xt/48));
    ctx.lineTo(300,300);
    ctx.lineTo(0,300);
    ctx.fill();
    ctx.stroke();
    ctx.beginPath();
    ctx.fillStyle = "rgba(255,254,0,0.7)"; n = n+4;
    for(var xt = 0; xt<300; xt+=1)
        ctx.lineTo(xt, 200+10*Math.sin(xt/32+n+Math.sin(xt/64+n))*5+Math.sin(xt/48));
    ctx.lineTo(300,300);
    ctx.lineTo(0,300);
    ctx.fill();
    ctx.stroke();    
}

  
var http = require('http');
var t = 0;
http.createServer(function (req, res) {
    
  var ctx1 = createContext(300,300, "argb");
  var ctx2 = createContext(300,300, "argb");
  
  drawAnim1(ctx1);
  drawAnim2(ctx2);
  console.log("Serving Canvas Images" +(t++));
  var buf = ctx1.pngBuffer();
  var buf2 = ctx2.jpgBuffer();
  res.writeHead(200, {'Content-Type': 'text/html'});
  res.end("<title>CanvasDemo</title><meta http-equiv='refresh' content='0.3;'>"+
    "<img alt='Embedded Image' src='data:image/png;base64,"+buf.toBase64()+"'>"+
     "<img alt='Embedded Image' src='data:image/png;base64,"+buf2.toBase64()+"'>");
}).listen(4000, "127.0.0.1");
console.log('Server running at http://127.0.0.1:4000/');



