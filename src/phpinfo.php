<?php
/*
   $dbhost = 'localhost';
   $dbuser = 'root';
   $dbpass = '123456';
   $dbname = 'WEB_PLAYER';
   $conn = mysql_connect($dbhost, $dbuser, $dbpass) or die('Error with MySQL connection');
   mysql_query("SET NAMES 'utf8'");
   mysql_select_db($dbname);
   $sql = "SELECT * FROM `BW_TABLE`";
   $result = mysql_query($sql) or die('MySQL query error');
   while($row = mysql_fetch_array($result)){
   echo $row['NAME'];
   }
 */
$ip = $_SERVER['REMOTE_ADDR'];
$file = 'ts/192.168.11.28-1.ts';
openlog("bwdetect", LOG_PID | LOG_PERROR, LOG_LOCAL0);

if (file_exists($file)) {
	syslog(LOG_WARNING, "exists");
	header('Content-Description: File Transfer');
	header('Content-Type: application/octet-stream');
	header('Content-Disposition: attachment; filename='.basename($file));
	header('Content-Transfer-Encoding: binary');
	header('Expires: 0');
	header('Cache-Control: must-revalidate');
	header('Pragma: public');
	header('Content-Length: ' . filesize($file));
	ob_clean();
	flush();
	
	syslog(LOG_WARNING, $ip . " requests " . $file . " " .  gettimeofday(true));
	syslog(LOG_WARNING, "before download: " . $file . " " .  gettimeofday(true));
	readfile($file);
	syslog(LOG_WARNING, "after download: " . $file . " " .  gettimeofday(true));
	
	/* update play list */

	/* ffmpeg */
}
?>
