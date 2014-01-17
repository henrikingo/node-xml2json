#ifndef O3_cSocket_LIBEVENT_H
#define O3_cSocket_LIBEVENT_H
#ifndef O3_WIN32
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#define HSOCKET SOCKET
#define SOCKETDATA void*
#define SOCKETERROR errno
#else
#include <winsock2.h>
#include <ws2tcpip.h>
#include <Mswsock.h>
#define HSOCKET int
#define SOCKETDATA char*
#define SOCKETERROR WSAGetLastError()
#define EWOULDBLOCK WSAEWOULDBLOCK
#define EALREADY WSAEALREADY
#define EISCONN WSAEISCONN
#define EINPROGRESS WSAEINPROGRESS 
#endif
#include <event.h>

namespace o3 {
    struct cSocket : cSocketBase {
        cSocket() {

        }
 
        cSocket(iCtx* ctx, HSOCKET sock = -1, Type type = TYPE_INVALID, int state = 0) 
			: m_sock(sock) 
		{
		    o3_trace_scrfun("cSocket");
            // Make socket non-blocking
#ifdef O3_WIN32
			unsigned long iMode = 1;
			ioctlsocket(sock, FIONBIO, &iMode);
#else
			int flags = fcntl(sock, F_GETFL, 0);            
			flags |= O_NONBLOCK;
			fcntl(sock, F_SETFL, flags);	
#endif 
			m_type = type;
            m_state = state;
			m_ctx = ctx;
			memSet(&m_ev_accept,0,sizeof(m_ev_accept));
			memSet(&m_ev_connect,0,sizeof(m_ev_connect));
			memSet(&m_ev_read,0,sizeof(m_ev_read));
			memSet(&m_ev_write,0,sizeof(m_ev_write));
        }

        ~cSocket()
        {
			close();
        }

        o3_begin_class(cSocketBase)
        o3_end_class();

		o3_glue_gen();

		static o3_ext("cO3") o3_fun siSocket socketUDP(iCtx* ctx)
		{			
			o3_trace_scrfun("socketUDP");			
			return create(ctx, TYPE_UDP);
		}

		static o3_ext("cO3") o3_fun siSocket socketTCP(iCtx* ctx) 
		{
			o3_trace_scrfun("socketTCP");
			return create(ctx, TYPE_TCP);
		}

		static o3_ext("cO3") o3_get siSocket socket(iCtx* ctx) 
		{
			return create(ctx, TYPE_TCP);
		}

		o3_fun siSocket createSocket(iCtx* ctx) 
		{
			return create(ctx, TYPE_TCP);
		}


        static siSocket create(iCtx* ctx, Type type) 
		{		
            o3_trace_scrfun("create");		
            int sock;            

            switch (type) {
            case TYPE_TCP:    
                sock = ::socket(PF_INET, SOCK_STREAM, 0);
                break;
            case TYPE_UDP:
                sock = ::socket(PF_INET, SOCK_DGRAM, 0);
                break;
            };
            if (sock < 0)
                return siSocket();
            
			siScr scr = o3_new(cSocket)(ctx, sock, type);
            return scr;
        }
        
        virtual bool bind(const char* host, int port) 
		{
            o3_trace_scrfun("bind");
            m_addr.sin_family = AF_INET;
            hostent *hp = gethostbyname(host);
            if (!hp)
                return false;
            memCopy(&m_addr.sin_addr.s_addr, hp->h_addr, hp->h_length);
            m_addr.sin_port = htons(port);

            ::bind(m_sock, (sockaddr*) &m_addr, sizeof(struct sockaddr_in));
            return true;
        }
        
