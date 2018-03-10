#define _BSD_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#define DURATION_TIME 10
#define RESOLUTION " "
void update(int index)
{
	FILE* fp = fopen("test.m3u8", "w");
	if(fp == NULL)
	{
		return;
	}
	fputs("#EXTM3U\n",fp);
	fprintf(fp,"#EXT-X-MEDIA-SEQUENCE:%d\n",index);
	//fprintf(fp, "#EXT-X-ALLOW-CACHE:YES");
	fprintf(fp, "#EXT-X-TARGETDURATION:%d\n", DURATION_TIME);
	fprintf(fp, "#EXTINF:%d,\n",DURATION_TIME);
	fprintf(fp,"http://192.168.11.29/ts/192.168.11.28-%d.ts\n", 2);
	fputs("\n",fp);
	fclose(fp);
}

int main(int argc, char *argv[])
{
	int start_time=0;
	int hr, min, sec;
	char cmd[1024];
	struct timeval tv, tv2;
	unsigned long long start_utime, end_utime;
	hr=min=sec=0;

	/* delete all temp ts */
	system("rm ts/*.ts");
	/* pre-transcode first ts */
	if ( argc == 2 )
	{
	sprintf(cmd, "ffmpeg -ss 00:00:00 -t 00:00:10 -i %s -f mpegts -vcodec libx264 -acodec libmp3lame -async 1 -threads 16 -y 192.168.11.28-1.ts", argv[1]);
	}
	else if ( argc == 3 )
	{
	sprintf(cmd, "ffmpeg -ss 00:00:00 -t 00:00:10 -i %s -f mpegts -vcodec libx264 -acodec libmp3lame -async 1 -threads 16 -s %s -y 192.168.11.28-1.ts", argv[1], argv[2]);
	}
	system(cmd);
	/* Get Duration time */
	FILE *pfp=NULL;
	int p_hr, p_min, p_sec, p_ms;
	int total_time=0;
	p_hr=p_min=p_sec=p_ms=0;
	sprintf(cmd, "ffmpeg -i %s 2>&1 | grep Duration | awk '{print $2}' | tr -d ,", argv[1]);
	pfp = popen(cmd, "r");
	if (pfp == NULL)
	{
		return 0;
	}
	fscanf(pfp, "%d:%d:%d.%d", &p_hr, &p_min, &p_sec, &p_ms);
	pclose(pfp);
	total_time = p_hr*3600+p_min*60+p_sec;
	printf("total time = %d\n", total_time);

	for(int index=1;index<total_time/10;index++)
	{
		start_time=index*DURATION_TIME;
		hr=start_time/3600;
		min=start_time%3600/60;
		sec=start_time%3600%60;
		update(index);
		if (argc == 2)
		{
		sprintf(cmd, "ffmpeg  -ss %02d:%02d:%02d -t 00:00:%02d -i %s -f mpegts -vcodec libx264 -acodec libmp3lame -async 1 -threads 16 -y ts/192.168.11.28-%d.ts &",
				hr, min, sec, DURATION_TIME, argv[1], 2);
		}
		else if (argc == 3)
		{
		sprintf(cmd, "ffmpeg  -ss %02d:%02d:%02d -t 00:00:%02d -i %s -f mpegts -vcodec libx264 -acodec libmp3lame -async 1 -threads 16 -s %s -y ts/192.168.11.28-%d.ts &",
				hr, min, sec, DURATION_TIME, argv[1], argv[2], 2);
		}
		printf("cmd = %s\n", cmd);
		system(cmd);
		sleep(10);
	}
	return 0;
}
