<?php

global $clean,$off,$on,$action;
global $user; if($user->super==0) {include('main.php'); exit;}

$bool=false;
foreach($zone AS $z) if($z["zone_module"]==-2/* && $z["zone_active"]==1*/){ $bool=true; break;}
if($bool) echo '<div><a href="deploy">Установка системы на другой сервер / Загрузка бэкапов</a></div>';
else echo '<div>Для того, чтобы иметь возможность загружать бэкапы или воспользоваться установкой системы Server2Server, найстройте сервер обновленйи (сайты -> добавить сайт -> модуль -> сервер обновлений)</div>';

echo '<div><a href="statistics?section=settings">Настройки статистики посещаемости</a></div>';

// Установка доступа 777 для папки files
if($action=='provide_access'){
	$files=scan_dir('files');
	chmod('files',0777);
	foreach($files AS $file){
		if(is_dir(DOCUMENT_ROOT.'/'.$file)){
			chmod(DOCUMENT_ROOT.'/'.$file,0777);
		} else chmod(DOCUMENT_ROOT.'/'.$file,0666);
	}
}

// Установка доступа 777 для папки core
if($action=='provide_access2'){
	$files=scan_dir('core');
	chmod('core',0777);
	foreach($files AS $file){
		if(is_dir(DOCUMENT_ROOT.'/'.$file)){
			chmod(DOCUMENT_ROOT.'/'.$file,0777);
		} else chmod(DOCUMENT_ROOT.'/'.$file,0666);
	}
}

// Смена IP тестера
if($action=='change_ip'){
	$ip_debug=$new_ip;
	save_config();
}

// Смена Email для отправки отчётов об ошибках
if($action=='change_tester_email'){
	$send_error_reports=$new_email;
	save_config();
}

// Выключение отладки
if($off==3){
	$debug=false;
	save_config();
}

// Включение отладки
if($on==3){
	$debug=true;
	save_config();
}

// Выключение расширенной отладки
if($off==4){
	$debug2=false;
	save_config();
}

// Включение расширенной отладки
if($on==4){
	$debug2=true;
	save_config();
}

// Выключение кеширование SQL
if($off==1){
	$cache_sql=false;
	save_config();
}

// Включение кеширование SQL
if($on==1){
	$cache_sql=true;
	save_config();
}

// Очистка кеша SQL
if($clean==1 || $clean==4 || $off==1){
	clean_sql();
}

// Выключение кеширование функций
if($off==2){
	$cache_func=false;
	save_config();
}

// Включение кеширование функций
if($on==2){
	$cache_func=true;
	save_config();
}

// Включение отладки Ajax
if($on==5){
	$debug_ajax=true;
	save_config();
}
// Выключение отладки Ajax
if($off==5){
	$debug_ajax=false;
	save_config();
}

// Включение кеша структур шаблонов
if($on==6){
	$cache_tpl2=true;
	save_config();
}
// Выключение кеша структур шаблонов
if($off==6){
	$cache_tpl2=false;
	save_config();
}

// Включение режима экономии ОЗУ
if($on==7){
	$no_cache=true;
	save_config();
}
// Выключение режима экономии ОЗУ
if($off==7){
	$no_cache=false;
	save_config();
}

// Очистка кеша функций
if($clean==2 || $clean==4 || $off==2){
    if ($objs = glob(FTEMP."*")) {
        foreach($objs as $obj) {
	if(!is_dir($obj)) unlink($obj);
        }
    }	
}

// Очистка кеша шаблонов
if($clean==3 || $clean==4){
    if ($objs = glob(FTEMP."*")) {
        foreach($objs as $obj) {
	if(is_dir($obj)){
		$o=explode('/',$obj);
		$name=$o[count($o)-1];
		if($name=='tpl' || $name=='h') removeDirRec($obj);
	}
        }
    }	
}

// Очистка кеша JS и CSS файлов
if($clean==8 || $clean==4){
    if ($objs = glob(DOCUMENT_ROOT.CTEMP."*")) {
        foreach($objs as $obj) {
		unlink($obj);
        }
    }	
}

// Очистка кеша шаблонов
if($clean==6 || $clean==4){
    if ($objs = glob(FTEMP."*")) {
        foreach($objs as $obj) {
	if(is_dir($obj)){
		$o=explode('/',$obj);
		$name=$o[count($o)-1];
		if($name=='tpl_struct') removeDirRec($obj);
	}
        }
    }	
}

// Выключение кеша URL-роутера
/*if($off==8){
	$cache_url=false;
	save_config();
}*/

