(function (){
	if (!require.s.o3modules)
		require.s.o3modules = {};
	require.s.o3modules["net"] = o3NetFactory;
})();

function o3NetFactory(o3) {
	o3.require('socket');
	if(!o3.loaded)
		o3.loaded = {};
		
	o3.loaded.net = function(){
		return {createServer: createServer, createConnection: createConnection, Stream:Stream};
	}	
	
	//o3.loaded = {net:function(){
	//	return {createServer: createServer, createConnection: createConnection, Stream:Stream};
	//}};
	
	//var o3loaded = o3.loaded;
	//var blah = o3loaded.net();
	//var blah2 = o3loaded["net"]();
	
	_doCallbacks = function(self, eventType, arg) {
		var tmp = self.cb, cbGroup = tmp[eventType];
			
		if (cbGroup)
			for (var i=0; i<cbGroup.length; i++)
				cbGroup[i](arg);
	}	
	
	function on(self, eventType, cb) {
		var cbGroup;		
		cbGroup = self.cb[eventType];
		if (!cbGroup)
			cbGroup = self.cb[eventType] = [];
		
		cbGroup.push(cb);
	}

	function createServer(listener) {
		return new Server(listener);
	}
	
	function createConnection(port, host) {
		if (!host)
			host = '127.0.0.1';
		var s = new Stream();
		s.connect(port, host);
		return s;
	}
	
	function inheritEvenEmitter(_self) {
		_self.cb = {};
		//this.on('connection', listener);
		
		_self.on = _self.addListener = function(eventType, cb) {
			on(_self, eventType, cb);
		}	

		_self.removeAllListeners = function() {
			_self.cb = {};
		}	
		
		_self.removeListener = function(event, listener) {
			var cbGroup = _self.cb[event];
			var idx;
			for (var v=0; v<cbGroup.length; v++)
				if (cbGroup[v] == listener)
					idx = v, break;
			if (idx) {		
				list.splice(idx, 1);
				if (list.length == 0)
				  delete _self.cb[event];
			}
		}
		
		_self.emit = function(event, arg) {
			_doCallbacks(_self,event,arg);
		}
		
		_self.listeners = function(event) {
			return _self.cb[event];
		}
	}
	
	function Server (listener) {
			
		var _self = o3.socketTCP();
		_self.maxConnections = 20;
		_self.connections = 0;
				
		inheritEvenEmitter(_self);
		
		_self.onaccept = function(socket) {
			var s = new Stream(socket)
			listener(s);
			_doCallbacks(s, 'connect', s);
			_doCallbacks(_self, 'connection', s);
		}
		
		_self.onerror = function(socket) {
			_doCallbacks(_self,"error"); // error info here
		}
	
		_self.listen = function(port, host/*, callback*/) {
			//this.on('connection', callback);
			_self.bind(host, port);
			_self.accept();    
		}
		
		return _self;
	};
	
	
	function Stream (socket) {

		var _self = socket ? socket : o3.socketTCP();

		inheritEvenEmitter(_self)'
		
		_self.onconnect = function(socket) {
			_self.receive();
			_doCallbacks(_self,"connect", _self);
		}
		
		_self.onreceive = function(socket) {
			_doCallbacks(_self,"data", socket.receivedBuf);
			socket.clearBuf();
		}
		
		_self.onerror = function(socket) {
			_doCallbacks(_self,"error"); // error info here
		}
		
		_self.receive();
			
		_self.connect = function(port, host) {
			if (!host)
				host = '127.0.0.1';
			
			_self.onreceive = function(socket) {
				_doCallbacks(_self,"data", socket.receivedBuf);
				socket.clearBuf();
			}
			_self._connect(host, port);
		}
			
		_self.write = function(data, encoding) {
			if (!encoding)
				encoding = 'ascii';

			// TODO: do encoding here...
			
			_self.send(data);
		}
		
		_self.end = function(data, encoding) {
			if (data)
				_self.write(data, encoding);
		
			_self.shutdown();
		}
		
		_self.destroy = function() {
			_self.close();
		}	
		
		return _self;
	};
	
	function Buffer (subject, encoding) {

		var _self;
		switch (type = typeof subject) {
		  case 'number':
			_self = o3.ScrBuf(number);
			break;

		  case 'string':
			_self = o3.BufFromString(Subject);
			break;

		  case 'object': // Assume object is an array
			_self = o3.ScrBuf(subject.length);
			for (var i = 0; i < _self.length; i++)
				_self[i] = subject[i];		
			break;

		  default:
			throw new Error("First argument need to be an number, array or string.");
		}

		return _self;
	}

}