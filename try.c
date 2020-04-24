#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>
#include <ctype.h>

static const char *dirpath = "/home/stoe/Documents";
static const char *log_path = "/home/stoe/fs.log";

char base[88]="9(ku@AW1[Lmvgax6q`5Y2Ry?+sF!^HKQiBXCUSe&0M.b%rI'7d)o4~VfZ*{#:}ETt$3J-zpc]lnh8,GwP_ND|jO";
char key[88];
int shift;

int stringcmp (const char *s1,char *s2)
{
    int i=0,diff=0;
    for(i=0; s2[i]!='\0'; i++)
    {
        if( s1[i]!=s2[i])
            return 1;           
    }
    return 0;
}

void change()
{
    int baselen = strlen(base), i, j;
    
    for(i=0, j=shift; j<baselen; j++, i++)
    {
        key[i] = base[j];
    }
    
    for(j=0; i<baselen; i++, j++)
    {
        key[i] = base[j];
    }
	printf("charlist : %s\n", base);
  	printf("list : %s\n", key);
}

void decrypt(char kal[1000], char dec[1000])
{
    int i, j;
    for(i=0; i<strlen(kal); i++)
    {
        for(j=0; j<strlen(base); j++)
        {
            if(kal[i]==base[j])
            {
                dec[i]=key[j];
                break;
            }
        }
        if(j==strlen(key))
        {
            dec[i]=kal[i];
        }
    }
}

void encrypt(char kal[1000], char enc[1000])
{
    //Encrypt
    int i, j;
    for(i=0; i<strlen(kal); i++)
    {
        for(j=0; j<strlen(key); j++)
        {
            if(kal[i] == key[j])
            {
                enc[i]=base[j];
                break;
            }
        }
        if(j==strlen(key))
        {
            enc[i]=kal[i];
        }
    }
}

void info_command(char *command){
	FILE * logFile = fopen(log_path, "a");
	time_t now;
	time ( &now );
	struct tm * timeinfo = localtime (&now);
	fprintf(logFile, "INFO::%d%d%d-%d:%d:%d::%s\n", timeinfo->tm_year+1900, timeinfo->tm_mon, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, command);
	fclose(logFile);
    return;
}

void warn_command(char *command){
	FILE * logFile = fopen(log_path, "a");
	time_t now;
	time ( &now );
	struct tm * timeinfo = localtime (&now);
	fprintf(logFile, "WARNING::%d%d%d-%d:%d:%d::%s\n", timeinfo->tm_year+1900, timeinfo->tm_mon, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, command);
	fclose(logFile);
    return;
}

