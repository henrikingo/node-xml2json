/*
 * See the NOTICE file distributed with this work for additional
 * information regarding copyright ownership.
 *
 * This is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation; either version 2.1 of
 * the License, or (at your option) any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this software; if not, write to the Free
 * Software Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA, or see the FSF site: http://www.fsf.org.
 *
 */

// TreeC a C language Family Token Tree (Java, C++, C, Javascript, etc?)
/*
    node {
        pn : node, // parentNode
        fc : node, // firstChild
        ns : node, // nextSibling
        ps : node, // prevSibling
        nv : string, // nodeValue 
        nt : id, //nodeType see exports.type
        ws : string // whitespace (and comments) before this token
        p : int // character position in source
        l : int // line number in source( 0 based)
    }
*/

//declare(function(require, exports, module){

    var node = function(nt, p, l, ws, nv, pn, ps){
       this.nt = nt, this.p = p, this.l = l, this.ws = ws, this.nv = nv, this.pn = pn, this.ps = ps;
    };
    var nodeSet =  function(set, chain){
        this.chain = chain;
        this.set = set, this.length = set.length;   
    };
        
    var end = new node(0, 0, 0, "", ""); end.pn = end.ps = end.ns = end.fc = end;
            
    (function(){
        this.type = {
            term:0,        // terminator node (similar to null)
            comment:1,     // comment (when parsed)
            open:2,        // [ { or ( is the only type that can have a firstChild subtree
            close:3,       // ] } or )
            operator:4,    // operator (+- etc)
            keyword:5,     // keyword (while for)
            constant:6,    // constant (true/false/null/undefined/NaN/Infinity)
            identifier:7,  // identifier
            number:8,      // number
            string:9,      // "' string
            regexp:10,     // /rx/ regexp
            unknown:11,    // unknown thing (&= and other unsupported operators)
            newline:12,    // not in tree
            whitespace:13, // not in tree
            eof:14         // not in tree
        }

        this.nt = 0;
        this.ws = this.nv = "";
        this.pn = this.ps = this.fc = this.ns = end;
        
        this.toString = function(nows, s, l){
            if(!s) s = [];
            if(!l) l = 0;
            if(this.nt){
                for (var n = this; n.nt; n = n.ns) {
                    nows?
                        s.push(n.nv ):
                        s.push( n.ws + n.nv );
                        
                    if (n.nt == 2)
                        this.toString.call( n.fc, nows, s, l + 1 );
                }
            }
            if(!l) return s.join('');
        }
        
        this.name = {};    
        for(var k in this.type)
            this.name[this.type[k]] = k;

        this.dump = function(s, l){
            if(!s) s = [];
            if(!l) l = 0;
            if(!this.nt)
                return "term";
            for (var n = this; n.nt; n = n.ns) {
                var type = (this.name[n.nt]+'        ').slice(0,10)+'  '+Array(l+1).join('  ');
                if (n.nt == 2){
                    s.push(type, n.nv,"\n");
                    this.dump.call(n.fc, s, l+1);
                }
                else 
                    s.push(type, n.nv,"\n");
            }
            if(!l) return s.join('');
        }
        
        function match(t1, t2, found){
           for(var n1 = t1, n2 = t2; (n1.nt && n2.nt); n1 = n1.ns, n2 = n2.ns){
           	if(n2.nv == '{' && n2.fc.nt && n2.fc.nt==10){
   				if(!n2.rx) 
   					n2.rx = new RegExp(n2.fc.nv.slice(1,-1));
   				
   				if(!n1.nv.match( n2.rx ))
   					break;
   				if(!n2.ns)console.log(n2);
   				
   				n2 = n2.ns;
   			} else {
   			    if(n2.nv.charAt(0)=='$'){ // eat any token following
   			      found[n2.nv.slice(1)] = n1;
   			    }else if(n1.nv != n2.nv)
       				break;
       			if(n2.nt == 2){
       				if(n2.fc.nt){
       				    if (!n2.fc.ns.nt && n2.fc.nv.charAt(0) == '$'){ // we need to store a subnode
       				        found[n2.fc.nv.slice(1)] = n1.fc;
       				    } else {
	       					var m = match( n1.fc, n2.fc, found);
	       					if(!m) break;
	       					if(!n2.ns.nt) return m;
       				    }
       				}
   					if(!n2.ns) return n1.fc;
       			}
   			}
       	}
   		if(!n2.nt)
   			return t1;
       }
       
       function scan(t1, t2, deep, results, l){
       	if(!results) results = [];
       	if(!l) l = 0;
           
       	for (var n = t1; n.nt; n = n.ns) {
       	    var found = {};
       		var m = match(n, t2, found);
           	
       		if(m){
       		    m.found = found;
       			results.push(m);
       		}
       		if(n.nt == 2 && deep){
           		if(deep!=1 || 
           		  !(n.nv=='{' && (n.ps.ps.ps.nv=='function' || n.ps.ps.ps.ps.nv=='function')))
           			scan(n.fc, t2, deep, results, l+1);
           	}
           }
       	if(!l)return results;
       }
   
       function scanrx(t, rx, deep, results){
           for(var n = t;n.nt;n = n.ns){
       		if(n.nv.match(rx))
       			results.push(n);
   			if(n.nt == 2 && n.fc.nt && deep){
   				matchrx(n.fc, rx, deep, results);
   			}
   		}
       }

       this.root = function() {
            var n = this;
            if (!ns.nt) return;
            while (n.pn.nt) n = n.pn;
            while (n.ps.nt) n = n.ps;
            return n;
        }        
        
        function search(where, what, deep, all){
            if(!where.nt)
                return all?[]:end;
            
            var set = [where];
            if(what.constructor == RegExp){
               var res = [];
               for(var i = 0;i<set.length;i++){
    	    		matchrx(set[i], rx, deep, res);
    	    	}
    	    	return opt.all?res:res[0];
        	} else {
    			var args = what.split("#");
    			for(var i = 0;i<args.length;i++){
    	    		var nw = parse(args[i],{noEOF:1});
    	    		
    	    		var set2 = [];
    	    		for(var j = 0;j<set.length;j++){
    	    			set2.push.apply(set2, scan(set[j], nw, deep));
    	    		}
    	    		set = set2;
    	    	}
    	    	return all?set:(set[0]||end);
    	    }
        }
        
        this.scanOne = function(what){
            return search(this, what, 0, 0);
        }
        
        this.scan = function(what){
            return nodeSet(search(this, what, 0, 1));
        }
        
        this.findOne = function(what){
            return search(this, what, 2, 0);
        }
        
        this.find = function(what){
            return new nodeSet(search(this, what, 2, 1));
        }
        
        this.query = function(){
            return new nodeSet([this]);
        }
         
        // split, does a token based split and serializes each chunk into an output array
        this.split = function(token, opt){
            var ws = opt?opt.ws:true;
            var out = [];
            for(var n = this;n.nt; n = n.ns){
                // scan to next token outputting data
                for(var s = [];n.nt && n.nv != token; n = n.ns){
                    s.push( ws?(n.ws+n.nv):n.nv );
                    if (n.nt == 2)
                        this.toString.call( n.fc, s, l + 1 );                
                }
                if(s.length)
                    out.push(s.join(''));
            }
            return out;
        }

        // adds nodes before
        this.before = function(thing){
            insertNodes(this, false, parse(thing, {noEOF:1}) );    
            return this;
        }
        
        // adds nodes after
        this.after = function(thing){
            insertNodes(this, true, parse(thing, {noEOF:1}) );    
            return this;
        }
        // replaces a number of nodes
        this.replace = function(thing, len){
            if(!this.nt) return;
            var n = this.ns;
            if(len==undefined || len<0) n = end;
            else while(len>1 && n.nt) len--, n = n.ns;
            replaceNodes(this, n, parse(thing, {noEOF:1}));
            return this;
        }
        
        this.replacews = function(newws, len){
            var n = this;
            if(len == undefined) len = 1;
            while(n.nt && len>0){
                n.ws = newws;
                n = n.ns, len--;
            }
            return this;
        }
        
        // removes nodes
        this.remove = function(len){
            if(!this.nt) return;
            var n = this.ns;
            if(len==undefined || len<0) n = end;
            else while(len>1 && n.nt) len--, n = n.ns;
            replaceNodes(this, n, end);
            return this;
        }
        
        function insertNodes( where, after, what ){
            var last = end;
            if(!where.nt) return; 
            if(!what.nt) return;
            if(after){
                if(where.ns.nt)
                    where.ns = what, what.ps = where; // plug inbetween previous and to
                what.pn = where.pn, last = where.ns;	                
            } else {
                if(where.ps.nt)
                    where.ps.ns = what, what.ps = where.ps; // plug inbetween previous and to
                else if(where.pn.nt)                  
                    where.pn.fc = what; // must be first child
                what.pn = where.pn, from = what, last = where;
            }
            for(var m, n = what.ns;n.nt && n!=last;n = n.ns) // walk to end replacing parents
                n.pn = where.pn, m = n; 
            if(last.nt) // wire up last
                last.ps = m, m.ns = last;
        }
        
        function replaceNodes( from, to, what ){ 
            if(!what.nt){ // remove stuff
                if(from.ps.nt)
                    from.ps.ns = to;
                else if(from.pn.nt)
                    from.pn.fc = to;
                if(to.nt)
                   to.ps = from.ps;
                return;
            }
            // overwrite from 
            from.nv = what.nv, from.ns = what.ns, from.nt = what.nt;
            if(what.ns)
                what.ns.ps = from;
            last = to;
            for(var m = from, n = what.ns; n.nt && n != to; n = n.ns) // walk to end replacing parents
               n.pn = from.pn, m = n; 
            if(to.nt) // wire up last
               to.ps = m, m.ns = to;
        }
    }).call(node.prototype);

    // jQuery like code-set
    (function(){
        
        // Searching
        this.find = function( what ){
            var set = [];
            for(var i = 0, s = this.set, l = s.length;i<l;i++){
               set.push.apply( set, s[l].find( what ) );
            }
            return new nodeSet(set, this);
        }
        
        this.scan = function( what ){
            var set = [];
            for(var i = 0, s = this.set, l = s.length;i<l;i++){
               set.push.apply( set, s[l].find( what ) );
            }
            return new nodeSet(set, this);
        }
        
        // Misc
        this.size = function(){
            return this.length;
        }

        this.toArray = function(){
            return set.slice(0);
        }
        
        // Traversal
        function traverse(pthis, f, cb){
            
            var set = [], fm = function(){return true;}
            if(f !== null && f!==undefined){
	            if(f.constructor == RegExp){
	                fm = function(n){ return n.nv.match(f); }
	            } else if(f.constructor == String){
	                fm = function(n){ return n.nv == f;}
	            } else if (f.constructor == Function){
	               fm = function(n,i){ return f.call(n,i);}
	            }
            }

            for(var i = 0, s = pthis.set, l = s.length, n; i<l; i++){
                n = s[i];
                if(cb(n, set, i, fm))
                    break;
            }
            return new nodeSet(set, this);
        }

        this.has = function(){
            return traverse(this, null, function(n, set, i){
                if(n.findOne(what)) set.push(n);
            });
        }
        
        this.next = function(f){
            return traverse(this, f, function(n, set, i, fm){
                if(n.ns.nt && fm(n.ns, i)) set.push(n.ns);
            });
        }
        
        this.nextAll = function(f){
            return traverse(this, f, function(n, set, i, fm){
                for(n = n.ns; n.nt; n = n.ns) 
                    if(fm(n, i)) set.push(n);
	        });
        }
        
        this.nextUntil = function(f){
	        return traverse(this, f, function(n, set, i, fm){
	            for(n = n.ns; n.nt; n = n.ns){ 
	                if(fm(n, i)) return; 
	                set.push(n);
	            }
	        });
        }
        
        this.siblings = function(f){
            return traverse(this, f, function(n, set, i, fm){
                var m = n, j = i;
                for(var m = n.ns; m.nt; m = m.ns) 
                    if(fm(n, i)) set.push(n);
                for(var m = n; m.nt; m = m.ps) 
                    if(fm(n, i)) set.push(n);
            });
        }
        
        this.prev = function(f){
            return traverse(this, f, function(n, set, i, fm){
                if(n.ps.nt && fm(n.ps, i)) set.push(n.ps);
            })
        }
        
        this.prevAll = function(f){
            return traverse(this, f, function(n, set, i, fm){
			    for(n = n.ps; n.nt; n = n.ps) 
			        if(fm(n, i)) set.push(n);
			});
	    }
        
        this.prevUntil = function(f){
            return traverse(this, f, function(n, set, i, fm){
                for(n = n.ns; n.nt; n = n.ns){ 
                    if(fm(n, i)) return; 
                    set.push(n);
                }
            });
        }
        
        this.parent = function(f){
            return traverse(this, f, function(n, set, i, fm){
                if(n.pn.nt && fm(n.pn, i)) 
                    set.push(n.pn);
            });
        }
        
        this.child = function(f){
            return traverse(this, f, function(n, set, i, fm){
                if(n.fc.nt && fm(n.fc, i)) set.push(n.fc);
            });
        }
        
        this.parents = function(f){
            return traverse(this, f, function(n, set, i, fm){
                for(n = n.pn; n.nt; n = n.pn){ 
                    if(fm(n, i)) set.push(n);
                }
            });
        }
        
        this.filter = function(f){
            return traverse(this, f, function(n, set, i, fm){
                if(fm(n, i)) set.push(n);
            });
        }
        
        this.not = function(f){
            return traverse(this, f, function(n, set, i, fm){
                if(!fm(n, i)) set.push(n);
            });
        }
        
        this.first = function(){
            return new nodeSet( this.set.length==0?[]:[this.set[0]], this );
        }
        
        this.last = function(){
            return new nodeSet( this.set.length==0?[]:[this.set[this.length-1]], this );
        }
        
        this.end = function(){
            return this.chain;
        }
        
        this.eq = function(i){
            return new nodeSet( this.set.length<i?[]:[this.set[i]], this );
        }
        
        this.slice = function(start, end){
            return new nodeSet( this.set.slice(start,end), this );
        }
        
        this.map = function(cb, f){
            return traverse(this, f, function(n, set, i, fm){
                if(fm(n, i)){
                    var r = cb(n);
                    if(r!==undefined) set.push(r);
                }
            });
        }
        
        this.each = function(cb, f){
            traverse(this, f, function(n, set, i, fm){
                
                if(fm(n, i)) cb(n);
            });
            return this;
        }
        
        // modification
        this.before = function(thing){
            insertNodes(this, false, parse(thing, {noEOF:1}) );    
            return this;
        }
        
        // adds nodes after
        this.after = function(thing){
            insertNodes(this, true, parse(thing, {noEOF:1}) );    
            return this;
        }
        
        // replaces a number of nodes
        this.after = function(thing, f){
             traverse(this, f, function(n, set, i, fm){
                 if(fm(n, i)) n.after(thing);
             })
             return this;
        }
        
        this.before = function(thing, f){
            traverse(this, f, function(n, set, i, fm){
                if(fm(n, i)) n.before(thing);
            })
            return this;
        }
        
        this.remove = function(len, f){
            traverse(this, f, function(n, set, i, fm){
                if(fm(n, i)) n.remove(len);
            })
            return this;            
        }
        
        this.replace = function(thing, len, f){
            traverse(this, f, function(n, set, i, fm){
                if(fm(n, i))  n.replace(len);
            })
            return this;                     
        }
        
        
        // value access
        this.ws = function(set){
            if(set!==undefined){ // read
               var out = [];
	            traverse(this, null, function(n, set, i, fm){
				    out.push(n.ws);
				})
				return out.join('');
            } 
            traverse(this, null, function(n, set, i, fm){
                n.ws = set;
            });
            return this;
        }
        
        this.text = function(){
        }
        
        this.val = function(set){
            if(set !== undefined){
                if(set.length==0) return this;
                set[0].nv = set;
                return this;
            }
            else 
            return set[0].split(tok);
        }
        
        this.nodes = function(set, f){
            // we can replace our entire nodeset   
        }
        
        this.values = function(set, f){
            if(set !== undefined){
                traverse(this, null, function(n, set, i, fm){
	                n.ns = set;
	            })
	            return this;
	        } else {
	           var out = [];
               traverse(this, null, function(n, set, i, fm){
	               n.ns = set;
	           })
	           return this;
	        }
        }
        
        this.split = function(tok){
            if(set.length == 0) return [];
            return set[0].split(tok);
        }
        
        this.serialize =
        this.toString = function(nows, f){
            var out = [];
            traverse(this, f, function(n, set, i, fm){
                if(fm(n, i))  out.push(n.toString(nows));
            });
            return out.join('');
        }
        
    }).call(nodeSet.prototype);
    
    
    var lut = {'"': 9, '\'': 9, '[': 2, ']': 3, '{': 2, '}': 3, '(': 2, ')': 3,'\n': 12, '\r\n': 12, '//': 1, '/*': 1, '*/': 1, '/':10};/**/
    var close  = {'}': '{', ']': '[', ')': '('};
        
    // tokenizer RX, looks to be complete but might miss a few odd ones
    var rx = /(\r?[\n]|\/[\/*]|\*\/|["'{(\[\])}\]\/])|([ \x7F\t]+)|([#\w\$._\x80-\xFF\u0080-\uFFFF])+|([!+-/<>^*]?[\=]+|::|>>|<<|->|>>=|<<=|[+]{2}|[-]{2}|&&|[|]{2}|\\?[\w._?,:;!=+-\\\/^&|*"'[\]{}()%$#@~`<>])|$/g;/**/
    var pre_rx  = {'throw': 1, 'return': 1, '(': 1, '[': 1, '{':1, ',': 1, '=': 1, ":": 1};
    
    exports.jskeywords = {
        "!":4,"%":4,"&":4,"*":4,"--":4,"-":4,"++":4,"+":4,"~":4,"===":4,"==":4,
        "=":4,"!=":4,"!==":4,"<=":4,">=":4,"<":4,">":4,"&&":4,"?":4,":":4,
        "*=":4,"%=":4,"+=":4,"-=":4,";":4,"<<":4,">>":4,",":4,":":4,
        "in":4, "new":4, "typeof":4, "instanceof":4, "void":4, "delete":4,
        "break":5,"case":5,"catch":5,"continue":5,"default":5,"do":5,"else":5,
        "finally":5,"for":5,"function":5,"if":5,"return":5,"switch":5,"throw":5,
        "try":5,"typeof":5,"var":5,"while":5,"with":5,
        "true":6, "false":6, "null":6, "undefined":6, "NaN":6, "Infinity":6
    };
    
    exports.ckeywords = { // TODO check and fix
        "!":4,"%":4,"&":4,"*":4,"--":4,"-":4,"++":4,"+":4,"~":4,"==":4,
        "=":4,"!=":4,"<=":4,"->":4,">=":4,"<":4,">":4,"&&":4,"?":4,":":4,"::":4,
        "*=":4,"%=":4,"+=":4,"-=":4,";":4,"<<":4,">>":4,"<<=":4,">>=":4,",":4,":":4,
        "new":4, "typeof":4, "delete":4,"sizeof":4,
        "namespace":5,"union":5,"struct":5,"class":5,"enum":5,"goto":5,"template":5,
        "break":5,"case":5,"catch":5,"continue":5,"default":5,"do":5,"else":5,
        "finally":5,"for":5,"function":5,"if":5,"return":5,"switch":5,"throw":5,
        "try":5,"typeof":5,"var":5,"while":5,"with":5,
        "true":6, "false":6, "null":6,
    };
    
    exports.javakeywords = { // TODO check and fix
        "!":4,"%":4,"&":4,"*":4,"--":4,"-":4,"++":4,"+":4,"~":4,"==":4,
        "=":4,"!=":4,"<=":4,"->":4,">=":4,"<":4,">":4,"&&":4,"?":4,":":4,
        "*=":4,"%=":4,"+=":4,"-=":4,";":4,"<<":4,">>":4,"<<=":4,">>=":4,",":4,":":4,
        "new":4, "typeof":4, "delete":4,"sizeof":4,
        "abstract":5,"continue":5,"for":5,"switch":5,
        "assert":5,"default":5,"goto":5,"package":5,"synchronized":5,
        "boolean":5,"do":5,"if":5,"private":5,"this":5,
        "break":5,"double":5,"implements":5,"protected":5,"throw":5,
        "byte":5,"else":5,"import":5,"public":5,"throws":5,
        "case":5,"enum":5,"instanceof":5,"return":5,"transient":5,
        "catch":5,"extends":5,"int":5,"short":5,"try":5,
        "char":5,"final":5,"interface":5,"static":5,"void":5,
        "class":5,"finally":5,"long":5,"strictfp":5,"volatile":5,
        "const":5,"float":5,"native":5,"super":5,"while":5,        
        "true":6, "false":6, "null":6,
    };    
    
    // Tokentree Javascript, with a few extras also parses other C family syntaxes
    var parse = 
    exports.parse = function(str, opts){
        var opts = opts || {},
            keywords = opts.keywords || exports.jskeywords,
            commentInDOM = opts.commentInDOM,
            dontEatSemi = opts.dontEatSemi;

        rx.lastIndex = 0;
        var root = new node(1,0,0,"","",end,end), n = root, b = 0, 
            t = 0, mode = 0, line = 0, err = [], ws = "",  ltok = null, m; 
        
        while (m = rx.exec(str)) {
            var tok = m[0], type = m[1] ? lut[m[1]] : (m[2] ? 13 : (m[3] ? 5 : ((m[4] || tok)? 11: 14)));    
            if (!mode) {
                
                n = n.ns = new node(type, rx.lastIndex, line, ws, tok, n.pn, n), ws = "";
                                
                switch (type) {
                    case 1: //Comment
                    	if(!commentInDOM) ws = n.ws, n = n.ps, n.ns = end;
                		mode = tok, b = [tok];
                        break;
                    case 2: // [ ( {
                    	n = n.fc = {pn: n};
                        break;
                    case 3: // } ) ]
                        if (!n.pn || n.pn.nv != close[tok])
                            n.err = n.pn?n.pn.nv:"empty", err.push(n);
                        else { // move node up and remove child
                            var m = n.pn;
                            if(n.ps) n.ps.ns = end;
                            n.pn = m.pn, n.ps = m, m.ns = n, m.fc = m.fc.ns;
                            //console.log(n);
                            if(n.fc.nt) n.fc.ps = end; 
                    	}
                        break;
                    case 5: // word or number
                        var t = tok.charCodeAt(0);
                        n.nt = (t>= 48 && t<= 57)?8:(keywords[tok] || 7);
                        break;
                    case 9: // String 
                        b = [mode = tok];
                        break;
                    case 10: // regex
                        if (pre_rx[ltok]) mode = tok, b = [mode = tok];
                        else n.nt = 1;
                        break;
                    case 11: // eat ; into ws if we are not in ()
                       if(!dontEatSemi && tok==';' && (!n.pn || n.pn.nv!='('))
                            ws = n.ws + tok, n = n.ps, n.ns = end, line++;
                       else // store operator type
                           n.nt = keywords[tok] || 11;
                       break;
                    case 12: // \n
                        line++;
                    case 13: // whitespace
                        ws = n.ws + tok, n = n.ps, n.ns = end;
                        break;
                }
            }
            else { //In comment or string mode
            
                b[b.length] = tok;

                switch (type) {
                    case 1: //Comment
                        if (tok == '*/'){
                            if(mode == '/*') {
	                            mode = 0;
	                            if(commentInDOM)
	                            	n.nv = b.join('');
	                            else
	                            	ws += b.join('');
                            } else if (mode == '/')
                                mode = 0, n.nv = b.join('');
                        }
                        break;
                    case 9: //String
                        if (mode == tok)
                            mode = 0, n.nv = b.join('');
                        break;                        
                    case 10: // regex
                        if(mode == '/') 
                            mode = 0, n.nv = b.join('');
                        break;
                    case 12: //New line
                        line++;
                    case 14: //eof
                        if (mode == '//'){
                            mode = 0;
                            if(commentInDOM)
                            	n.nv = b.join('');
                            else {
                            	ws += b.join('');
                            	if(type == 14)
                            	    n = n.ns = new node(type,rx.lastIndex,line,ws,'',n.pn,n), ws = "";
                            }
                        }
                        break;
                }
            }
            if(type <= 11) ltok = tok;
            else if(type == 14)break;
        };
        
        if(opts.noEOF && n.nt ==14){
            if(n.ws) n.nt = 11;
            else n.ps.ns = end;
        } 
        if(root.ns)
           root = root.ns, root.ps = null;
        if (mode)
            err.push(n);
       
        while (n.pn.nt){
            err.push(n);
            n = n.pn, n.fc = n.fc.ns;
            if(n.fc.nt) n.fc.ps = end;
        }
        root.err = err.length?err:null;
        return root;
    };
           
          // before } when not object
          // newline before statement
          //     for
          //     if
          // 	while
          // 	switch
          // 	var
          // 	function x
          // 	delete
          // 	debugger
          // 	do
           //; anywhere except IN ()
           
           //newline not after or before
           // 	+ - / = += -= >> << , .
           //	'nothing'
           	
           //var x,y,z 
           //+ call();
           
//});
