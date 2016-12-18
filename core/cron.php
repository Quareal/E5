<?php

// Подключаем ядро, если скрипт был вызван через CRON
if(!defined("DOCUMENT_ROOT") || !isset($GLOBALS["lvl"])){
	$_GET["cron"]=1;
	$_GET["cron2"]=1;
	include(dirname(dirname(__FILE__)).'/index.php');
	exit;
}

if(!$is_console) set_time_limit(500);
else set_time_limit(0);
ignore_user_abort(1);

global $db;
global $limit_cron;
$limit_cron=$cron_jobs; //лимит исполнения частей за один проход крона
$clim=0;
$shp=Array();

//индексируем содержимое для быстрого поиска
start_auto_index();

global $last_cron_hour, $last_cron_date;
$cron_is_new_hour=($last_cron_hour!=date('Y-m-d H'));
$cron_is_new_day=($last_cron_date!=date('Y-m-d'));
$last_cron_date=date('Y-m-d');
$last_cron_hour=date('Y-m-d H');
save_timing();

// Очищаем кеш, при превышении размера временных папок
if($cron_is_new_hour){
	if(!empty($GLOBALS['max_cache_size_mb'])){
		$size=0;
		foreach($GLOBALS['CACHE_FOLDERS'] AS $obj) $size+=sizeDirRec($obj);
		if($size/1024/1024>=$GLOBALS['max_cache_size_mb'])
			foreach($GLOBALS['CACHE_FOLDERS'] AS $obj) removeDirRec($obj);
	}
}

if($cron_is_new_day){
	// Закрываем старые незакрытые сессии
	$old_date=day_plus(date('Y-m-d'),-2).' 00:00:00';
	$db->query("UPDATE auth_session SET session_active=0 WHERE session_active=1 AND session_last<'$old_date'");
}

//выполняем бэкап базы данных
$dbn=DOCUMENT_ROOT.DEPLOY_PATH.'E5-'.date('Y-m-d').'.sql.gz';
if($do_backup && !file_exists($dbn)){
	check_deploy_path();
	backup_db($dbn);
}

//выполняем бэкап файлов пользователя
$dbn=DOCUMENT_ROOT.DEPLOY_PATH.'E5-files-'.date('Y-m-d').'.tar.gz';
if($do_backup_files && !file_exists($dbn)){
	check_deploy_path();
	ini_set('max_execution_time',0);
	gzip_folders2(DOCUMENT_ROOT.DEPLOY_PATH.'E5-files-'.date('Y-m-d').'.tar.gz',Array('files'),1,0,Array('files/deploy'));
}

//запуск части
$prts=getall($db,"SELECT * FROM main_part WHERE timer_type!=0 ORDER BY part_shell");
foreach($prts AS $prt)if(empty($shp[$prt["part_id"]])){
	$shp[$prt["part_id"]]=1;
	foreach($prt AS $var=>$value) $$var=$value;
	//запуск с правами суперпользователя (включается в настройках)
	if(!empty($cron_su)){
		global $user;
		$user->super=1; $user->id=0;
	}
	if($part_ex==0 && $parser_end_ex>0 && $timer_type!=0){
		//перебор экземпляров для части, у которой установлено поочерёдное применение ко всем экземплярам модуля
		run_part($part_id,0,1);
		$clim++;
		if($clim>=$limit_cron) break;
	} else{
		//запуск части по таймеру
		$curd=date('Y-m-d');
		$curt=date('H:i:s');
		$cur=$curd.' '.$curt;
		$par=explode(' ',$timer_last);
		$par_d=$par[0];
		$par_t=$par[1];
		$par_t2=$timer_time;
		$bool=false;
		if($timer_type==1) if($timer_date.' '.$timer_time<=$cur) $bool=true;
		if($timer_type==2) if($timer_last=='0000-00-00 00:00:00' || get_min($cur)-get_min($timer_last)>$timer_x) $bool=true;
		if($timer_type==3){
				if($timer_last=='0000-00-00 00:00:00' || get_day($curd)!=get_day($par_d) && $curt>$par_t2) $bool=true;
		}
		if($bool){
			run_part($part_id,0,1);
			$clim++;
			if($clim>=$limit_cron) break;
		}
	}
}

?>