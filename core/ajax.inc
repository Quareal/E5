<?php

ob_start();
header ("Content-Type: text/html; charset=utf-8");	// посылаем заголовок ответа
										// в дальнейшем, можно внедрить определение Content-Type частью вывода
	
// Системные вызовы
										
if(!empty($_GET["exsys5"])){

	$action=$_GET["exsys5"];
	
	// Определение соли для сессии пользователя при защищённой авторизации
	
	if($action=='getsolt' && !empty($_POST["z-login"])){
		if(isset($_POST["solt"])) $solt=$_POST["solt"];
		if(isset($_POST["hash"])) $hash=$_POST["hash"];
		if(isset($_POST["z-login"])) $login=$_POST["z-login"];
		if(empty($su_login)) $su_login=cfg_extract('su_login');
		if(empty($su_pwl)) $su_pwl=cfg_extract('su_pwl');	
		$upwl='';
		if($su_login==$login) $upwl=$su_pwl;
		else {
			getrow($db,"SELECT * FROM main_auth WHERE auth_type=0 AND user_login='$login' AND auth_enable=1",1,"main_auth",__LINE__,__FILE__);
			if(!empty($db->Record)) $upwl=$db->Record["user_pwl"];
		}
		if(!empty($upwl) && md5($upwl.$solt)==$hash){
			$solt=get_code3(64);
			$user_safe=md5($upwl.$solt);
			$user_safe=$user_safe.'|-|-|'.$user_safe;
			if($login==$su_login){
				global $su_safe;
				$su_safe=$user_safe;
				//save_config();
				save_timing();
			} else {
				$db->query("UPDATE main_auth SET user_safe='$user_safe' WHERE auth_id=".$db->Record["auth_id"],3,"main_auth");
			}
			echo $solt; // вывод нужного значения
		} else echo 1; // ошибка аутентификации
		unset($su_login);
		unset($su_pwl);
		exit;
	}
	
} else {

	if(isset($_GET["ajax"])) $aj=$_GET["ajax"];
	if(isset($_POST["ajax"])) $aj=$_POST["ajax"];
	if(isset($aj) && !strstr($aj,'%') && !strstr($aj,':') && strstr($aj,'.')){
		// Вызовы частей через обычный ajax
		// для модулей ?ajax=module.exname.part (только для типов 0 и 1 - полноценных частей и виджетов, не для обработчиков)
		// для частей компонентов ?ajax=type.part
		// такие части должны использовать флаг "не проверять входящие GET переменные"
		$GLOBALS["cur_ex"]=0;
		$GLOBALS["from_ajax"]=1;
		$p_par=Array();
		foreach($_GET AS $var=>$value) if($var!='ajax'){
			$p_par[$var]=urldecode($value);
		}
		$GLOBALS["puid"]=get_uid($p_par); // Подгружаем хеш входящих переменных для определения кеша
		if(substr_count($aj,'.')==1){
			$t=explode('.',$aj);
			if($t[0]=='func' || $t[0]=='function') $part_proc=0;
			if($t[0]=='show') $part_proc=1;
			if($t[0]=='component') $part_proc=2;
			if($t[0]=='form') $part_proc=3;
			$part_sname=safe_sql_input($t[1]);
			$part_id=getrowval("SELECT part_sname, part_id, part_type, part_proc, part_unsafe FROM main_part WHERE part_sname='$part_sname' AND part_type=2 AND part_proc=$part_proc AND part_unsafe=1","part_id");
			if(empty($part_id)) exit;
			$r=shell_part($part_id); // Запуск
			echo $r; // Вывод результата
		} else {
			$t=explode('.',$aj);
			$module_sname=safe_sql_input($t[0]);
			$ex_sname=safe_sql_input($t[1]);
			$part_sname=safe_sql_input($t[2]);
			$module_id=getrowval("SELECT module_sname, module_id FROM main_module WHERE module_sname='$module_sname'",'module_id');
			if(empty($module_id)) exit;
			$ex_id=getrowval("SELECT ex_sname, ex_id FROM ex_module WHERE ex_sname='$ex_sname' AND ex_module=$module_id",'ex_id');
			if(empty($ex_id)) exit;
			$GLOBALS["cur_ex"]=$ex_id;
			$part_id=getrowval("SELECT part_sname, part_id, part_type, part_module, part_unsafe FROM main_part WHERE part_sname='$part_sname' AND (part_type=0 OR part_type=1) AND part_module=$module_id AND part_unsafe=1","part_id");
			if(empty($part_id)) exit;
			$r=shell_part($part_id,$ex_id); // Запуск
			echo $r; // Вывод результата
		}
	} else {
		// Вызовы частей (через метод ajax), генерируемый через part.ajax.part_sname

		$r=check_url($GLOBALS["sforward"]);	// Проверка входящих переменных (защита от подстановки)

		if(!$r) echo 'Ajax protection error';	// Если проверка не прошла успешно, выводим это сообщение

		if($r || (!empty($r) && is_array($r))) if(!empty($r["ajax"])){
			$var=explode('.',$r["ajax"]);
			unset($r["ajax"]);
			$part=$r["part"];
			unset($r["part"]);
			$GLOBALS["p_par"]=$r;
			$GLOBALS["cur_ex"]=0;
			$GLOBALS["from_ajax"]=1;
			if(!empty($p_par["cur_ex"])) $GLOBALS["cur_ex"]=$p_par["cur_ex"];
			if(!empty($p_par["cur_row"])) $GLOBALS["cur_row"]=$p_par["cur_row"];		
			
			// Запускаем выбранную часть
			if(!empty($part)){
				$GLOBALS["puid"]=get_uid($p_par); // Подгружаем хеш входящих переменных для определения кеша
				$r=shell_part($part,$GLOBALS["cur_ex"]); // Запуск
				echo $r; // Вывод результата
			}
		}
	}
}

// Отладчик Ajax вызовов

if(!empty($GLOBALS["debug_ajax"])){
	$p='';
	foreach($_POST AS $var=>$value){
		$p.=$var.'	=	'.$value.VSP2;
	}
	$g='';
	foreach($_GET AS $var=>$value){
		$g.=$var.'	=	'.$value.VSP2;
	}
	$temp=ob_get_contents();	// данные части (или ошибки, возникшие в процессе выполнения)
	e5log('Ajax',
		'URL: '.$GLOBALS["sforward"].VSP2.
		'POST'.VSP2.$p.VSP2.
		'GET'.VSP2.$g.VSP2.
		$temp
	);
}

?>