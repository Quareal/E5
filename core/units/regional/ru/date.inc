<?php

function get_spec_month($month){
	switch ($month){
		case 1:return 'января';break;
		case 2:return 'февраля';break;
		case 3:return 'марта';break;
		case 4:return 'апреля';break;
		case 5:return 'мая';break;
		case 6:return 'июня';break;
		case 7:return 'июля';break;
		case 8:return 'августа';break;
		case 9:return 'сентября';break;
		case 10:return 'октября';break;
		case 11:return 'ноября';break;
		case 12:return 'декабря';break;
	}
}

function get_spec_month2($month){
	switch ($month){
		case 1:return 'январь';break;
		case 2:return 'февраль';break;
		case 3:return 'март';break;
		case 4:return 'апрель';break;
		case 5:return 'май';break;
		case 6:return 'июнь';break;
		case 7:return 'июль';break;
		case 8:return 'август';break;
		case 9:return 'сентябрь';break;
		case 10:return 'октябрь';break;
		case 11:return 'ноябрь';break;
		case 12:return 'декабрь';break;
	}
}

function date_to_weekday($date){
	$w=date_to_new_format($date,'w');
	switch($w){
		case 0: return 'воскресенье'; break;
		case 1: return 'понедельник'; break;
		case 2: return 'вторник'; break;
		case 3: return 'среда'; break;
		case 4: return 'четверг'; break;
		case 5: return 'пятница'; break;
		case 6: return 'суббота'; break;
	}
}

function date_to_weekday2($date){
	$w=date_to_new_format($date,'w');
	switch($w){
		case 0: return 'вс'; break;
		case 1: return 'пн'; break;
		case 2: return 'вт'; break;
		case 3: return 'ср'; break;
		case 4: return 'чт'; break;
		case 5: return 'пт'; break;
		case 6: return 'сб'; break;
	}
}

function get_normal_date($date){
	if(strpos($date,'-')){
		$date=explode("-",$date);
		return $date[2].'&nbsp;'.get_spec_month($date[1]).'&nbsp;'.$date[0];
	} else return $date;
}

function get_normal_date2($date){
	$date=explode("-",$date);
	return $date[2].' '.get_spec_month($date[1]).' '.$date[0];
}

?>