var createContext = require('../index.js')
var fs = require('fs');
var Buffer = require('buffer').Buffer;

var tests = {};

tests['blender test'] = function(ctx)
{
  var lingrad = ctx.createLinearGradient(0,0,20,0);
  lingrad.addColorStop(0, 'white');
  lingrad.addColorStop(1, 'rgba(255,255,255,0)');
  ctx.fillStyle = lingrad;
  ctx.fillRect(0,0,20,6);
  ctx.fillRect(0,2,20,4);
  ctx.fillRect(0,4,20,2);

  var lingrad = ctx.createLinearGradient(0,0,70,0);
  lingrad.addColorStop(0, 'white');
  lingrad.addColorStop(1, 'rgba(255,255,255,0)');
  ctx.fillStyle = lingrad;

  ctx.fillRect(0,10,70,40);
  ctx.fillRect(0,20,70,30);
  ctx.fillRect(0,30,70,20);
  ctx.fillRect(0,40,70,10);

  };

tests['thin color lines'] = function(ctx)
{
  // this test shows how well the antialiasing system deals with very thin, NEARLY aligned lines.
  ctx.lineWidth = 1;
  ctx.strokeStyle = "rgb(255,0,0)";
  ctx.beginPath();
  ctx.moveTo(0,50);
  ctx.lineTo(200,52);
  ctx.moveTo(210,0);
  ctx.lineTo(211,200);
  ctx.stroke();

  ctx.strokeStyle = "rgb(255,255,0)";
  ctx.beginPath();
  ctx.moveTo(0,60);
  ctx.lineTo(200,62);
  ctx.moveTo(220,0);
  ctx.lineTo(221,200);
  ctx.stroke();

  ctx.strokeStyle = "rgb(0,255,0)";
  ctx.beginPath();
  ctx.moveTo(0,70);
  ctx.lineTo(200,72);
  ctx.moveTo(230,0);
  ctx.lineTo(231,200);
  ctx.stroke();

  ctx.strokeStyle = "rgb(0,0,255)";
  ctx.beginPath();
  ctx.moveTo(0,80);
  ctx.lineTo(200,82);
  ctx.moveTo(240,0);
  ctx.lineTo(241,200);
  ctx.stroke();
};

tests['clearRect()'] = function(ctx)
{
// clearrect rectangles should be cleared to transparent black. vertices need to be transformed according to the global transformation matrix.
  ctx.fillRect(25,25,100,100);
  ctx.clearRect(45,45,60,60);
  ctx.fillRect(50,50,50,50);
};

tests['strokeRect()'] = function(ctx){
  ctx.fillRect(25,25,100,100);
  ctx.clearRect(45,45,60,60);
  ctx.strokeRect(50,50,50,50);
};

tests['lineTo()'] = function(ctx){
  // Filled triangle
  ctx.beginPath();
  ctx.moveTo(25.5,25);
  ctx.lineTo(105,25);
  ctx.lineTo(25,105);
  ctx.fill();

  // Stroked triangle
  ctx.beginPath();
  ctx.moveTo(125,125);
  ctx.lineTo(125,45);
  ctx.lineTo(45,125);
  ctx.closePath();
  ctx.stroke();
};

tests['arc()'] = function(ctx){
  ctx.beginPath();
  ctx.arc(75,75,50,0,Math.PI*2,true); // Outer circle
  ctx.moveTo(110,75);
  ctx.arc(75,75,35,0,Math.PI,false);   // Mouth
  ctx.moveTo(65,65);
  ctx.arc(60,65,5,0,Math.PI*2,true);  // Left eye
  ctx.moveTo(95,65);
  ctx.arc(90,65,5,0,Math.PI*2,true);  // Right eye
  ctx.stroke();
};

tests['arc() - various'] = function(ctx){
  ctx.lineWidth = 5;
  ctx.strokeStyle = "rgba(0,0,0,0.5)";
for (var i = 1;i<6;i++)
{
  ctx.beginPath();
  ctx.arc(25 + (i-1)*60,30,25,0,i*(6.283/5),true); 
  ctx.stroke();
  ctx.beginPath();
  ctx.arc(25 + (i-1)*60,90,25,0,i*(6.283/5),false); 
  ctx.stroke();
 }; 
};

tests['arc() 2'] = function(ctx){
  for(var i=0;i<4;i++){
    for(var j=0;j<3;j++){
      ctx.beginPath();
      var x              = 25+j*50;               // x coordinate
      var y              = 25+i*50;               // y coordinate
      var radius         = 20;                    // Arc radius
      var startAngle     = 0;                     // Starting point on circle
      var endAngle       = Math.PI+(Math.PI*j)/2; // End point on circle
      var anticlockwise  = i%2==0 ? false : true; // clockwise or anticlockwise

      ctx.arc(x,y,radius,startAngle,endAngle, anticlockwise);

      if (i>1){
        ctx.fill();
      } else {
        ctx.stroke();
      }
    }
  }
};
tests['bezierCurveTo()'] = function(ctx){
  ctx.beginPath();
  ctx.moveTo(75,40);
  ctx.bezierCurveTo(75,37,70,25,50,25);
  ctx.bezierCurveTo(20,25,20,62.5,20,62.5);
  ctx.bezierCurveTo(20,80,40,102,75,120);
  ctx.bezierCurveTo(110,102,130,80,130,62.5);
  ctx.bezierCurveTo(130,62.5,130,25,100,25);
  ctx.bezierCurveTo(85,25,75,37,75,40);
  ctx.fill();
};

tests['quadraticCurveTo()'] = function(ctx){
  ctx.beginPath();
  ctx.moveTo(75,25);
  ctx.quadraticCurveTo(25,25,25,62.5);
  ctx.quadraticCurveTo(25,100,50,100);
  ctx.quadraticCurveTo(50,120,30,125);
  ctx.quadraticCurveTo(60,120,65,100);
  ctx.quadraticCurveTo(125,100,125,62.5);
  ctx.quadraticCurveTo(125,25,75,25);
  ctx.stroke();
};

tests['rotate()'] = function(ctx){
  ctx.rotate(0.4);
  ctx.translate(30,0);
  ctx.rect(0,0,50,50);
  ctx.stroke();
};
tests['rotate() 2'] = function(ctx){
  ctx.translate(75,75);

  for (var i=1;i<6;i++){ // Loop through rings (from inside to out)
    ctx.save();
    ctx.fillStyle = 'rgb('+(51*i)+','+(255-51*i)+',255)';

    for (var j=0;j<i*6;j++){ // draw individual dots
      ctx.rotate(Math.PI*2/(i*6));
      ctx.beginPath();
      ctx.arc(0,i*12.5,5,0,Math.PI*2,true);
      ctx.fill();
    }

    ctx.restore();
  }
};