// Включение кеша URL-роутера
/*if($on==8){
	$cache_url=true;
	save_config();
}*/

// Выключение use_dnct
if($on==9){
	$use_dnct=false;
	save_config();
}

// Включение use_dnct
if($off==9){
	$use_dnct=true;
	save_config();
}

// Включение принудительного кеширования шаблонов
if($on==10){
	$cache_tpl=true;
	save_config();
}

// Выключение принудительного кеширования шаблонов
if($off==10){
	$cache_tpl=false;
	save_config();
}

// Включение фиксирования времени исполнения частей
if($on==11){
	$do_part_log=true;
	save_config();
}

// Выключение фиксирования времени исполнения частей
if($off==11){
	$do_part_log=false;
	save_config();
}

// Обнуление времени и количества исполнения частей
if($clean==7){
	$db->query("UPDATE main_part SET part_shellcount=0, part_shelltime=0");
}

// Очистка кеша URL-роутера
/*if($clean==8 || $clean==4 || $off==8){
    if ($objs = glob(FTEMP."*")) {
        foreach($objs as $obj) {
	if(is_dir($obj)){
		$o=explode('/',$obj);
		$name=$o[count($o)-1];
		if($name=='url') removeDirRec($obj);
	}
        }
    }	
}*/

    $size=0;$tsize=0;$fsize=0;$tsize2=0;$usize=0;$csize=0;
    if ($objs = glob(FTEMP."/*",GLOB_BRACE)) {
        foreach($objs as $obj) {
	if(is_dir($obj)){
		$o=explode('/',$obj);
		$name=$o[count($o)-1];
		if($name!='tpl' && $name!='json' && $name!='tpl_struct' && $name!='url' && $name!='explodeA' && $name!='strposA' && $name!='h') $size+=sizeDirRec($obj);
		else if($name=='explodeA' || $name=='strposA') $fsize+=sizeDirRec($obj);
		else if($name=='url') $usize=sizeDirRec($obj);
		else if($name=='tpl_struct'/* || $name=='h'*/) $tsize2=sizeDirRec($obj);
  		else $tsize=sizeDirRec($obj);
	} else {
		$o=explode('/',$obj);
		$name=$o[count($o)-1];
		if($name!='proc.log'){
			$fsize+=filesize($obj);
		}
	}}
    }
    if ($objs = glob(DOCUMENT_ROOT.CTEMP."*",GLOB_BRACE)) {
        foreach($objs as $obj) $csize+=filesize($obj);
    }

echo '<h1 style="text-align: left;">Кеш</h1>';

echo '<div>Кеш SQL: '.smart_size($size);
if($size!=0) echo ' (<a href="settings?clean=1">очистить</a>)';
if($cache_sql){
	echo ' [<a href="settings?off=1">Выключить и очистить</a>]';
	echo ' &nbsp; [';
	if($use_dnct) echo 'режим: <b>исключать</b> набор часто изменяемых таблиц - <a href="settings?on=9">включить</a>';
	else echo 'режим: <b>кешировать все</b> SELECT запросы - <a href="settings?off=9">исключить часто изменяемые</a>';
	echo ']';
} else echo ' [<a href="settings?on=1">Включить</a>]';
echo '</div>';

//больше не актуально
/*echo '<div>Кеш функций: '.smart_size($fsize);
if($fsize!=0) echo ' (<a href="settings?clean=2">очистить</a>)';
if($cache_func) echo ' [<a href="settings?off=2">Выключить и очистить</a>]';
else echo ' [<a href="settings?on=2">Включить</a>]';
echo '</div>';*/

/*echo '<div>Кеш шаблонов принудительный: ';
if($cache_tpl) echo '<b>Активно</b> [<a href="settings?off=10">Выключить</a>]';
else echo 'Выключено [<a href="settings?on=10">Включить</a>]';
echo '</div>';*/

/*echo '<div>Кеш URL-роутера: '.smart_size($usize);
if($usize!=0) echo ' (<a href="settings?clean=8">очистить</a>)';
if($cache_url) echo ' [<a href="settings?off=8">Выключить и очистить</a>]';
else echo ' [<a href="settings?on=8">Включить</a>]';
echo '</div>';*/

echo '<div>Кеш схем шаблона: '.smart_size($tsize2);
if($tsize2!=0) echo ' (<a href="settings?clean=6">очистить</a>)';
if($cache_tpl2) echo ' [<a href="settings?off=6&clean=6">Выключить и очистить</a>]';
else echo ' [<a href="settings?on=6">Включить</a>]';
echo '</div>';

