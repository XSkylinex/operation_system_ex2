#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#define NOEQU 1
#define EQU   2
typedef char* String;

int openFile(char* path, int flag);
long getFileSize(char* path);

struct RESRC {
  int opens[2];  // never more then 2 files checked
  int count;
};
struct RESRC ReSrc = { {0,0}, 0 };
void exitComp(int result);

int main(int argc,char* argv[])
{
  String path1, path2;
  int fileDsc1, fileDsc2;
  long size1, size2;

  char ch1 = '\0', ch2 = '\0';
  int read1, read2;

  if (argc < 3) {
    exitComp(NOEQU);   // perror("not enough arguments");
  }
  path1 = argv[1];  path2 = argv[2];
  fileDsc1 = openFile(path1, O_RDONLY);
  fileDsc2 = openFile(path2, O_RDONLY);

  if (strcmp(path1, path2) == 0)
    // same path == same file!
    exitComp(EQU);

  if (getFileSize(path1) != getFileSize(path2))
    // content length NOEQUAL
    exitComp(NOEQU);

  // two file, same length, but different paths
  do {
    if( (read1 = read(fileDsc1, &ch1, 1)) == -1 ||
        (read2 = read(fileDsc2, &ch2, 1)) == -1
    ) exitComp(NOEQU);  // perror(read related error)
    if (ch1 != ch2)
      exitComp(NOEQU);  // file contents don't match !
  } while (read1 != 0 || read2 != 0);

  exitComp(EQU);  // same file contents !
}

void exitComp(int result) {
  int i;
  for (i=0; i<ReSrc.count; i++)
    close(ReSrc.opens[i]);
  // printf("%d\n",result);   // DEBUG
  exit(result);
}

int openFile(char* path, int flag) {
  int fd = open(path, flag);
  if (fd == -1)
    exitComp(NOEQU);   // perror("file error");
  ReSrc.opens[ReSrc.count++] = fd;
  return fd;
}

long getFileSize(char* path) {
  struct stat st;
  stat(path, &st);
  return st.st_size;
}