tests['translate()'] = function(ctx){
  ctx.fillRect(0,0,300,300);
  for (var i=0;i<3;i++) {
    for (var j=0;j<3;j++) {
      ctx.save();
      ctx.strokeStyle = "#9CFF00";
      ctx.translate(50+j*100,50+i*100);
      drawSpirograph(ctx,20*(j+2)/(j+1),-8*(i+3)/(i+1),10);
      ctx.restore();
    }
  }
  function drawSpirograph(ctx,R,r,O){
    var x1 = R-O;
    var y1 = 0;
    var i  = 1;
    ctx.beginPath();
    ctx.moveTo(x1,y1);
    do {
      if (i>20000) break;
      var x2 = (R+r)*Math.cos(i*Math.PI/72) - (r+O)*Math.cos(((R+r)/r)*(i*Math.PI/72))
      var y2 = (R+r)*Math.sin(i*Math.PI/72) - (r+O)*Math.sin(((R+r)/r)*(i*Math.PI/72))
      ctx.lineTo(x2,y2);
      x1 = x2;
      y1 = y2;
      i++;
    } while (x2 != R-O && y2 != 0 );
	
    ctx.stroke();
  }
};
tests['scale()'] = function(ctx){
  ctx.strokeStyle = "#fc0";
  ctx.lineWidth = 1.5;
  ctx.fillRect(0,0,300,300);

  // Uniform scaling
  ctx.save()
  ctx.translate(50,50);
  drawSpirograph(ctx,22,6,5);  // no scaling

  ctx.translate(100,0);
  ctx.scale(0.75,0.75);
  drawSpirograph(ctx,22,6,5);

  ctx.translate(133.333,0);
  ctx.scale(0.75,0.75);
  drawSpirograph(ctx,22,6,5);
  ctx.restore();

  // Non-uniform scaling (y direction)
  ctx.strokeStyle = "#0cf";
  ctx.save()
  ctx.translate(50,150);
  ctx.scale(1,0.75);
  drawSpirograph(ctx,22,6,5);

  ctx.translate(100,0);
  ctx.scale(1,0.75);
  drawSpirograph(ctx,22,6,5);

  ctx.translate(100,0);
  ctx.scale(1,0.75);
  drawSpirograph(ctx,22,6,5);
  ctx.restore();

  // Non-uniform scaling (x direction)
  ctx.strokeStyle = "#cf0";
  ctx.save()
  ctx.translate(50,250);
  ctx.scale(0.75,1);
  drawSpirograph(ctx,22,6,5);

  ctx.translate(133.333,0);
  ctx.scale(0.75,1);
  drawSpirograph(ctx,22,6,5);

  ctx.translate(177.777,0);
  ctx.scale(0.75,1);
  drawSpirograph(ctx,22,6,5);
  ctx.restore();
  function drawSpirograph(ctx,R,r,O){
    var x1 = R-O;
    var y1 = 0;
    var i  = 1;
    ctx.beginPath();
    ctx.moveTo(x1,y1);
    do {
      if (i>20000) break;
      var x2 = (R+r)*Math.cos(i*Math.PI/72) - (r+O)*Math.cos(((R+r)/r)*(i*Math.PI/72))
      var y2 = (R+r)*Math.sin(i*Math.PI/72) - (r+O)*Math.sin(((R+r)/r)*(i*Math.PI/72))
      ctx.lineTo(x2,y2);
      x1 = x2;
      y1 = y2;
      i++;
    } while (x2 != R-O && y2 != 0 );
    ctx.stroke();
  }
};

tests['rect()'] = function(ctx){
  ctx.rect(5,5,50,50);
  ctx.strokeStyle = 'yellow';
  ctx.fill();
  ctx.stroke();
};

tests['clip()'] = function(ctx){
  ctx.arc(50,50,50,0,Math.PI * 2);
  ctx.stroke();
  ctx.clip();
  ctx.fillStyle = 'rgba(0,0,0,.5)';
  ctx.fillRect(0,0,100,100);
};

tests['clip() 2'] = function(ctx){
  function drawStar(ctx,r){
    ctx.save();
    ctx.beginPath()
    ctx.moveTo(r,0);
    for (var i=0;i<9;i++){
      ctx.rotate(Math.PI/5);
      if(i%2 == 0) {
        ctx.lineTo((r/0.525731)*0.200811,0);
      } else {
        ctx.lineTo(r,0);
      }
    }
    ctx.closePath();
    ctx.fill();
    ctx.restore();
  }
  
  ctx.fillRect(0,0,150,150);
  ctx.translate(75,75);

  // Create a circular clipping path
  ctx.beginPath();
  ctx.arc(0,0,60,0,Math.PI*2,true);
  ctx.clip();

  // draw background
  var lingrad = ctx.createLinearGradient(0,-75,0,75);
  lingrad.addColorStop(0, '#232256');
  lingrad.addColorStop(1, '#143778');

  ctx.fillStyle = lingrad;
  ctx.fillRect(-75,-75,150,150);

  // draw stars
  for (var j=1;j<50;j++){
    ctx.save();
    ctx.fillStyle = '#fff';
    ctx.translate(75-Math.floor(Math.random()*150),
                  75-Math.floor(Math.random()*150));
    drawStar(ctx,Math.floor(Math.random()*4)+2);
    ctx.restore();
  }
  
};

tests['createLinearGradient()'] = function(ctx){
  var lingrad = ctx.createLinearGradient(0,0,0,150);
  lingrad.addColorStop(0, '#00ABEB');
  lingrad.addColorStop(0.5, '#fff');
  lingrad.addColorStop(0.5, '#26C000');
  lingrad.addColorStop(1, '#fff');

  var lingrad2 = ctx.createLinearGradient(0,50,0,95);
  lingrad2.addColorStop(0.5, '#000');
  lingrad2.addColorStop(1, 'rgba(0,0,0,0)');

  ctx.fillStyle = lingrad;
  ctx.strokeStyle = lingrad2;

  ctx.fillRect(10,10,130,130);
  ctx.strokeRect(50,50,50,50);  
};

tests['createRadialGradient()'] = function(ctx){
  // Create gradients

  var radgrad = ctx.createRadialGradient(45,45,10,52,50,30);
  radgrad.addColorStop(0, '#A7D30C');
  radgrad.addColorStop(0.9, '#019F62');
  radgrad.addColorStop(1, 'rgba(1,159,98,0)');

  var radgrad2 = ctx.createRadialGradient(105,105,20,112,120,50);
  radgrad2.addColorStop(0, '#FF5F98');
  radgrad2.addColorStop(0.75, '#FF0188');
  radgrad2.addColorStop(1, 'rgba(255,1,136,0)');

  var radgrad3 = ctx.createRadialGradient(95,15,15,102,20,40);
  radgrad3.addColorStop(0, '#00C9FF');
  radgrad3.addColorStop(0.8, '#00B5E2');
  radgrad3.addColorStop(1, 'rgba(0,201,255,0)');

  var radgrad4 = ctx.createRadialGradient(0,150,50,0,140,90);
  radgrad4.addColorStop(0, '#F4F201');
  radgrad4.addColorStop(0.8, '#E4C700');
  radgrad4.addColorStop(1, 'rgba(228,199,0,0)');

  // draw shapes
  ctx.fillStyle = radgrad4;
  ctx.fillRect(0,0,150,150);
  ctx.fillStyle = radgrad3;
  ctx.fillRect(0,0,150,150);
  ctx.fillStyle = radgrad2;
  ctx.fillRect(0,0,150,150);
  ctx.fillStyle = radgrad;
  ctx.fillRect(0,0,150,150);
};

