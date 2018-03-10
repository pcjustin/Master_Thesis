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

$opt = (integer)$_REQUEST['opt'];

switch($opt){
	case 0:
		phpinfo();
		break;
	case 1: //get default list or search list
		
		$sql = "SELECT * FROM `media_list` WHERE 1=1";
		if($_REQUEST['search'] != "") $sql.=" AND name like '%" . $_REQUEST['search']."%'";
		
		$result = mysql_query($sql) or die('MySQL query error');
		$datas = array();
		while($row = mysql_fetch_array($result)){
			array_push($datas, array(
				"fname"=>$row["Name"],
				"time"=> $row["Duration"]
				));
			
			}
		
		echo json_encode( $datas ); 
		break;
	case 2:// set play file
		$sql = "DELETE FROM `session` WHERE Address = $iplong";
		@$result = mysql_query($sql);
		
		
		$sql = "INSERT `session`(Address,Sequence,File) VALUES($iplong,0,'".$_REQUEST['fname']."')";
		$result = mysql_query($sql) or die('MySQL query error');
		
		echo json_encode( array('result'=>'ok') ); 
		break;
	}

?>
