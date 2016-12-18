<?php

if(!defined('DEF_CHMOD')) define('DEF_CHMOD',$def_chmod);
if(!defined('DEF_DRMOD')) define('DEF_DRMOD',$def_drmod);

include_once(DOCUMENT_ROOT.'/core/install/sql.inc');

header("Content-type: text/html; charset=utf-8");

$vars=Array();
$content='';
if(!file_exists(DOCUMENT_ROOT.'/core/config.inc')){
	if(!empty($_POST["db_server"])){	
		$err=false;
		if($_POST["db_type"]=="mysql"){
			$t = @mysql_connect($_POST["db_server"],$_POST["db_login"],$_POST["db_password"]);
			if(!$t || $t->connect_error) $err="Не удаётся соеденится с сервером";
			if(empty($err)) $t2=@mysql_select_db($_POST["db_database"],$t);
			if(empty($err) && $t2===false) $err="Не удаётся подсоеденится к указанной базе данных";
			if(empty($err)){
				if(@mysql_query("CREATE TABLE `tmp_tbl` (`tmp_col` int(11) NOT NULL AUTO_INCREMENT,  PRIMARY KEY (`tmp_col`)) ENGINE=MyISAM  DEFAULT CHARSET=utf8 COLLATE=utf8_general_ci",$t2)===false) $err='У пользователя нет доступа для правки базы данных. Попробуйте разрешить удалённыый доступ к базе данных для пользователя.';
				else @mysql_query("DROP TABLE `tmp_tbl`");
			}
		}
		if($_POST["db_type"]=="mysqli"){
			@$t = new mysqli($_POST["db_server"], $_POST["db_login"], $_POST["db_password"], $_POST["db_database"]);
			if(!$t || $t->connect_error) $err="Не удаётся соеденится с сервером или с базой данных (".$t->connect_error.")";
			if(empty($err)) $t2=@mysqli_select_db($_POST["db_database"],$t);
			if(empty($err) && $t2===false) $err="Не удаётся подсоеденится к указанной базе данных";
			if(empty($err)){
				if(@mysqli_query("CREATE TABLE `tmp_tbl` (`tmp_col` int(11) NOT NULL AUTO_INCREMENT,  PRIMARY KEY (`tmp_col`)) ENGINE=MyISAM  DEFAULT CHARSET=utf8 COLLATE=utf8_general_ci",$t2)===false) $err='У пользователя нет доступа для правки базы данных. Попробуйте разрешить удалённыый доступ к базе данных для пользователя.';
				else @mysqli_query("DROP TABLE `tmp_tbl`",$t2);
			}
		}
		if($_POST["db_type"]=="postgre"){
			if(!empty($_POST["db_port"])) $pname=" port=".$_POST["db_port"]; else $pname='';
			$options=" options='--client_encoding=UTF8'";
			$line="host=".$_POST["db_server"].$pname.$options." dbname=".$_POST["db_database"]." user=".$_POST["db_login"]." password=".$_POST["db_password"];
			//$t=pg_connect($_POST["db_server"], $_POST["db_port"],"--client_encoding=UTF8", $_POST["db_password"], $_POST["db_database"]);
			$t=pg_connect($line);
			if(!$t) $err="Не удаётся соеденится с сервером или с базой данных (".pg_connection_status($t).")";
		}
		
		if(!$err){
			$GLOBALS["database_type"]=$_POST["db_type"];
			$GLOBALS["server"]=$_POST["db_server"];
			$GLOBALS["username"]=$_POST["db_login"];
			$GLOBALS["password"]=$_POST["db_password"];
			$GLOBALS["database"]=$_POST["db_database"];
			$GLOBALS["port"]=$_POST["db_port"];
			if(!empty($_POST["host"])) $GLOBALS["update_server"]=$_POST["host"];
			
			$su_login='admin';
			$su_pwl='admin';
			
			save_config();
			
			//user_auth('admin','admin',1);
			
			
			if(!empty($_POST["restore"])){
				global $restore;
				$restore=1;
			}
		}		
	}
	if(empty($_POST["db_server"]) || $err){
		if(!empty($err)) $error='<div><b style="color: #FF0000;">Ошибка</b><br>'.$err.'</div><br>';
		$admin_url='admin';
		$ruri=explode('?',$_SERVER['REQUEST_URI']);
		$ruri=$ruri[0];
		if(strstr($_SERVER['REQUEST_URI'],'update') && !strstr($_SERVER['REQUEST_URI'],'update_server')) $admin_url=str_replace('/update','',$ruri);
		else if(strstr($_SERVER['REQUEST_URI'],'admin')) $admin_url=$ruri;
		if(empty($_GET["install"]) && empty($_POST["install"])){
			$vars['title']='Не найден файл конфигурации';
			//echo '<html><head><title>Не найден файл конфигурации</title><meta http-equiv="Content-Type" content="text/html; charset=utf-8"></head><style>div{padding: 5px;}</style><body style="font-family: Arial; font-size: 12px; color: #444444;">';			
			$content.='<h2>Отсутствует файл конфигурации</h2>';
			if(!empty($error)) $content.=$error;
			$content.='<div><span style="cursor: pointer; color: #0000FF;" OnClick="var obj=document.getElementById(\'conf\');obj.style.display=\'\';">Завести новый</span></div>';
			$content.='<form action="'.$admin_url.'" method="post" id="conf" style="display: none;">';//убран слеш перед admin, чтобы поддерживать возможность установки из папки
		} else {
			$vars['title']='Установка системы';
			//echo '<html><head><title>Установка системы</title><meta http-equiv="Content-Type" content="text/html; charset=utf-8"></head><style>div{padding: 5px;}</style><body style="font-family: Arial; font-size: 12px; color: #444444;">';
			$content.='<h2>Установка системы</h2>';
			if(!empty($error)) $content.=$error;
			$content.='<div>Для продолжения установки введите следующие реквизиты</div>';
			//$content.='<form action="'.$admin_url.'/update?action=check2" method="post" id="conf">';//убран слеш перед admin, чтобы поддерживать возможность установки из папки
			$content.='<form action="'.$admin_url.'" method="post" id="conf">';
			$content.='<input type="hidden" name="install" value="1">';
		}
		/*
		$content.='<div><b>Тип соединения:</b><br><select name="db_type" OnChange="';
		$content.="
			document.getElementById('c5').style.display='none';
			if(this.selectedIndex==0) v='MySQL';
			if(this.selectedIndex==1) v='MySQLi';
			if(this.selectedIndex==2){
				document.getElementById('c5').style.display='';
				v='PostgreSQL';
			}
			document.getElementById('c1').innerHTML=v;
			document.getElementById('c2').innerHTML=v;
			document.getElementById('c3').innerHTML=v;
			document.getElementById('c4').innerHTML=v;
		";
		$content.='">
			<option value="mysql" selected>MySQL</option>
			<option value="mysqli">MySQLi</option>';
		//echo '<option value="postgre">PostgreSQL</option>';
		echo '</select></div>';
		*/
		
		if(function_exists('mysqli_connect')) $content.='<input type="hidden" name="db_type" value="mysqli">';
		else $content.='<input type="hidden" name="db_type" value="mysql">';
		
		$content.='<br><div><b><span id="c1">MySQL</span> Сервер</b><br><input type="input" name="db_server" value="'.(empty($_POST['db_server'])?'localhost':$_POST['db_server']).'"></div>';
		$content.='<div><b><span id="c2">MySQL</span> Логин</b><br><input type="input" name="db_login" value="'.(empty($_POST['db_login'])?'':$_POST['db_login']).'"></div>';
		$content.='<div><b><span id="c3">MySQL</span> Пароль</b><br><input type="password" name="db_password" value="'.(empty($_POST['db_password'])?'':$_POST['db_password']).'"></div>';
		$content.='<div><b><span id="c4">MySQL</span> База данных</b><br><input type="input" name="db_database" value="'.(empty($_POST['db_database'])?'':$_POST['db_database']).'"></div>';
		$content.='<div id="c5" style="display: none;"><b>PostgreSQL порт</b><br><input type="input" name="db_port" value="'.(empty($_POST['db_port'])?'5432':$_POST['db_port']).'"></div>';
		$content.='<div><label style="cursor: pointer;"><input name="remove" type="checkbox" class="checkbox"'.(empty($_POST['remove'])?'':' checked').'> Удалить содержимое БД</label></div>';
		$content.='<div align="center">Логин администратора: <b>admin</b><br>Пароль администратора: <b>admin</b><br>Не забудьте поменять реквизиты доступа в разделе "Настройки"</div>';
		if(empty($_GET["install"]) && empty($_POST["install"])){
			$content.='<div><label style="cursor: pointer;"><input name="restore" type="checkbox" class="checkbox"'.(empty($_POST['restore'])?'':' checked').'> Воссоздать структуру базы данных</label></div>';
			$content.='<br><div><input style="cursor: pointer; padding: 5px;" type="submit" value="Проверить и сохранить"></div>';
		} else {
			$content.='<input name="restore" type="hidden" value="1">';
			if(!empty($_GET["host"])) $content.='<input name="host" type="hidden" value="'.$_GET["host"].'">';
			if(!empty($_POST["host"])) $content.='<input name="host" type="hidden" value="'.$_POST["host"].'">';
			$content.='<br><div><input style="cursor: pointer; padding: 5px;" type="submit" value="Продолжить установку"></div>';
		}
		$content.='</form>';
		
		$vars['content']=$content;
		echo shell_tpl_admin('system/startpage',$vars);
		//echo '</body></html>';
		exit;
	}
}
	
?>