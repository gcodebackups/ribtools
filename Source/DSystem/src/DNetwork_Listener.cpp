//==================================================================
/// DNetwork_Listener.cpp
///
/// Created by Davide Pasca - 2009/8/1
/// See the file "license.txt" that comes with this project for
/// copyright info. 
//==================================================================

#include "DNetwork.h"
#include "DNetwork_Listener.h"

//==================================================================
namespace DNET
{

//==================================================================
Listener::Listener( U32 port ) :
	mPort(port),
	mListenSock(INVALID_SOCKET)
{
}

//==================================================================
Listener::~Listener()
{
	Stop();
}

//==================================================================
bool Listener::Start()
{
	struct sockaddr_in	in_sa;

	mListenSock = socket(PF_INET, SOCK_STREAM, 0);

	if ( mListenSock == INVALID_SOCKET )
		return false;

	memset( &in_sa, 0, sizeof(in_sa) );
	in_sa.sin_family = AF_INET;
	in_sa.sin_addr.s_addr = htonl( INADDR_ANY );
	in_sa.sin_port        = htons( mPort );

	if ( -1 == bind( mListenSock, (struct sockaddr *)&in_sa, sizeof(in_sa) ) )
	{
		closesocket( mListenSock );
		mListenSock = -1;
		return false;
	}

	if ( -1 == listen( mListenSock, SOMAXCONN ) )
	{
		closesocket( mListenSock );
		mListenSock = -1;
		return false;
	}

	if NOT( SetNonBlocking( mListenSock ) )
	{
		closesocket( mListenSock );
		mListenSock = -1;
		return false;
	}

	return true;
}

//==================================================================
void Listener::Stop()
{
	if ( mListenSock != INVALID_SOCKET )
	{
		closesocket( mListenSock );
		mListenSock = INVALID_SOCKET;
	}
}

//==================================================================
Listener::IdleRetType Listener::Idle( SOCKET &out_acceptedSock, U32 waitMSec )
{
	struct timeval	tv;
	fd_set			rdset;

	out_acceptedSock = INVALID_SOCKET;

	if ( mListenSock == INVALID_SOCKET )
		return IRT_NOTHING;

	FD_ZERO( &rdset );
	FD_SET( mListenSock, &rdset );

	tv.tv_sec = 0;
	tv.tv_usec = waitMSec * 1000;

	if ( select( 1, &rdset, NULL, 0, &tv ) == -1 )
	{
		Stop();
		return IRT_ERROR;
	}

	if ( FD_ISSET( mListenSock, &rdset ) )
	{
		out_acceptedSock = accept( mListenSock, 0, 0 );

		if ( out_acceptedSock != INVALID_SOCKET )
		{
			// make non-blocking
			if NOT( SetNonBlocking( mListenSock ) )
			{
				closesocket( out_acceptedSock );
				out_acceptedSock = INVALID_SOCKET;
				return IRT_ERROR;
			}

			return IRT_CONNECTED;
		}
	}

	return IRT_NOTHING;
}

//==================================================================
}
