#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>

static const char *dirpath = "/home/stoe/Documents";

char base[88]="9(ku@AW1[Lmvgax6q`5Y2Ry?+sF!^HKQiBXCUSe&0M.b%rI'7d)o4~VfZ*{#:}ETt$3J-zpc]lnh8,GwP_ND|jO";
char key[88];
int shift;
void caesar()
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

static int xmp_getattr(const char *path, struct stat *stbuf)
{
  int res;
	char fpath[1000], trupath[1000]={0}, ppath[1000];
    printf("INI PPATHHHHHHHHH GANSSSSS : %s\n",path);
    strcpy(ppath, path);
    if(strcmp(path, ".")!= 0 && strcmp(path,"..")!=0)
    {
        encrypt(ppath, trupath);
        sprintf(fpath,"%s%s",dirpath,trupath);
    printf("INI trueFPATHHHHHHHHH GANSSSSS : %s\n",trupath);
    }
    else
    {
        sprintf(fpath,"%s%s",dirpath,path);
    printf("INI FPATHHHHHHHHH GANSSSSS : %s\n",fpath);
    }
    // printf("INI FPAAATHHH getattr: %s\n", fpath);
	res = lstat(fpath, stbuf);
    // printf(" INI FPATHHHHHHHHH GANSSSSS : %s\n",fpath);

	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
		       off_t offset, struct fuse_file_info *fi)
{
    char fpath[1000], ppath[1000];
    printf("INI PPATHHHHHHHHH READD : %s\n",path);
    strcpy(ppath, path);
    if(strcmp(path,"/") == 0)
	{
		path=dirpath;
		sprintf(fpath,"%s",path);
	}
	else
    {
        encrypt(ppath, ppath);
        sprintf(fpath, "%s%s",dirpath,ppath);
        printf("INI FPAAATHHH PERTAMA readdir: %s\n", fpath);
    }
    
    int res = 0;

	DIR *dp;
	struct dirent *de;

	(void) offset;
	(void) fi;

	dp = opendir(fpath);
	if (dp == NULL)
		return -errno;

	while ((de = readdir(dp)) != NULL) {
		char fname[1000]={0};
        struct stat st;
		memset(&st, 0, sizeof(st));
		st.st_ino = de->d_ino;
		st.st_mode = de->d_type << 12;
        
        if(strcmp(de->d_name, ".")!= 0 && strcmp(de->d_name,"..")!=0)
        {
            decrypt(de->d_name, fname);
            printf("FNAAMEE: %s\n", fname);
    		res = (filler(buf,fname, &st, 0));
            printf("masuk1\n");
        }
        else
        {
            printf("DNAAMEE: %s\n", de->d_name);
            res = (filler(buf,de->d_name, &st, 0));    
        }
		if(res!=0) break;
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

static int xmp_mkdir(const char *path, mode_t mode)
{
    char fpath[1000], ppath[1000];
    strcpy(ppath, path);
    encrypt(ppath, ppath);
    printf("INI PPATH-NYA MKDIR: %s\n", ppath);
    sprintf(fpath, "%s%s", dirpath, ppath);
    printf("INI FPATH-NYA MKDIR: %s\n", fpath);
    if(strstr(fpath, "@ZA>AXio"))
    {
        mode = 0750;
    }

	int res;
	res = mkdir(fpath, mode);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_create(const char* path, mode_t mode, struct fuse_file_info* fi)
{
    printf("MASUK CREATEEEEEE\n");
    (void) fi;

    char fpath[1000], ppath[1000];
    strcpy(ppath, path);
    encrypt(ppath, ppath);
    printf("INI PPATH-NYA create: %s\n", ppath);
    sprintf(fpath, "%s%s", dirpath, ppath);
    printf("INI FPATH-NYA create: %s\n", fpath);
    if(strstr(fpath, "@ZA>AXio"))
    {
        sprintf(fpath, "%s%s", fpath, "`[S%");
        mode = 0640;
    }

    int res;
    res = creat(fpath, mode);
    if(res == -1)
	return -errno;

    close(res);

    return 0;
}

static int xmp_utimens(const char *path, const struct timespec ts[2])
{
    char fpath[1000], ppath[1000];
    strcpy(ppath, path);
    encrypt(ppath, ppath);
    printf("INI PPATH-NYA utimens: %s\n", ppath);
    sprintf(fpath, "%s%s", dirpath, ppath);
    printf("INI FPATH-NYA utimens: %s\n", fpath);

	int res;
	struct timeval tv[2];

	tv[0].tv_sec = ts[0].tv_sec;
	tv[0].tv_usec = ts[0].tv_nsec / 1000;
	tv[1].tv_sec = ts[1].tv_sec;
	tv[1].tv_usec = ts[1].tv_nsec / 1000;

	res = utimes(path, tv);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_truncate(const char *path, off_t size)
{
    char fpath[1000], ppath[1000];
    strcpy(ppath, path);
    encrypt(ppath, ppath);
    printf("INI PPATH-NYA utimens: %s\n", ppath);
    sprintf(fpath, "%s%s", dirpath, ppath);
    printf("INI FPATH-NYA utimens: %s\n", fpath);

	int res;

	res = truncate(fpath, size);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_chmod(const char *path, mode_t mode)
{
	int res;
    char fpath[1000], ppath[1000];
    strcpy(ppath, path);
    encrypt(ppath, ppath);
    sprintf(fpath, "%s%s", dirpath, ppath);

    pid_t child_id;

    DIR* cekdir = opendir(fpath);

    if (strstr(ppath, "`[S%") && (cekdir == NULL)) {
        child_id = fork();
        if (child_id != 0)
        { //ini parent
            return 0;
        }
        else
        { //ini child
            char *argv[] = {"zenity", "--error", "--text=\"File ekstensi iz1 tidak boleh diubah permissionnya.", NULL};
            execv("/usr/bin/zenity", argv);
        }
    }
    closedir(cekdir);
    
	res = chmod(fpath, mode);
	if (res == -1)
		return -errno;

	return 0;
}

static struct fuse_operations xmp_oper = {
	.getattr	= xmp_getattr,
	.readdir	= xmp_readdir,
	// .read		= xmp_read,
    // .mkdir      = xmp_mkdir,
    // .create     = xmp_create,
    // .utimens    = xmp_utimens,
    // .truncate   = xmp_truncate,
    // .chmod      = xmp_chmod,
};

int main(int argc, char *argv[])
{
	printf("shift is ... : ");
  	scanf("%d", &shift);
    caesar();
	umask(0);
	return fuse_main(argc, argv, &xmp_oper, NULL);
}