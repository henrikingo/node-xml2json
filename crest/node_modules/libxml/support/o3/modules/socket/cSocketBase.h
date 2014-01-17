namespace o3 {    
    struct iSocket : iUnk {
        enum Type {
            TYPE_INVALID,
            TYPE_UDP,
            TYPE_TCP
        };

        enum State {        
            STATE_ERROR = 1,
            STATE_CLOSED = 2,
            STATE_CREATED = 4,
            STATE_CONNECTED = 8,
            STATE_CONNECTING = 16,
            STATE_ACCEPTING = 32,
            STATE_RECEIVING = 64,
            STATE_SENDING = 128,
			STATE_SHUTDOWN = 256
        };

        virtual bool bind(const char* addr, int port) = 0;
        virtual bool accept() = 0;
        virtual bool _connect(const char* host, int port) = 0;
        virtual bool receive() = 0;
        virtual bool send(uint8_t* data, size_t size) = 0;
        virtual bool sendTo(uint8_t* data, size_t size, const char* url, int port) = 0;
        virtual void close() = 0;
		virtual void shutdown() = 0;
    };

    o3_iid(iSocket, 0xe7ff0650, 0xee4f, 0x4bb2, 0xa0, 0x8, 0x8, 0x8f, 0x5c, 0xa4, 0xc, 0x45);

    struct cSocketBase: cScr, iSocket {
        cSocketBase() : m_packet_size(1024),
                       m_bytes_sent(0),
                       m_bytes_received(0),
					   m_min_receive_size(0),
                       m_state(0),
                       m_type(TYPE_INVALID) 
        {
        }

        virtual ~cSocketBase() 
        {
        }

        o3_begin_class(cScr)    
            o3_add_iface(iSocket)
        o3_end_class();

		o3_glue_gen();

        o3_enum( "State",        
            STATE_ERROR = 1,
            STATE_CLOSED = 2,
            STATE_CREATED = 4,
            STATE_CONNECTED = 8,
            STATE_CONNECTING = 16,
            STATE_ACCEPTING = 32,
            STATE_RECEIVING = 64,
            STATE_SENDING = 128
        );

		o3_enum( "Type",
			TYPE_INVALID,
			TYPE_UDP,
			TYPE_TCP
		);       

		o3_get Str readyState() 
		{
			o3_trace_scrfun("error");
			if (m_state & STATE_CLOSED || m_state & STATE_ERROR)
				return "closed";
			if (m_state & STATE_CONNECTED && m_state & STATE_RECEIVING) 
				return "open";
			if (m_state & STATE_CONNECTING)	
				return "opening";
			if (m_state & STATE_SHUTDOWN)
				return "readOnly";
			if (m_state & STATE_CONNECTED && !(m_state & STATE_RECEIVING))	
				return "writeOnly";
			return "closed";
		}

        o3_get bool error()
        {
            o3_trace_scrfun("error");
            return (m_state & STATE_ERROR) > 0;
        }

        o3_get bool isClosed() 
        {
            o3_trace_scrfun("isClosed");
            return (m_state & STATE_CLOSED) > 0;
        }

        o3_get bool isCreated() 
        {
            o3_trace_scrfun("isCreated");
            return (m_state & STATE_CREATED) > 0;
        }

        o3_get bool isConnected() 
        {
            o3_trace_scrfun("isConnected");
            return (m_state & STATE_CONNECTED) > 0;
        }

        o3_get bool isConnecting() 
        {
            o3_trace_scrfun("isConnecting");
            return (m_state & STATE_CONNECTING) > 0;
        }

        o3_get bool isAccepting() 
        {
            o3_trace_scrfun("isAccepting");
            return (m_state & STATE_ACCEPTING) > 0;
        }

        o3_get bool isReceiveing() 
        {
            o3_trace_scrfun("isReceiveing");
            return (m_state & STATE_RECEIVING) > 0;
        }

        o3_get bool isSending()
		{
            o3_trace_scrfun("isSending");
            return (m_state & STATE_SENDING) > 0;
        }
                        
        o3_fun bool bind(int port)
        {
            o3_trace_scrfun("bind");
            return bind("0.0.0.0", port);
        }

        o3_fun bool bind(const char* full)
        {
            o3_trace_scrfun("bind");
            Str host;
            int port;
            parseUrl(full, host, port);
            return bind(host, port);
        }

        o3_fun bool bind(const char* full, int port) = 0;

        o3_fun bool _connect(const char* full) 
        {
            o3_trace_scrfun("connect");
            Str host;
            int port;
            parseUrl(full, host, port);
            return _connect(host, port);
        }

        o3_fun bool _connect(const char* host, int port) = 0;

		o3_fun bool accept() = 0;

		o3_fun bool receive() = 0;

        o3_fun bool send(const char* data) 
        {
            o3_trace_scrfun("send");
            return send((uint8_t*)data, strLen(data));
        }

        o3_fun bool send(iBuf* ibuf) 
        {            
            o3_trace_scrfun("send");            
            if (!ibuf)
                return false;

            return send((uint8_t*)ibuf->unwrap().ptr(), 
				ibuf->unwrap().size());
        }

        virtual bool send(uint8_t* data, size_t size) = 0;

        o3_fun bool sendTo(iBuf* ibuf, const char* host, int port)
        {
			o3_trace_scrfun("sendTo");
			host; port;
			// TODO: handle host and port
            if (!ibuf)
                return false;

            return send((uint8_t*)ibuf->unwrap().ptr(), 
				ibuf->unwrap().size());
        }

        o3_fun bool sendTo(iBuf* ibuf, const char* full_addr)
        {
            o3_trace_scrfun("sendTo");
            Str host;
            int port;
            parseUrl(full_addr, host, port);
            return sendTo(ibuf, host, port);
        }

        virtual bool sendTo(uint8_t* data, size_t 
            size, const char* url, int port) = 0;
        

        o3_get Buf receivedBuf() 
        {
			o3_trace_scrfun("receivedBuf");
			return m_received_buf; 
        }
        
        o3_get Str receivedText() 
		{
			o3_trace_scrfun("receivedText");
			size_t next_zero;
			int8_t zero=0;
			Buf buf(m_received_buf);
			while( NOT_FOUND != (next_zero 
				= buf.find(&zero,1)))
			{
				buf.remove(next_zero,1);
			}

			return buf;
        }

		o3_fun void clearBuf()
		{
			o3_trace_scrfun("clearBuf");
			m_received_buf.clear();
			m_bytes_received = 0;
		}
		

        bool parseUrl(const char* url, Str& host, int& port) 
		{
            o3_trace_scrfun("parseUrl");
            const char* p;

            for (p = url; *p; ++p)
                if (*p == ':')
                    break;
            host = Str(url, p-url);
            port = Str(++p).toInt32();
            return true;
        }

		o3_get siScr onaccept()
		{
			o3_trace_scrfun("onaccept");
			return m_on_accept;
		}
		o3_set siScr setOnaccept(iScr* cb)
		{
			o3_trace_scrfun("setOnaccept");
			return m_on_accept = cb;
		}

		o3_get siScr onconnect()
		{
			o3_trace_scrfun("onconnect");
			return m_on_connect;
		}
		o3_set siScr setOnconnect(iScr* cb)
		{
			o3_trace_scrfun("setOnconnect");
			return m_on_connect = cb;
		}

		o3_get siScr onreceive()
		{
			o3_trace_scrfun("onreceive");
			return m_on_receive;
		}
		o3_set siScr setOnreceive(iScr* cb)
		{
			o3_trace_scrfun("setOnreceive");
			return m_on_receive = cb;
		}

		o3_get siScr onsend()
		{
			o3_trace_scrfun("onsend");
			return m_on_send;
		}
		o3_set siScr setOnsend(iScr* cb)
		{
			o3_trace_scrfun("setOnsend");
			return m_on_send = cb;
		}

		o3_get siScr onerror()
		{
			return m_on_error;
		}
		o3_set siScr setOnerror(iScr* cb)
		{
			return m_on_error = cb;
		}

		Str className()
		{
			if (m_class_name.size())
				return m_class_name;
			else
				return cScr::className();
		}

		o3_fun void close() = 0;

		o3_fun void shutdown() = 0;

        o3_prop     size_t  m_packet_size;
        o3_prop     size_t  m_min_receive_size;
					Str		m_class_name;
		o3_get		size_t  m_bytes_sent;
        o3_get		size_t  m_bytes_received;                
        o3_get      Type    m_type;
        o3_get		Str     m_src_address;
		

		siScr m_on_accept;
		siScr m_on_connect;
		siScr m_on_receive;
		siScr m_on_send;
		siScr m_on_error;

        Buf     m_received_buf;
        int     m_state;
		siCtx	m_ctx;

    };     
}
