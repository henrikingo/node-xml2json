var parser = require("./treec");
var pu = require('./parseutil');
var o3root = "../..";
var fs = require('fs');

//TODO: template functions / const functions

//var files = pu.loadFiles("./", /\.h$/, /\_old/, "c");
var scrCallable = pu.loadFiles(o3root + "/modules", /\.h$/, /\_glue/, "c");
scrCallable = scrCallable.concat(pu.loadFileList( 
	prependFiles(o3root + "/include", [
		"cO3.h",
		"cStream.h",
		"cStream_posix.h",
		"cStream_win32.h",
		"cStreamBase.h",
		"cScrBuf.h"]
	), "c"));
	
genTrace(scrCallable,"o3_trace_scrfun", true, false);	
	
var lowlevelTools = pu.loadFileList( 
	prependFiles(o3root + "/include", [
		"tools.h",
		"tools_atomic.h",
		"tools_atomic_apple.h",
		"tools_atomic_linux.h",
		"tools_atomic_win32.h",
		"tools_axhost.h",
		"tools_chr.h",
		"tools_glue.h",
		"tools_math.h",
		"tools_mem.h",
		"tools_str.h"]
	), "c");	
genTrace(lowlevelTools,"o3_trace_tools", true, false);
	
var comBase = pu.loadFileList( 
	prependFiles(o3root + "/include", [
		"iScr.h", 
		"iUnk.h", 
		"cUnk.h"]
	), "c");	
genTrace(comBase,"o3_trace_combase", true, false);
	
var comGlue = pu.loadFiles(o3root + "/modules", /\_glue\.h$/, /\_old/, "c");
comGlue = comGlue.concat(
	pu.loadFiles(o3root + "/include", 
		/\_glue\.h$/, /tools/, "c"),
	pu.loadFileList( 
		prependFiles(o3root + "/include", [
			"cScr.h",
			"Var.h",	
			"cMgr.h"]
		), "c")
);	
genTrace(comGlue,"o3_trace_comglue", true, false);
	
var containers = pu.loadFileList( 
	prependFiles(o3root + "/include", [
		"Buf.h",	
		"tList.h",
		"tMap.h",
		"tStr.h",
		"tVec.h"]
	), "c");
genTrace(containers,"o3_trace_containers", true, false);
	
var sys = pu.loadFileList( 
	prependFiles(o3root + "/include", [
		"cSys.h",
		"cSys_posix.h",
		"cSys_win32.h",
		"cSysBase.h"]
	), "c");	
genTrace(sys,"o3_trace_sys", true, false);
	
var hostGlue = 	pu.loadFileList( 
	prependFiles(o3root, [
		"include/tools_glue_idispatch.h",
		"modules/js/cJs_win32.h",
		"modules/js/cJs_posix.h",
		"modules/js/cJsBase.h",
		"hosts/plugin/np_plugin.h",
		"hosts/plugin/ax_plugin.h"]
	), "c");
genTrace(hostGlue,"o3_trace_hostglue", true, false);	
	
//var fns = {};
//var traceType = "Trace";

function prependFiles(dir, files){
	for (var i=0; i<files.length; i++) {
		files[i] = dir + "/" + files[i];
	}
	return files;
}

function genTrace(files, traceType, overwrite, force){
	for(var i = 0;i<files.length;i++){
		var f = files[i];		
		console.log("--------file:" + f.name);
		f.root.find("($a){}").prev().filter(/^\w+$/).not(/if|else|for|while|switch/).each(function(n){
			//fns[n.nv] = 1; 
			//console.log(n.nv);

			console.log("-------------------node val:" + n.nv);
			var ps = n.ps, funNode; 
			if (ps.nv == ':' && ps.ps.nv == ')' || ps.nv == ',') {
				while (ps.nv != ':' || !ps.nt){
					ps = ps.ps;
				}
				funNode = ps.ps.ps.ps;
				//console.log(funName+"("+ps.ps.ps.fc.toString({nows:1})+")");	
			}
			else {	
				funNode = n;
			}
			first = n.ns.ns.ns.fc;
			
			addTrace(funNode, first, traceType, overwrite, force);		
		});
		//console.log(f.root.toString());
		fs.writeFileSync(f.name,f.root.toString());
	}
}

