var xvert = 64;
var yvert = 64;
var vertspersquare = 6;
index = 0;
var midx = -(xvert-1)/2;
var midy = -(yvert-1)/2;
var ws = 4;

ctx = o3.canvas(128,128,"argb");

mnth = o3.canvas(xvert,yvert,"argb");
grass = o3.canvas(256,256,"argb");
rock = o3.canvas(256,256,"argb");

grass.src = LoadFileData("grass.png");
rock.src = LoadFileData("rock.png");
var thetime = 0;


var gl = o3.createGLWindow("o3-OpenGL output",10,10,640,480);


var mntvert = LoadFile("mntvert.txt");
var mntfrag =  LoadFile("mntfrag.txt");

var vert = "uniform float time;void main() { vec4 newpos = gl_Vertex;newpos.y += sin(time+newpos.x*2)*0.5; gl_Position = gl_ModelViewProjectionMatrix * newpos;}";
var frag = "void main(){ gl_FragColor = vec4(sin(gl_FragCoord.x/10),cos(gl_FragCoord.y/10),1.0,1.0);}"

var SimpleShader = gl.shaderprogram(vert, frag);
var MntShader = gl.shaderprogram(mntvert, mntfrag);
var SomeCube = gl.cube();
var MntArray = gl.vertexarray(xvert*yvert*vertspersquare);


var Tex = gl.texture(128, 128);
var GrassTex = gl.texture(256, 256);
var RockTex = gl.texture(256, 256);


GrassTex.upload(grass, 256,256);
RockTex.upload(rock, 256,256);

var ColorTex = gl.texture(1024, 512);
var ColorCanvas = o3.canvas(640,480,"argb");

var index =0 ;


var K = o3.kinect();

var FractalHeight = []
var FractalNormal = []
var FractalVertex = []
function FractalGenerate(xsize, ysize, octaves)
{
	// begin by initialising everywhere to 0
	for (var x = 0; x< xsize;x++)
	{
		FractalHeight[x]=[]
		FractalNormal[x]=[]
		FractalVertex[x]=[]
		for (var y = 0; y< ysize;y++)
		{
			FractalHeight[x][y]=0
		}
	}

	// add each octave to the fractal
	FractalMaxHeight = 0
	FractalAddOctave(xsize, ysize, octaves)

	for (var x = 0; x< xsize;x++)
	{
		for (var y = 0; y< ysize;y++)
		{
			FractalHeight[x][y]*=0.10
			FractalHeight[x][y]-=1;
			FractalHeight[x][y] += (Math.sin(x*0.4)*Math.cos(y*0.4)*2)*3 + (Math.sin(x*0.1+0.3)*Math.cos(y*0.1)*2)*3
		}
	}

}

function RandRange(min, max)
{
	return Math.random()*max;
};
// recursively adds octaves to the fractal
function FractalAddOctave(xsize, ysize, octave)
{
  // add this octave
  var step = Math.pow(2,(octave - 1));
  var scale = step
  FractalMaxHeight = FractalMaxHeight + scale
  for (var x = 0; x< xsize;x+=step)
  {
     for (var y = 0; y< ysize;y+=step)
	{
	  // Add random value at this point
//	  print(x,y,scale)
	  FractalHeight[x][y] = FractalHeight[x][y] + RandRange(0,scale)
	  //if we aren't on the last octave, interpolate intermediate values
	  if (step > 1)
	  {
	    if (x > 1)
		{
	      FractalHeight[x - step / 2][y] = (FractalHeight[x - step][y] + FractalHeight[x][y]) / 2
		}
        if (y > 1)
		{
		  FractalHeight[x][y - step / 2] = (FractalHeight[x][y - step] + FractalHeight[x][y]) / 2
		}
		if (x > 1 && y > 1)
		{
		  FractalHeight[x - step / 2][y - step / 2] = (FractalHeight[x - step / 2][y - step] + FractalHeight[x - step / 2][y]) / 2
		}
	  }
	}
  }

  // add the next octave
  if (octave > 1)
  {
    FractalAddOctave(xsize, ysize, octave - 1)
  };
  
}

FractalGenerate(xvert, yvert, 4);
function Normal(vv1, vv2, vv3)
{
	var vvA = [vv1[0], vv1[1], vv1[2]];
	vvA[0]-=vv2[0];
	vvA[1]-=vv2[1];
	vvA[2]-=vv2[2];
	
	var vvB = [vv1[0], vv1[1], vv1[2]];
	vvB[0]-=vv3[0];
	vvB[1]-=vv3[1];
	vvB[2]-=vv3[2];

	
	
	return [vvA[1]* vvB[2] - vvA[2] * vvB[1],
	vvA[2]* vvB[0] - vvA[0] * vvB[2],
	vvA[0]* vvB[1] - vvA[1] * vvB[0]];
}