tests['globalAlpha'] = function(ctx){
  ctx.globalAlpha = 0.5;
  ctx.fillStyle = 'rgba(0,0,0,0.5)';
  ctx.strokeRect(0,0,50,50);

  ctx.globalAlpha = 0.8;
  ctx.fillRect(20,20,20,20);

  ctx.fillStyle = 'black';
  ctx.globalAlpha = 1;
  ctx.fillRect(25,25,10,10);
};
tests['globalAlpha 2'] = function(ctx){
  ctx.fillStyle = '#FD0';
  ctx.fillRect(0,0,75,75);
  ctx.fillStyle = '#6C0';
  ctx.fillRect(75,0,75,75);
  ctx.fillStyle = '#09F';
  ctx.fillRect(0,75,75,75);
  ctx.fillStyle = '#F30';
  ctx.fillRect(75,75,150,150);
  ctx.fillStyle = '#FFF';

  ctx.globalAlpha = 0.2;

  for (var i=0;i<7;i++){
      ctx.beginPath();
      ctx.arc(75,75,10+10*i,0,Math.PI*2,true);
      ctx.fill();
  }
};

tests['fillStyle'] = function(ctx){
  for (i=0;i<6;i++){
    for (j=0;j<6;j++){
      ctx.fillStyle = 'rgb(' + Math.floor(255-42.5*i) + ',' + 
                       Math.floor(255-42.5*j) + ',0)';
      ctx.fillRect(j*25,i*25,25,25);
    }
  }
};
tests['fillStyle old'] = function(ctx){
  ctx.fillStyle = 'rgb(0,55,0)';
  ctx.fillRect(10, 10, 50, 50);

  ctx.fillStyle = 'rgba(0,0,0,0.1)';
  ctx.fillRect(60, 60, 50, 50);

  ctx.fillStyle = '#000';
  ctx.fillRect(110, 110, 50, 50);
};
tests['strokeStyle'] = function(ctx){
  for (var i=0;i<6;i++){
    for (var j=0;j<6;j++){
      ctx.strokeStyle = 'rgb(0,' + Math.floor(255-42.5*i) + ',' + 
                       Math.floor(255-42.5*j) + ')';
      ctx.beginPath();
      ctx.arc(12.5+j*25,12.5+i*25,10,0,Math.PI*2,true);
      ctx.stroke();
    }
  }
};
tests['fill with stroke'] = function(ctx){
  ctx.beginPath();
  ctx.arc(75,75,50,0,Math.PI*2,true);
  ctx.fill();
  ctx.closePath();
  ctx.beginPath();
  ctx.fillStyle = 'red';
  ctx.strokeStyle = 'yellow';
  ctx.arc(75,75,30,0,Math.PI*2,true);
  ctx.fill();
  ctx.stroke();
};

tests['lineWidth'] = function(ctx){
  for (var i = 0; i < 10; i++){
    ctx.lineWidth = 1+i;
    ctx.beginPath();
    ctx.moveTo(5+i*14,5);
    ctx.lineTo(5+i*14,140);
    ctx.stroke();
  }
};
tests['line caps'] = function(ctx){
  var lineCap = ['butt','round','square'];

  ctx.strokeStyle = '#09f';
  ctx.beginPath();
  ctx.moveTo(10,10);
  ctx.lineTo(140,10);
  ctx.moveTo(10,140);
  ctx.lineTo(140,140);
  ctx.stroke();

  ctx.strokeStyle = 'black';
  for (var i=0;i<lineCap.length;i++){
    ctx.lineWidth = 15;
    ctx.lineCap = lineCap[i];
    ctx.beginPath();
    ctx.moveTo(25+i*50,10);
    ctx.lineTo(25+i*50,140);
    ctx.stroke();
  }
};

tests['line join'] = function(ctx){
  var lineJoin = ['round','bevel','miter'];
  ctx.lineWidth = 10;
  for (var i=0;i<lineJoin.length;i++){
    ctx.lineJoin = lineJoin[i];
    ctx.beginPath();
    ctx.moveTo(-5,5+i*40);
    ctx.lineTo(35,45+i*40);
    ctx.lineTo(75,5+i*40);
    ctx.lineTo(115,45+i*40);
    ctx.lineTo(155,5+i*40);
    ctx.stroke();
  }
};

tests['lineCap default'] = function(ctx){
  ctx.beginPath();
  ctx.lineWidth = 10.0;
  ctx.moveTo(50, 50);
  ctx.lineTo(50, 100);
  ctx.lineTo(80, 120);
  ctx.stroke();
};

tests['lineCap'] = function(ctx){
  ctx.beginPath();
  ctx.lineWidth = 10.0;
  ctx.lineCap = 'round';
  ctx.moveTo(50, 50);
  ctx.lineTo(50, 100);
  ctx.lineTo(80, 120);
  ctx.stroke();
};

tests['lineJoin'] = function(ctx){
  ctx.beginPath();
  ctx.lineWidth = 10.0;
  ctx.lineJoin = 'round';
  ctx.moveTo(50, 50);
  ctx.lineTo(50, 100);
  ctx.lineTo(80, 120);
  ctx.stroke();
};

tests['states'] = function(ctx){
  
  ctx.save();
  ctx.rect(50, 50, 100, 100);
  ctx.stroke();

  ctx.restore();
  ctx.save();
  ctx.translate(50,50);
  ctx.scale(.5,.5);
  ctx.strokeRect(51, 51, 100, 100);

  ctx.restore();
  ctx.translate(95,95);
  ctx.fillRect(0,0,10,10);
};

tests['states with stroke/fill/globalAlpha'] = function(ctx){
  ctx.fillRect(0,0,150,150); 
  ctx.save();                
                             
  ctx.fillStyle = '#09F'     
  ctx.fillRect(15,15,120,120);
                             
  ctx.save();                
  ctx.fillStyle = '#FFF'     
  ctx.globalAlpha = 0.5;     
  ctx.fillRect(30,30,90,90); 
                             
  ctx.restore();             
  ctx.fillRect(45,45,60,60); 
                             
  ctx.restore();             
  ctx.fillRect(60,60,30,30); 
};

tests['invalid stroke/fill styles'] = function(ctx){
  ctx.fillStyle = 'red';
  ctx.strokeStyle = 'yellow';
  ctx.rect(50,50,50,50);
  ctx.fill();
  ctx.stroke();
  ctx.beginPath();
  ctx.fillStyle = 'asdf';
  ctx.strokeStyle = 'asdf';
  ctx.rect(100,80,15,15);
  ctx.fill();
  ctx.stroke();
};

tests['fillText()'] = function(ctx){
  ctx.fontFamily="arial.ttf";
  ctx.fontSize = 30;

  ctx.font = '30px Arial';
  ctx.rotate(.1);
  ctx.lineTo(10,10);
  ctx.fillText("Awesome!", 50, 100);
  var te = ctx.measureText('Awesome!');
  ctx.strokeStyle = 'rgba(0,0,0,0.5)';
  ctx.lineTo(50, 102);
  ctx.lineTo(50 + te.width, 102);
  console.log("5");
  ctx.stroke();
};

tests['fillText() transformations'] = function(ctx){
  ctx.strokeStyle = '#666';
  ctx.font = 'bold 12px Helvetica';

  ctx.strokeRect(0,0,200,200);
  ctx.lineTo(0,100);
  ctx.lineTo(200,100);
  ctx.stroke();

  ctx.beginPath();
  ctx.lineTo(100,0);
  ctx.lineTo(100,200);
  ctx.stroke();

  ctx.rotate(0.2);
  ctx.fillText("foo", 150, 100);
  ctx.font = 'normal 30px Arial';
  ctx.fillText("bar", 50, 100);

  
  
  };

