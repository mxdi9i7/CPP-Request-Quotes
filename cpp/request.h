#ifndef __REQUEST__
#define __REQUEST__

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>


using namespace std;

class http
{
private:
  FILE* fp;

public:
  http(const int fd) : fp(fdopen(fd, "r+")) { }

  http(const char* const address, const u_short port)
  {
    struct sockaddr_in dstAddr;  
    bzero((char*)&dstAddr, sizeof(dstAddr));
    dstAddr.sin_family = AF_INET;
    dstAddr.sin_port = htons(port);
  
    const struct hostent * const hp = gethostbyname(address);
    if(hp==NULL) throw h_errno;
    bcopy(hp->h_addr, &dstAddr.sin_addr, hp->h_length);
    
    const int dstSocket = socket(AF_INET, SOCK_STREAM, 0);
  
    const int ret_connect = connect(dstSocket, (struct sockaddr*)&dstAddr, sizeof(dstAddr));
    if(ret_connect<0) throw ret_connect;

    this->fp = fdopen(dstSocket, "r+");
  }
  
  int get(const char* const path) const
  {
    return fprintf(fp, "GET %s HTTP/1.1\r\n", path);
  }

  int header_host(const char* const hostname) const
  {
    return fprintf(fp, "HOST: %s\r\n", hostname);
  }

  int header_range(const int start, const int end) const
  {
    return fprintf(fp, "Range: bytes=%d-%d\r\n", start, end);
  }

  int header_connection(const bool alive) const
  {
    if(alive)
      return fprintf(fp, "Connection: Keep-Alive\r\n");
    else
      return fprintf(fp, "Connection: close\r\n");
  }

  int request() const
  {
    const int ret_fprintf = fprintf(fp, "\r\n");
    if(ret_fprintf<0) return ret_fprintf;
    const int ret_fflush = fflush(fp);
    return ret_fflush;
  }

  char* read_get(FILE* const fp_out) const
  {

    {
      char version_str[8];
      int status_num;
      char message_str[128];
      const int ret_fscanf = fscanf(fp, "HTTP/%s %d %[^\r]\r\n", version_str, &status_num, message_str);
      if(ret_fscanf!=3) throw ret_fscanf;
    }
    
    int content_length = -1;

    while(true) {
      char buf[1024];
      const char* const ret_fgets = fgets(buf, sizeof(buf), fp);
      if(ret_fgets==NULL) throw ret_fgets;

      char key_str[128]; char value_str[128];
      const int ret_sscanf = sscanf(buf, "%[^:]: %[^\r]\r\n", key_str, value_str);
      if(ret_sscanf!=2) {
	if(strcmp(buf, "\r\n")) throw;
	break;
      }

      if(strcmp(key_str, "Content-Length")==0) {
	sscanf(value_str, "%d", &content_length);
      }
    }

    if(content_length>0) {
      char* body_ary = (char*)malloc(content_length);
      fread(body_ary, 1, content_length, fp);
      return body_ary;
    } else {
      ;
    }
    
  }

  int close() const { return fclose(this->fp); }
};

#endif