var cq = require("./codequery");
var fs = require('fs');
var diff = require('./diff');

exports.loadFiles = function(path, filedo, filedont, lang){
    function getFileList(dir,match, dont, out){
        if(!out) out = [];
        var names = fs.readdirSync(dir);
    	if(names)
	    for(var i = 0;i<names.length;i++){
    		var n = names[i];
    		if(n!='.' && n!='..'){
    			var fp = dir+"/"+n;
    			if(!dont || !fp.match(dont)){
        			if(fp.match(match))
        				out.push(fp);
        			else if (fs.statSync(fp).isDirectory() ){
        				getFileList(fp, match, dont, out);
        			}
    			}
    		}
    	}
    	return out;
    }
    
    var list;
    //if(!file){
    list = getFileList(path, filedo, filedont);
    //}else 
     //   list = [file];
	return exports.loadFileList(list, lang);
}

exports.loadFileList = function(list, lang){
    var opts = {};
    if(lang){
        opts.keywords = cq[lang+"keywords"];   
        if(lang == "c")
            opts.dontEatSemi = 1;
    }
   // lets now parse each file
    console.log("Loading...");
    for(var i = 0;i<list.length;i++){
        //console.log(list[i]);
    	var f = list[i] = {name:list[i]};
    	//console.log(n.name)
    	f.data = fs.readFileSync(f.name).toString().replace(/\r?\n/g,"\n");
    	f.root = cq.parse(f.data, opts);
    	f.root.file = f;
    	f.check = f.root.toString();
        if(f.root.err)
            console.log("Parse error in "+f.name);	
    	if(f.data != f.check){
//    	     require('util').debug(f.data);
         	require('util').debug("Fatal error in file " +f.name+"\n"+diff.diffString(f.data,f.check));
    		//console.log(f.check);
    		//console.log(js.debug(f.node));
    		return;
    	}
    }
    console.log("Done.");
    return list;
}


