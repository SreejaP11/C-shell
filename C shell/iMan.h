#ifndef __IMAN_H
#define __IMAN_H

#define SERVER "man.he.net"
#define PORT 80
#define RESPONSE_SIZE 8192

void strip_html_tags(char *src, char *dest);
void fetch_man_page(const char *command);

#endif