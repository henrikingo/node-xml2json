require("./declare");

var cq = require("./codequery");
var pu = require('./parseutil');
var fs = require('fs');
var files = pu.loadFiles("./", /\.h$/, /deps|\_glue/, "c");
var OutputData = []
var util = require('util');

function Parameter(type, name, defaultvalue)
{
	return {type:type, name: name, defaultvalue: defaultvalue};
}

function Def(o3def, exttype, returntype, name, parameters)
{
	return {o3def: o3def, exttype: exttype, returntype: returntype, name: name, parameters: parameters};
}

function Enum(name, value)
{
	return {name: name, value: value};
};

function hasProp(obj)
{
	for (var v in obj)
		return true;
	return false;	
};

function addDef(prop, def)
{
	var propName = def.name;
	if (propName.indexOf('__')!=-1)
		return;		
	propName = propName.replace(/^m?\_+/, "").replace(/_[a-z]/g, function(m,a) {
		return m.slice(1).toUpperCase();                    
	});		
	if (def.o3def == 'o3_set') {
		propName = propName.replace(/^set([A-Z])/, 
			function(m, a){ return a.toLowerCase();});
	}

	var prop = prop[propName] ? prop[propName] : prop[propName] = {};

	switch (def.o3def) {
		case 'o3_fun': prop.methods ? prop.methods.push(def) : prop.methods = [def];
		//TODO: error reports here
		case 'o3_set': prop.setters ? prop.setters.push(def) : prop.setters = [def]; break;
		case 'o3_get': prop.getter = def; break;
		case 'o3_prop': 
			def.o3def = 'o3_get'; 
			addDef(prop,def);
			addDef(prop,Def('o3_set', def.exttype, def.returntype, def.name));
			break;		
	}	
}

files.forEach(function(f){
    //console.log(f.name);
	var Results = [];
	var Classes = [];
	var Enums = [];
	var Functions = [];
	var Properties = {};
	var Output = [];
	var gluegen = false;
	
    f.root.find("o3_glue_gen").parent().prevScan(/class|struct/).next().each(function(n){
        Properties = {};
        console.log(n.nv);
		var classname = n.nv;
        var scope = n.scan("{}").first().child();
        var baseClass = scope.scan("o3_begin_class(<$>)").child().eq(0).val();
        var ifaces = scope.scan("o3_add_iface(<$>)").child();
		var ext = "";
        gluegen = true;
		scope.scan(/o3_fun|o3_set|o3_get|o3_prop/).each(function(n){
			ext = "";
            n.query().prev().prev().prev().filter('o3_ext').filled(function(){
                //console.log( "Function is member of: "+this.next().child() );
				ext = this.next().child().val();
            });
            var s = n.scan("</[a-zA-Z\_]+.*/><$>(<$>)", ";");
            if(s.empty()){ // o3_set o3_prop or o3_get on immediate
               s = n.query().scanOne(/\;/);
               var propname = s.prev().val(),
                   type = s.prev().prev().serializePrevUntil(2, n.nv); 
               //console.log(n.nv+" ## "+type+" ## "+propname);
			   //Functions.push(Def(n.nv, ext, type, propname));
               addDef(Properties, Def(n.nv, ext, type, propname));
			   return;
            }
            var returnType = s.eq(0).prev().serializePrevUntil(2, n.nv);
            var fname = s.eq(0).val();
            //console.log(n.nv+" ## "+returnType+" ## "+fname);
		  
			var Params = [];
            // split arguments
            s.eq(1).child().split(",",2).forEach(function(i){ 
                cq.parse(i).query().scan('=').empty(function(){
                    var type = this.end().lastSibling().prev().prev().serializePrevUntil(2, false); // type
					var name = this.end().lastSibling().prev().eq(0).val();
					Params.push(Parameter(type, name));
                }).filled(function(){
                    var type = this.prev().prev().serializePrevUntil(2,false); // type
                    var defval = this.next().serialize(2); // default value
                    var name = this.end().lastSibling().prev().eq(0).val();
					Params.push(Parameter(type, name, defval));
                });
            })
			//Functions.push(Def(n.nv, ext, returnType, fname, Params));
			addDef(Properties,Def(n.nv, ext, returnType, fname, Params));
		});
        var def = 0;
        scope.scan("o3_enum(<$>)").child().each(function(n){
             n.split(",",2).slice(1).forEach(function(i){
                 var name, value = def++;
                 cq.parse(i).query().scan('=').empty(function(){
                     name = this.end().serialize(2) // name
                 }).filled(function(){
                     name = this.prev().serializePrevUntil(2,false); // name
                     value = this.next().serialize(2); // value
                 });
                 //console.log(name +" ::: "+value);
				 Enums.push(Enum(name, value));
             });
        });
		//if (!hasProp(Properties)) {
		//	return;
		//}	
		var Class = [];
		Class.properties = Properties;
		if (Enums.length)
		{
			Class.enums = Enums;
		}
		Class.classname = classname;
		Class.base = baseClass;
		Classes.push(Class);		
    });
	if (gluegen)
		generate(f,Classes);
	gluegen = false;	
});

