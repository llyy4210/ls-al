/*************************************************************************
	> File Name: ls.c
	> Author: 
	> Mail: 
	> Created Time: 2018年09月23日 星期日 11时09分03秒
 ************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <pwd.h>
#include <utime.h>
#include <time.h>
#include <grp.h>

#ifndef FILE_MAX
#define FILE_MAX 1024
#endif

char fileName[FILE_MAX][FILENAME_MAX];

int rwxMode[] = {0,S_IRUSR, S_IWUSR, S_IXUSR, S_IRGRP, S_IWGRP, S_IXGRP, S_IROTH, S_IWOTH, S_IXOTH};

char getFileType(mode_t mode){
      if(S_ISDIR(mode)) return 'd';
      else if(S_ISCHR(mode))  return 'c';
      else if(S_ISBLK(mode)) return 'b';
      else if(S_ISFIFO(mode)) return 'f';
      else if(S_ISLNK(mode)) return 'l';
      else if(S_ISSOCK(mode)) return 's';
      else return '-';

}

void sort(char array[][FILENAME_MAX], int n){
      int i,j;
      char temp[FILENAME_MAX];
      for(i = n-1; i >= 0; --i)
        for(j = 0; j < i; ++j)
    if(strcmp(array[j], array[j+1]) > 0){
                strcpy(temp, array[j]);
                strcpy(array[j], array[j+1]);
                strcpy(array[j+1], temp);
              
    }

}

int main(int argc, char *argv[]){
      char *dirPath;
      if(argc >= 2) dirPath = argv[1];
    else{
            dirPath = malloc(FILENAME_MAX);
        if(getcwd(dirPath, FILENAME_MAX) == NULL){
                  perror("getcwd returns error");
                  exit(EXIT_FAILURE);
                
        }
          
    }
      
      DIR * dirp = opendir(dirPath);
    if(dirp == NULL){
            perror("opendir returns error");
            exit(EXIT_FAILURE);
          
    }
      int filen = 0, i, j;
    while(1){
            struct dirent * fileInfo = readdir(dirp);
            if(fileInfo == NULL) break;
            strcpy(fileName[filen++], fileInfo->d_name);
          
    }
      sort(fileName, filen);
    for(i = 0; i < filen; ++i){
            struct passwd * userInfo;
            struct group * groupInfo;
            struct stat fileStat;
            struct tm *mTime;
            char fileMode[11], filePath[FILENAME_MAX];
            if(dirPath[strlen(dirPath)-1] != '/')
              sprintf(filePath,"%s/%s",dirPath, fileName[i]);
            else sprintf(filePath, "%s%s",dirPath, fileName[i]);
        if(stat(filePath, &fileStat) < 0){
                  perror("stat returns error");
                  exit(EXIT_FAILURE);
                
        }
            strcpy(fileMode, "-rwxrwxrwx");
            fileMode[0] = getFileType(fileStat.st_mode);
            for(j = 1; j < 10; ++j)
              if(!(fileStat.st_mode & rwxMode[j])) fileMode[j] = '-';

            userInfo = getpwuid(fileStat.st_uid);
            groupInfo = getgrgid(fileStat.st_gid);
        if(userInfo == NULL || groupInfo == NULL){
                  perror("getpwuid returns error");
                  exit(EXIT_FAILURE);
                
        }
            mTime = localtime(&fileStat.st_mtime);
            printf("%s %2d %8s %8s %5d %2d月 %2d %02d:%02d %s\n",fileMode,fileStat.st_nlink,userInfo->pw_name, groupInfo->gr_name,(int)fileStat.st_size,mTime->tm_mon+1, mTime->tm_mday, mTime->tm_hour, mTime->tm_min, fileName[i]); 
          
    }
  if(argc < 2) free(dirPath);
    return 0;

}
