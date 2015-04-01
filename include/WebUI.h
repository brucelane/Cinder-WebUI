#pragma once

#include "WebSocketServer.h"
#include "cinder/Json.h"
#include "boost/variant.hpp"
#include "boost/signals2.hpp"
#include <map>

namespace webui {

////////////////////////////////////////////////////////////////////////////////
// Event
class Event
{
public:
    enum class Type { UNKNOWN, GET, SET };
    Event();
    Event( const Type &type, const ci::JsonTree &data );

    Type                    getType() const { return mType; }
    const ci::JsonTree &    getData() const { return mData; }
    ci::JsonTree            getData() { return mData; }
private:
    Type                    mType;
    ci::JsonTree            mData;
};

////////////////////////////////////////////////////////////////////////////////
// Server
class Server : public WebSocketServer
{
public:
    typedef boost::signals2::signal< void ( Event ) > EventSignal;
    typedef std::map< Event::Type, EventSignal > EventSignalContainer;

    Server();

    EventSignal &               getEventSignal( const Event::Type &type );

    void                        get( const std::string &name );
    template< typename T >
    void                        set( const std::string &name, const T &value );

private:
    void						onConnect();
    void						onDisconnect();
    void						onError( std::string err );
    void						onInterrupt();
    void						onPing( std::string msg );
    void						onRead( std::string msg );

    EventSignalContainer        mEventSignals;
    void                        dispatch( const Event &event );
};

////////////////////////////////////////////////////////////////////////////////
// BoundParam
template< typename T >
class BoundParam
{
public:
    typedef T value;

    BoundParam() {};
    BoundParam( const T &v ) : mValue( v ) {};

    operator                    T () const { return mValue; }
    T operator                  () () const { return mValue; }
    T & operator                () () { return mValue; }
    T & operator                = ( const T & v ) { mValue = v; return mValue; }
    T & operator                += ( const T & v ) { mValue += v; return mValue; }
    T & operator                -= ( const T & v ) { mValue -= v; return mValue; }
    T & operator                ++ () { mValue++; return mValue; }
    T & operator                -- () { mValue--; return mValue; }
private:
    T                           mValue;
};

////////////////////////////////////////////////////////////////////////////////
// WebUI
class WebUI
{
public:
    WebUI();

    void                            update();
    void                            listen( uint16_t port );

    typedef boost::variant< BoundParam< float >* > bound_param_ptr;
    typedef std::map< std::string, bound_param_ptr > bound_params_container;

    template< typename T >
    void                            bind( const std::string &name, T *param )
    {
        mParams.emplace( name, param );
    }

    bound_params_container::iterator findParam( const std::string &name );

private:
    void                            onSet( Event event );
    void                            onGet( Event event );

    bound_params_container          mParams;

    Server                          mServer;
};

} // end namespace webui