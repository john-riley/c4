/* cfengine for GNU
 
        Copyright (C) 1995
        Free Software Foundation, Inc.
 
   This file is part of GNU cfengine - written and maintained 
   by Mark Burgess, Dept of Computing and Engineering, Oslo College,
   Dept. of Theoretical physics, University of Oslo
 
   This program is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the
   Free Software Foundation; either version 2, or (at your option) any
   later version.
 
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
 
  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA

*/


/*********************************************************************/
/*                                                                   */
/*  TOOLKITS: network library                                        */
/*                                                                   */
/*********************************************************************/

#include "cf.defs.h"
#include "cf.extern.h"

/*************************************************************************/

int SendTransaction(int sd,char *buffer,int len,char status)

{ char work[CF_BUFSIZE];
  int wlen;
 
if (len == 0) 
   {
   wlen = strlen(buffer);
   }
else
   {
   wlen = len;
   }
    
if (wlen > CF_BUFSIZE-CF_INBAND_OFFSET)
   {
   FatalError("SendTransaction software failure");
   }
 
snprintf(work,CF_INBAND_OFFSET,"%c %d",status,wlen);

memcpy(work+CF_INBAND_OFFSET,buffer,wlen);

Debug("Transaction Send[%s][Packed text]\n",work); 
 
if (SendSocketStream(sd,work,wlen+CF_INBAND_OFFSET,0) == -1)
   {
   return -1;
   }

return 0; 
}

/*************************************************************************/

int ReceiveTransaction(int sd,char *buffer,int *more)

{ char proto[CF_INBAND_OFFSET+1];
  char status = 'x';
  unsigned int len = 0;
 
memset(proto,0,CF_INBAND_OFFSET+1);

if (RecvSocketStream(sd,proto,CF_INBAND_OFFSET,0) == -1)   /* Get control channel */
   {
   return -1;
   }

sscanf(proto,"%c %u",&status,&len);
Debug("Transaction Receive [%s][%s]\n",proto,proto+CF_INBAND_OFFSET);

if (len > CF_BUFSIZE - CF_INBAND_OFFSET)
   {
   snprintf(OUTPUT,CF_BUFSIZE,"Bad transaction packet -- too long (%c %d) Proto = %s ",status,len,proto);
   CfLog(cferror,OUTPUT,"");
   return -1;
   }

if (strncmp(proto,"CAUTH",5) == 0)
   {
   Debug("Version 1 protocol connection attempted - no you don't!!\n");
   return -1;
   }
 
 if (more != NULL)
    {
    switch(status)
       {
       case 'm': *more = true;
           break;
       default: *more = false;
       }
    }
 
return RecvSocketStream(sd,buffer,len,0);
}

/*************************************************************************/
 
int RecvSocketStream(int sd,char buffer[CF_BUFSIZE],int toget,int nothing)
 
{ int already, got;
  static int fraction;

Debug("RecvSocketStream(%d)\n",toget);

if (toget > CF_BUFSIZE-1)
   {
   CfLog(cferror,"Bad software request for overfull buffer","");
   return -1;
   }

for (already = 0; already != toget; already += got)
   {
   got = recv(sd,buffer+already,toget-already,0);

   if (got == -1)
      {
      CfLog(cfverbose,"Couldn't recv","recv");
      return -1;
      }
 
   if (got == 0)   /* doesn't happen unless sock is closed */
      {
      Debug("Transmission empty or timed out...\n");
      fraction = 0;
      buffer[already] = '\0';
      return already;
      }

   Debug("    (Concatenated %d from stream)\n",got);

   if (strncmp(buffer,"AUTH",4) == 0 && (already == CF_BUFSIZE))
      {
      fraction = 0;
      buffer[already] = '\0';
      return already;
      }
   }

buffer[toget] = '\0';
return toget;
}


/*************************************************************************/

/*
 * Drop in replacement for send but includes
 * guaranteed whole buffer sending.
 * Wed Feb 28 11:30:55 GMT 2001, Morten Hermanrud, mhe@say.no
 */

int SendSocketStream(int sd,char buffer[CF_BUFSIZE],int tosend,int flags)

