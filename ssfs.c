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
	FILE * LOG = fopen(log_path, "a");
	time_t now;
	time ( &now );
	struct tm * timeinfo = localtime (&now);
	fprintf(LOG, "INFO::%d%d%d-%d:%d:%d::%s\n", timeinfo->tm_year+1900, timeinfo->tm_mon, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, command);
	fclose(LOG);
    return;
}

void warn_command(char *command){
	FILE * LOG = fopen(log_path, "a");
	time_t now;
	time ( &now );
	struct tm * timeinfo = localtime (&now);
	fprintf(LOG, "WARNING::%d%d%d-%d:%d:%d::%s\n", timeinfo->tm_year+1900, timeinfo->tm_mon, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, command);
	fclose(LOG);
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
    // sprintf(fpath, "%s",from);
    // sprintf(name, "%s", to);
    char str[100];
    sprintf(str, "RENAME::%s::%s", from, to);
	  info_command(str);
    // if(stringcmp(to,"/encv1_") == 0){
        sprintf(fpath, "%s%s",dirpath, from);
        // enk(name);
        sprintf(name, "%s%s", dirpath, to);
    // }
	// decrypt(fpath,fpath);
  // decrypt(name,name);
  printf("%s--%s\n",fpath,name);
	res = rename(fpath, name);
	if (res == -1)
		return -errno;
 
	return 0;
}

