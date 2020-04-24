#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>
#include <stdlib.h>

static const char *dirpath = "/home/stoe/Documents";
char pwd[500];

int isDirectory(const char *path){
  struct stat path_stat;
  stat(path, &path_stat);
  return S_ISDIR(path_stat.st_mode);
}

static int xmp_getattr(const char *path, struct stat *stbuf)
{
  int res;
	char fpath[1000];
	sprintf(fpath,"%s%s",dirpath,path);
	res = lstat(fpath, stbuf);

	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
		       off_t offset, struct fuse_file_info *fi)
{
  char fpath[1000];
  char name[1000];
	if(strcmp(path,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,path);
	int res = 0;

	DIR *dp;
	struct dirent *de;

	(void) offset;
	(void) fi;

	dp = opendir(fpath);
	if (dp == NULL)
		return -errno;

	while ((de = readdir(dp)) != NULL) {
		struct stat st;
		memset(&name, 0, sizeof(name));
		sprintf(name,"%s_stoe",de->d_name);
		sprintf(de->d_name,"%s_stoe",de->d_name);
		memset(&st, 0, sizeof(st));
		st.st_ino = de->d_ino;
		st.st_mode = de->d_type << 12;
		res = (filler(buf,de->d_name, NULL, 0));
			if(res!=0) break;
		// res = (filler(buf,"p", NULL, 0));
	// 	// 	if(res!=0) break;
	}
// 
	closedir(dp);
	return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset,
		    struct fuse_file_info *fi)
{
  char fpath[1000];
	if(strcmp(path,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,path);
	int res = 0;
  int fd = 0 ;

	(void) fi;
	fd = open(fpath, O_RDONLY);
	if (fd == -1)
		return -errno;

	res = pread(fd, buf, size, offset);
	if (res == -1)
		res = -errno;

	close(fd);
	return res;
}

void change_ext(char* path){
	DIR *d;
	struct dirent *dir;
	char files[500][500];
	char new_name[500][500];
	int file_count=0;
	// chdir(path);
	printf("%s %s %s\n",path,new_name[file_count],files[file_count]);
	d = opendir(".");
	if (d){
	printf("1.1\n");
      while (1){
	printf("1.2\n");

   memset(new_name,0,sizeof(new_name));
   memset(files,0,sizeof(files));
		dir = readdir(d);
	
		if(dir==0){
			break;
		}
	    else if(strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0 && !isDirectory(dir->d_name)){
          char tmp[100];
	printf("1.3\n");
          strcpy(tmp, dir->d_name);
          sprintf(files[file_count], "%s", dir->d_name);
		printf("1. path:%s new:%s file:%s dname:%s temp:%s\n",path,new_name[file_count],files[file_count],dir->d_name,tmp);
		  strcpy(new_name[file_count],files[file_count]);
		//   new_name[file_count]=files[file_count];
		  strcat(new_name[file_count],".bak");
        //   sprintf(new_name[file_count], "%s.bak", files[file_count]);
		  printf("2. path:%s new:%s file:%s dname:%s temp:%s\n",path,new_name[file_count],files[file_count],dir->d_name,tmp);
		//   printf("%s %s\n",files[file_count],new_name[file_count]);
		rename(files[file_count],new_name[file_count]);
		printf("3. new:%s file:%s\n",new_name[file_count],files[file_count]);
		// printf("1.4\n");
	   chmod(new_name[file_count],0444);
        file_count++;
		}
      }
      closedir(d);
    }

	// for (int i = 0; i < file_count; i++){
    //    rename(files[file_count],new_name[file_count]);
	// printf("2. new:%s file:%s\n",new_name[file_count],files[file_count]);
	// 	printf("1.4\n");
	//    chmod(new_name[file_count],0444);
    // }
}

static struct fuse_operations xmp_oper = {
	.getattr	= xmp_getattr,
	.readdir	= xmp_readdir,
	.read		= xmp_read,
};

int main(int argc, char *argv[])
{
	umask(0);
	// // getcwd(pwd, sizeof(pwd));
	// printf("2\n");
	// char fpath[1000];
    // strcpy(fpath, dirpath);
	// printf("3\n");
    // // change_ext(fpath);
	// printf("4\n");
	// // chdir(pwd);
	return fuse_main(argc, argv, &xmp_oper, NULL);
}