function generate(file, classes) {
	var clazz,out=[];
	out.push('namespace o3 {\n\n');	
	for (var c=0; c<clazzes.length; c++) {
		clazz = clazzes[c];
		out.push(genFunctions(clazz.clazzname, clazz.properties),
			genExtTraitTable(clazz.clazzname, clazz.properties),
			genInit(clazz.clazzname, clazz.base, clazz.properties, clazz.enums)
		);

	}
	out.push('}\n');
	fs.writeFileSync(file.name.replace('.h','_glue.h'), out.join(''));
} 

function genInit(className, baseClass, properties, enums) {
	function setMethod(className, propName) {
		var ret = []; 
		ret.push('		target->Set(v8::String::NewSymbol("',propName,'"),\n\
				v8::FunctionTemplate::New(',className,'_',propName,')->GetFunction());\n'); 
		return ret.join('');
	}

	function setProp(className, propName, getter, setter) {
		var ret = [];	
		ret.push('		target->SetAccessor(String::New("',propName,'"),',
			getter,',', setter,');\n');
		return ret.join('');
	}
	
	var ret = [], prop;

	ret.push( 'void ',className,'::initInternal(Handle<Object> target)\n','{\n',
		baseClass=='cScr' ? '' : ('		' + baseClass + '::init(target);\n'), 
'		HandleScope scope;\n\
		iCtx* ctx = (iCtx*) Local<External>::Cast(\n\
			target->GetInternalField(0))->Value();\n\
		siMgr mgr = ctx->mgr();\n\
		mgr->collectV8ExtTraits("',className,'",target);\n');

	for (p in properties) {
		var getter, setter;
		prop = properties[p];
		if (prop.methods)
			ret.push(setMethod(className,p));
		else {
			getter = className + '_' + p;
			setter = prop.setters ? className + '_' + p : '0';
			ret.push(setProp(className, p,getter,setter));
		}
	}

	if (enums)
		for (var i=0; i<enums.length; i++) {
			ret.push('		target->Set(v8::String::NewSymbol("',enums[i].name,'"),\n\
					v8::Integer::New(',enums[i].value,'));\n'); 
		}

	ret.push('}\n\n');
	return ret.join('');
}	

