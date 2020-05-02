# SoalShiftSISOP20_modul4_A07
**Soal**
Di suatu perusahaan, terdapat pekerja baru yang super jenius, ia bernama jasir. Jasir baru bekerja selama seminggu di perusahan itu, dalam waktu seminggu tersebut ia selalu terhantui oleh ketidak amanan dan ketidak efisienan file system yang digunakan perusahaan tersebut. Sehingga ia merancang sebuah file system yang sangat aman dan efisien. Pada segi keamanan, Jasir telah menemukan 2 buah metode enkripsi file. Pada mode enkripsi pertama, nama file-file pada direktori terenkripsi akan dienkripsi menggunakan metode substitusi. Sedangkan pada metode enkripsi yang kedua, file-file pada direktori terenkripsi akan di-split menjadi file-file kecil. Sehingga orang-orang yang tidak menggunakan filesystem rancangannya akan kebingungan saat mengakses direktori terenkripsi tersebut. Untuk segi efisiensi, dikarenakan pada perusahaan tersebut sering dilaksanakan sinkronisasi antara 2 direktori, maka jasir telah merumuskan sebuah metode agar filesystem-nya mampu mengsingkronkan kedua direktori tersebut secara otomatis. Agar integritas filesystem tersebut lebih terjamin, maka setiap command yang dilakukan akan dicatat kedalam sebuah file log.
(catatan: filesystem berfungsi normal layaknya linux pada umumnya)
(catatan: mount source (root) filesystem adalah direktori /home/[user]/Documents)
Berikut adalah detail filesystem rancangan jasir:

 **1.  Enkripsi versi 1:**
1.  Jika sebuah direktori dibuat dengan awalan “encv1_”, maka direktori tersebut akan menjadi direktori terenkripsi menggunakan metode enkripsi v1.
2.  Jika sebuah direktori di-rename dengan awalan “encv1_”, maka direktori tersebut akan menjadi direktori terenkripsi menggunakan metode enkripsi v1.
3.  Apabila sebuah direktori terenkripsi di-rename menjadi tidak terenkripsi, maka isi adirektori tersebut akan terdekrip.
4.  Setiap pembuatan direktori terenkripsi baru (mkdir ataupun rename) akan tercatat ke sebuah database/log berupa file.
5.  Semua file yang berada dalam direktori ter enkripsi menggunakan caesar cipher dengan key.  

```9(ku@AW1[Lmvgax6q`5Y2Ry?+sF!^HKQiBXCUSe&0M.b%rI'7d)o4~VfZ*{#:}ETt$3J-zpc]lnh8,GwP_ND|jO```

Misal kan ada file bernama “kelincilucu.jpg” dalam directory FOTO_PENTING, dan key yang dipakai adalah 10  
“encv1_rahasia/FOTO_PENTING/kelincilucu.jpg” => “encv1_rahasia/ULlL@u]AlZA(/g7D.|_.Da_a.jpg

Note  : Dalam penamaan file ‘/’ diabaikan, dan ekstensi tidak perlu di encrypt.

6.  Metode enkripsi pada suatu direktori juga berlaku kedalam direktori lainnya yang ada didalamnya.

Untuk sub-soal pertama, kita diminta membuat sebuah metode enkripsi yang dapat melakukan *caesar cipher* pada direktori yang ditentukan.

berikut fungsi-fungsi yang digunakan:

```c
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
```

Agar file dapat ditampilkan di dalam filesystem, pada fungsi implementasi getattr dan readdir,memanggil fungsi enkripsi apabila ditemukab folder dengan nama `encv1_` diawalnya, agar file yang didalamnya terenkripsi

```c
static int xmp_getattr(const char *path, struct stat *stbuf)
{
  int res;
  printf("masuk GETATTRIBUT %s/%s\n",dirpath, path);
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

```
**4. Log system:**
    
1.  Sebuah berkas nantinya akan terbentuk bernama "fs.log" di direktori *home* pengguna (/home/[user]/fs.log) yang berguna menyimpan daftar perintah system call yang telah dijalankan.
2.  Agar nantinya pencatatan lebih rapi dan terstruktur, log akan dibagi menjadi beberapa level yaitu INFO dan WARNING.
3.  Untuk log level WARNING, merupakan pencatatan log untuk syscall rmdir dan unlink.
4.  Sisanya, akan dicatat dengan level INFO.
5.  Format untuk logging yaitu:

<kbd>    
[LEVEL]::[yy][mm][dd]-[HH]:[MM]:[SS]::[CMD]::[DESC ...]<br>
LEVEL : Level logging<br>
yy : Tahun dua digit<br>
mm : Bulan dua digit<br>
dd : Hari dua digit<br>
HH : Jam dua digit<br>
MM : Menit dua digit<br>
SS : Detik dua digit<br>
CMD : System call yang terpanggil<br>
DESC : Deskripsi tambahan (bisa lebih dari satu, dipisahkan dengan ::)
</kbd>

Contoh format logging nantinya seperti:
<kbd>
INFO::200419-18:29:28::MKDIR::/iz1
INFO::200419-18:29:33::CREAT::/iz1/yena.jpg
INFO::200419-18:29:33::RENAME::/iz1/yena.jpg::/iz1/yena.jpeg
</kbd>


Untuk sub-soal keempat, kita diminta membuat *logfile* di semua operasi. Maka, dibuat sebuah fungsi `warn` dan `info` untuk fungsi *logging*:

```c
void info_command(char *command){
	FILE * LOG = fopen(log_path, "a");
	time_t now;
	time ( &now );
	struct tm * timeinfo = localtime (&now);
	fprintf(LOG, "INFO::%02d%02d%02d-%02d:%02d:%02d::%s\n", timeinfo->tm_year+1900-2000, timeinfo->tm_mon+1, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, command);
	fclose(LOG);
    return;
}

void warn_command(char *command){
	FILE * LOG = fopen(log_path, "a");
	time_t now;
	time ( &now );
	struct tm * timeinfo = localtime (&now);
	fprintf(LOG, "WARNING::%02d%02d%02d-%02d:%02d:%02d::%s\n", timeinfo->tm_year+1900-2000, timeinfo->tm_mon+1, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, command);
	fclose(LOG);
    return;
}
```

Fungsi itu dapat dipanggil pada fungsi-fungsi implementasi FUSE untuk mencatat command yang digunakan kedalam file log.
