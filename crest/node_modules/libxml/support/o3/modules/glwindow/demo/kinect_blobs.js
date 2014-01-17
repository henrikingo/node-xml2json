
var gl = o3.createGLWindow("o3-OpenGL output",10,10,640,480);
var K = o3.kinect();

var kinect_depth_ctx = o3.canvas(640,480,"argb");
var kinect_color_ctx = o3.canvas(640,480,"argb");

var kinect_color_tex = gl.texture(1024, 512);
var kinect_depth_tex = gl.texture(1024, 512);
var kinect_thres_tex = gl.texture(1024, 512);

var done = false;

gl.onclose = function()
{
	done = true;
};

var StartT = gl.Time();
var T = 0;
var Front = K.CMToDepth(30);
var Back = K.CMToDepth(100);

var Blobs = [];

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
	
	gl.Ortho2D(0,1,1,0);
	
	gl.MatrixMode(gl.MODELVIEW);
	gl.LoadIdentity();
	
//	gl.LookAt(0,0,-4,        0,0,0,		 0,-1,0);
	
	if (1)
	{
		K.processEvents();
		
		if (K.newRGBAvailable())
		{
			K.RGBToCanvas(kinect_color_ctx);
			kinect_color_tex.upload(kinect_color_ctx);
		}
		
		var blobDiv = 10;

		if (K.newDepthAvailable())
		{
			K.DepthToCanvas(kinect_depth_ctx);
			kinect_depth_tex.upload(kinect_depth_ctx);
			//K.depthThreshold(kinect_depth_ctx, Front, Back);
			K.findBlobs(Front, Back,blobDiv,2);
			kinect_thres_tex.upload(kinect_depth_ctx);
			Blobs = [];
			for (var i = 0;i<blobCount;i++)
			{
			//    o3.print("blob found! ");
			    var blobx = K.blobX(i);
			    var bloby = K.blobY(i);
			    var blobz = K.blobZ(i);
			    var blobsize = K.blobSize(i);
			    Blobs.push({x:blobx, y:bloby, z:blobz, size:blobsize});
			}
		}
		
		gl.Enable(gl.TEXTURE_2D);
		gl.Disable(gl.CULL_FACE);
		kinect_color_tex.bind();
		
    	gl.MatrixMode(gl.TEXTURE);
		gl.LoadIdentity();
		gl.Scale(639/1024, 479/512, 1);
				 
		gl.MatrixMode(gl.MODELVIEW);
		gl.Color4(1,1,0,0.2);
	    kinect_thres_tex.bind();
	    
	    gl.Begin(gl.QUADS);	
		gl.TexCoord2(0,0);gl.Vertex2(0,0);
		gl.TexCoord2(1,0);gl.Vertex2(1,0);
		gl.TexCoord2(1,1);gl.Vertex2(1,1);
		gl.TexCoord2(0,1);gl.Vertex2(0,1);
		gl.End();
		
		gl.MatrixMode(gl.TEXTURE);
		gl.LoadIdentity();
		gl.MatrixMode(gl.MODELVIEW);
		gl.Disable(gl.TEXTURE_2D);
        gl.Disable(gl.DEPTH_TEST);
        
        var blobCount = K.blobCount();
        gl.Color3(1,0,0);     
        
        Blobs.sort(function(a,b){if (a.size<b.size) return true;return false;});
        
        for (var i = 0;i<Math.min(2, Blobs.length);i++)
        {
               
            gl.PointSize(Math.sqrt(Blobs[i].size));
            gl.Begin(gl.POINTS);
        //    o3.print(blobx + " " + bloby + " " + blobz + " " + blobsize + "\n");
            gl.Vertex3(Blobs[i].x / (640.0/blobDiv), Blobs[i].y / (480.0/blobDiv),-0.01);
            gl.End();
//            gl.Vertex2(blobx / 64, bloby  
        }
        
        if (Blobs.length >= 2)
        {
            gl.Color4(0,1,0,1);
            gl.LineWidth(Math.min(Math.sqrt(Blobs[1].size), Math.sqrt(Blobs[0].size)));
            gl.Begin(gl.LINES);        
            gl.Vertex3(Blobs[0].x / (640.0/blobDiv), Blobs[0].y / (480.0/blobDiv),-0.01);
            gl.Vertex3(Blobs[1].x / (640.0/blobDiv), Blobs[1].y / (480.0/blobDiv),-0.01);
            gl.End();
        }
	};	
	gl.EndFrame();
	o3.wait(0);
//	thetime++;	
};
