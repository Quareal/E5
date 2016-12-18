<?php

/* замеряет кол-во потребляемой оперативной памяти и времени */

global $start_time;
$mtime = microtime();$mtime = explode(" ",$mtime);$mtime = $mtime[1] + $mtime[0];$end_time = $mtime;
$total_time = ($end_time - $start_time);
$size=memory_get_usage();
if(function_exists('memory_get_peak_usage')) $msize=memory_get_peak_usage();
$msize=round($msize/1024/1024);	
get_loadavg();
$gfm=get_free_mem();
if(!empty($gfm)){
	$gfm=smart_size($gfm,1,1);
}
$GLOBALS['msize']=$msize;
$GLOBALS['ttime']=substr($total_time,0,5);
$GLOBALS['tmsize']=$gfm;

?>