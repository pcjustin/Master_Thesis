<?php
include 'define.php';
include 'cred.php';
openlog(__FILE__, LOG_PID | LOG_PERROR, LOG_LOCAL0);
$ip = $_SERVER['REMOTE_ADDR'];
syslog(LOG_WARNING, "ip = " . $ip);
$iplong = ip2long($ip);
syslog(LOG_WARNING, "iplong = " . $iplong);
$conn = mysql_connect($dbhost, $dbuser, $dbpass) or die('Error with MySQL connection');
mysql_query("SET NAMES 'utf8'");
mysql_select_db($dbname);

$sql = "SELECT * FROM `session` WHERE Address = " . $iplong;
$result = mysql_query($sql) or die('MySQL query error');
$seq = 0;
while($row = mysql_fetch_array($result))
{
	$mediafilename = $row['File'];
	if(strlen($mediafilename) == 0)
	{
		die("unable to list files\n");
	}
	syslog(LOG_WARNING, "mediafilename = " . $mediafilename);
	$seq = $row['Sequence'];
	syslog(LOG_WARNING, "seq = " . $seq);
}
syslog(LOG_WARNING, "seq = " . $seq);
$sql = "SELECT Duration FROM `media_list` WHERE Name = \"" . $mediafilename . "\"";
$result = mysql_query($sql) or die('MySQL query error');

while($row = mysql_fetch_array($result))
{
	$mediaduration = $row['Duration'];
	if(strlen($mediaduration) == 0)
	{
		die("unable to get duration of  files\n");
	}
	syslog(LOG_WARNING, "mediaduration = " . $mediaduration);
}

if($mediaduration - $seq * DURATION_PER_SLICE < 0)
{
	$sql = "UPDATE `session` SET Sequence=0 WHERE Address=" . $iplong;
	syslog(LOG_WARNING, "sql = " . $sql);
	$result = mysql_query($sql) or die('MySQL query error' . mysql_error());
	syslog(LOG_WARNING, "media ended");
	die("media ended");	
}
$bandwidth = 0;

$streamfilename = "ts/" . $ip . "-" . $seq . ".ts";
if($seq == 0)
{
	$streamfilename = "ts/default.ts";
}
syslog(LOG_WARNING, "streamfilename = " . $streamfilename);
$mediafilename = "media/" . $mediafilename;
syslog(LOG_WARNING, "mediafilename = " . $mediafilename);
if (file_exists($streamfilename)) {

	$streamfilesize = filesize($streamfilename);
	header('Content-Description: File Transfer');
	header('Content-Type: application/octet-stream');
	header('Content-Disposition: attachment; filename='.basename($streamfilename));
	header('Content-Transfer-Encoding: binary');
	header('Expires: 0');
	header('Cache-Control: must-revalidate');
	header('Pragma: public');
	header('Content-Length: ' . $streamfilesize);
	ob_clean();
	$download_start_time = gettimeofday(true);
	syslog(LOG_WARNING, "before download: " . $streamfilename . " " . $download_start_time);
	readfile($streamfilename);
	$download_end_time = gettimeofday(true);
	syslog(LOG_WARNING, "after download: " . $streamfilename . " " . $download_end_time);
	$download_diff_time = $download_end_time - $download_start_time;
	syslog(LOG_WARNING, "download duration: " . $streamfilename . " " . $download_diff_time);
	$bandwidth = $streamfilesize/$download_diff_time;
	syslog(LOG_WARNING, "bandwidth: " . $bandwidth);
	$sql = "UPDATE `session` SET Sequence=" . ($seq+1)  . ", Bandwidth=" . $bandwidth . " WHERE Address=" . $iplong;
	syslog(LOG_WARNING, "sql = " . $sql);
	$result = mysql_query($sql) or die('MySQL query error' . mysql_error());
	$movie_start_time = $seq * DURATION_PER_SLICE;
	$outputstreamfilename = "ts/" . $ip . "-" . ($seq+1) . ".ts";
	$cmd = "ffmpeg -ss " . $movie_start_time . " -t 00:00:10 -i " . $mediafilename . " -f mpegts -vcodec libx264 -acodec libmp3lame -async 1 -threads 16 -b:v " . ((int)($bandwidth) * 4) . " -ab 64k " . " -y " . $outputstreamfilename . " &"; 
	syslog(LOG_WARNING, "cmd = " . $cmd);
	$shell_output = system($cmd, $ret_val);
	syslog(LOG_WARNING, "shell_output = " . $shell_output);
	syslog(LOG_WARNING, "ret_val = " . $ret_val);
	exit;
}
?>
