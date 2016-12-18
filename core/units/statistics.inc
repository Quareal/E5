<?php

// Функции для модуля статистики

// Пересчитать статистику за период и добавить значение в период более высокого уровня
// например, после окончания дня сформировать суточную запись на основе статистики всех часов
function recalc_stat($type, $period, $start_time, $period2, $period2_time){
	global $db;
	getrow($db,"SELECT * FROM visit_object WHERE visit_type=$type AND visit_period=$period2 AND visit_time=$period2_time");
	if(!empty($db->Record)) return false;
	/*$zone_stat=getall($db,	"SELECT 
							v1.zone_id AS visit_object,
							(SELECT SUM(visit_hit) FROM visit_object AS v2 WHERE v2.visit_object=v1.zone_id AND v2.visit_type=0 AND v2.visit_period=0 AND v2.visit_time>$yesterday_hour) AS hits,
							(SELECT SUM(visit_host) FROM visit_object AS v2 WHERE v2.visit_object=v1.zone_id AND v2.visit_type=0 AND v2.visit_period=0 AND v2.visit_time>$yesterday_hour) AS hosts
						FROM main_zone AS v1 WHERE v1.zone_active=1");*/
	$stats=getall($db,	"SELECT 
							v1.visit_zone AS visit_zone,
							v1.visit_object AS vo,
							(SELECT SUM(visit_hit) FROM visit_object AS v2 WHERE v2.visit_object=v1.visit_object AND v2.visit_type=$type AND v2.visit_period=$period AND v2.visit_time>$start_time AND v2.visit_zone=v1.visit_zone) AS hits,
							(SELECT SUM(visit_host) FROM visit_object AS v2 WHERE v2.visit_object=v1.visit_object AND v2.visit_type=$type AND v2.visit_period=$period AND v2.visit_time>$start_time AND v2.visit_zone=v1.visit_zone) AS hosts
						FROM visit_object AS v1 WHERE v1.visit_type=$type AND visit_period=$period AND visit_time>$start_time GROUP BY visit_object, visit_zone");
	
	if(!empty($stats)) foreach($stats AS $zs){
		$object=$zs["vo"];
		$zone=$zs["visit_zone"];
		$hosts=0;
		$hits=0;
		if(!empty($zs)){
			$hits=$zs["hits"];
			$hosts=$zs["hosts"];
		}
		if($hits!=0 || $hosts!=0)
		$db->query("INSERT INTO visit_object (visit_type, visit_object, visit_period, visit_time, visit_host, visit_hit, visit_zone)
					VALUES ($type, $object, $period2, $period2_time, $hosts, $hits, $zone)");
	}
}

// Обновить или добавить новое значение в visit_object
function update_hit($is_host, $type, $object, $period, $time){
	global $db, $zi;
	if(empty($zi)) return false;
	getrow($db,"SELECT * FROM visit_object WHERE visit_type=$type AND visit_object=$object AND visit_period=$period AND visit_time=$time AND visit_zone=$zi");
	if(empty($db->Record)){
		$db->query("INSERT INTO visit_object (visit_type, visit_object, visit_period, visit_time, visit_hit, visit_host, visit_zone)
							VALUES ($type, $object, $period, $time, 1, 1, $zi)");
	} else {
		$chost=$db->Record["visit_host"];
		$chit=$db->Record["visit_hit"];
		$chit++;
		if($is_host) $chost++;
		//можно добавить visit_id - боюсь на переполнения лимита INT (так можно будет предсказкать время жизни системы)
		//занулить auto_increment можно так ALTER TABLE <имя таблицы> AUTO_INCREMENT=0
		$db->query("UPDATE visit_object SET visit_host=$chost, visit_hit=$chit WHERE visit_type=$type AND visit_object=$object AND visit_period=$period AND visit_time=$time AND visit_zone=$zi");
	}	
}

// Обновляет или добавляет информацию об источнике
// type==0 добавляет информацию о том, с какого домена был осуществлён вход
// type>0 добавляет информацию о том, с какой страницы домена (или с какой поисковой фраз) был сделан вход с привязкой к domain (ссылка на visit_data)
function update_source($day,$zone,$type,$url,$domain=0){
	global $db;
	if(empty($zone)) return false;
	if($type==0){
		$data_id=get_visit_data_id($url,0);
		getrow($db,"SELECT * FROM visit_source WHERE source_day=$day AND source_type=$type AND source_zone=$zone AND source_domain=$data_id");
		if(empty($db->Record)){
			$db->query("INSERT INTO visit_source (source_zone, source_type, source_domain, source_day, source_visit)
						VALUES ($zone, $type, $data_id, $day, 1)");
		} else {
			$visit=$db->Record["source_visit"]+1;
			$db->query("UPDATE visit_source SET source_visit=$visit WHERE source_day=$day AND source_type=$type AND source_zone=$zone AND source_domain=$data_id");
		}
		return $data_id;
	}
	if($type==1){
		$data_id=get_visit_data_id($url,1);
		getrow($db,"SELECT * FROM visit_source WHERE source_day=$day AND source_type=$type AND source_zone=$zone AND source_domain=$domain AND source_data=$data_id");
		if(empty($db->Record)){
			$db->query("INSERT INTO visit_source (source_zone, source_type, source_domain, source_data, source_day, source_visit)
						VALUES ($zone, $type, $domain, $data_id, $day, 1)");
		} else {
			$visit=$db->Record["source_visit"]+1;
			$db->query("UPDATE visit_source SET source_visit=$visit WHERE source_day=$day AND source_type=$type AND source_zone=$zone AND source_domain=$domain AND source_data=$data_id");
		}
		return $data_id;
	}
	return false;
}

// Доступ к ID нужного объекта по его имени и типу (например, к домену, URL, ключевым словам, экшену)
function get_visit_data_id($data,$type){
	global $db;
	if(strlen($data)>200) $data=substr($data,0,200);
	$data=safe_sql_input($data);
	getrow($db,"SELECT * FROM visit_data WHERE data_name='$data' AND data_type=$type");
	if(empty($db->Record)){
		$db->query("INSERT INTO visit_data (data_name, data_type)
					VALUES ('$data',$type)");
		getrow($db,"SELECT LAST_INSERT_ID() as sid");
		$sid=$db->Record["sid"];
	} else $sid=$db->Record["data_id"];
	return $sid;
}

// Удалить источники посещаемости (но не раньше чем за $day дней)
function clean_source_last($day){
	global $db;
	$day=date_to_xday()-$day;
	$db->query("DELETE FROM visit_source WHERE source_day<=".$day);
	$db->query("DELETE FROM visit_data WHERE 
				(data_type=0 AND (SELECT count(*) FROM visit_source WHERE source_type=0 AND source_domain=data_id)=0) OR
				(data_type=1 AND (SELECT count(*) FROM visit_source WHERE source_type=1 AND (source_domain=data_id OR source_data=data_id))=0)");
}

// Удаляем посещения по дням (но не раньше чем за $day дней)
function clean_day_last($day){
	global $db;
	$day=date_to_xday()-$day;
	$db->query("DELETE FROM visit_object WHERE visit_period=1 AND visit_time<=".$day);
	//после этой операции рекомендуется подчищать триггеры в visit_data
}

// Удаляем посещения по часам (но не раньше чем за $hour часов)
function clean_hour_last($hour){
	global $db;
	$hour=date_to_xhour()-$hour;
	$db->query("DELETE FROM visit_object WHERE visit_type=0 AND visit_period=0 AND visit_time<=".$hour);
}

//трфнсформация IP в INT64
function prepend_ip_v4($ip){
	$x=ip2long($ip);
	if($x<0) $x=-$x; //для поддержки 32-битных систем
	return $x;
}

// Два вида проверки IP
// 0 - хост или хит
// 1 - проверка допустимости действия в соответствии с лимитами
// 2 - тоже самое, но вместо IP выступает ID пользователя
function check_visit_ip($type,$zone,$ip='',$action='',$limit_count=0,$limit_hours=0,$view_only=false){ // ip может быть также ID юзера для type==3
	global $db,$user,$zi,$del_ip_history;
	if(empty($zone)) $zone=$zi;
	if(empty($zone)) return false;
	if(empty($ip) && ($type==0 || $type==1) && is_object($user) &&  isset($user->ip)) $ip=prepend_ip_v4($user->ip);
	else $ip=prepend_ip_v4($ip);
	if(empty($ip)) $ip=0;
	if($type==0){
		//$day_total=date_to_xday();
		$hour_total=date_to_xhour();
		$min=date_to_xhour()*60+date('i');
		getrow($db,"SELECT * FROM visit_ip WHERE ip_time>".($hour_total-24)." AND ip_value=$ip AND ip_zone=$zone AND ip_type=0 ORDER BY ip_time DESC LIMIT 1");
		if(!empty($db->Record)){
			//уточняем есть ли у нас уже именно этот час
			if($db->Record["ip_time"]==$hour_total){
				/*if(!$del_ip_history)*/ $db->query("UPDATE visit_ip SET ip_visit=".($db->Record["ip_visit"]+1).", ip_action=$min WHERE ip_time=$hour_total AND ip_value=$ip AND ip_zone=$zone AND ip_type=0");
			} else {
				$db->query("INSERT INTO visit_ip (ip_time, ip_value, ip_visit, ip_type, ip_zone, ip_action)
							VALUES ($hour_total, $ip, 1, 0, $zone, $min)");
			}
			return false;
		} else {
			// если это хост - добавляем его
			$db->query("INSERT INTO visit_ip (ip_time, ip_value, ip_visit, ip_type, ip_zone)
				VALUES ($hour_total, $ip, 1, 0, $zone)");
			return true;
		}
	}
	if(($type==1 || $type==2) && !empty($action)){
		$current_hour=date_to_xhour();
		$action_id=get_visit_data_id($action,5);
		if($limit_hours[0]=='!'){
			$limit_hours=substr($limit_hours,1);
			$current_hour=$current_hour*60+date('i');
		}
		$lim=$current_hour-$limit_hours;
		if(/*!$view_only && */$limit_hours>0) $db->query("DELETE FROM visit_ip WHERE ip_type=$type AND ip_zone=$zone AND ip_action=$action_id AND ip_value=$ip AND ip_time<$lim");
		getrow($db,"SELECT SUM(ip_visit) AS total_visit FROM visit_ip WHERE ip_type=$type AND ip_zone=$zone AND ip_action=$action_id AND ip_value=$ip");
		if($view_only) return $db->Record["total_visit"];
		if(empty($db->Record) || $db->Record["total_visit"]<$limit_count){
			$visit=getrowval("SELECT * FROM visit_ip WHERE ip_type=$type AND ip_zone=$zone AND ip_action=$action_id AND ip_value=$ip AND ip_time=$current_hour","ip_visit");
			if($visit) $db->query("UPDATE visit_ip SET ip_visit=".($visit+1)." WHERE  ip_type=$type AND ip_zone=$zone AND ip_action=$action_id AND ip_value=$ip AND ip_time=$current_hour");
			else $db->query("INSERT INTO visit_ip (ip_zone, ip_type, ip_action, ip_value, ip_visit, ip_time)
								VALUES ($zone, $type, $action_id, $ip, 1, $current_hour)");
			return true;
		} else {
			return false;
		}
	}
}

function collect_stat($skip_collect=false){
	// Установка начальных данных
	global $last_cow, $is_new_day, $db, $zi, $statistics_action, $user, $del_ip_history, $stop_stat, $is_new_hour, $collect_object_history, $collect_sources_history;
	global $ignore_bot, $user_agents;
	
	// Проверка на валидность юзер-агента
	if($ignore_bot && !$skip_collect){
		if(empty($_SERVER["HTTP_USER_AGENT"])){
			$skip_collect=true;
		} else {
			$sua=strtolower($_SERVER["HTTP_USER_AGENT"]);
			$have_ua=false;
			foreach($user_agents AS $ua) if(!(strpos($sua,$ua)===false)){
				$have_ua=true;
				break;
			}
			if(!$have_ua) $skip_collect=true;
		}
	}
	
	$day_total=date_to_xday(); //текущий день
	$month_total=date_to_xmonth(); //текущий месяц
	$years_total=date_to_xyear();	//текущий год
	
	$yesterday_hour=($day_total-2)*24;	//час, с которого начался отсчёт часов вчерашнего дня
	if(date('m')==1){
		$prev_month=12;
		$prev_month_year=date('Y')-1;
	} else {
		$prev_month=date('m')-1;
		$prev_month_year=date('Y');
	}
	$prev_month_day=date_to_xday($prev_month_year,$prev_month,1);	//первый день прошлого месяца
	$current_hour=date_to_xhour(); //текущий час
	$prev_year_month=date_to_xmonth(date('Y')-1,1);//первый месяц прошлого года
	
	if($is_new_day){
		//новый день
		
		// пересчёт статистики из часов в дни для зон
		recalc_stat(0,0,$yesterday_hour,1,$day_total-1);
		
		// пересчёт статистики из часов в дни для экшенов
		recalc_stat(3,0,$yesterday_hour,1,$day_total-1);		
	}
	if($is_new_day && date('d')==1){
		//новый месяц
		
		// пересчёт статистики из дней в месяц для зон
		recalc_stat(0,1,$prev_month_day,2,$month_total-1);
		
		// пересчёт статистики из дней в месяц для объектов
		recalc_stat(2,1,$prev_month_day,2,$month_total-1);
		
		// пересчёт статистики из дней в месяц для экшенов
		recalc_stat(3,1,$prev_month_day,2,$month_total-1);
	}
	if($is_new_day && date('d')==1 && date('m')==1){
		//новый год
		
		// пересчёт статистики из месяца в года для зон
		recalc_stat(0,2,$prev_year_month,3,$years_total-1);
		
		// пересчёт статистики из месяца в года для объектов
		recalc_stat(2,2,$prev_year_month,3,$years_total-1);
		
		// пересчёт статистики из месяца в года для экшенов
		recalc_stat(3,2,$prev_year_month,3,$years_total-1);
	}
	if($is_new_hour){
		// IP имеет смысл удалять раз в час и только те, которые старее 24 часов, соответственно и новый или старый хост считать если хост не заходил уже 24 часа
		// иначе в промежуток с 23 по 0:00 все хиты превращаются в хиты (как в золушке)
		if($del_ip_history){
			//$db->query("DELETE FROM visit_ip WHERE ip_type=0 AND ip_time!=$day_total"); // удаляем старые IP-шники
			$db->query("DELETE FROM visit_ip WHERE ip_type=0 AND ip_time<=".($current_hour-24)); // удаляем старые IP-шники
		}
	}
	if($is_new_day){
		if($GLOBALS["del_source_history"]) clean_source_last(62);
		if($GLOBALS["del_day_history"]) clean_day_last(62);
		if($GLOBALS["del_hour_history"]) clean_hour_last(48);
	}

	
	if($stop_stat || $skip_collect) return false;
	
	//проверка - хит или хост
	$is_host=check_visit_ip(0,$zi);
	
	// добавляем или обновляем статистику часа для зоны
	update_hit($is_host, 0, $zi, 0, $current_hour);
	
	// добавляем или обновляем статистику дня для объекта
	if($last_cow && $collect_object_history) update_hit($is_host, 2, $last_cow, 1, $day_total);
	
	// добавляем экшен
	if($statistics_action) update_hit($is_host, 3, get_visit_data_id($statistics_action,5), 0, $current_hour);
	
	// добавляем источники посетителей
	if(!empty($_SERVER["HTTP_REFERER"]) && $collect_sources_history && strstr(strtolower($_SERVER["HTTP_REFERER"]),'http')){
		$ref=$_SERVER["HTTP_REFERER"];
		$ref_tmp=explode('/',$ref);
		$ref_domain=str_replace('www.','',$ref_tmp[2]);
		$dest_domain=str_replace('www.','',$_SERVER["SERVER_NAME"]);		
		if($ref_domain!=$dest_domain){
			$ref_url=explode($ref_domain,$ref);
			if(!empty($ref_url[1])) $ref_url=$ref_url[1];
			else $ref_url='';
			if($ref_url=='/') $ref_url='';
			if(strstr($ref_url,'?') && strlen($ref_url)>150){
				$ref_url=explode('?',$ref_url);
				$ref_url=$ref_url[0];
			}
			$dom=update_source($day_total,$zi,0,strtolower($ref_domain));
			if(!empty($ref_url)) update_source($day_total,$zi,1,$ref_url,$dom);
		}
	}
	
	return true;
	
}

function get_online($zone=0,$time=5){
	global $db, $online_count;
	if(isset($online_count[$zone])) return $online_count[$zone];
	$sql_zone='';
	if(!empty($zone)) $sql_zone=' AND ip_zone='.$zone;	
	$min=date_to_xhour()*60+date('i')-$time;
	getrow($db,"SELECT count(*) AS hosts FROM visit_ip WHERE ip_type=0 AND ip_action>=$min".$sql_zone,1,"visit_ip");
	if(!empty($db->Record)) $res=$db->Record["hosts"];
	else $res=0;
	$online_count[$zone]=$res;
	return $res;
}
	
?>