tests['strokeText()'] = function(ctx){
  ctx.fontFamily="arial.ttf";

  ctx.strokeStyle = '#666';
  ctx.strokeRect(0,0,200,200);
  ctx.lineTo(0,100);
  ctx.lineTo(200,100);
  ctx.stroke();

  ctx.beginPath();
  ctx.lineTo(100,0);
  ctx.lineTo(100,200);
  ctx.stroke();

  ctx.strokeStyle = 'red';
  ctx.font = 'normal 50px Arial';
  
  ctx.fontSize = 50;

  ctx.strokeText("bar", 100, 100);
};

tests['textAlign right'] = function(ctx){
  ctx.strokeStyle = '#666';
  ctx.strokeRect(0,0,200,200);
  ctx.lineTo(0,100);
  ctx.lineTo(200,100);
  ctx.stroke();

  ctx.beginPath();
  ctx.lineTo(100,0);
  ctx.lineTo(100,200);
  ctx.stroke();

  ctx.font = 'normal 20px Arial';
  ctx.textAlign = 'right';
  ctx.fillText("right", 100, 100);
};

tests['textAlign left'] = function(ctx){
  ctx.strokeStyle = '#666';
  ctx.strokeRect(0,0,200,200);
  ctx.lineTo(0,100);
  ctx.lineTo(200,100);
  ctx.stroke();

  ctx.beginPath();
  ctx.lineTo(100,0);
  ctx.lineTo(100,200);
  ctx.stroke();

  ctx.font = 'normal 20px Arial';
  ctx.textAlign = 'left';
  ctx.fillText("left", 100, 100);
};

tests['textAlign center'] = function(ctx){
  ctx.strokeStyle = '#666';
  ctx.strokeRect(0,0,200,200);
  ctx.lineTo(0,100);
  ctx.lineTo(200,100);
  ctx.stroke();

  ctx.beginPath();
  ctx.lineTo(100,0);
  ctx.lineTo(100,200);
  ctx.stroke();

  ctx.font = 'normal 20px Arial';
  ctx.textAlign = 'center';
  ctx.fillText("center", 100, 100);
};

tests['textBaseline alphabetic'] = function(ctx){
  ctx.strokeStyle = '#666';
  ctx.strokeRect(0,0,200,200);
  ctx.lineTo(0,100);
  ctx.lineTo(200,100);
  ctx.stroke();

  ctx.font = 'normal 20px Arial';
  ctx.textBaseline = 'alphabetic';
  ctx.textAlign = 'center';
  ctx.fillText("alphabetic", 100, 100);
};

tests['textBaseline top'] = function(ctx){
  ctx.strokeStyle = '#666';
  ctx.strokeRect(0,0,200,200);
  ctx.lineTo(0,100);
  ctx.lineTo(200,100);
  ctx.stroke();

  ctx.font = 'normal 20px Arial';
  ctx.textBaseline = 'top';
  ctx.textAlign = 'center';
  ctx.fillText("top", 100, 100);
};

tests['textBaseline hanging'] = function(ctx){
  ctx.strokeStyle = '#666';
  ctx.strokeRect(0,0,200,200);
  ctx.lineTo(0,100);
  ctx.lineTo(200,100);
  ctx.stroke();

  ctx.font = 'normal 20px Arial';
  ctx.textBaseline = 'hanging';
  ctx.textAlign = 'center';
  ctx.fillText("hanging", 100, 100);
};

tests['textBaseline bottom'] = function(ctx){
  ctx.strokeStyle = '#666';
  ctx.strokeRect(0,0,200,200);
  ctx.lineTo(0,100);
  ctx.lineTo(200,100);
  ctx.stroke();

  ctx.font = 'normal 20px Arial';
  ctx.textBaseline = 'bottom';
  ctx.textAlign = 'center';
  ctx.fillText("bottom", 100, 100);
};

tests['textBaseline ideographic'] = function(ctx){
  ctx.strokeStyle = '#666';
  ctx.strokeRect(0,0,200,200);
  ctx.lineTo(0,100);
  ctx.lineTo(200,100);
  ctx.stroke();

  ctx.font = 'normal 20px Arial';
  ctx.textBaseline = 'ideographic';
  ctx.textAlign = 'center';
  ctx.fillText("ideographic", 100, 100);
};

tests['textBaseline middle'] = function(ctx){
  ctx.strokeStyle = '#666';
  ctx.strokeRect(0,0,200,200);
  ctx.lineTo(0,100);
  ctx.lineTo(200,100);
  ctx.stroke();

  ctx.font = 'normal 20px Arial';
  ctx.textBaseline = 'middle';
  ctx.textAlign = 'center';
  ctx.fillText("ideographic", 100, 100);
};

tests['integration 1'] = function(ctx){
var actx = {};
actx.lineTo = function(x,y){};
actx.moveTo = function(x,y){};

  ctx.fillStyle = '#eee';
	ctx.fillRect(0,0,300,300);

	ctx.fillStyle = "rgba(0,0,200,0.5)";
	ctx.strokeStyle= "rgba(100,100,100,0.5)";
	var n = 0;
	var xs = 10, ys = 10, cx = xs/2, cy=ys/2, xd = 300/xs, yd = 300/ys;
	for(var xt = 0; xt<300; xt+=xd){
		for(var yt = 0; yt<300; yt+=yd){
		  actx.moveTo(xt,0);
		  actx.lineTo(xt,300);
		  actx.moveTo(0,yt);
		  actx.lineTo(300,yt);
		}
	}
	ctx.stroke();
	ctx.strokeWidth = 5;
	ctx.strokeStyle = "rgba(0,0,0,0.5)";
  ctx.fillStyle= "rgba(0,174,239,0.8)";
  ctx.beginPath();
  actx.moveTo(0,300);
  for(var xt = 0; xt<300; xt+=1)
      actx.lineTo(xt, 100+10*Math.sin(xt/32+n+Math.sin(xt/64+n))*5+Math.sin(xt/48));
  actx.lineTo(300,300);
  actx.lineTo(0,300);
  ctx.fill();
  ctx.stroke();
  ctx.beginPath();
  ctx.fillStyle = "rgba(242,0,144,0.7)"; n = n+2;
  for(var xt = 0; xt<300; xt+=1)
      actx.lineTo(xt, 150+10*Math.sin(xt/32+n+Math.sin(xt/64+n))*5+Math.sin(xt/48));
  actx.lineTo(300,300);
  actx.lineTo(0,300);
  ctx.fill();
  ctx.stroke();
  ctx.beginPath();
  ctx.fillStyle = "rgba(255,254,0,0.7)"; n = n+4;
  for(var xt = 0; xt<300; xt+=1)
      actx.lineTo(xt, 200+10*Math.sin(xt/32+n+Math.sin(xt/64+n))*5+Math.sin(xt/48));
  actx.lineTo(300,300);
  actx.lineTo(0,300);
  ctx.fill();
  ctx.stroke();
};