{ int sent,already=0;

do
   {
   Debug("Attempting to send %d bytes\n",tosend-already);

   sent=send(sd,buffer+already,tosend-already,flags);
   
   switch(sent)
      {
      case -1:
          CfLog(cfverbose,"Couldn't send","send");
          return -1;
      default:
          Debug("SendSocketStream, sent %d\n",sent);
          already += sent;
          break;
      }
   }
 while (already < tosend); 

 return already;
}

/*************************************************************************/

/**
 * Set timeout for recv(), in milliseconds.
 * @param ms must be > 0.
 */
int SetReceiveTimeout(int fd, unsigned long ms)
{
    assert(ms > 0);

    Debug("Setting socket timeout to %lu seconds.\n", ms/1000);

/* On windows SO_RCVTIMEO is set by a DWORD indicating the timeout in
 * milliseconds, on UNIX it's a struct timeval. */

#if !defined(__MINGW32__)
    struct timeval tv = {
        .tv_sec = ms / 1000,
        .tv_usec = (ms % 1000) * 1000
    };
    int ret = setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));
#else
    int ret = setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &ms, sizeof(ms));
#endif

    if (ret != 0)
    {
		Debug("Failed to set socket timeout to %lu milliseconds.", ms);
        return -1;
    }

    return 0;
}

/*************************************************************************/

ConnectionInfo *ConnectionInfoNew(void)
{
    struct ConnectionInfo *info;

    if ((info = (struct ConnectionInfo *)malloc(sizeof(struct ConnectionInfo))) == NULL)
    {
        FatalError("Memory Allocation failed for ConnectionInfoNew()");
    }
    info->sd = SOCKET_INVALID;

    return info;
}

struct Key {
    RSA *key;
    Hash *hash;
};

void KeyDestroy(Key **key)
{
    if (!key || !*key)
    {
        return;
    }
    if ((*key)->key)
    {
        RSA_free((*key)->key);
    }
    HashDestroy(&(*key)->hash);
    free (*key);
    *key = NULL;
}

void ConnectionInfoDestroy(ConnectionInfo **info)
{
    if (!info || !*info)
    {
        return;
    }
    /* Destroy everything */
    if ((*info)->ssl)
    {
        SSL_free((*info)->ssl);
    }
    KeyDestroy(&(*info)->remote_key);
    free(*info);
    *info = NULL;
}

ProtocolVersion ConnectionInfoProtocolVersion(const ConnectionInfo *info)
{
    return info ? info->protocol : CF_PROTOCOL_UNDEFINED;
}

void ConnectionInfoSetProtocolVersion(ConnectionInfo *info, ProtocolVersion version)
{
    if (!info)
    {
        return;
    }
    switch (version)
    {
    case CF_PROTOCOL_UNDEFINED:
    case CF_PROTOCOL_CLASSIC:
    case CF_PROTOCOL_TLS:
        info->protocol = version;
        break;
    default:
        break;
    }
}

int ConnectionInfoSocket(const ConnectionInfo *info)
{
    return info ? info->sd : -1;
}

void ConnectionInfoSetSocket(ConnectionInfo *info, int s)
{
    if (!info)
    {
        return;
    }
    info->sd = s;
}

SSL *ConnectionInfoSSL(const ConnectionInfo *info)
{
    return info ? info->ssl : NULL;
}

void ConnectionInfoSetSSL(ConnectionInfo *info, SSL *ssl)
{
    if (!info)
    {
        return;
    }
    info->ssl = ssl;
}

const Key *ConnectionInfoKey(const ConnectionInfo *info)
{
    const Key *key = info ? info->remote_key : NULL;
    return key;
}

void ConnectionInfoSetKey(ConnectionInfo *info, Key *key)
{
    if (!info)
    {
        return;
    }
    /* The key can be assigned only once on a session */
    if (info->remote_key)
    {
        return;
    }
    if (!key)
    {
        return;
    }
    info->remote_key = key;
}

const unsigned char *ConnectionInfoBinaryKeyHash(ConnectionInfo *info, unsigned int *length)
{
    if (!info)
    {
        return NULL;
    }
    Key *connection_key = info->remote_key;
    unsigned int real_length = 0;
    const char *binary = 'TODO';
    if (length)
    {
        *length = real_length;
    }
    return binary;
}

const char *ConnectionInfoPrintableKeyHash(ConnectionInfo *info)
{
    return info ? 'TODO' : NULL;
}