        virtual bool _connect(const char* host, int port)
		{
            /*
             * Cannot connect if an error was raised or there still is a pending
             * connect or accept request.
             */
            o3_trace_scrfun("connect");
            /*
             * Cannot connect if an error was raised or there still is a pending
             * connect or accept request.
             */
            if (m_state & (STATE_ERROR | STATE_CONNECTING | STATE_ACCEPTING))
                return false;
            
            /*
             * If the socket is already connected, close the previous connection
             * first. This will cause the all flags to be cleared.
             */
            if (m_state & STATE_CONNECTED)
                close();

            // Set up the address to connect to
            m_addr.sin_family = AF_INET;
            hostent *hp = gethostbyname(host);
            if (!hp)
                return false;
            memCopy(&m_addr.sin_addr.s_addr, hp->h_addr, hp->h_length);
            m_addr.sin_port = htons(port);

            m_bytes_sent        = 0;
            m_bytes_received    = 0;

            /*
             * Set the connecting flag. This will cause connect() to be called
             * in the callback function.
             */
            m_state = STATE_CONNECTING;

			event_set(&m_ev_connect, m_sock, EV_WRITE|EV_PERSIST, onconnect, this);
			event_base_set(siCtx(m_ctx)->eventBase(),&m_ev_connect);
			event_add(&m_ev_connect, NULL);

			::connect(m_sock, (sockaddr*) &m_addr,sizeof(sockaddr_in));
			return true;
        }
        
        virtual bool accept() 
		{
            /*
             * Cannot accept if an error was raised or there still is a pending
             * connect or accept request.
             */
            o3_trace_scrfun("accept");
            /*
             * Cannot accept if an error was raised or there still is a pending
             * connect or accept request.
             */
            if (m_state & (STATE_ERROR | STATE_CONNECTING | STATE_ACCEPTING))
                return false;

            /*
             * If the socket is already connected, close the previous connection
             * first. This will cause the all flags to be cleared.
             */
            if (m_state & STATE_CONNECTED)
                close();

            // Set up the address to accept from
            m_addr.sin_family = AF_INET;
            m_addr.sin_addr.s_addr = INADDR_ANY;
            m_addr.sin_port = 0; // INPORT_ANY

            ::listen(m_sock, 10);

            /*
             * Set the accepting flag. This will cause accept() to be called
             * in the callback function.
             */
            m_state = STATE_ACCEPTING;

            // Set up a timer listener so that the callback will be called
			event_set(&m_ev_accept, m_sock, EV_READ|EV_PERSIST, onaccept, this);
			event_base_set(siCtx(m_ctx)->eventBase(),&m_ev_accept);
			event_add(&m_ev_accept, NULL);
			return true;
		}
        
        virtual bool send(uint8_t* data, size_t size) 
		{
            o3_trace_scrfun("send");
            int err;

            /* 
             * Cannot send if an error was raised or the socket is not
             * connected.
             */
            if (m_state & STATE_ERROR || !(m_state & STATE_CONNECTED))
                return false;

            switch (m_type) {
            // For TCP sockets, we send the data as a stream
            case TYPE_TCP:
                // Append the data to be sent to the output buf
                m_out_buf.append(data, size);

                /*
                 * Set the sending flag. And start a write event.
                 */
				if ( !(m_state & STATE_SENDING)) {
					m_state |= STATE_SENDING;
            
					event_set(&m_ev_write, m_sock, EV_WRITE|EV_PERSIST, onwrite, this);
					event_base_set(siCtx(m_ctx)->eventBase(),&m_ev_write);
					event_add(&m_ev_write, NULL);
				}
				break;
       
            case TYPE_UDP:
				return false;
            };
            return true;
        }
        
        virtual bool sendTo(uint8_t* data, size_t size, const char* host, int port)
        {
            o3_trace_scrfun("sendTo");
            return true;
        }
        