tests['clock'] = function(ctx)
{
  ctx.save();  
  ctx.clearRect(0,0,150,150);  
  ctx.translate(75,75);  
  ctx.scale(0.4,0.4);  
  ctx.rotate(-Math.PI/2);  
  ctx.strokeStyle = "black";  
  ctx.fillStyle = "white";  
  ctx.lineWidth = 8;  
  ctx.lineCap = "round";  
  
  // Hour marks  
  ctx.save();  
  for (var i=0;i<12;i++){  
    ctx.beginPath();  
    ctx.rotate(Math.PI/6);  
    ctx.moveTo(100,0);  
    ctx.lineTo(120,0);  
    ctx.stroke();  
  }  
  ctx.restore();  
  
  // Minute marks  
  ctx.save();  
  ctx.lineWidth = 5;  
  for (i=0;i<60;i++){  
    if (i%5!=0) {  
      ctx.beginPath();  
      ctx.moveTo(117,0);  
      ctx.lineTo(120,0);  
      ctx.stroke();  
    }  
    ctx.rotate(Math.PI/30);  
  }  
  ctx.restore();  
    
  var sec = 40;  
  var min = 25;  
  var hr  = 1;  
  hr = hr>=12 ? hr-12 : hr;  
  
  ctx.fillStyle = "black";  
  
  // write Hours  
  ctx.save();  
  ctx.rotate( hr*(Math.PI/6) + (Math.PI/360)*min + (Math.PI/21600)*sec )  
  ctx.lineWidth = 14;  
  ctx.beginPath();  
  ctx.moveTo(-20,0);  
  ctx.lineTo(80,0);  
  ctx.stroke();  
  ctx.restore();  
  
  // write Minutes  
  ctx.save();  
  ctx.rotate( (Math.PI/30)*min + (Math.PI/1800)*sec )  
  ctx.lineWidth = 10;  
  ctx.beginPath();  
  ctx.moveTo(-28,0);  
  ctx.lineTo(112,0);  
  ctx.stroke();  
  ctx.restore();  
    
  // Write seconds  
  ctx.save();  
  ctx.rotate(sec * Math.PI/30);  
  ctx.strokeStyle = "#D40000";  
  ctx.fillStyle = "#D40000";  
  ctx.lineWidth = 6;  
  ctx.beginPath();  
  ctx.moveTo(-30,0);  
  ctx.lineTo(83,0);  
  ctx.stroke();  
  ctx.beginPath();  
  ctx.arc(0,0,10,0,Math.PI*2,true);  
  ctx.fill();  
  ctx.beginPath();  
  ctx.arc(95,0,10,0,Math.PI*2,true);  
  ctx.stroke();  
  ctx.fillStyle = "#555";  
  ctx.arc(0,0,3,0,Math.PI*2,true);  
  ctx.fill();  
  ctx.restore();  
  
  ctx.beginPath();  
  ctx.lineWidth = 14;  
  ctx.strokeStyle = '#325FA2';  
  ctx.arc(0,0,142,0,Math.PI*2,true);  
  ctx.stroke();  
  
  ctx.restore(); 
  }

tests['baseline compare'] = function(ctx)
{
	ctx.strokeStyle = '#666';
	ctx.strokeRect(0,0,200,200);
	ctx.lineTo(0,100);
	ctx.lineTo(200,100);
	ctx.stroke();

	ctx.font = 'normal 20px Arial';
	ctx.fontFamily = "arial.ttf";
	ctx.fontSize = 20;

	var baselines = ['top','hanging', 'middle', 'alphabetic', 'ideographic', 'bottom'];
	var x = 5;
	var te = ctx.measureText('OoYjH');
	for (a in baselines)
	{
		ctx.textBaseline = baselines[a];
		ctx.textAlign = 'left';
		ctx.fillText('OoYjH', x, 100);
		ctx.textBaseline = 'top';
		ctx.textAlign = 'left';
		ctx.fillText(baselines[a], x, 150);
		x+=te.width + 5;
	}
};


tests['font size px'] = function(ctx){
  ctx.strokeStyle = '#666';
  ctx.strokeRect(0,0,200,200);
  ctx.lineTo(0,100);
  ctx.lineTo(200,100);
  ctx.stroke();

  ctx.font = 'normal 14px Arial';
  ctx.textAlign = 'center';
  ctx.fillText("normal 14px Arial", 100, 100);
};

tests['font size pt'] = function(ctx){
  ctx.strokeStyle = '#666';
  ctx.strokeRect(0,0,200,200);
  ctx.lineTo(0,100);
  ctx.lineTo(200,100);
  ctx.stroke();

  ctx.font = 'normal 14pt Arial';
  ctx.textAlign = 'center';
  ctx.fillText("normal 14pt Arial", 100, 100);
};

tests['font size mm'] = function(ctx){
  ctx.strokeStyle = '#666';
  ctx.strokeRect(0,0,200,200);
  ctx.lineTo(0,100);
  ctx.lineTo(200,100);
  ctx.stroke();

  ctx.font = 'normal 3mm Arial';
  ctx.textAlign = 'center';
  ctx.fillText("normal 3mm Arial", 100, 100);
};

tests['font size cm'] = function(ctx){
  ctx.strokeStyle = '#666';
  ctx.strokeRect(0,0,200,200);
  ctx.lineTo(0,100);
  ctx.lineTo(200,100);
  ctx.stroke();

  ctx.font = 'normal 0.6cm Arial';
  ctx.textAlign = 'center';
  ctx.fillText("normal 0.6cm Arial", 100, 100);
};

tests['font weight bold'] = function(ctx){
  ctx.strokeStyle = '#666';
  ctx.strokeRect(0,0,200,200);
  ctx.lineTo(0,100);
  ctx.lineTo(200,100);
  ctx.stroke();

  ctx.font = 'bold 14px Arial';
  ctx.textAlign = 'center';
  ctx.fillText("bold 14px Arial", 100, 100);
};

tests['font weight lighter'] = function(ctx){
  ctx.strokeStyle = '#666';
  ctx.strokeRect(0,0,200,200);
  ctx.lineTo(0,100);
  ctx.lineTo(200,100);
  ctx.stroke();

  ctx.font = 'lighter 14px Arial';
  ctx.textAlign = 'center';
  ctx.fillText("lighter 14px Arial", 100, 100);
};

tests['font weight lighter italic'] = function(ctx){
  ctx.strokeStyle = '#666';
  ctx.strokeRect(0,0,200,200);
  ctx.lineTo(0,100);
  ctx.lineTo(200,100);
  ctx.stroke();

  ctx.font = 'lighter italic 14px Arial';
  ctx.textAlign = 'center';
  ctx.fillText("lighter italic 14px Arial", 100, 100);
};

tests['font weight 200'] = function(ctx){
  ctx.strokeStyle = '#666';
  ctx.strokeRect(0,0,200,200);
  ctx.lineTo(0,100);
  ctx.lineTo(200,100);
  ctx.stroke();

  ctx.font = '200 14px Arial';
  ctx.textAlign = 'center';
  ctx.fillText("200 14px Arial", 100, 100);
};

tests['font weight 800'] = function(ctx){
  ctx.strokeStyle = '#666';
  ctx.strokeRect(0,0,200,200);
  ctx.lineTo(0,100);
  ctx.lineTo(200,100);
  ctx.stroke();

  ctx.font = '800 14px Arial';
  ctx.textAlign = 'center';
  ctx.fillText("800 14px Arial", 100, 100);
};

tests['font family serif'] = function(ctx){
  ctx.strokeStyle = '#666';
  ctx.strokeRect(0,0,200,200);
  ctx.lineTo(0,100);
  ctx.lineTo(200,100);
  ctx.stroke();

  ctx.font = '14px serif';
  ctx.textAlign = 'center';
  ctx.fillText("14px serif", 100, 100);
};

