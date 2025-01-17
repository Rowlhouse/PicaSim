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

     $Id: netMessage.h,v 1.11 2004/03/21 17:41:07 sjbaker Exp $
*/

/****
* NAME
*   netMessage - message buffer and channel classes
*
* DESCRIPTION
*   messages are a binary format for sending buffers over a channel.
*   message headers contain a type field and length.
*
* AUTHOR
*   Dave McClurg <dpm@efn.org>
*
* CREATION DATE
*   Dec-2000
****/

#ifndef __NET_MESSAGE__
#define __NET_MESSAGE__


#include "netBuffer.h"

#ifndef UL_MSVC
#include <netinet/in.h> // ntohs() etc prototypes
#endif

#ifdef __FreeBSD__
#include <arpa/inet.h>
#endif


class netGuid //Globally Unique IDentifier
{
public:
  unsigned char data [ 16 ] ;

  netGuid () {}

  netGuid ( unsigned int   l,
            unsigned short w1, unsigned short w2,
            unsigned char  b1, unsigned char  b2,
            unsigned char  b3, unsigned char  b4,
            unsigned char  b5, unsigned char  b6,
            unsigned char  b7, unsigned char  b8 )
  {
    //store in network format (big-endian)

    data[ 0] = (unsigned char)(l>>24) ;
    data[ 1] = (unsigned char)(l>>16) ;
    data[ 2] = (unsigned char)(l>> 8) ;
    data[ 3] = (unsigned char)(l    ) ;
    data[ 4] = (unsigned char)(w1>>8) ;
    data[ 5] = (unsigned char)(w1   ) ;
    data[ 6] = (unsigned char)(w2>>8) ;
    data[ 7] = (unsigned char)(w2   ) ;

    data[ 8] = b1 ;
    data[ 9] = b2 ;
    data[10] = b3 ;
    data[11] = b4 ;
    data[12] = b5 ;
    data[13] = b6 ;
    data[14] = b7 ;
    data[15] = b8 ;
  }

  bool operator ==( const netGuid& guid ) const
  { 
    return memcmp ( data, guid.data, sizeof(data) ) == 0 ;
  }

  bool operator !=( const netGuid& guid ) const
  { 
    return memcmp ( data, guid.data, sizeof(data) ) != 0 ;
  }
} ;


class netMessage : public netBuffer
{
  int pos ;

  void seek ( int new_pos ) const
  {
    if ( new_pos < 0 )
      new_pos = 0 ;
    else
    if ( new_pos > length )
      new_pos = length ;

    //logical const-ness

    ((netMessage*)this) -> pos = new_pos ;
  }

  void skip ( int off ) const
  {
    seek(pos+off);
  }

public:

  // incoming message; header is already there
  netMessage ( const char* s, int n ) : netBuffer(n)
  {
    assert ( n >= 5 ) ;
    append(s,n);
    pos = 5 ; // seek past header
  }

  // outgoing message
  netMessage ( int type, int to_id, int from_id=0, int n=256 ) : netBuffer(n)
  {
    // output header
    putw ( 0 ) ;  //msg_len
    putbyte ( type ) ;
    putbyte ( to_id ) ;
    putbyte ( from_id ) ;
  }

  int getType () const { return ( (unsigned char*)data )[ 2 ] ; }
  int getToID () const { return ( (unsigned char*)data )[ 3 ] ; }
  int getFromID () const { return ( (unsigned char*)data )[ 4 ] ; }
  void setFromID ( int from_id ) { ( (unsigned char*)data )[ 4 ] = (unsigned char)from_id; }

  void geta ( void* a, int n ) const
  {
    assert (pos>=0 && pos<length && (pos+n)<=length) ;
    //if (pos>=0 && pos<length && (pos+n)<=length)
    {
      memcpy(a,&data[pos],n) ;
      seek(pos+n);
    }
  }
  void puta ( const void* a, int n )
  {
    append((const char*)a,n);
    pos = length;
    *((unsigned short*)data) = (unsigned short)length ; //update msg_len
  }

  int getbyte () const
  {
    unsigned char temp ;
    geta(&temp,sizeof(temp)) ;
    return temp ;
  }
  void putbyte ( int c )
  {
    unsigned char temp = c ;
    puta(&temp,sizeof(temp)) ;
  }

  bool getb () const
  {
    unsigned char temp ;
    geta(&temp,sizeof(temp)) ;
    return temp != 0 ;
  }
  void putb ( bool b )
  {
    unsigned char temp = b? 1: 0 ;
    puta(&temp,sizeof(temp)) ;
  }

  int getw () const
  {
    unsigned short temp ;
    geta ( &temp, sizeof(temp) ) ;
    return int ( ntohs ( temp ) ) ;
  }
  void putw ( int i )
  {
    unsigned short temp = htons ( (unsigned short) i ) ;
    puta ( &temp, sizeof(temp) ) ;
  }

  int geti () const
  {
    unsigned int temp ;
    geta ( &temp, sizeof(temp) ) ;
    return int ( ntohl ( temp ) ) ;
  }
  void puti ( int i )
  {
    unsigned int temp = htonl ( (unsigned int) i ) ;
    puta ( &temp, sizeof(temp) ) ;
  }

  void getfv ( float* fv, int n ) const
  {
    unsigned int* v = (unsigned int*)fv;
    geta ( v, (n<<2) ) ;
    for ( int i=0; i<n; i++ )
      v[i] = ntohl ( v[i] ) ;
  }
  void putfv ( const float* fv, int n )
  {
    const unsigned int* v = (const unsigned int*)fv;
    for ( int i=0; i<n; i++ )
    {
      unsigned int temp = htonl ( v[i] ) ;
      puta ( &temp, sizeof(temp) ) ;
    }
  }
  
  float getf () const
  {
    unsigned int temp ;
    geta ( &temp, sizeof(temp) ) ;
    temp = ntohl ( temp ) ;
    return *((float*)&temp) ;
  }
  void putf ( float f )
  {
    unsigned int temp = *((unsigned int*)&f) ;
    temp = htonl ( temp ) ;
    puta ( &temp, sizeof(temp) ) ;
  }

  void gets ( char* s, int n ) const
  {
    char* src = &data[pos];
    char* dst = s;
    while (pos<length)
    {
      char ch = *src++;
      if ((dst-s)<(n-1))
        *dst++ = ch ;
      ((netMessage*)this)->pos++;
      if (ch==0)
        break;
    }
    *dst = 0 ;
  }
  void puts ( const char* s )
  {
    puta(s,strlen(s)+1);
  }

  void print ( FILE *fd = stderr ) const
  {
    fprintf ( fd, "netMessage: %p, length=%d\n", this, length ) ;
    fprintf ( fd, "  header (type,to,from) = (%d,%d,%d)\n",
      getType(), getToID(), getFromID() ) ;
    fprintf ( fd, "  data = " ) ;
    for ( int i=0; i<length; i++ )
      fprintf ( fd, "%02x ", data[i] ) ;
    fprintf ( fd, "\n" ) ;
  }
};


class netMessageChannel : public netBufferChannel
{
  virtual void handleBufferRead (netBuffer& buffer) ;

public:

  bool sendMessage ( const netMessage& msg )
  {
    return bufferSend ( msg.getData(), msg.getLength() ) ;
  }

  virtual void handleMessage ( const netMessage& msg ) {}
};


#endif //__NET_MESSAGE__