echo '<div>Кеш шаблонов: '.smart_size($tsize);
if($tsize!=0) echo ' (<a href="settings?clean=3">очистить</a>)';
//echo '<div style="color: #999999;">[на данный момент глобальный кеш шаблонов недоступен, возможно только принудительное кеширование частей с префиксом ^&]</div>';
echo '</div>';

if(!empty($csize)) echo '<div>Кеш CSS и JS: '.smart_size($csize).' (<a href="settings?clean=8">очистить</a>)</div>';

echo '<div><a href="settings?clean=4">Очистить всё</a></div>';

echo '<h1 style="text-align: left;">Отладка</h1>';

if(!$debug) echo '<div>[<a href="settings?on=3">Включить режим отладки</a>]</div>'; else {
	echo '<div>[<a href="settings?off=3">Выключить отладку</a>]</div>';
	if($debug2) echo '<div>[<a href="settings?off=4">Выключить расширения отладки</a>]</div>';
	else echo '<div>[<a href="settings?on=4">Включить расширенную отладку</a>]</div>';
	echo '<div style="color: #999999;">[расширенная отладка показывает время и количество запусков функций, а также дерево вложенности вызовов функций]</div>';
	global $user;
	echo '<br><div>IP-адрес тестера (если хотите сделать отладку доступной всем - оставьте поле пустым):<br><form action="settings" method="post"><input type="hidden" name="action" value="change_ip"><input type="text" name="new_ip" value="'.$ip_debug.'" class="button"> <input class="button" type="submit" value=" > "> (ваш IP: '.$user->ip.')</form></div>';
}
echo '<br><div>Email тестера (на него будут приходить сообщения об ошибках не чаще 1 раза в 5 минут):<br><form action="settings" method="post"><input type="hidden" name="action" value="change_tester_email"><input type="text" name="new_email" value="'.$send_error_reports.'" class="button"> <input class="button" type="submit" value=" > "></form></div>';
if(!isset($debug_ajax)) $debug_ajax=false;
if(!$debug_ajax) echo '<div>[<a href="settings?on=5">Включить отладку Ajax</a>] (результаты запросов через Ajax вызовы частей будут записываться в /core/cache/logs)</div>';
else 			 echo '<div>[ - <a href="settings?off=5">Выключить отладку Ajax</a>] (результаты запросов через Ajax вызовы частей будут записываться в /core/cache/logs)</div>';
if(!isset($do_part_log)) $do_part_log=false;
if(!$do_part_log) echo '<div>[<a href="settings?on=11">Включить запись времени исполнения частей</a>]</div>';
else {
	echo '<br><div>[<a href="settings?off=11">Выключить запись времени исполнения частей</a>] - [<span class="link" OnClick="showhide(\'part_timing\');">Обзор статистики</span>]</div>';
	echo '<div id="part_timing" style="'.(empty($part_sort)?'display: none;':'').' padding: 10px; margin: 10px; background-color: #FAFAFA;">';
	echo '<h2>Статистика запусков частей</h2><a name="partstat"></a>';
	$part_stat_tmp=getall($db,"SELECT part_id, part_name, part_type, part_module, part_proc, part_shellcount, part_shelltime FROM main_part WHERE part_shellcount>0");
	if(empty($part_stat_tmp)){
		echo 'За время проверки части не выполнялись';
	} else {
		echo '<div style="margin-bottom: 5px;">[ ';
		if(!empty($part_sort) && $part_sort!='middle') echo '<a href="settings?part_sort=middle#partstat">Сортировать по среднему времени</a>'; else echo '<b>Сортировать по среднему времени</b>';
		echo ' | ';
		if(empty($part_sort) || $part_sort!='count') echo '<a href="settings?part_sort=count#partstat">Сортировать по количеству запусков</a>'; else echo '<b>Сортировать по количеству запусков</b>';
		echo ' | ';
		if(empty($part_sort) || $part_sort!='total') echo '<a href="settings?part_sort=total#partstat">Сортировать по суммарному времени</a>'; else echo '<b>Сортировать по суммарному</b>';
		echo ' ]</div>';
		echo '<table id="records" cellpadding="3" cellspacing="1" style="width: 530px;">
			<tr>
				<th>Часть</th>
				<th>Среднее время выполнения</th>
				<th>Кол-во запусков</th>
				<th>Суммарное время</th>
			</tr>';
		$part_stat=Array();
		if(empty($part_sort) || $part_sort=='middle'){
			foreach($part_stat_tmp AS $tmp){
				$mid_time=$tmp["part_shelltime"]/$tmp["part_shellcount"];
				$part_stat[$mid_time*10000][$tmp["part_id"]]=$tmp;
			}
		} else if($part_sort=='total') {
			foreach($part_stat_tmp AS $tmp){
				$tmp["mid_time"]=$tmp["part_shelltime"]/$tmp["part_shellcount"];
				$part_stat[$tmp["part_shelltime"]*10000][$tmp["part_id"]]=$tmp;
			}
		} else if($part_sort=='count'){
			foreach($part_stat_tmp AS $tmp){
				$tmp["mid_time"]=$tmp["part_shelltime"]/$tmp["part_shellcount"];
				$part_stat[$tmp["part_shellcount"]][$tmp["part_id"]]=$tmp;
			}
		}
		krsort($part_stat);
		$tmp_modules=getall6($db,"SELECT module_id, module_name FROM main_module","module_id","module_name");		
		foreach($part_stat AS $mid=>$tmp)foreach($tmp AS $part_id=>$part){
			echo '<tr>';
			echo '<td>'.$part["part_name"].' (';
			if(!empty($part["part_module"])){
				echo $tmp_modules[$part["part_module"]];
			} else {
				if($part["part_proc"]==0) echo 'функции';
				if($part["part_proc"]==1) echo 'отображения';
				if($part["part_proc"]==2) echo 'компоненты';
				if($part["part_proc"]==3) echo 'формы';
			}
			echo ')</td>';
			if(empty($part_sort) || $part_sort=='middle'){
				echo '<td>'.substr(($mid/10000),0,7).'</td>';
			} else {
				echo '<td>'.substr($part["mid_time"],0,7).'</td>';
			}
			echo '<td>'.$part["part_shellcount"].'</td>';
			echo '<td>'.$part["part_shelltime"].'</td>';
			echo '</tr>';
		}
		echo '</table>';
		echo '<br><div>[<a href="settings?clean=7">Очистить время и количество запусков частей</a>]</div>';
	}
	echo '</div>';
}