tests['font family sans-serif'] = function(ctx){
  ctx.strokeStyle = '#666';
  ctx.strokeRect(0,0,200,200);
  ctx.lineTo(0,100);
  ctx.lineTo(200,100);
  ctx.stroke();

  ctx.font = '14px sans-serif';
  ctx.textAlign = 'center';
  ctx.fillText("14px sans-serif", 100, 100);
};

tests['font family Impact'] = function(ctx){
  ctx.strokeStyle = '#666';
  ctx.strokeRect(0,0,200,200);
  ctx.lineTo(0,100);
  ctx.lineTo(200,100);
  ctx.stroke();

  ctx.font = '18px Impact';
  ctx.textAlign = 'center';
  ctx.fillText("18px Impact", 100, 100);
};

tests['font family invalid'] = function(ctx){
  ctx.strokeStyle = '#666';
  ctx.strokeRect(0,0,200,200);
  ctx.lineTo(0,100);
  ctx.lineTo(200,100);
  ctx.stroke();

  ctx.font = '14px Foo, Invalid, Impact, sans-serif';
  ctx.textAlign = 'center';
  ctx.fillText("14px Invalid, Impact", 100, 100);
};

tests['globalCompositeOperation source-over'] = function(ctx){
  ctx.fillStyle = 'blue';
  ctx.fillRect(0,0,100,100);
  ctx.globalCompositeOperation = 'source-over';
  ctx.fillStyle = 'red';
  ctx.arc(80,80,50,0,Math.PI * 2);
  ctx.fill();
};

tests['globalCompositeOperation source-in'] = function(ctx){
  ctx.fillStyle = 'blue';
  ctx.fillRect(0,0,100,100);
  ctx.globalCompositeOperation = 'source-in';
  ctx.fillStyle = 'red';
  ctx.arc(80,80,50,0,Math.PI * 2);
  ctx.fill();
};

tests['globalCompositeOperation source-out'] = function(ctx){
  ctx.fillStyle = 'blue';
  ctx.fillRect(0,0,100,100);
  ctx.globalCompositeOperation = 'source-out';
  ctx.fillStyle = 'red';
  ctx.arc(80,80,50,0,Math.PI * 2);
  ctx.fill();
};

tests['globalCompositeOperation destination-in'] = function(ctx){
  ctx.fillStyle = 'blue';
  ctx.fillRect(0,0,100,100);
  ctx.globalCompositeOperation = 'destination-in';
  ctx.fillStyle = 'red';
  ctx.arc(80,80,50,0,Math.PI * 2);
  ctx.fill();
};

tests['globalCompositeOperation destination-out'] = function(ctx){
  ctx.fillStyle = 'blue';
  ctx.fillRect(0,0,100,100);
  ctx.globalCompositeOperation = 'destination-out';
  ctx.fillStyle = 'red';
  ctx.arc(80,80,50,0,Math.PI * 2);
  ctx.fill();
};

tests['globalCompositeOperation destination-atop'] = function(ctx){
  ctx.fillStyle = 'blue';
  ctx.fillRect(0,0,100,100);
  ctx.globalCompositeOperation = 'destination-atop';
  ctx.fillStyle = 'red';
  ctx.arc(80,80,50,0,Math.PI * 2);
  ctx.fill();
};

tests['globalCompositeOperation xor'] = function(ctx){
  ctx.fillStyle = 'blue';
  ctx.fillRect(0,0,100,100);
  ctx.globalCompositeOperation = 'xor';
  ctx.fillStyle = 'red';
  ctx.arc(80,80,50,0,Math.PI * 2);
  ctx.fill();
};

tests['globalCompositeOperation copy'] = function(ctx){
  ctx.fillStyle = 'blue';
  ctx.fillRect(0,0,100,100);
  ctx.globalCompositeOperation = 'copy';
  ctx.fillStyle = 'red';
  ctx.arc(80,80,50,0,Math.PI * 2);
  ctx.fill();
};

tests['globalCompositeOperation lighter'] = function(ctx){
  ctx.fillStyle = 'blue';
  ctx.fillRect(0,0,100,100);
  ctx.globalCompositeOperation = 'lighter';
  ctx.fillStyle = 'red';
  ctx.arc(80,80,50,0,Math.PI * 2);
  ctx.fill();
};

tests['globalCompositeOperation darker'] = function(ctx){
  ctx.fillStyle = 'blue';
  ctx.fillRect(0,0,100,100);
  ctx.globalCompositeOperation = 'darker';
  ctx.fillStyle = 'red';
  ctx.arc(80,80,50,0,Math.PI * 2);
  ctx.fill();
};

tests['globalCompositeOperation multiply'] = function(ctx){
  ctx.fillStyle = 'rgba(0,0,255,0.6)';
  ctx.fillRect(0,0,100,100);
  ctx.globalCompositeOperation = 'multiply';
  var grad = ctx.createRadialGradient(80,80,5,60,60,60);
  grad.addColorStop(0, 'yellow');
  grad.addColorStop(0.2, 'red');
  grad.addColorStop(1, 'black');
  ctx.fillStyle = grad;
  ctx.arc(80,80,50,0,Math.PI * 2);
  ctx.fill();
};

tests['globalCompositeOperation screen'] = function(ctx){
  ctx.fillStyle = 'rgba(0,0,255,0.6)';
  ctx.fillRect(0,0,100,100);
  ctx.globalCompositeOperation = 'screen';
  var grad = ctx.createRadialGradient(80,80,5,60,60,60);
  grad.addColorStop(0, 'yellow');
  grad.addColorStop(0.2, 'red');
  grad.addColorStop(1, 'black');
  ctx.fillStyle = grad;
  ctx.arc(80,80,50,0,Math.PI * 2);
  ctx.fill();
};

tests['globalCompositeOperation overlay'] = function(ctx){
  ctx.fillStyle = 'rgba(0,0,255,0.6)';
  ctx.fillRect(0,0,100,100);
  ctx.globalCompositeOperation = 'overlay';
  var grad = ctx.createRadialGradient(80,80,5,60,60,60);
  grad.addColorStop(0, 'yellow');
  grad.addColorStop(0.2, 'red');
  grad.addColorStop(1, 'black');
  ctx.fillStyle = grad;
  ctx.arc(80,80,50,0,Math.PI * 2);
  ctx.fill();
};

tests['globalCompositeOperation hard-light'] = function(ctx){
  ctx.fillStyle = 'rgba(0,0,255,0.6)';
  ctx.fillRect(0,0,100,100);
  ctx.globalCompositeOperation = 'hard-light';
  var grad = ctx.createRadialGradient(80,80,5,60,60,60);
  grad.addColorStop(0, 'yellow');
  grad.addColorStop(0.2, 'red');
  grad.addColorStop(1, 'black');
  ctx.fillStyle = grad;
  ctx.arc(80,80,50,0,Math.PI * 2);
  ctx.fill();
};

tests['globalCompositeOperation hsl-hue'] = function(ctx){
  ctx.fillStyle = 'rgba(0,0,255,0.6)';
  ctx.fillRect(0,0,100,100);
  ctx.globalCompositeOperation = 'hsl-hue';
  var grad = ctx.createRadialGradient(80,80,5,60,60,60);
  grad.addColorStop(0, 'yellow');
  grad.addColorStop(0.2, 'red');
  grad.addColorStop(1, 'black');
  ctx.fillStyle = grad;
  ctx.arc(80,80,50,0,Math.PI * 2);
  ctx.fill();
};

