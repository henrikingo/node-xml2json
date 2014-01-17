(function (){
	if (!require.s.o3modules)
		require.s.o3modules = {};
	require.s.o3modules["xml"] = o3XmlFactory;
})();

function o3XmlFactory(o3) {
	o3.require('xml');
	if(!o3.loaded)
		o3.loaded = {};
		
	o3.loaded.xml = function(){
		return o3.xml;
	}	
}