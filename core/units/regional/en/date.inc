<?php

function get_spec_month($month){
	switch ($month){
		case 1:return 'january';break;
		case 2:return 'february';break;
		case 3:return 'march';break;
		case 4:return 'april';break;
		case 5:return 'may';break;
		case 6:return 'june';break;
		case 7:return 'july';break;
		case 8:return 'august';break;
		case 9:return 'september';break;
		case 10:return 'october';break;
		case 11:return 'novemver';break;
		case 12:return 'december';break;
	}
}

function get_spec_month2($month){
	switch ($month){
		case 1:return 'january';break;
		case 2:return 'february';break;
		case 3:return 'march';break;
		case 4:return 'april';break;
		case 5:return 'may';break;
		case 6:return 'june';break;
		case 7:return 'july';break;
		case 8:return 'august';break;
		case 9:return 'september';break;
		case 10:return 'october';break;
		case 11:return 'novemver';break;
		case 12:return 'december';break;
	}
}

function date_to_weekday($date){
	$w=date_to_new_format($date,'w');
	switch($w){
		case 0: return 'sunday'; break;
		case 1: return 'monday'; break;
		case 2: return 'tuesday'; break;
		case 3: return 'wednesday'; break;
		case 4: return 'thursday'; break;
		case 5: return 'friday'; break;
		case 6: return 'saturday'; break;
	}
}

function date_to_weekday2($date){
	$w=date_to_new_format($date,'w');
	switch($w){
		case 0: return 'sun'; break;
		case 1: return 'mon'; break;
		case 2: return 'w'; break;
		case 3: return 'wed'; break;
		case 4: return 'th'; break;
		case 5: return 'fr'; break;
		case 6: return 'sat'; break;
	}
}

function get_normal_date($date){
	if(strpos($date,'-')){
		$date=explode("-",$date);
		return $date[2].'&nbsp;'.get_spec_month($date[0]).'&nbsp;'.$date[1];
	} else return $date;
}

function get_normal_date2($date){
	$date=explode("-",$date);
	return $date[2].' '.get_spec_month($date[0]).' '.$date[1];
}

?>