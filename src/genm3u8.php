<?php
include 'cred.php';
openlog(__FILE__, LOG_PID | LOG_PERROR, LOG_LOCAL0);
$ip = $_SERVER['REMOTE_ADDR'];
syslog(LOG_WARNING, "ip: " . $ip);
$iplong = ip2long($ip);
syslog(LOG_WARNING, "iplong: " . $iplong);
$conn = mysql_connect($dbhost, $dbuser, $dbpass) or die('Error with MySQL connection' . mysql_error());
mysql_query("SET NAMES 'utf8'");
mysql_select_db($dbname);
$sql = "SELECT Sequence FROM `session` WHERE Address = " . $iplong;
$result = mysql_query($sql) or die('MySQL query error' . mysql_error());
$seq = 0;
while($row = mysql_fetch_array($result))
{
	$seq = $row['Sequence'];
	syslog(LOG_WARNING, "seq = " . $seq);
}
syslog(LOG_WARNING, "seq = " . $seq);

$sql = "SELECT * FROM `media_list`";
$result = mysql_query($sql) or die('MySQL query error' . mysql_error());

while($row = mysql_fetch_array($result))
{
        $streamfilename = $row['Name'];
        syslog(LOG_WARNING, "streamfilename = " . $streamfilename);
} 

if(strlen($streamfilename) == 0)
{
	die("unable to list files\n");
}
$bandwidth = 0;
if($seq == 0)
{
	$sql = "INSERT INTO `session` (Address,Bandwidth,Sequence,File) VALUES (" . $iplong . "," . $bandwidth . "," . $seq . ",\"" . $streamfilename . "\")";
        syslog(LOG_WARNING, "sql = " . $sql);
	$result = mysql_query($sql);// or die('MySQL query error' . mysql_errno());
	$errcode = mysql_errno();
	if($errcode != 0 && $errcode != 1062)
	{
		die('MySQL query error' . mysql_error());
	}
}


/*
#EXTM3U
#EXT-X-MEDIA-SEQUENCE:1
#EXT-X-TARGETDURATION:10
#EXTINF:10,
http://192.168.11.29/phpinfo.php
*/
$m3u8filename = $ip . ".m3u8"; 
$streamfilename = "media/" . $streamfilename;
if (file_exists($streamfilename)) {

	header('Content-Description: File Transfer');
	header('Content-Type: application/octet-stream');
	header('Content-Disposition: attachment; filename='.basename($m3u8filename));
	header('Content-Transfer-Encoding: binary');
	header('Expires: 0');
	header('Cache-Control: must-revalidate');
	header('Pragma: public');
	header('Content-Length: ' . filesize($m3u8filename));
	ob_clean();
	//flush();

	syslog(LOG_WARNING, "before download: " . $m3u8filename . " " . gettimeofday(true));
	printf("#EXTM3U\n");
	printf("#EXT-X-MEDIA-SEQUENCE:%d\n",$seq);
	printf("#EXT-X-TARGETDURATION:10\n");
	printf("#EXTINF:10,\n");
	printf("getts.php\n");
	syslog(LOG_WARNING, "after download: " . $m3u8filename . " " .  gettimeofday(true));
	exit;
}
?>
