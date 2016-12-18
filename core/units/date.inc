<?php

if(!empty($local_region) && file_exists(DOCUMENT_ROOT.'/core/units/regional/'.$local_region.'/date.inc')) include_once(DOCUMENT_ROOT.'/core/units/regional/'.$local_region.'/date.inc');

$GLOBALS["timestamp"]=date('Y-m-d H:i:s');
$GLOBALS["cur_min"]=get_min($GLOBALS["timestamp"]);

function get_min($dt){
	global $min_cache;
	if(empty($dt)) return 0;
	if(!empty($min_cache[$dt])) return $min_cache[$dt];
	$odt=$dt;
	if(!strpos($dt,' ')){
		$time=explode(':',$dt);
		return $time[0]*60+$time[1];
	}
	$dt=explode(' ',$dt);
	$date=explode('-',$dt[0]);
	$time=explode(':',$dt[1]);
	if(empty($time[2])) $time[2]=0;
	//return ($date[0]-1980)*365*31*24*60+$date[1]*31*24*60+$date[2]*24*60+$time[0]*60+$time[1];
	$a=mktime($time[0],$time[1],$time[2],$date[1],$date[2],$date[0]);
	$a=floor($a/60);
	$min_cache[$odt]=$a;
	return $a;
}

function get_day($date){
	if(empty($date)) return 0;
	$date=explode('-',$date);
	if(!isset($date[2]) || !isset($date[1]) || !isset($date[0])) return 0;
	if(empty($date[0])) $date[0]=0;
	if(empty($date[1])) $date[1]=0;
	if(empty($date[2])) $date[2]=0;
	$a=mktime(0,0,0,$date[1],$date[2],$date[0]);
	$a=floor($a/60/60/24);
	return $a;
}

function get_month($date){
	if(empty($date)) return 0;
	$date=explode('-',$date);	
	return $date[0]*12+$date[1];
}

function day_left($date){
	$a1=get_day($date);
	$a2=get_day(date('Y-m-d'));
	return $a1-$a2;
}

function day_left2($date){
	$a1=get_day($date);
	$a2=get_day(date('Y-m-d'));
	return $a2-$a1;
}

function is_expite($date){
	return (day_left($date)<0);
}

function day_plus($date,$inc){
	$date=explode('-',$date);
	$a=mktime(0,0,0,$date[1],$date[2]+$inc,$date[0]);
	return date('Y-m-d', $a);
}

function month_plus($date,$inc){
	$date=explode('-',$date);
	$a=mktime(0,0,0,$date[1]+$inc,$date[2],$date[0]);
	return date('Y-m-d', $a);
}

function start_timer(){
	global $last_timer;
	$mtime=microtime();$mtime = explode(" ",$mtime);
	$res = $mtime[1] + $mtime[0];
	$last_timer=$res;
	return $res;
}

function end_timer($timer=0){
	if(empty($GLOBALS["last_timer"])) $GLOBALS["last_timer"]=0;
	if($timer==0) $timer=$GLOBALS["last_timer"];
	$mtime = microtime();$mtime = explode(" ",$mtime);$mtime = $mtime[1] + $mtime[0];
	return $mtime - $timer;
}

function xday_left($date=''){
	if(empty($date)) $date=date('Y-m-d');
	$a1=get_day($date);
	$a2=get_day('2010-01-01');
	return $a1-$a2;
}

function date_to_xhour($year=0,$month=0,$day=0,$hour=0){
	if(empty($year)) $year=date('Y');
	if(empty($month)) $month=date('m');
	if(empty($day)) $day=date('d');
	if(empty($hour)) $hour=date('H');
	$day=xday_left($year.'-'.$month.'-'.$day);
	return ($day-1)*24+$hour;
	//return ($year-2010-1)*365*24+($month-1)*31*24+($day-1)*24+$hour;
}

function xyear_to_date($xyear){
	$day='01';
	$month='01';
	$year=$xyear+2010;
	return $year.'-'.$month.'-'.$day;
}

function xmonth_to_date($xmonth){
	$day='01';
	$month=$xmonth % 12;
	if($month==0) $month=12;
	if(strlen($month)<2) $month='0'.$month;
	$year=floor($xmonth/12)+2010+1;
	if($xmonth % 12==0) $year--;//??
	return $year.'-'.$month.'-'.$day;
}

function xday_to_date($xday){
	return day_plus(date('Y-m-d'),$xday-xday_left());
}

function xhour_to_hour($xhour){
	return $xhour % 24;
}

function date_to_xday($year=0,$month=0,$day=0){
	if(!empty($year) && strpos($year,'-')){
		$tmp=explode('-',$year);
		$year=$tmp[0];
		$month=$tmp[1];
		$day=$tmp[2];
	}
	if(empty($year)) $year=date('Y');
	if(empty($month)) $month=date('m');
	if(empty($day)) $day=date('d');	
	$day=xday_left($year.'-'.$month.'-'.$day);
	return $day;
	//return ($year-2010-1)*365+($month-1)*31+$day;
}

function date_to_xmonth($year=0,$month=0){
	if(!empty($year) && strpos($year,'-')){
		$tmp=explode('-',$year);
		$year=$tmp[0];
		$month=$tmp[1];
	}
	if(empty($year)) $year=date('Y');
	if(empty($month)) $month=date('m');
	return ($year-2010-1)*12+$month;
}

function date_to_xyear($year=0){
	if(!empty($year) && strpos($year,'-')){
		$tmp=explode('-',$year);
		$year=$tmp[0];
	}
	if(empty($year)) $year=date('Y');
	return $year-2010;
}

function xday_to_xmonth($xday){
	$tmp=explode('-',xday_to_date($xday));
	return date_to_xmonth($tmp[0],$tmp[1]);
}

function date_to_new_format($date,$format){//date - Y-m-d
	if(empty($date)) return 0;
	$date=explode('-',$date);
	if(!isset($date[2]) || !isset($date[1]) || !isset($date[0])) return 0;
	if(empty($date[0])) $date[0]=0;
	if(empty($date[1])) $date[1]=0;
	if(empty($date[2])) $date[2]=0;
	$a=mktime(0,0,0,$date[1],$date[2],$date[0]);
	return date($format,$a);
}


?>