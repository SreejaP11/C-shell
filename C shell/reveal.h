#ifndef __REVEAL_H
#define __REVEAL_H

void reveal(bool showAll,bool showDetails,char *path,char* home_directory);
void printColoredFileType(struct dirent *entry, const char *path);

#endif