function addTrace(funNode, first, traceType, overwrite, force){
		var funName = funNode.nv;
		if (funNode.ps.nv == "~")
			funName = "~" + funName;
			
		var code = first.ws 
				+ traceType + '("' 
				+ funName + '");';
		
		var oldExists = (first.nv.indexOf("o3_trace") == 0);
		var oldDiff = oldExists ? (first.nv != traceType) : false;
		var remOld = oldExists ? (oldDiff ? force : overwrite) : false;			
		var writeNew = oldExists ? remOld : true;	
		
		if (oldExists && first.nv == "o3_trace_no_trace")
			return;
		
		if (writeNew)
			first.before(code);
		
		if (remOld) {	
			var len = 0, 
				eol = first;
			while (eol.nv != ';'){
				len++;
				eol = eol.ns;
			}
			eol = eol.ns;
			first.remove(len+1);
			first = eol;
		}		
}
//for(k in fns){
//    console.log(k+'()');
//}

//console.log( files[384].root.dump() );


/*
    
    var n = f.root.scan("define([$a], function($b){$c})");
	if(n.nt){
	    var v = n.found;
	    var deps = v.a.split(",", {ws:0}),
	        args = v.b.split(",", {ws:0});
	    n.replace("module.declare",1);
        v.a.pn.remove(3);
        v.b.replace("require, exports, module");
        n.ns.fc.scan("function").replacews("");
        v.c.scan("return").replace("module.exports = ", 1);
        
        for(var i = 0; i < args.length; i++)
            v.c.before( (i==0?"\n":"") + "    var " + args[i] + " = require(" + deps[i] + ");\n");
        console.log(f.root.toString());
        return;
	}
	
*/    
	// if we are finding stuff with markers, how do we get to the markers?
	// alright now i want to do some useful querying
	// operations we'd wanna do...
	// replace a range with new code?
	// n[2].replace("require, exports, module");
	// js.replace("define", fn[0]);
	// lets add new things
	// js.add("var "+args[0]+" = "+deps[0]+";\n", code);
	// console.log(js.serialize(args));
	//  }
//}

console.log("END");

return;
//
//for(k in classes){
//    console.log( "Class"+k);
//	var t = classes[k].apf;
//	for(var i = 0;i<t.length;i++)
//		console.log(t[i].v);
//}
//return;
// lets scan for all apf.bla functions
//var functions = {};

//var ns, apf = {};
//
//for(k in dict){    
//    ns = apf;
//    if(k.match(/^apf/)) {
//        var chunks = k.split(".");        
//        for (var i=1; i<chunks.length; i++) {
//            var chunk = chunks[i];
//            ns = ns[chunk] || (ns[chunk] = {});
//        }
//    }
//}
//require("fs").writeFileSync("apf-deps.json", JSON.stringify(apf, 4));
/*
var apfdeps = require("./apf-deps.json");

for (var key in apfdeps) {
    var chunks = key.split(" ");

    if (chunks.length != 2)
        continue;

    var cls = classes[chunks[1]];
    if (!cls) {
        console.log("cound not find " + key);
        continue;
    }
    
    for (var method in apfdeps[key]) {
        if (!cls.methods[method] && !cls.props[method]) {
            var id = "apf." + chunks[0] + "." + method;
            console.log("cannot find function " + id);
            printIndentifier(id);
        } else {
            //console.log("FOUND function " + method);
        }
    }
}

function printIndentifier(name) {
    var t = dict[name];
    if (!t) return;
    
    for (var i=0; i<t.length; i++) {
        var file = js.findRoot(t[i]).file;
        console.log(file.name + ":" + (t[i].l+1));
    }
};


for (var name in functions) {
    //console.log(name);
}
console.log(Object.keys(functions).length);
*/
// apf.bla = function when no new
// search function name in type == 'object'
// replace apf. with name of object .. make sure there is no variable name of that object

// apf.bla = class
// new keyword in front of it
// when not in classes, fix

// check if namespace is a module itself OR
// repeat process of A and B in namespace 
// apf.bla = namespace
//        . = function / class

// apf.bla = constant
//function : () .call .apply

//for(k in dict){
//    if(k.match(/^apf/)){
//	   console.log(k);
//}

	