        virtual bool receive() 
		{
            /* 
             * Cannot receive if an error was raised or the socket is not
             * connected.
             */
            o3_trace_scrfun("receive");
            /* 
             * Cannot receive if an error was raised or the socket is not
             * connected.
             */
            if (m_state & STATE_ERROR || m_type == TYPE_TCP && !(m_state & STATE_CONNECTED))
                return false;

            /*
             * Set the receiving flag. And set up a read event.
             */
			if ( !(m_state & STATE_RECEIVING)) {
				m_state |= STATE_RECEIVING;				
				event_set(&m_ev_read, m_sock, EV_READ|EV_PERSIST, onread, this);
				event_base_set(siCtx(m_ctx)->eventBase(),&m_ev_read);
				event_add(&m_ev_read, NULL);
			}
			return true;
        }
        
        virtual void close() 
		{
			o3_trace_scrfun("close");
			if (m_sock)
#ifdef O3_WIN32
			::closesocket(m_sock);
#else			
			::close(m_sock);
#endif
			if (m_ev_accept.ev_base)
				event_del(&m_ev_accept);
			if (m_ev_connect.ev_base)
				event_del(&m_ev_connect);
			if (m_ev_read.ev_base)
				event_del(&m_ev_read);
			if (m_ev_write.ev_base)
				event_del(&m_ev_write);
            m_state = STATE_CLOSED;
			m_sock = 0;
        }

		virtual void shutdown()
		{
			if (m_sock)
			::shutdown (m_sock, 1);
			m_state &= STATE_SHUTDOWN;
		}

		static void onread(int fd, short ev, void *arg) 
		{			
            /*
             * The socket is ready for reading, we receive 
			 * at most m_packet_size bytes from the
             * socket to the input buf.
             */
			o3_trace_scrfun("onread");			
            /*
             * The socket is ready for reading, we receive 
			 * at most m_packet_size bytes from the
             * socket to the input buf.
             */
			cSocket* pthis = (cSocket*)arg;
            Buf buf;            
            buf.reserve(pthis->m_packet_size);
            sockaddr_in addr;
            socklen_t addrlen = sizeof(struct sockaddr_in);
            int size = ::recvfrom(pthis->m_sock, (SOCKETDATA)buf.ptr(), buf.capacity(), 0,
                                      (sockaddr*) &addr, &addrlen);
            //pthis->m_src_address = Str(inet_ntoa(addr.sin_addr));
            if (size < 0) {
                switch (SOCKETERROR) {
                case EINTR:
                    /*
                     * If the call to recv() was interrupted, the socket
                     * will still be ready for reading on the next pass,
                     * so we will just retry on the next callback.
                     */
                    break;
                default:
                    // In all other cases, we set the error flag
                    pthis->m_state |= STATE_ERROR;
					Delegate(siCtx(pthis->m_ctx), pthis->m_on_error)(
						siScr(pthis));
                }
            } else if (size == 0) {
                /*
                 * We assume that if the socket was ready for reading,
                 * but we received 0 bytes, that one side of the
                 * connection was closed. The most obvious way to deal
                 * with this seems to be to put the other end of the
                 * connection in an erroneous state.
                 */
                pthis->m_state |= STATE_ERROR;
				Delegate(siCtx(pthis->m_ctx), pthis->m_on_error)(
					siScr(pthis));
            } else {
                /*
                 * If the call to recv() succeeds, we append the
                 * received data to the end of the input buf, and
                 * trigger the onReceive event.
                 */
                buf.resize(size);
                pthis->m_received_buf.append(buf.ptr(), buf.size());
                pthis->m_bytes_received += size;
				Delegate(siCtx(pthis->m_ctx), pthis->m_on_receive)(
					siScr(pthis));
            }
        }
		