exports.parseClasses = function(list) {
    var classes = {};
	var t = 0;
	for(var fi = 0;fi<list.length;fi++){
	    var f = list[fi];
	
	    // f.name = filename
	    // f.node is first node in the syntax tree. try js.dump(f.node);
	    // check js.js for other details.
	    // js.find tries to search for tree-matches ignoring subtrees
	    // NOTE: parser regexp is not unicode safe (easy to fix in js.js)
	    // Also please be aware that the parser is not by the book, just tested
	    // on quite a huge sourcebase completely parsing and reserializing without errors.
	    var cls = null;
	    var fn = js.find(f.node,"define([], function(){");
	    if(!fn.length){
	//        console.log(__dirname+'/'+f.name.slice(2)+":0 Not a proper module!");
	    }else{
	        // alright lets fetch the return
	        var rt = js.find(fn[0],"return");
	        if(rt.length!=1){
	             //console.log(__dirname+'/'+f.name.slice(2)+":0 Odd module!");
	        } else {
	            // lets check if we have { or name
	            var obj = f.name.match(/([a-zA-Z0-9]+)\.js/)[1];
	            var cname = rt[0].dn.v
				if(cname == 'new'){
					cname = rt[0].dn.dn.v;
				}
	            if(cname == '{'){
					//return {...a:b};
	                //console.log(__dirname+'/'+f.name.slice(2)+":0 Return object");
	                cls = classes[obj] = {type:'object',file:f, methods:{}, props:{}};
	                // lets build the names
	                for(var n = rt[0].dn.ch;n;n = n.dn)if(n.v == ':'){
	                    if(n.dn.v == 'function')
	                        cls.methods[n.up.v] = n.up;
	                    else
	                        cls.props[n.up.v] = n.up;     
	                };
	            }else if (cname.match(/^[a-zA-Z0-9\_\$]+$/)){
					
	                //var bla = function(){} ;
					//(function(){}).call(bla.prototype);
	                // lets see if we can find the prototype
	                var pt = js.find(fn[0],".call("+rt[0].dn.v+".prototype)");
	                if(pt.length){
	                    // we have a classdef
	                    //console.log(__dirname+'/'+f.name.slice(2)+":0 Classdef");
	                    cls = classes[cname] = {type:'class',file:f, name:cname, methods:{}, props:{},inherit:{},decorate:{}};
	                    // lets fetch the inherit/decorate classes too
	                    var dep = js.findrx(fn[0],/oop\.decorate|oop\.inherit/)
	                    if(dep.length){
	                        for(var i = 0;i<dep.length;i++){
	                            var base = dep[i].dn.ch.dn.dn.v;
	                            if(dep[i].v=='oop.decorate')
	                                cls.decorate[base] = dep[i];
	                            else
	                                cls.inherit[base] = dep[i];
	                            //console.log(cname+ " depends on " + dep[i].dn.ch.dn.dn.v );    
	                        }
	                    }
		                var it = js.find( pt[0].up.up.ch.dn.dn.dn.ch, "{/this\\.*/} =");
	                    for(var i = 0;i<it.length;i++)if(it[i].v){
	                        var m = it[i].v.match(/this\.([a-zA-Z0-9\_\$]+)/);
	                        if(m){
	                            // lets check if its a function
	                            if(it[i].dn.dn.v == 'function')
	                                cls.methods[m[1]] = it[i];
	                            else
	                                cls.props[m[1]] = it[i];
	                             
	                            //console.log(cname + "::"+ m[1] );
	                        }
	                    }
	                }else{
						// var bla = function(){};
	                    var pt = js.find(fn[0],"var "+cname+" = {");
	                    if(pt.length){
	                        //console.log(__dirname+'/'+f.name.slice(2)+":0 Object def as var");
	                        // we have objectdef;    
			                cls = classes[cname] = {type:'object',file:f, methods:{}, props:{}};
			                // lets build the names
			                for(var n = fn[0];n;n = n.dn)if(n.v == ':'){
			                    if(n.dn.dn.v == 'function')
			                        cls.methods[n.up.v] = n.up;
			                    else
			                        cls.props[n.up.v] = n.up;     
			                };                        
	                    }else{
							var pt = js.find(fn[0],"var "+cname+" = function(){");
							if(pt.length){
								cls = classes[cname] = {type:'decorator',file:f, methods:{}, props:{}};
								
								var it = js.find( pt[0], "{/this\\.*/} =");
			                    for(var i = 0;i<it.length;i++)if(it[i].v){
			                        var m = it[i].v.match(/this\.([a-zA-Z0-9\_\$]+)/);
		                            // lets check if its a function
		                            if(it[i].dn.dn.v == 'function')
		                                cls.methods[m[1]] = it[i];
		                            else
		                                cls.props[m[1]] = it[i];
								}
							}else{
								// find the bla.x = 
								var it =  js.findrx(fn[0],"{/"+cname+"\\..*/} =");
								if(it.length){
									cls = classes[cname] = {type:'singleton',file:f, methods:{}, props:{}};								
									for(var i = 0;i<it.length;i++)if(it[i].v){
				                        var m = it[i].v.match(/this\.([a-zA-Z0-9\_\$]+)/);
			                            // lets check if its a function
			                            if(it[i].dn.dn.v == 'function')
			                                cls.methods[m[1]] = it[i];
			                            else
			                                cls.props[m[1]] = it[i];
									}
								}
								else {
							        var pt = js.find(fn[0],"var "+cname+" = new (function(){");
	        						if(pt.length){
	    								cls = classes[cname] = {type:'singleton',file:f, methods:{}, props:{}};
	    								var it = js.find( pt[0], "{/this\\.*/} =");
	    			                    for(var i = 0;i<it.length;i++)if(it[i].v){
	    			                        var m = it[i].v.match(/this\.([a-zA-Z0-9\_\$]+)/);
	    		                            // lets check if its a function
	    		                            if(it[i].dn.dn.v == 'function')
	    		                                cls.methods[m[1]] = it[i];
	    		                            else
	    		                                cls.props[m[1]] = it[i];
	    								}
	        						}
							    }
							}
						}
	                }    
	            }
	        }
			var apf = js.findrx(f.node,/apf\..*/,2);
	        if(!cls){
				//if(  apf.length)console.log( f.name+ ":0 APF symbols but no module");
			}else{
				cls.apf = apf;
			}
			
			// lets also find all apf.* things
	        // after tree modification this reserializes the entire js file
	        //fs.writeFileSync(f.name, js.serialize(f.node));        
	    }
	}
	return classes;
}
