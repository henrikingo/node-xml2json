try{
	var o3 = require('./o3.js');
}catch(ex){
	var o3 = require('../o3.js');
}

function rxc(){
   return Array.prototype.map.call(arguments,function(b){return b.toString().slice(1,-1)}).join('');
}
var fontcache = {};
var fb = /(?:\s*,\s*('[^']+'|"[^"]+"|\w[\w\s-]+\w))?/
var fontrx = new RegExp(rxc(
        /^\s*/ ,
		/(?:(normal|italic|oblique)\s*)?/ ,             // 1 style 
		/(?:(normal|small-caps|inherit)\s*)?/ ,         // 2 variant
		/(?:(normal|bold|bolder|lighter|[1-9]00)\s*)?/ ,// 3 weight
		/(?:([\d\.]+)?(px|pt|pc|in|cm|mm|%)\s*)?/ ,         // 4 5 size
		/(?:\/([\d\.]+)?(px|pt|pc|in|cm|mm|%)\s*)?/ ,       // 6 7 line height
		/(?:('[^']+'|"[^"]+"|\w[\w\s-]+\w))?/,          // 8+ Family and fallbacks
		fb,fb,fb,fb,fb,fb,fb 
	));

var fontscale = {
    pt : 1/0.75,
    in : 96,
    mm : 96/25.4,
    cm : 96/2.54
};
var familylookup = {
	"sans": "arial",
	"helvetica" : "arial",
	"times new roman" : "times"
}
var fontlookup = {
	"arial": "arial.ttf",
	"arial bold": "arialbd.ttf",
	"arial italic": "ariali.ttf",
	"arial bold italic": "arialbi.ttf",
	"sans-serif": "arial.ttf",
	"times": "times.ttf",
	"times bold": "timesbd.ttf",
	"times italic": "timesi.ttf",
	"times bold italic": "timesbi.ttf",

}

module.exports = function(x,y,mode){
    
    var ctx = o3.canvas(x,y,mode);
    ctx.onSetFont = function(font){// need getter/setter overload in O3 too
       
       var m = fontcache[font] || (fontcache[font] = String(font).match(fontrx));
       if(!m) return;
       // ctx.fontStyle = m[0]; // and so on.
	   var fontextra = "";
	   var family = "arial";
	   
	   if(m[2]) ctx.fontVariant =    m[2].toLowerCase();

	   if(m[3]){ ctx.fontWeight =    m[3].toLowerCase();fontextra += " " + m[3].toLowerCase();}
	   if(m[1]){ ctx.fontStyle  =    m[1].toLowerCase();fontextra += " " + m[1].toLowerCase();}

       if(m[4]) ctx.fontSize = parseFloat(m[4]) * (fontscale[m[5]] || 1);
	   if(m[6]) ctx.fontSize = parseFloat(m[6]) * (fontscale[m[7]] || 1);
	   
	   
		if(m[8])
		{
			ctx.fontFamily =   fontlookup[(familylookup[m[8].toLowerCase()]||"sans") +fontextra] || "arial.ttf";
		}
		else
		{
		ctx.fontFamily =   fontlookup["sans" +fontextra] || "arial.ttf";
		}
	   
	 //  console.log("font parsing results: ");
	  // for (a in m)
	  // {
	//	if (m[a]) console.log(a +":" +m[a]);
	 // } 
    };
    return ctx;
};