tests['globalCompositeOperation hsl-saturation'] = function(ctx){
  ctx.fillStyle = 'rgba(0,0,255,0.6)';
  ctx.fillRect(0,0,100,100);
  ctx.globalCompositeOperation = 'hsl-saturation';
  var grad = ctx.createRadialGradient(80,80,5,60,60,60);
  grad.addColorStop(0, 'yellow');
  grad.addColorStop(0.2, 'red');
  grad.addColorStop(1, 'black');
  ctx.fillStyle = grad;
  ctx.arc(80,80,50,0,Math.PI * 2);
  ctx.fill();
};

tests['globalCompositeOperation hsl-color'] = function(ctx){
  ctx.fillStyle = 'rgba(0,0,255,0.6)';
  ctx.fillRect(0,0,100,100);
  ctx.globalCompositeOperation = 'hsl-color';
  var grad = ctx.createRadialGradient(80,80,5,60,60,60);
  grad.addColorStop(0, 'yellow');
  grad.addColorStop(0.2, 'red');
  grad.addColorStop(1, 'black');
  ctx.fillStyle = grad;
  ctx.arc(80,80,50,0,Math.PI * 2);
  ctx.fill();
};

tests['globalCompositeOperation hsl-luminosity'] = function(ctx){
  ctx.fillStyle = 'rgba(0,0,255,0.6)';
  ctx.fillRect(0,0,100,100);
  ctx.globalCompositeOperation = 'hsl-luminosity';
  var grad = ctx.createRadialGradient(80,80,5,60,60,60);
  grad.addColorStop(0, 'yellow');
  grad.addColorStop(0.2, 'red');
  grad.addColorStop(1, 'black');
  ctx.fillStyle = grad;
  ctx.arc(80,80,50,0,Math.PI * 2);
  ctx.fill();
};

tests['shadowBlur'] = function(ctx){
  ctx.fillRect(150,10,20,20);

  ctx.lineTo(20,5);
  ctx.lineTo(100,5);
  ctx.stroke();

  ctx.shadowColor = '#000';
  ctx.shadowBlur = 5;
  ctx.fillRect(20,20,100,100);

  ctx.beginPath();
  ctx.lineTo(20,150);
  ctx.lineTo(100,150);
  ctx.stroke();

  ctx.shadowBlur = 0;
  
  ctx.beginPath();
  ctx.lineTo(20,180);
  ctx.lineTo(100,180);
  ctx.stroke();
  
  ctx.fillRect(150,150,20,20);
};

tests['shadowColor'] = function(ctx){
  ctx.fillRect(150,10,20,20);

  ctx.lineTo(20,5);
  ctx.lineTo(100,5);
  ctx.stroke();

  ctx.shadowColor = '#c00';
  ctx.shadowBlur = 5;
  ctx.fillRect(20,20,100,100);

  ctx.beginPath();
  ctx.lineTo(20,150);
  ctx.lineTo(100,150);
  ctx.stroke();

  ctx.shadowBlur = 0;
  
  ctx.beginPath();
  ctx.lineTo(20,180);
  ctx.lineTo(100,180);
  ctx.stroke();
  
  ctx.fillRect(150,150,20,20);
};

tests['shadowOffset{X,Y}'] = function(ctx){
  ctx.fillRect(150,10,20,20);

  ctx.lineTo(20,5);
  ctx.lineTo(100,5);
  ctx.stroke();

  ctx.shadowColor = '#c00';
  ctx.shadowBlur = 5;
  ctx.shadowOffsetX = 2;
  ctx.shadowOffsetY = 2;
  ctx.fillRect(20,20,100,100);

  ctx.beginPath();
  ctx.lineTo(20,150);
  ctx.lineTo(100,150);
  ctx.stroke();

  ctx.shadowBlur = 0;
  
  ctx.beginPath();
  ctx.lineTo(20,180);
  ctx.lineTo(100,180);
  ctx.stroke();
  
  ctx.fillRect(150,150,20,20);
};

tests['shadowOffset{X,Y} large'] = function(ctx){
  ctx.fillRect(150,10,20,20);

  ctx.lineTo(20,5);
  ctx.lineTo(100,5);
  ctx.stroke();

  ctx.shadowColor = '#c00';
  ctx.shadowBlur = 5;
  ctx.shadowOffsetX = 10;
  ctx.shadowOffsetY = 10;
  ctx.fillRect(20,20,100,100);

  ctx.beginPath();
  ctx.lineTo(20,150);
  ctx.lineTo(100,150);
  ctx.stroke();

  ctx.shadowBlur = 0;
  
  ctx.beginPath();
  ctx.lineTo(20,180);
  ctx.lineTo(100,180);
  ctx.stroke();
  
  ctx.fillRect(150,150,20,20);
};

tests['shadowOffset{X,Y} negative'] = function(ctx){
  ctx.fillRect(150,10,20,20);

  ctx.lineTo(20,5);
  ctx.lineTo(100,5);
  ctx.stroke();

  ctx.shadowColor = '#c00';
  ctx.shadowBlur = 5;
  ctx.shadowOffsetX = -10;
  ctx.shadowOffsetY = -10;
  ctx.fillRect(20,20,100,100);

  ctx.beginPath();
  ctx.lineTo(20,150);
  ctx.lineTo(100,150);
  ctx.stroke();

  ctx.shadowBlur = 0;
  
  ctx.beginPath();
  ctx.lineTo(20,180);
  ctx.lineTo(100,180);
  ctx.stroke();
  
  ctx.fillRect(150,150,20,20);
};

tests['shadowBlur values'] = function(ctx){
  ctx.fillRect(150,10,20,20);

  ctx.lineTo(20,5);
  ctx.lineTo(100,5);
  ctx.stroke();

  ctx.shadowColor = '#c00';
  ctx.shadowBlur = 25;
  ctx.shadowOffsetX = 2;
  ctx.shadowOffsetY = 2;
  ctx.fillRect(20,20,100,100);

  ctx.beginPath();
  ctx.lineTo(20,150);
  ctx.lineTo(100,150);
  ctx.stroke();

  ctx.shadowColor = 'rgba(0,0,0,0)';
  
  ctx.beginPath();
  ctx.lineTo(20,180);
  ctx.lineTo(100,180);
  ctx.stroke();
  
  ctx.fillRect(150,150,20,20);
};

tests['shadow strokeRect()'] = function(ctx){
  ctx.strokeRect(150,10,20,20);

  ctx.lineTo(20,5);
  ctx.lineTo(100,5);
  ctx.stroke();

  ctx.shadowColor = '#000';
  ctx.shadowBlur = 5;
  ctx.shadowOffsetX = 2;
  ctx.shadowOffsetY = 2;
  ctx.strokeRect(20,20,100,100);

  ctx.beginPath();
  ctx.lineTo(20,150);
  ctx.lineTo(100,150);
  ctx.stroke();

  ctx.shadowColor = 'rgba(0,0,0,0)';
  
  ctx.beginPath();
  ctx.lineTo(20,180);
  ctx.lineTo(100,180);
  ctx.stroke();
  
  ctx.strokeRect(150,150,20,20);
};

