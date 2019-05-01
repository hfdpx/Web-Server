#ifndef _HTTP_SERVER_H_
#define _HTTP_SERVER_H_
#include "csapp.h"

/*
  ��������

  fd  ����Ϳͻ������ӵ�socket������

*/
void doit(int fd);

/*
  ����ͷ���뺯��

  ������������ͷ��Ϣ

  fd  ����Ϳͻ������ӵ�socket��������

*/
void read_requesthdrs(rio_t *rp);

/*
  URL��������

  URL�����õ��ļ�·����CGI����
  uri         URL����
  filename    �ļ�·��
  cgiargs     CGI����
*/
int parse_uri(char *uri, char *filename, char *cgiargs);


/*
  ��̬��ҳ������

  fd         ����Ϳͻ������ӵ�socket������
  filename   �ļ�����·��
  filesize   �ļ���С

*/
void serve_static(int fd, char *filename, int filesize);

/*
  �ļ�������������

  ���ļ����õ��ļ�������
*/
void get_filetype(char *filename, char *filetype);

/*
  ��̬��ҳ������

  �����;�̬��������һ��
*/
void serve_dynamic(int fd, char *filename, char *cgiargs);

/*
  ��������
*/
void clienterror(int fd, char *cause, char *errnum,char *shortmsg, char *longmsg);

#endif