		static void onwrite(int fd, short ev, void *arg) 
		{
            /*
             * If the sending flag is set and the socket is ready for
             * writing, we send at most m_packet_size bytes from the
             * output buf to the socket.
             */
            o3_trace_scrfun("onwrite");
            /*
             * If the sending flag is set and the socket is ready for
             * writing, we send at most m_packet_size bytes from the
             * output buf to the socket.
             */
            cSocket* pthis = (cSocket*)arg;
			void*   data = pthis->m_out_buf.ptr();
            int size = min(pthis->m_out_buf.size(), pthis->m_packet_size);

            size = ::send(pthis->m_sock, (SOCKETDATA)data, size, 0);
            if (size < 0) {
                pthis->m_state |= STATE_ERROR;
				Delegate(siCtx(pthis->m_ctx), pthis->m_on_error)(
					siScr(pthis));
            } else {

                /*
                 * If the call to send() succeeds, the onSend event is
                 * triggered, end the data sent is removed from the
                 * output buf. If the output buf becomes empty as a
                 * result, the sending bit is cleared as well.
                 */
                pthis->m_bytes_sent += size;
				Delegate(siCtx(pthis->m_ctx), pthis->m_on_send)(
					siScr(pthis));
                pthis->m_out_buf.remove(0, size);
                if (pthis->m_out_buf.empty()) {
                    pthis->m_state &= ~STATE_SENDING;                    
					event_del(&pthis->m_ev_write);	
				}
            }
		}

		static void onaccept(int fd, short ev, void *arg) 
		{            
            o3_trace_scrfun("onaccept");            
            cSocket* pthis = (cSocket*)arg;
			socklen_t addr_len = sizeof(sockaddr_in);
            int sock = ::accept(pthis->m_sock, (sockaddr*) &pthis->m_addr,
                                  &addr_len);
            
            if (sock < 0) {
                switch (SOCKETERROR) {
                case EWOULDBLOCK:
                    /*
                     * If the call to accept() would block because no
                     * incoming connections are available, we just retry on
                     * the next callback.
                     */
                    break;
                default:
                    // In all other cases, we set the error flag
                    pthis->m_state |= STATE_ERROR;
					Delegate(siCtx(pthis->m_ctx), pthis->m_on_error)(
						siScr(pthis));
                }
            } else {
                siScr scr = o3_new(cSocket)(siCtx(pthis->m_ctx), sock, 
					pthis->m_type, STATE_CONNECTED);
              
				Delegate(siCtx(pthis->m_ctx), pthis->m_on_accept)(scr);                
            }
		}

		static void onconnect(int fd, short ev, void *arg) 
		{
			o3_trace_scrfun("onconnect");
			cSocket* pthis = (cSocket*)arg;
            int err = ::connect(pthis->m_sock, (sockaddr*) &pthis->m_addr,
                                sizeof(sockaddr_in));

            if (err < 0) {
                switch (SOCKETERROR) {
                case EALREADY:
                    /*
                     * If a previous call to connect was already done, we
                     * just retry on the next callback.
                     */
                    break;
                case EISCONN:
                    /*
                     * If the socket is already connected, we assume that
                     * a previous call to connect succeeded just before the
                     * current one.
                     */
                    pthis->m_state = STATE_CONNECTED;
					Delegate(siCtx(pthis->m_ctx), pthis->m_on_connect)(pthis);
					event_del(&pthis->m_ev_connect);
					return;
					break;
                case EINPROGRESS:
                    /*
                     * If the current call to connect is still in progress,
                     * we just retry on the next callback.
                     */
                    break;
                default:// In all other cases, we set the error flag
                    pthis->m_state |= STATE_ERROR;
					Delegate(siCtx(pthis->m_ctx), pthis->m_on_error)(
						siScr(pthis));
                }
				
            } else {
                /*
                 * If the call to connect() succeeds, the connecting flag is
                 * cleared, the connected flag is set, and the onConnect
                 * event is triggered.
                 */
                pthis->m_state = STATE_CONNECTED;
				Delegate(siCtx(pthis->m_ctx), pthis->m_on_connect)(pthis);
				event_del(&pthis->m_ev_connect);
            }
		}

		HSOCKET     m_sock;
		sockaddr_in m_addr;
		Buf         m_out_buf;
		struct event m_ev_accept;
		struct event m_ev_connect;
		struct event m_ev_read;
		struct event m_ev_write;	


    };
}

#endif // O3_cSocket_LIBEVENT_H