function genFunctions(className, properties) {
	
	function initFuncionForMethod(className, propName) {
		var ret = [];
		
		ret.push('Handle<Value> ',className,'_',propName, "(const Arguments& args) \n\
{\n\
		HandleScope scope;\n\
		iCtx* ctx = (iCtx*) Local<External>::Cast(\n\
			args.This()->GetInternalField(0))->Value();\n\
		",className,"* pthis = (",className,"*) (iScr*) Local<External>::Cast(\n\
			args.This()->GetInternalField(1))->Value();\n\n");
		return ret.join("");	
	}

	function initFuncionForProperty(className, propName, bSetter) {
		var ret = [];
		ret.push(bSetter ? 'void ' : 'Handle<Value> ',className,'_',propName, "(Local<String> property,",
			bSetter ?  "Local<Value> value," : ""," const AccessorInfo &info) \n\
{\n\
		HandleScope scope;\n\
		iCtx* ctx = (iCtx*) Local<External>::Cast(\n\
			info.Holder()->GetInternalField(0))->Value();\n\
		",className,"* pthis = (",className,"*) (iScr*) Local<External>::Cast(\n\
			info.Holder()->GetInternalField(1))->Value();\n\n");
		return ret.join("");	
	}	
	
	function checkArgs(fun) {
		var spec=0, def=0, first=null;
		for (var i=0; i<fun.parameters.length; i++) {			
			if (genParamForCall([], fun.parameters[i], i-spec) > 0)
				spec++;
			else if (!first)
				first = i;
				
			if (defval(fun.parameters[i]))
				def++;
		}
		return {min: fun.parameters.length - spec - def, 
				max: fun.parameters.length - spec,
				first: first,
				spec: spec,
				def: def}	
	}
	
	function argcCheck(fun) {
//		var args = countArgs(fun);
//		if (args.min == args.max)
//			return "		if (arg.Length() < " + args.min + " )"
	}
	
	function defval(param) {
		// TODO: handle special cases here
		if (param.type == 'siEx*' || param.type == 'siEx *')
			return null;
		return param.defaultvalue ? param.defaultvalue : null;
	}
	
	function genParamForCall(output, param, index, bSetter) {		
		function convert(output, param, index, bSetter) {
			var from = bSetter ? 'value' : 'args[' + index + ']',
				base, wrap, spec = 0, arg = 'ctx->toVar(' + from + ')';		
			switch (param.type) {
				case 'bool' : base = arg + '.toBool()'; break;
				case 'int' : base = arg + '.toInt32()'; break;
				case 'int32_t' : base = arg + '.toInt32()'; break;
				case 'size_t' : base = arg + '.toInt32()'; break;
				case 'int64_t' : base = arg + '.toInt64()'; break;
				case 'double' : base = arg + '.toDouble()'; break;
				case 'const char*' : base = arg + '.toStr()'; break;
				case 'const Str&' : case 'const Str &' : base = arg + '.toStr()'; break;
				case 'const wchar_t *' : case 'const wchar_t*' : base = arg + '.toWStr()'; break;
				case 'const Var&' : case 'const Var &' : base = arg; break
					//'Var *'	
				case 'const WStr&' : case 'const WStr &' : base = arg + '.toWStr()'; break
				case 'const Buf&' : case 'const Buf &' : base = arg + '.toScr()';
					wrap = {start:'Buf(siBuf(' , end: '))'}; break;
				case 'iScr*' : case 'iScr *' : base = arg + '.toScr()'; break;
				case 'iCtx*' : case 'iCtx *' : base = 'ctx'; spec = 1; break;
				case 'iUnk*' : case 'iUnk *' : base = 'pthis'; spec = 1; break;
				case 'siEx*' : case 'siEx *' : base = '&ex'; spec = 1; break;
				default:
					if (param.type.match(/i[\w]+[\s]*\*/)) {				
						base = arg + ".toScr()";
						wrap = {start:'s' +  param.type.substring(0,param.type.indexOf('*')) + '(', end:')'};
					} 
					else
						console.log('unknown arg type: ', param.type);
			}

			if (wrap) output.push(wrap.start);
			output.push(base);
			if (wrap) output.push(wrap.end);	
			return spec;
		}
		
		var def = defval(param);
			
		if (def) output.push('args.Length() > ', index, '? ');
		var spec = convert(output, param, index, bSetter)
		if (def) output.push(' : ', def);	
		return spec;
	}
	
	function genFunctionCall(className, fun, bSetter) {		
		var ret = [],spec = 0, wrapper = {start:'', end:''};
		if (fun.returntype == 'Buf')
			wrapper = {start: 'o3_new(cScrBuf)(',end:')'}
		
		if (fun.parameters) {		
			ret.push(wrapper.start, fun.exttype ? (className + '::') : 'pthis->', fun.name, '(');
			//console.log('genFor: ' + fun.name);
			for (var i=0; i<fun.parameters.length; i++) {
				spec += genParamForCall(ret, fun.parameters[i], i-spec, bSetter);
				ret.push(',');
			}
			if (fun.parameters.length>0)
				ret.pop();
				
			ret.push(')',wrapper.end);	
		}
		else {
			if (bSetter) {
				ret.push('pthis->', fun.name, ' = ');
				genParamForCall(ret, {type: fun.returntype}, 0, true);
			}
			else {
				ret.push('pthis->', fun.name);
			}	
		}
		
		
		return ret.join('');	
	}
	
	function genOverloadedCall(className, functions, bSetter) {
		var singleArgCase = true,
			ret = [],
			call = calls = [],
			firsts = [], args;
			
		for (var i=0; i<functions.length; i++) {
			args = checkArgs(functions[i]);
			if (args.min != args.max) {
				console.log('Erro: default value for overloaded functions are not supported');
				return '';
			}
			if (args.min != 1)
				singleArgCase = false;
			if (calls[args.min] && !(singleArgCase && args.min == 1)) {
				console.log('Error: overloading with same argument count is not supported only if argument count is one');
				return '';
			}
			call = calls[args.min] = ((functions[i].returntype == 'void') ? '' : 'ret = ') +
				genFunctionCall(className, functions[i], bSetter);
			
			if (singleArgCase) {
				firsts.push({ idx: args.first,
					arg: functions[i].parameters[args.first],
					call: call});
			}
		}	 	
		
		if (singleArgCase) {
			ret.push('		');
			var commonType = null, type, iface, regular, from;
			for (var i=0; i<firsts.length; i++) {
				type = firsts[i].arg.type;
				if (type.indexOf('const Buf')==0) {
					iface = 'siBuf';
				} 
				else if (type.match(/i[\w]+\*/)) {
					iface = 's' +  type.substring(0,type.indexOf('*'));
				} 
				else {
					iface = null;
					if (regular)
						console.log('Error: multiple overloads for a function with one argumen is supported only for only one none overload that takes other as argument as an interface (or buffer)');
					regular = firsts[i].call;
					continue;
				}
				from = bSetter ? 'value' : 'args[' + firsts[i].idx + ']'
				ret.push('if(',iface,'((ctx->toVar(',from,')).toScr())) \n');
				ret.push('			',firsts[i].call,';\n		else ');
			}
			if (regular)
				ret.push('			',regular,';\n');
			else
				ret.push('{\n		// throw exception here\n		}\n');		
		}
		else {
			ret.push('		');
			for (var c in calls) {
				ret.push('if(args.Length()==',c,')\n',
					'			', calls[c], ';\n		else ');	
			}
			ret.push('{\n		// throw exception here\n		}\n');
		}
		
		return ret.join('');
	}
	
	
		
	function checkEx(fun) {
		if (fun.length) {
			for(var i=0;i<fun.length;i++)
				if (checkEx(fun[i]))
					return true;
			return false;		
		}

		var params = fun.parameters;
		if (params)
			for (var i=0;i<params.length;i++)
				if (params[i].type == 'siEx*' || params[i].type == 'siEx *')
					return true;
				
		return false;		
	}
	
	function genThrowEx() {
		return "		if (ex)\
			ThrowException(String::New(ex->message()));\n";
	}
	
	function genHelperForMethods(className, propName, prop) {
		var ret = [], ex;
		ret.push(initFuncionForMethod(className, propName));		
		if (ex = checkEx(prop.methods))
			ret.push('		siEx ex;\n')
			
		if (prop.methods.length == 1) 
			if (prop.methods[0].returntype == 'void')
				ret.push('		Var ret;\n		',genFunctionCall(className, prop.methods[0]),';\n');
			else
				ret.push('		Var ret(',genFunctionCall(className, prop.methods[0]), ');\n');
		else { 
			ret.push('		Var ret;\n',
				genOverloadedCall(className, prop.methods));
		}
		if (ex)
			ret.push(genThrowEx());
		ret.push('		return scope.Close(ctx->toValue(ret));\n}\n\n');
		return ret.join('');
	}
	
	function genHelperForGetters(className, propName, prop) {
		var ret = [], ex;
		ret.push(initFuncionForProperty(className, propName));
		if (prop.getter) {
			if (ex = checkEx(prop.getter))
				ret.push('		siEx ex;\n');
			ret.push('		Var ret(',genFunctionCall(className, prop.getter),');\n');			
		}
		else 
			ret.push('		Var ret;\n');		
		if (ex)
			ret.push(genThrowEx());
		ret.push('		return scope.Close(ctx->toValue(ret));\n}\n\n');
		return ret.join('');	
	}
	
	function genHelperForSetters(className, propName, prop) {
		var ret = [], ex;
		ret.push(initFuncionForProperty(className, propName, true));
		if (ex = checkEx(prop.setters))
			ret.push('		siEx ex;\n');

		if (prop.setters.length == 1)
			ret.push('		',genFunctionCall(className, prop.setters[0],true),';');
		else	
			ret.push('		Var ret;\n', genOverloadedCall(className, prop.setters, true));	
		if (ex)
			ret.push(genThrowEx());
		ret.push('\n}\n\n');	
		return ret.join('');
	}
	
	var prop, ret = [];
	for (p in properties) {
		prop = properties[p];
		if (prop.methods)
			ret.push(genHelperForMethods(className, p, prop));
		else {
			ret.push(genHelperForGetters(className, p, prop));	
			if (prop.setters)
				ret.push(genHelperForSetters(className, p, prop));
		}	
	}
	
	return ret.join('');	
}