static int xmp_getattr(const char *path, struct stat *stbuf)
{
  int res;
	char fpath[1000];
	char fpath2[1000];
  char path2[1000];
  bzero(path2,sizeof(path2));
  sprintf(path2,"%s",path);
  int c=0;
	for(int i=1; path[i]!='/' && path[i]!='\0' ; i++)
    {
        c++;        
    }
    printf("%d\n",c);
	char jwb[c+1];
  char jwb2[sizeof(jwb)];
  bzero(jwb,sizeof(jwb));
		for(int i=1; i<=c ; i++)
    {
      printf("%d ",i);
        jwb[i-1]=path[i];           
    }
    printf("ini nama dir awal : %s\n",jwb);

  if(stringcmp(jwb,"encv1_") == 0){
    printf("berhasil : %s\n",jwb);
    encrypt(path2,path2);
    // decrypt(jwb2,jwb);
    for(int i=1; i<=c ; i++)
    {
      printf("%d ",i);
        jwb2[i-1]=path2[i];           
    }
    // char* p = strstr(path2, jwb2);
    int i=0,j=0;
    while(path2[i]!='\0'){
      if(path2[i]!=jwb2[j]){
        i++;
        j=0;
      }
      else if(path2[i+1]!=jwb2[j+1] && jwb2[j+1]!='\0'){
        i++;
        j=0;
      }
      else{
        path2[i]=jwb[j];
        i++;
        j++;
      }

    }
    // if(p)printf("ADA GANS : %s\n",p);
    printf("path: %s--%s--%s\n",jwb2,path,path2);
  	sprintf(fpath,"%s%s",dirpath,path2);

        // sprintf(name, "%s", to);
        // enk(name);
    }
  else{
	sprintf(fpath,"%s%s",dirpath,path);
  printf("path: %s--%s--%s\n",jwb2,path,path2);
  }
  // sprintf(fpath2,"%s/%s ",dirpath,jwb);
  // printf("fpath2: %s\n",fpath2);
  bzero(jwb,sizeof(jwb));
  bzero(jwb2,sizeof(jwb));
  // bzero(path,sizeof(path));
  bzero(path2,sizeof(path2));
	res = lstat(fpath, stbuf);

	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
		       off_t offset, struct fuse_file_info *fi)
{
  char fpath[1000];
	if(strcmp(path,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,path);
	int res = 0;

  printf("fpath readir: %s MULAI\n",path);
	DIR *dp;
	struct dirent *de;

	(void) offset;
	(void) fi;

	dp = opendir(fpath);
	if (dp == NULL)
		return -errno;
 if(!stringcmp(path,"/encv1_")){
	while ((de = readdir(dp)) != NULL) {
		struct stat st;
		memset(&st, 0, sizeof(st));
		st.st_ino = de->d_ino;
		st.st_mode = de->d_type << 12;
   
      decrypt(de->d_name,de->d_name);
    printf("name: %s \n",de->d_name);
		res = (filler(buf, de->d_name, &st, 0));
			if(res!=0) break;
	}
 }
  else{
  	while ((de = readdir(dp)) != NULL) {
		struct stat st;
		memset(&st, 0, sizeof(st));
		st.st_ino = de->d_ino;
		st.st_mode = de->d_type << 12;
		res = (filler(buf, de->d_name, &st, 0));
			if(res!=0) break;
	  }
  }

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

static int xmp_mkdir(const char *path, mode_t mode){
	int res;

	char fpath[1000];
  char name[1000];

  if(stringcmp(path,"/encv1_") == 0){
    printf("berhasil : \n");
        // sprintf(name, "%s", to);
        // enk(name);
    }

  sprintf(fpath,"%s%s", dirpath, path);
  printf("path : %s\n",path);

  char str[100];
	sprintf(str, "MKDIR::%s", path);
	info_command(str);

	res = mkdir(fpath, mode);
	if (res == -1)
		return -errno;

	return 0;
}
static int xmp_rename(const char *from, const char *to){
	int res;
    char fpath[1000];
    char name[1000];
    sprintf(fpath, "%s",from);
    sprintf(name, "%s", to);

    // if(stringcmp(to,"/encv1_") == 0){
        sprintf(fpath, "%s%s",dirpath, from);
        // enk(name);
        sprintf(name, "%s%s", dirpath, to);
    // }
  char str[100];
	// decrypt(fpath,fpath);
  // decrypt(name,name);
  printf("%s--%s\n",fpath,name);
	res = rename(fpath, name);
	if (res == -1)
		return -errno;
  sprintf(str, "RENAME::%s::%s", from, to);
	info_command(str);
	return 0;
}

static struct fuse_operations xmp_oper = {
	.getattr	= xmp_getattr,
	.readdir	= xmp_readdir,
	.read		= xmp_read,
  .mkdir		= xmp_mkdir,
	.rename		= xmp_rename,
};

int main(int argc, char *argv[])
{
  	printf("shift is ... : ");
  	scanf("%d", &shift);
    FILE *fpointer = fopen(log_path, "rb+");
    if(fpointer == NULL){
      fpointer = fopen(log_path, "wb");
      fclose(fpointer);
    }else{
      fclose(fpointer);
    }
    change();
	umask(0);
	return fuse_main(argc, argv, &xmp_oper, NULL);
}