static int xmp_access(const char *path, int mask)
{
	int res;

    char fpath[1000];
	if(strcmp(path,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,path);
    
    char str[100];
	sprintf(str, "ACCESS::%s", path);
	info_command(str);
    
	res = access(fpath, mask);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_readlink(const char *path, char *buf, size_t size)
{
	int res;

    char fpath[1000];
	if(strcmp(path,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,path);

    char str[100];
	sprintf(str, "READLINK::%s", path);
	info_command(str);

	res = readlink(dirpath, buf, size - 1);
	if (res == -1)
		return -errno;

	buf[res] = '\0';
	return 0;
}

static int xmp_mknod(const char *path, mode_t mode, dev_t rdev)
{
	int res;
    char fpath[1000];
    if(strcmp(path,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,path);

    char str[100];
	sprintf(str, "MKNOD::%s", path);
	info_command(str);

	/* On Linux this could just be 'mknod(path, mode, rdev)' but this
	   is more portable */
	if (S_ISREG(mode)) {
		res = open(fpath, O_CREAT | O_EXCL | O_WRONLY, mode);
		if (res >= 0)
			res = close(res);
	} else if (S_ISFIFO(mode))
		res = mkfifo(fpath, mode);
	else
		res = mknod(fpath, mode, rdev);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_unlink(const char *path)
{
	int res;
     char fpath[1000];
    if(strcmp(path,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,path);

    char str[100];
	sprintf(str, "UNLINK::%s", path);
	warn_command(str);

	res = unlink(fpath);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_rmdir(const char *path)
{
	int res;
    char fpath[1000];
    if(strcmp(path,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,path);

    char str[100];
	sprintf(str, "RMDIR::%s", path);
	warn_command(str);

	res = rmdir(fpath);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_symlink(const char *from, const char *to)
{
	int res;
     char fpath[1000];
    char name[1000];
    // sprintf(fpath, "%s",from);
    // sprintf(name, "%s", to);
    char str[100];
    sprintf(str, "SYMLINK::%s::%s", from, to);
	  info_command(str);
    // if(stringcmp(to,"/encv1_") == 0){
        sprintf(fpath, "%s%s",dirpath, from);
        // enk(name);
        sprintf(name, "%s%s", dirpath, to);
    
	res = symlink(fpath, name);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_link(const char *from, const char *to)
{
	int res;
    char fpath[1000];
    char name[1000];
    // sprintf(fpath, "%s",from);
    // sprintf(name, "%s", to);
    char str[100];
    sprintf(str, "LINK::%s::%s", from, to);
	  info_command(str);
    // if(stringcmp(to,"/encv1_") == 0){
        sprintf(fpath, "%s%s",dirpath, from);
        // enk(name);
        sprintf(name, "%s%s", dirpath, to);

	res = link(fpath, name);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_chmod(const char *path, mode_t mode)
{
	int res;
    char fpath[1000];
    if(strcmp(path,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,path);

    char str[100];
	sprintf(str, "CHMOD::%s", path);
	info_command(str);

	res = chmod(fpath, mode);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_chown(const char *path, uid_t uid, gid_t gid)
{
	int res;
    char fpath[1000];
    if(strcmp(path,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,path);

    char str[100];
	sprintf(str, "CHOWN::%s", path);
	info_command(str);

	res = lchown(fpath, uid, gid);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_truncate(const char *path, off_t size)
{
	int res;
    char fpath[1000];
    if(strcmp(path,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,path);

    char str[100];
	sprintf(str, "TRUNCATE::%s", path);
	info_command(str);

	res = truncate(fpath, size);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_utimens(const char *path, const struct timespec ts[2])
{
	int res;
	struct timeval tv[2];
    char fpath[1000];
    if(strcmp(path,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,path);

    char str[100];
	sprintf(str, "UTIMENS::%s", path);
	info_command(str);

	tv[0].tv_sec = ts[0].tv_sec;
	tv[0].tv_usec = ts[0].tv_nsec / 1000;
	tv[1].tv_sec = ts[1].tv_sec;
	tv[1].tv_usec = ts[1].tv_nsec / 1000;
    
	res = utimes(fpath, tv);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_open(const char *path, struct fuse_file_info *fi)
{
	int res;
    char fpath[1000];
    if(strcmp(path,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,path);

    char str[100];
	sprintf(str, "OPEN::%s", path);
	info_command(str);

	res = open(fpath, fi->flags);
	if (res == -1)
		return -errno;

	close(res);
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

    char str[100];
	sprintf(str, "READ::%s", path);
	info_command(str);

	int fd;
	int res;

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

static int xmp_write(const char *path, const char *buf, size_t size,
		     off_t offset, struct fuse_file_info *fi)
{   
    char fpath[1000];
    if(strcmp(path,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,path);

    char str[100];
	sprintf(str, "WRITE::%s", path);
	info_command(str);

	int fd;
	int res;

	(void) fi;
	fd = open(fpath, O_WRONLY);
	if (fd == -1)
		return -errno;

	res = pwrite(fd, buf, size, offset);
	if (res == -1)
		res = -errno;

	close(fd);
	return res;
}

static int xmp_statfs(const char *path, struct statvfs *stbuf)
{
	int res;
    char fpath[1000];
    if(strcmp(path,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,path);

    char str[100];
	sprintf(str, "STATFS::%s", path);
	info_command(str);

	res = statvfs(fpath, stbuf);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_create(const char* path, mode_t mode, struct fuse_file_info* fi) {

    (void) fi;
    char fpath[1000];
    if(strcmp(path,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,path);

    char str[100];
	sprintf(str, "CREATE::%s", path);
	info_command(str);

    int res;
    res = creat(fpath, mode);
    if(res == -1)
	return -errno;

    close(res);

    return 0;
}

#ifdef HAVE_SETXATTR
static int xmp_setxattr(const char *path, const char *name, const char *value,
			size_t size, int flags)
{
    char fpath[1000];
    if(strcmp(path,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,path);

    char str[100];
	sprintf(str, "SETXATTR::%s", path);
	info_command(str);

	int res = lsetxattr(fpath, name, value, size, flags);
	if (res == -1)
		return -errno;
	return 0;
}

static int xmp_getxattr(const char *path, const char *name, char *value,
			size_t size)
{
    char fpath[1000];
    if(strcmp(path,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,path);

    char str[100];
	sprintf(str, "GETXATTR::%s", path);
	info_command(str);
    
	int res = lgetxattr(fpath, name, value, size);
	if (res == -1)
		return -errno;
	return res;
}

static int xmp_listxattr(const char *path, char *list, size_t size)
{
    char fpath[1000];
    if(strcmp(path,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,path);

    char str[100];
	sprintf(str, "LISTXATTR::%s", path);
	info_command(str);

	int res = llistxattr(fpath, list, size);
	if (res == -1)
		return -errno;
	return res;
}

static int xmp_removexattr(const char *path, const char *name)
{
    char fpath[1000];
    if(strcmp(path,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else sprintf(fpath, "%s%s",dirpath,path);

    char str[100];
	sprintf(str, "REMOVEXATTR::%s", path);
	info_command(str);

	int res = lremovexattr(fpath, name);
	if (res == -1)
		return -errno;
	return 0;
}
#endif /* HAVE_SETXATTR */

static struct fuse_operations xmp_oper = {
	.getattr	= xmp_getattr,
	.access		= xmp_access,
	.readlink	= xmp_readlink,
	.readdir	= xmp_readdir,
	.mknod		= xmp_mknod,
	.mkdir		= xmp_mkdir,
	.symlink	= xmp_symlink,
	.unlink		= xmp_unlink,
	.rmdir		= xmp_rmdir,
	.rename		= xmp_rename,
	.link		= xmp_link,
	.chmod		= xmp_chmod,
	.chown		= xmp_chown,
	.truncate	= xmp_truncate,
	.utimens	= xmp_utimens,
	.open		= xmp_open,
	.read		= xmp_read,
	.write		= xmp_write,
	.statfs		= xmp_statfs,
	.create         = xmp_create,
#ifdef HAVE_SETXATTR
	.setxattr	= xmp_setxattr,
	.getxattr	= xmp_getxattr,
	.listxattr	= xmp_listxattr,
	.removexattr	= xmp_removexattr,
#endif
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
