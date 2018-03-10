#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <mysql/mysql.h>
#define SELECT_QUERY "select Name from `media_list` where Name = %s"
static int file_filter(const struct dirent *entry)
{
	if ((strcmp(entry->d_name, ".") == 0) || (strcmp(entry->d_name, "..") == 0))
		return 0;
	else
		return 1;
}
int write2DB(char *filename, int duration)
{
	MYSQL mysql,*sock;    
	MYSQL_RES *res;       
	MYSQL_FIELD *fd ;     
	MYSQL_ROW row ;       
	char  qbuf[160];

	mysql_init(&mysql);
	if (!(sock = mysql_real_connect(&mysql,"localhost","root","123456","WEB_PLAYER",0,NULL,0))) {
		fprintf(stderr,"Couldn't connect to engine!\n%s\n\n",mysql_error(&mysql));
		perror("");
		exit(1);
	}

	sprintf(qbuf,SELECT_QUERY,"");
	if(mysql_query(sock,qbuf)) {
		fprintf(stderr,"Query failed (%s)\n",mysql_error(sock));
		exit(1);
	}
	
	
}
int main(int argc, char *argv[])
{
	struct dirent **namelist;
	char tsdir[256];
	FILE *pfp=NULL;

	if ( argc != 2 )
	{
		printf("not input ts dir\n");
		return 1;
	}
	strcpy(tsdir, argv[1]);
	int n=0;
	n = scandir(tsdir, &namelist, file_filter, alphasort);
	if (n < 0)
	{
		printf("scan dirent error!\n");
		return 0;
	}
	else
	{
		while (n--)
		{
			printf("%s\n", namelist[n]->d_name);
			/* get duration time */
			char cmd[1024];
			unsigned int p_hr, p_min, p_sec, p_ms;
			int total_time=0;
			memset(cmd, 0, sizeof(char));
			p_hr=p_min=p_sec=p_ms=0;
			sprintf(cmd, "ffmpeg -i %s/%s 2>&1 | grep Duration | awk '{print $2}' | tr -d ,", tsdir, namelist[n]->d_name);
			pfp = popen(cmd, "r");
			if (pfp == NULL)
			{
				return 0;
			}
			fscanf(pfp, "%d:%d:%d.%d", &p_hr, &p_min, &p_sec, &p_ms);
			pclose(pfp);
			total_time = p_hr*3600+p_min*60+p_sec;
			printf("total time = %d\n", total_time);
			/* write to db */
			write2DB(namelist[n]->d_name, total_time);
			free(namelist[n]);
		}
		free(namelist);
	}
	return 0;
}

