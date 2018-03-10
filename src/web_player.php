<?php
    $dbhost = 'localhost';
    $dbuser = 'root';
    $dbpass = '123456';
    $dbname = 'WEB_PLAYER';
    $conn = mysql_connect($dbhost, $dbuser, $dbpass) or die('Error with MySQL connection');
    mysql_query("SET NAMES 'utf8'");
    mysql_select_db($dbname);
    $sql = "SELECT *  FROM `FILE_TABLE`";
    $result = mysql_query($sql) or die('MySQL query error');
    while($row = mysql_fetch_array($result)){
        echo $row['NAME'];
    }
?>
