<?php

include 'cred.php';
define("MEDIA_LIST_TABLE", "media_list");

define("SESSION_TABLE", "session");

function get_sequence_by_ip($conn, $ip)
{
	$sql = "SELECT Sequence FROM `session` WHERE Address = " . $iplong;
	$result = mysql_query($sql) or die('MySQL query error' . mysql_error());
	$seq = -1;
	while($row = mysql_fetch_array($result))
	{
		$seq = $row['Sequence'];
		syslog(LOG_WARNING, "seq = " . $seq);
		return $seq;
	}
	return $seq;
}
?>