for (var xx = 0;xx<xvert;xx++)
{
	for (var yy = 0;yy<yvert;yy++)
	{
		FractalVertex[xx][yy] = [(midx+xx)*ws,FractalHeight[xx][yy], (midy+yy)*ws]
	}	
};
for (var xx = 0;xx<xvert-1;xx++)
{
	for (var yy = 0;yy<yvert-1;yy++)
	{
		FractalNormal[xx][yy] = Normal(FractalVertex[xx][yy], FractalVertex[xx+1][yy],FractalVertex[xx][yy+1]);
	}
};

for (var xx = 0;xx<xvert-2;xx++)
{
	for (var yy = 0;yy<yvert-2;yy++)
	{
	
		var x1 = xx;
		var x2 = xx+1;
		var y1 = yy;
		var y2 = yy+1;
		
		var vv1 = FractalVertex[x1][y1];
		var vv2 = FractalVertex[x2][y1];
		var vv3 = FractalVertex[x2][y2];
		var vv4 = FractalVertex[x1][y2];
		
		var n1 = FractalNormal[x1][y1];
		var n2 = FractalNormal[x2][y1];
		var n3 = FractalNormal[x2][y2];
		var n4 = FractalNormal[x1][y2];
		
		MntArray.setVertex(index , vv1[0],vv1[1],vv1[2],  n1[0],n1[1],n1[2],x1,y1); index++;
		MntArray.setVertex(index , vv2[0],vv2[1],vv2[2],  n2[0],n2[1],n2[2],x2,y1); index++;
		MntArray.setVertex(index , vv3[0],vv3[1],vv3[2],  n3[0],n3[1],n3[2],x2,y2); index++;

		MntArray.setVertex(index , vv1[0],vv1[1],vv1[2],  n1[0],n1[1],n1[2],x1,y1); index++;
		MntArray.setVertex(index , vv3[0],vv3[1],vv3[2],  n3[0],n3[1],n3[2],x2,y2); index++;
		MntArray.setVertex(index , vv4[0],vv4[1],vv4[2],  n4[0],n4[1],n4[2],x1,y2); index++;
	}                                                      
	
}

function Height(xx,yy)
{
//	xx/=ws;
	//yy/=ws;
	xx-=midx;
	yy-=midy;
	var ix = Math.floor(xx);
	var ax = xx-ix;
	var iax = 1.0-ax;
	
	var iy = Math.floor(yy);
	var ay = yy-iy;
	var iay = 1.0-ay;
	
	var h1  = FractalHeight[ix  ][iy];
	var h2  = FractalHeight[ix+1][iy];
	var h3  = FractalHeight[ix  ][iy+1];
	var h4  = FractalHeight[ix+1][iy+1];
	
	return (h1*iax + h2*ax)*iay + (h3*iax + h4*ax)*ay;
};

//var Teapot = LoadWavefrontObj("teapot.obj");
//var Plane = LoadWavefrontObj("cessna.obj");
function LoadFile(filename)
{
	return o3.cwd.get(filename).data;
}

function LoadFileData(filename)
{
	return o3.cwd.get(filename).buffer;
}


var done = false;
gl.onclose = function()
{
	done = true;
};

var StartT = gl.Time();
var T = 0;

var px = 0;
var py = 0;
var dpz = 0;
var pz = Height(px,py)+2;
var playerdir = 0;
var ddr = 0;
	