function genExtTraitTable(className, properties) {
	var ret = [];
	ret.push("V8Trait* ",className,"::v8ExtTraits()\n{\n\
		static V8Trait TRAITS[] = { \n");
	
	for (p in properties) {
		prop = properties[p];
		if (prop.methods) {
			for (var i=0; i<prop.methods.length; i++) {
				if (prop.methods[i].exttype)
					ret.push('			{V8Trait::TYPE_METHOD, ',prop.methods[i].exttype,', "',
						p,'", (void*) ',className,'_',prop.methods[i].name,' },\n');
			}
		}
		else {
			if (prop.getter && prop.getter.exttype)
				ret.push('			{V8Trait::TYPE_GETTER, ',prop.getter.exttype,', "',
						p,'", (void*) ',className,'_',prop.getter.name,' },\n');
			if (prop.setters)
				for (var i=0; i<prop.setters; i++)
				if (prop.setters[i].exttype)
					ret.push('			{V8Trait::TYPE_SETTER, ',prop.setters[i].exttype,', "',
						p,'", (void*) ',className,'_',prop.setters[i].name,' },\n');
		}	
	}
			
	ret.push('			{V8Trait::TYPE_END, 0, 0, 0 }\n','		};\n',
'		return TRAITS;','\n}\n\n');
	return ret.join('');
}