//упразднено из-за низкой эффективности
//if(!$no_cache) echo '<div>[<a href="settings?on=7">Включить режим экономии ОЗУ (замедляет работу)</a>]</div>'; else 
//echo '<div>[<a href="settings?off=7">Выключить режим экономии ОЗУ (рекомендуется для повышения производительности)</a>]</div>';


echo '<h1 style="text-align: left;">Соединение с БД</h1>';

// Удалить все сессии
if(!empty($_GET['reset_session'])){
	global $db;
	$db->query('DELETE FROM auth_session');
}

// Изменение реквизитов БД и сохранение настроек
if($action=='mysql' && check_form_protection_key($_POST['key'],'settings',1)){
	clean_sql();
	$test=new DB_Sql;
	$temp=ob_get_contents();
	if($temp) ob_end_clean();
	ob_start();
	$test->Host=$server2;
	$test->Database=$database2;
	$test->User=$username2;
	$test->Password=$password2;
	$test->Halt_On_Error='no';
	$test->Die_On_Error='no';
	//$test->query("SELECT * FROM main_zone");
	$test->query("SET NAMES `utf8` COLLATE `utf8_general_ci`");
	$temp2=ob_get_contents();
	ob_end_clean();
	if($temp){
		ob_start();
		echo $temp;
	}
	$test->Halt_On_Error='yes';
	$test->Die_On_Error='yes';
	if((!empty($restore_db) || $use_dump || $use_dump2)){
		if(!empty($clear_db)) remove_db($database2,$test);
		if(!empty($use_dump2)){
			$dump=DOCUMENT_ROOT.'/core/install/package.sql';
			load_dump('',$test,0,$dump);
		} else if(empty($use_dump)){
			include_once(DOCUMENT_ROOT.'/core/install/sql.inc');
			load_dump($dump,$test);
		} else {
			load_dump('',$test,$database2,DOCUMENT_ROOT.'/'.$use_dump);
		}
		true_unicode($test);
	}
	if(!empty($temp2) && !$test->error && empty($restore_db) && empty($use_dump)){
		echo '<div><b style="color: #FF0000;">Ошибка</b><br>Не удалось установить подключение по указанным реквизитам, попробуйте указать другие реквизиты</div>';		
	} else {
		$server=$server2;
		$database=$database2;
		$username=$username2;
		$password=$password2;
		$no_update=array_flip(explode('
',$no_update2));
		$tmp=array_flip(explode('
',$black_ips2));
		if(isset($_SERVER["REMOTE_ADDR"]) && isset($tmp[$_SERVER["REMOTE_ADDR"]])){
			echo '<div style="font-size: 16px; color: #990000;" align="center"><b>Нельзя заблокировать собственный IP адрес</b></div>';
			unset($tmp[$_SERVER["REMOTE_ADDR"]]);			
		}
		$black_ips=$tmp;
		if(empty($is_offline2)) $is_offline2=false; else $is_offline2=true;
		if(empty($cron_type2)) $cron_type2=0; else $cron_type2=1;
		if(empty($cron_su2)) $cron_su=0; else $cron_su=1;
		if(empty($correct_utf2)) $correct_utf=0; else $correct_utf=1;
		if(empty($protect_admin_form2)) $protect_admin_form=0; else $protect_admin_form=1;
		if(empty($do_backup2)) $do_backup2=0; else $do_backup2=1;
		if(empty($do_backup_files2)) $do_backup_files=0; else $do_backup_files=1;
		if(empty($check_for_xss2)) $check_for_xss2=0; else $check_for_xss2=1;
		if(empty($use_ace2)) $use_ace2=0; else $use_ace2=1;
		if(empty($show_404_2)) $show_404_2=0; else $show_404_2=1;
		if(empty($session_multy2)) $session_multy2=0; else $session_multy2=1;
		if(empty($session_everytime2)) $session_everytime2=0; else $session_everytime2=1;
		if(empty($max_cache_size_mb2)) $max_cache_size_mb=0; else $max_cache_size_mb=$max_cache_size_mb2;
		$system_email=$system_email2;
		$is_offline=$is_offline2;
		$cron_type=$cron_type2;
		$def_chmod=$def_chmod2;
		$def_drmod=$def_drmod2;
		$def_charset=$def_charset2;
		$do_backup=$do_backup2;
		$show_404=$show_404_2;
		$cron_jobs=$cron_jobs2;
		$check_for_xss=$check_for_xss2;
		$use_ace=$use_ace2;		
		$rewrite_upload_max_filesize=$rewrite_upload_max_filesize2;
		$rewrite_post_max_size=$rewrite_post_max_size2;
		$rewrite_memory_limit=$rewrite_memory_limit2;
		$rewrite_max_execution_time=$rewrite_max_execution_time2;
		$rewrite_max_input_time=$rewrite_max_input_time2;
		$session_lifetime=$session_lifetime2;
		$session_multy=$session_multy2;
		$session_storage=$session_storage2;
		$session_everytime=$session_everytime2;
		$superuser_ip=$superuser_ip2;
		save_config(true);
	}
}

if(empty($username)) $username=cfg_extract('username');
/*if(empty($password))*/ $password=cfg_extract('password');
if(empty($su_login)) $su_login=cfg_extract('su_login');
/*if(empty($su_pwl))*/ $su_pwl=cfg_extract('su_pwl');

$have_dump=file_exists(DOCUMENT_ROOT.'/core/install/package.sql');

echo '<form action="settings" method="post">
<input type="hidden" name="action" value="mysql" autocomplete="off">
<input name="password3" id="fake_pwl" value="" type="password" style="border: 0px; width: 1px; height: 1px; padding: 0px;">
<input style="display:none" type="text" name="fakeusername"/>
<input style="display:none" type="password" name="fakepassword"/>
<table id="records" cellpadding="3" cellspacing="1" style="width: 330px;">
	<tr><td>Сервер:</td><td><input name="server2" value="'.$server.'" OnKeyUp="document.getElementById(\'restore-check\').style.display=\'\';"></td></tr>
	<tr><td>Логин:</td><td><input name="username2" value="'.$username.'"></td></tr>
	<tr><td>Пароль:</td><td><input name="password2" id="password2" value="'.$password.'" style="width: 192px;" type="password" autocomplete="off" autofill="off"><span OnClick="s=document.getElementById(\'password2\'); if(s.type==\'password\') s.type=\'\'; else s.type=\'password\';" class="link">'.si('view').'</span></td></tr>
	<tr><td>База данных:</td><td><input name="database2" value="'.$database.'" OnKeyUp="document.getElementById(\'restore-check\').style.display=\'\';"></td></tr>
</table>
<div id="restore-check" style="display: none;"><input type="checkbox" name="restore_db" style="width: 15px; height: 15px;"> восстановить структуру таблиц на новой БД
<br><input type="checkbox" name="clear_db" style="width: 15px; height: 15px;"> удалить содержимое БД'
.($have_dump?'<br><input type="checkbox" name="use_dump2" style="width: 15px; height: 15px;"> использовать дамп, полученный вместе с установщиком':'').'
<br>'.($have_dump?'или':'') .'использовать дамп: <input type="text" style="width: 250px;" name="use_dump" value=""> (укажите его расположение относительно головной папки, начинать без слеша)</div>

<h1 style="text-align: left;"><a name="su"></a>Реквизиты суперпользователя</h1>

<table id="records" cellpadding="3" cellspacing="1" style="width: 400px;">
	<tr><td>Логин:</td><td><input name="su_login" value="'.$su_login.'"></td></tr>
	<tr><td>Пароль:</td><td><input name="su_pwl" id="su_pwl" value="'.$su_pwl.'" style="width: 225px;" type="password" autocomplete="off" autofill="off"><span OnClick="s=document.getElementById(\'su_pwl\'); if(s.type==\'password\') s.type=\'\'; else s.type=\'password\';" class="link">'.si('view').'</span></td></tr>
	<tr><td>Фиксированный IP:</td><td><input name="superuser_ip2" value="'.$superuser_ip.'"></td></tr>
</table>';

echo '<div><a href="settings?reset_session=1">Сбросить все сессии</a></div>';

echo get_form_protection_key('settings',1,1);

echo '<h1 style="text-align: left;">Скорость хостинга</h1>';

global $speed_in,$speed_out,$speed_cpu,$speed_hdd,$speed_db,$testings;
if(!empty($testings) && $testings==2){
	echo '<div id="afh" style="font-size: 16px; color: #770000;"><b>Начинается проверка</b>...</div>';

	$speed_out=floor(  (1024*1024*2)/$_GET["x"]   );
	$speed_cpu=floor(13-$_GET["y"]); if($speed_cpu<0) $speed_cpu=0; if($speed_cpu>10) $speed_cpu=10;

	//проверка HDD
	$td=FTEMP;
	check_dir(FTEMP);
	for($i=0;$i<100;$i++) $f[$i]=get_code2(rand(1024,1024*5));
	start_timer();
	for($i=0;$i<100;$i++){
		$fi=fopen($td.'/test'.$i,'w');fwrite($fi,$f[$i]);fclose($fi);
	}
	for($i=0;$i<100;$i++){
		$fi=fopen($td.'/test'.$i,'r');fread($fi,filesize($td.'/test'.$i));fclose($fi);
	}
	for($i=0;$i<100;$i++){
		unlink($td.'/test'.$i);
	}
	$sp3=end_timer()*10;
	$speed_hdd=floor(12-$sp3); if($speed_hdd<0) $speed_hdd=0; if($speed_hdd>10) $speed_hdd=10;

	//проверка СУБД
	global $db;
	start_timer();
	for($i=0;$i<100;$i++) $db->query("INSERT INTO row_value (value_module, value_table, value_row, value_col, value_value)
									VALUES (-5,-5,-5,-100,'$f[$i]')");
	getall($db,"SELECT * FROM row_value WHERE value_col=-100");
	getall($db,"SELECT * FROM row_value WHERE value_col=-100 AND value_value LIKE '%ab%'");
	$db->query("DELETE FROM row_value WHERE value_col=-100");
	$sp3=end_timer()*100;
	$speed_db=floor(22-$sp3); if($speed_db<0) $speed_db=0; if($speed_db>10) $speed_db=10;

	//проверка входящей скорости
	start_timer();
	$fs=do_get('http://www.google.ru/').do_get('http://www.yandex.ru/').do_get('http://www.youtube.com/');
	$speed_in=floor(    (strlen($fs))/end_timer()   );//очень примерное измерение... вообще не точное

	save_config();

	echo '<script>document.getElementById("afh").style.display="none";</script>';
}

echo '<table id="records" cellpadding="3" cellspacing="1" style="width: 330px;">
	<tr><td>Общая:</td><td>';
	
	$sp1=0;
	if($speed_in>64*1024) $sp1=3;
	if($speed_in>128*1024) $sp1=5;
	if($speed_in>256*1024) $sp1=7;
	if($speed_in>512*1024) $sp1=10;
	$sp2=0;
	if($speed_out>64*1024) $sp2=3;
	if($speed_out>256*1024) $sp2=5;
	if($speed_out>512*1024) $sp2=7;
	if($speed_out>1024*1024) $sp2=10;
	//if($speed_out>512*1024) $sp2=3;
	//if($speed_out>1024*1024) $sp2=5;
	//if($speed_out>4096*1024) $sp2=7;
	//if($speed_out>16384*1024) $sp2=10;
	echo floor(($speed_cpu+$speed_hdd+$speed_db+$sp1+$sp2)/5);

	echo ' из 10</td></tr>
	<tr><td>ЦПУ:</td><td>'.(int)$speed_cpu.' из 10</td></tr>
	<tr><td>Обращения к диску:</td><td>'.(int)$speed_hdd.' из 10</td></tr>
	<tr><td>Обращения к БД:</td><td>'.(int)$speed_db.' из 10</td></tr>
	<tr><td>Входящая скорость:</td><td>'.smart_size($speed_in).'/с</td></tr>
	<tr><td>Скорость отдачи:</td><td>'.smart_size($speed_out).'/с</td></tr>
</table>
<a href="settings?testings=1">Проверить</a>

<h1 style="text-align: left;">Прочие настройки</h1>

<p>Системный email:<br><input type="text" name="system_email2" style="width: 250px;" value="'.$GLOBALS["system_email"].'"></p>

<p>Права для файлов (например 0777):<br><input type="text" name="def_chmod2" style="width: 250px;" value="';
if(!empty($action) && $action=='mysql') echo $GLOBALS["def_chmod"]; else echo '0'.decoct($GLOBALS["def_chmod"]);
echo '"></p>
<p>Права для папок:<br><input type="text" name="def_drmod2" style="width: 250px;" value="';
if(!empty($action) && $action=='mysql') echo $GLOBALS["def_drmod"]; else echo '0'.decoct($GLOBALS["def_drmod"]);
echo '"></p>

<p>Отдавать HTML контент в кодировке (если ваш сервер по умолчанию отдаёт не в utf-8):<br><input type="text" name="def_charset2" style="width: 250px;" value="'.$GLOBALS["def_charset"].'"></p>

<p>Автоматически удалять кеш, если он превышает указанное число мегабайт (0 - не удалять кеш):<br><input type="text" name="max_cache_size_mb2" style="width: 250px;" value="'.$GLOBALS["max_cache_size_mb"].'"></p>

<p><input type="checkbox" class="button" style="width: 30px; height: 30px;" name="is_offline2"'.($GLOBALS["is_offline"]?"checked":"").'> Работа в Off-line режиме
<br><i>Необходима в случаях, если вы запускаете систему на локальном веб-сервере без доступа к интернету<br>Данная настройка отключает проверку IP пользователя</i></p>

<p><input type="checkbox" class="button" style="width: 30px; height: 30px;" name="do_backup2"'.($GLOBALS["do_backup"]?"checked":"").'> Архивировать базу данных
<br><input type="checkbox" class="button" style="width: 30px; height: 30px;" name="do_backup_files2"'.($GLOBALS["do_backup_files"]?"checked":"").'> Архивировать файлы пользователя
<br><i>1 раз в день запускает архивацию базы данных. Загрузить архивные копии можно в разделе "Установка системы на другой сервер" (доступна из раздела Настройка, при наличии сервера обновлений)</i></p>

<p><input type="checkbox" class="button" style="width: 30px; height: 30px;" name="cron_type2"'.($GLOBALS["cron_type"]?"checked":"").'> Осуществлять отложенный запуск частей (по расписанию, CRON) через посетителей
<br><i>Необходимо в тех случаях, когда невозможно настроить CRON на веб-сервере. Опасность такого способа заключается в том, что запуски могут происходить не регулярно, если веб-сайты не имеют регулярных посетителей<br>Если эта опция отключена, Вам необходимо настроить CRON самостоятельно.</i>
<br><b>Реквизиты для CRON / CronTAB</b><br>Адрес скрипта: <b>/core/cron.php</b> (например "~/public_html/core/cron.php")<br>Адрес для настройки через WGET: адрес любого из сайтов, прикреплённых к системе?cron=1 (например: "wget -O - -q http://site1.ru?cron=1")
<br>Внимание!<br>Некоторые хостеры требуют прописывать путь до PHP в первой строчке скрипта. В нашем файле cron.php (/core/cron.php) сейчас находится строчка #!/usr/local/bin/php</p>

<p><input type="checkbox" class="button" style="width: 30px; height: 30px;" name="cron_su2"'.($GLOBALS["cron_su"]?"checked":"").'> Запускать CRON с правами суперпользователя</p>

<p>Кол-во заданий, обрабатываемых за одно выполнение CRON:<br><input type="input" style="width: 250px;" name="cron_jobs2" value="'.$GLOBALS["cron_jobs"].'"></p>

<p><input type="checkbox" class="button" style="width: 30px; height: 30px;" name="correct_utf2"'.($GLOBALS["correct_utf"]?"checked":"").'> Корректировать кодировку (отмечать в случае, если вместо текста показываются знаки вопроса)</p>

<p><input type="checkbox" class="button" style="width: 30px; height: 30px;" name="check_for_xss2"'.($GLOBALS["check_for_xss"]?"checked":"").'> Защищать формы на сайтах от XSS атак</p>

<p><input type="checkbox" class="button" style="width: 30px; height: 30px;" name="protect_admin_form2"'.($GLOBALS["protect_admin_form"]?"checked":"").'> Защищать формы от возможной повторной отправки (распространяется только на формы операций данными внутри таблиц в административном кабинете)</p>

<p><input type="checkbox" class="button" style="width: 30px; height: 30px;" name="show_404_2"'.($GLOBALS["show_404"]?"checked":"").'> Отправлять 404 ошибку в случае, если имеется неразобранный аппендикс URL</p>

<h1 style="text-align: left;">Сессии</h1>

<p><input type="checkbox" class="button" style="width: 30px; height: 30px;" name="session_multy2"'.($GLOBALS["session_multy"]?"checked":"").'> Разрешать множественные сессии для пользователя</p>

<p>Максимальное время простоя сессии (в минутах, 0 - ограниченно 48 часами, либо неограниченно, если отключён CRON):<br><input type="text" name="session_lifetime2" style="width: 250px;" value="'.$GLOBALS["session_lifetime"].'"></p>

<p>Количество хранимых закрытых сессий пользователя для истории (0 - не хранить, -1 - неограниченно):<br><input type="text" name="session_storage2" style="width: 250px;" value="'.$GLOBALS["session_storage"].'"></p>

<p><input type="checkbox" class="button" style="width: 30px; height: 30px;" name="session_everytime2"'.($GLOBALS["session_everytime"]?"checked":"").'> Генерировать новый ключ сессии при каждом соединении</p>

<!-- <p><input type="checkbox" class="button" style="width: 30px; height: 30px;" name="use_ace2"'.($GLOBALS["use_ace"]?"checked":"").'> Использовать Ace-editor для пракви частей</p> -->
<input type="hidden" class="button" style="width: 30px; height: 30px;" name="use_ace2" value="'.($GLOBALS["use_ace"]?"1":"").'">

<h1 style="text-align: left;">Файлы, которые не следует обновлять</h1>

<div><textarea name="no_update2" style="width: 330px; height: 120px;">'.implode('
',array_flip($no_update)).'</textarea></div><br>

<h1 style="text-align: left;">Заблокированные IP адреса (сервер будет возвращать им ошибку 503, одна строчка - один адрес)</h1>

<div><textarea name="black_ips2" style="width: 330px; height: 120px;">'.implode('
',array_flip($black_ips)).'</textarea></div><br>

<h1 style="text-align: left;">Перекрытие базовых настроек PHP</h1>
<p>Оставьте пустое значение для использование настроек по умолчанию (которые описаны в файле index.php)</p>
<p>Максимальный размер загружаемого файла (upload max filesize), в мб:<br><input type="text" name="rewrite_upload_max_filesize2" style="width: 250px;" value="'.$GLOBALS["rewrite_upload_max_filesize"].'"></p>
<p>Максимальный размер всех загружаемых файлов (post max size), в мб:<br><input type="text" name="rewrite_post_max_size2" style="width: 250px;" value="'.$GLOBALS["rewrite_post_max_size"].'"></p>
<p>Лимит памяти (в мб):<br><input type="text" name="rewrite_memory_limit2" style="width: 250px;" value="'.$GLOBALS["rewrite_memory_limit"].'"></p>
<p>Максимальное время выполнения скрипта (установите "-" для неограниченного времени исполнения) :<br><input type="text" name="rewrite_max_execution_time2" style="width: 250px;" value="'.$GLOBALS["rewrite_max_execution_time"].'"></p>
<p>Максимальное время загрузки файлов (max_input_time) (установите "-" для неограниченного времени исполнения) :<br><input type="text" name="rewrite_max_input_time2" style="width: 250px;" value="'.$GLOBALS["rewrite_max_input_time"].'"></p>

<div><a href="settings?action=provide_access">Установить доступ 777 для всех папок и доступ 666 для файлов в папке files</a></div>

<div><a href="settings?action=provide_access2">Установить доступ 777 для всех папок и доступ 666 для файлов в папке core</a></div>

<br><br>

<input type="submit" class="button" value="Сохранить"></form>';

?>