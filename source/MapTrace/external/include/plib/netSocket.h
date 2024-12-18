/*
     PLIB - A Suite of Portable Game Libraries
     Copyright (C) 1998,2002  Steve Baker
 
     This library is free software; you can redistribute it and/or
     modify it under the terms of the GNU Library General Public
     License as published by the Free Software Foundation; either
     version 2 of the License, or (at your option) any later version.
 
     This library is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
     Library General Public License for more details.
 
     You should have received a copy of the GNU Library General Public
     License along with this library; if not, write to the Free Software
     Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 
     For further information visit http://plib.sourceforge.net

     $Id: netSocket.h,v 1.20 2004/03/28 03:58:24 sjbaker Exp $
*/

/****
* NAME
*   netSocket - network sockets
*
* DESCRIPTION
*   netSocket is a thin C++ wrapper over bsd sockets to
*   facilitate porting to other platforms
*
* AUTHOR
*   Dave McClurg <dpm@efn.org>
*
* CREATION DATE
*   Dec-2000
*
****/

#ifndef NET_SOCKET_H
#define NET_SOCKET_H


#include "ul.h"
#include <errno.h>

/*
 * Socket address, internet style.
 */
class netAddress
{
  /* DANGER!!!  This MUST match 'struct sockaddr_in' exactly! */
  short          sin_family     ;
  unsigned short sin_port       ;
  unsigned int   sin_addr       ;
  char           sin_zero [ 8 ] ;

public:
  netAddress () {}
  netAddress ( const char* host, int port ) ;

  void set ( const char* host, int port ) ;
  const char* getHost () const ;
  int getPort() const ;

  static const char* getLocalHost () ;

  bool getBroadcast () const ;
};


/*
 * Socket type
 */
class netSocket
{
  int handle ;

public:

  netSocket () ;
  virtual ~netSocket () ;

  int getHandle () const { return handle; }
  void setHandle (int handle) ;
  
  bool  open        ( bool stream=true ) ;
  void  close		    ( void ) ;
  int   bind        ( const char* host, int port ) ;
  int   listen	    ( int backlog ) ;
  int   accept      ( netAddress* addr ) ;
  int   connect     ( const char* host, int port ) ;
  int   send	    ( const void * buffer, int size, int flags = 0 ) ;
  int   sendto      ( const void * buffer, int size, int flags, const netAddress* to ) ;
  int   recv	    ( void * buffer, int size, int flags = 0 ) ;
  int   recvfrom    ( void * buffer, int size, int flags, netAddress* from ) ;

  void setBlocking ( bool blocking ) ;
  void setBroadcast ( bool broadcast ) ;

  static bool isNonBlockingError () ;
  static int select ( netSocket** reads, netSocket** writes, int timeout ) ;
} ;


int netInit ( int* argc, char** argv = NULL ) ;  /* Legacy */

int netInit () ;


const char* netFormat ( const char* fmt, ... ) ;


#endif // NET_SOCKET_H