while (!done)
{
	var NT = (gl.Time()-StartT)*0.001;
	DT = NT-T;
	T = NT;
	gl.BeginFrame();
	gl.Viewport(0,0,gl.width, gl.height);
	gl.ClearColor(0.9,0.95,1.0,1);
	gl.Clear(gl.COLOR_BUFFER_BIT + gl.DEPTH_BUFFER_BIT);
	gl.MatrixMode(gl.PROJECTION);
	gl.LoadIdentity();
	gl.Perspective(45, gl.width/gl.height, 0.1, 1000);
	gl.MatrixMode(gl.MODELVIEW);
	gl.LoadIdentity();
	
	px += Math.sin(playerdir)*DT*2;
	py += Math.cos(playerdir)*DT*2;

	ddr += (Math.random()-0.5)*DT*5;
	ddr *= 0.9;
	playerdir+= ddr*DT;
	if (px < (-xvert/2)+1) px = xvert/2 -1;
	if (py < (-yvert/2)+1) py = yvert/2 -1;
	
	if (px > (xvert/2)-1) px = -xvert/2 +1;
	if (py > (yvert/2)-1) py = -yvert/2 +1;

	var nz = Height(px,py)+1;
	if (nz> pz ) 
	{
		pz = nz;
		dpz = 0;
	}
	else
	{
		dpz += DT*10;
		pz -= dpz*DT;
		if (nz>= pz )
		{
			pz = nz;
			dpz = 0;
		};
	};
		if (1)
		{
			gl.LookAt(-15*5,15,0,
			           0,0,0,
			0,1,0);
		}
		else
		{
	gl.LookAt(px*ws-Math.sin(playerdir)*6, pz+10 , py*ws-Math.cos(playerdir)*6,
	         px*ws, pz , py*ws,
			 0,1,0);
			 };
//	gl.LookAt(-14,14,-14,
	//         0,0,0,
		//	 0,1,0);
	
	if (1)
	{
		gl.Enable(gl.LIGHTING);
		gl.Enable(gl.NORMALIZE);
		gl.Enable(gl.LIGHT0);
		
		gl.Material(gl.FRONT, gl.AMBIENT, 1,1,1,1);
		gl.Material(gl.FRONT, gl.DIFFUSE, 1,1,1,1);
		gl.Material(gl.FRONT, gl.SPECULAR, 0,0,0,1);
		gl.Material(gl.FRONT, gl.SHININESS, 1,1,1,1);
		gl.Material(gl.FRONT, gl.EMISSION, 0,0,0,0);
		
		gl.Light(gl.LIGHT0, gl.AMBIENT, 0.1, 0.1, 0.1, 1.0);
		gl.Light(gl.LIGHT0, gl.DIFFUSE, 1.0, 1.0, 1.0, 1.0);
		gl.Light(gl.LIGHT0, gl.SPECULAR, 1.0, 1.0, 1.0, 1.0);
		gl.Light(gl.LIGHT0, gl.POSITION, -0.5,-1,-0.5, 1.0);
		gl.Enable(gl.TEXTURE_2D);
		gl.UseProgram(MntShader);
		var lp = o3.vec3(Math.sin(T),-1,Math.cos(T));
		var colcol = o3.vec4(Math.sin(T),1,Math.cos(T),1.0);
		gl.SetUniform(MntShader, "gCol", colcol);
		gl.SetUniform(MntShader, "gIncomingLight", lp);
		gl.SetUniform(MntShader, "gGrass", GrassTex);
		gl.SetUniform(MntShader, "gRock", RockTex);
		gl.SetUniform(MntShader, "xvert", xvert*1.0000001);
		gl.SetUniform(MntShader, "yvert", yvert*1.0000001);
		gl.Color4(0.6,0.6,0.6,1);
		gl.Enable(gl.CULL_FACE);
		gl.CullFace(gl.FRONT);
		gl.PushMatrix();
		gl.Render(MntArray);
		gl.PopMatrix();
		gl.UseProgram(0);
		gl.Disable(gl.LIGHTING);
		gl.Disable(gl.LIGHT0);
		
	};
	if (0)
	{
		gl.PointSize(5);
		gl.Begin(gl.POINTS);
		gl.Color3(1,0,0);	gl.Vertex3(-1,-1,-1);
		gl.Color3(1,1,0);	gl.Vertex3( 1,-1,-1);
		gl.Color3(0,1,0);	gl.Vertex3( 1, 1,-1);
		gl.Color3(0,0,1);	gl.Vertex3(-1, 1,-1);
		gl.Color3(1,0.5,0);	gl.Vertex3(-1,-1, 1);
		gl.Color3(0.5,0,1);	gl.Vertex3( 1,-1, 1);
		gl.Color3(1,1,1);	gl.Vertex3( 1, 1, 1);
		gl.Color3(0.1,0.1,0.1);	gl.Vertex3(-1, 1, 1);
		gl.End();
	}
	
	if (0)
	{
		
		gl.Enable(gl.TEXTURE_2D);

		ctx.clear(0);
		var lingrad = ctx.createLinearGradient(0,0,0,128);
		lingrad.addColorStop(0, '#00ABEB');
		lingrad.addColorStop(1, '#fff');
		ctx.fillStyle = lingrad;
		ctx.fillRect(0,0,128,128);
  
		Tex.upload(ctx, 128,128);

		Tex.bind();
		
		gl.Enable(gl.LIGHTING);
		gl.Enable(gl.LIGHT0);
		gl.Enable(gl.LIGHT1);
		gl.Enable(gl.LIGHT2);

		gl.Light(gl.LIGHT0, gl.AMBIENT, 0.5, 0.5, 0.5, 1.0);
		gl.Light(gl.LIGHT0, gl.DIFFUSE, 1.0, 0.0, 0.0, 1.0);
		gl.Light(gl.LIGHT0, gl.SPECULAR, 1.0, 0.0, 0.0, 1.0);
		gl.Light(gl.LIGHT0, gl.POSITION, Math.cos(T*2.1)*4, 1.0, Math.sin(T*1.1)*4, 1.0);
		
		gl.Light(gl.LIGHT1, gl.AMBIENT, 0.0, 0.0, 0.0, 1.0);
		gl.Light(gl.LIGHT1, gl.DIFFUSE, 0.0, 1.0, 0.0, 1.0);
		gl.Light(gl.LIGHT1, gl.SPECULAR, 0.0, 1.0, 0.0, 1.0);
		gl.Light(gl.LIGHT1, gl.POSITION, Math.cos(T*1.2)*4, 1.0, Math.sin(T*1.2)*4, 1.0);

		gl.Light(gl.LIGHT2, gl.AMBIENT, 0.0, 0.0, 0.0, 1.0);
		gl.Light(gl.LIGHT2, gl.DIFFUSE, 0.0, 0.0, 1.0, 1.0);
		gl.Light(gl.LIGHT2, gl.SPECULAR, 0.0, 0.0, 1.0, 1.0);
		gl.Light(gl.LIGHT2, gl.POSITION, Math.cos(T*1.1)*4, 1.0, Math.sin(T*1.1)*4, 1.0);
		
		for (var x = -4;x<5;x++)
		{
			for (var y = -4;y<5;y++)
			{
				gl.PushMatrix();
				gl.Translate(x,5+y,0);
				gl.Color3(1,1,1);
				gl.Scale(0.5,0.5,0.5);				
				gl.Rotate(x*10,0,1,0);
				gl.Render(SomeArray);
				gl.PopMatrix();
				gl.PushMatrix();
				gl.Color3(0,0.5,0);
				gl.Translate(x,0,y);				
				gl.Render(SomeArray);
				gl.PopMatrix();
			};
		};
		
		gl.Disable(gl.LIGHTING);
		gl.Disable(gl.TEXTURE_2D);
		
	}
	
	if (0)
	{
		gl.PushMatrix();
		gl.Scale(0.2,0.2,0.2);
		gl.PushMatrix();
		gl.Color3(0.9,0.9,0.9);
		for (var i = 0;i<10;i++)
		{
			//SomeCube.Render(gl.TRIANGLES,0,18);
			SomeCube.Render();
			gl.Translate(1.5,0,0);
		};
		gl.PopMatrix();
		gl.Color3(0.9,0.9,0.2);
		gl.UseProgram(SimpleShader);
		gl.SetUniform(SimpleShader, "time", T*5);
		for (var i = 0;i<10;i++)
		{
			//SomeCube.Render(gl.TRIANGLES,0,18);
			gl.Render(SomeCube);
			gl.Translate(0,1.5,0);
		};
		gl.UseProgram(0);		
		gl.PopMatrix();
	}
	
	if (1)
	{
		K.processEvents();
		if (K.newRGBAvailable())
		{
			K.RGBToCanvas(ColorCanvas);
			ColorTex.upload(ColorCanvas);
		}
		
		gl.Enable(gl.TEXTURE_2D);
		gl.Disable(gl.CULL_FACE);
		ColorTex.bind();
		gl.MatrixMode(gl.TEXTURE);
		gl.LoadIdentity();
		gl.Scale(640/1024, 
		         480/512, 
				 
				 1);
		gl.MatrixMode(gl.MODELVIEW);
		
		gl.Color4(1,1,1,1);
		gl.PushMatrix();
		gl.Translate(px*ws,Height(px, py),py*ws);
		gl.Scale(0.2,0.2,0.2);
		gl.Render(SomeCube);
		gl.PopMatrix();
		gl.MatrixMode(gl.TEXTURE);
		gl.LoadIdentity();
		gl.MatrixMode(gl.MODELVIEW);
		gl.Disable(gl.TEXTURE_2D);

	};	
	gl.EndFrame();
	o3.wait(0);
	thetime++;	
};