tests['shadow fill()'] = function(ctx){
  ctx.strokeRect(150,10,20,20);

  ctx.lineTo(20,5);
  ctx.lineTo(100,5);
  ctx.stroke();

  ctx.shadowColor = '#000';
  ctx.shadowBlur = 5;
  ctx.shadowOffsetX = 2;
  ctx.shadowOffsetY = 2;
  ctx.rect(20,20,100,100);
  ctx.fill();

  ctx.beginPath();
  ctx.lineTo(20,150);
  ctx.lineTo(100,150);
  ctx.stroke();

  ctx.shadowColor = 'rgba(0,0,0,0)';
  
  ctx.beginPath();
  ctx.lineTo(20,180);
  ctx.lineTo(100,180);
  ctx.stroke();
  
  ctx.strokeRect(150,150,20,20);
};

tests['shadow stroke()'] = function(ctx){
  ctx.strokeRect(150,10,20,20);

  ctx.lineTo(20,5);
  ctx.lineTo(100,5);
  ctx.stroke();

  ctx.shadowColor = '#000';
  ctx.shadowBlur = 5;
  ctx.shadowOffsetX = 2;
  ctx.shadowOffsetY = 2;
  ctx.rect(20,20,100,100);
  ctx.stroke();

  ctx.beginPath();
  ctx.lineTo(20,150);
  ctx.lineTo(100,150);
  ctx.stroke();

  ctx.shadowColor = 'rgba(0,0,0,0)';
  
  ctx.beginPath();
  ctx.lineTo(20,180);
  ctx.lineTo(100,180);
  ctx.stroke();
  
  ctx.strokeRect(150,150,20,20);
};

tests['shadow globalAlpha'] = function(ctx){
  ctx.lineTo(0,0);
  ctx.lineTo(50,0);
  ctx.lineTo(50,150);
  ctx.stroke();

  ctx.lineWidth = 5;
  ctx.globalAlpha = 0.3;
  ctx.shadowColor = '#00c';
  ctx.shadowBlur = 2;
  ctx.shadowOffsetX = 8;
  ctx.shadowOffsetY = 8;

  ctx.lineTo(0,150);
  ctx.stroke();
};

tests['shadow fillText()'] = function(ctx){
  ctx.shadowColor = 'rgba(0,0,255,0.3)';
  ctx.shadowBlur = 4;
  ctx.shadowOffsetX = 8;
  ctx.shadowOffsetY = 8;
  ctx.textAlign = 'center';
  ctx.font = '50px Arial';
  ctx.fillText("Shadow", 100, 100);
};

tests['shadow strokeText()'] = function(ctx){
  ctx.shadowColor = 'rgba(0,0,255,0.3)';
  ctx.shadowBlur = 4;
  ctx.shadowOffsetX = 8;
  ctx.shadowOffsetY = 8;
  ctx.textAlign = 'center';
  ctx.font = '50px Arial';
  ctx.strokeText("Shadow", 100, 100);
};

tests['shadow integration'] = function(ctx){
  ctx.shadowBlur = 5;
  ctx.shadowOffsetX = 10;
  ctx.shadowOffsetY = 10;
  ctx.shadowColor = '#eee';
  ctx.lineWidth = 3;

  var grad1 = ctx.createLinearGradient(105,0, 200,100);
  grad1.addColorStop(0,    'yellow');
  grad1.addColorStop(0.25, 'red');
  grad1.addColorStop(0.75, 'blue');
  grad1.addColorStop(1,    'limegreen');

  var grad2 = ctx.createRadialGradient(50,50,10,50,50,50);
  grad2.addColorStop(0,    'yellow');
  grad2.addColorStop(0.25, 'red');
  grad2.addColorStop(0.75, 'blue');
  grad2.addColorStop(1,    'limegreen');

  // linear grad box
  ctx.fillStyle = grad1;
  ctx.fillRect(105,0, 100,100);

  // skyblue box
  ctx.fillStyle = "skyblue";
  ctx.fillRect(105,101, 100,100);

  // radial grad oval
  ctx.beginPath();
  ctx.arc(50, 50, 50, 0, Math.PI*2, false);
  ctx.fillStyle = grad2;
  ctx.fill();

  // gold oval
  ctx.beginPath();
  ctx.arc(50, 151, 50, 0, Math.PI*2, false);
  ctx.fillStyle = "gold";
  ctx.fill();
};

  
var http = require('http');
var t = 0;


function DumpTest(name)
{
}
http.createServer(function (req, res) 
{
	if (req.url == '/performance')
	{
		console.log("running integration 100x!")
		
		
		var ctx = createContext(410,210, "argb");
	
		var start = Date.now();
		
		for (var i =0;i<100;i++)
		{
			ctx.save();
			ctx.clearRect(0,0,410,210);		
			tests['integration 1'](ctx);
			ctx.restore();
		};
		
		var end = Date.now();
		
		var runtime = end-start;
		console.log(runtime);
		res.writeHead(200, {'Content-Type': 'text/html'});
		res.end("<html>"+runtime+" millisconds</html>");

		return;
	}
	if (req.url != '/')
	{
		res.writeHead(200, {'Content-Type': 'text/html'});
		res.end("<html>nothing to see..</html>");
		return;
	};
	
	//	var Query = require('url').parse(req.url, true).query;
//	if (Query.text)
	//{
		//drawText(ctx1, Query.text);
	//}
	//else
	{
//		drawText(ctx1, "wooYay!");
	};
	//console.log(req);
	
	res.writeHead(200, {'Content-Type': 'text/html'});
	var Output = "<html><head><style>html{font-family:arial;};h3{font-family:arial;}</style><title>Server - O3 Canvas Test Suite</title></head><body>";
	console.log("running tests..");
	var count = 0;
	var ctx = createContext(410,210, "argb");
	for (var testname  in  tests)
	{
		ctx.clearRect(0,0,410,210);
		ctx.save();
		count  = count +1;
		var runtime = -1;
		try 
		{
			console.log("running " + testname);
			var start = Date.now();
			tests[testname](ctx);
			var end = Date.now();
			runtime = end-start;
			var start = Date.now();
			var buf = ctx.pngBuffer();
			var end = Date.now();
			compresstime = end-start;
			var base64buf = buf.toBase64();
			Output += "<div style=\"border:1px solid gray;background-color:#f0f0f0;margin:10px;float:left;\"><h3>"+testname+"</h3><img alt='Embedded Image' src='data:image/png;base64,"+base64buf+"'><br>time to generate: "+runtime.toString()+" msec. ("+Math.round((1000/Math.max(runtime,1))).toString()+" fps)<br>time to compress: "+compresstime.toString()+" msec.</div>";
			
		}
		catch(e)
		{
			console.log("error in test "+testname+ ": "+e.message);
		}
		try 
		{
			
			//console.log("attempting to make buffer..");			
			//var buf2 = new Buffer(ctx.pngBuffer().toBase64(),'base64');
			//console.log(buf2);
			//console.log("buffer made -> " + buf2.byte);			
			//fs.writeFileSync("output.png", base64buf,'base64');
			//buf2 = null;
		}
		catch(e)
		{
			console.log("error in writing png for test "+testname+ ": "+e.message);
		}
		ctx.restore();
		
	};
		
	console.log("done running tests!");
	
    res.end(Output + "</body></html>");
}).listen(4000, "127.0.0.1");

console.log('Server running at http://127.0.0.1:4000/');
