<?php

$mod=getall($db,"SELECT * FROM main_module ORDER BY module_name",2,'main_module');
//=======================
//  Добавление
//=======================
if(!empty($action) && $action=='add' && check_mod(0,'add')){
	if(!empty($module_major)) $module_major=1; else $module_major=0;
	$module_uin=uuin();	
	if(!empty($_FILES['module_icon'])) $module_icon='module_icon';
	if(!empty($module_icon)) $module_icon=upload_file('module_icon','/files/editor/modules/','module-icon',1);
	if(empty($module_icon)) $module_icon='';
	$db->query("INSERT INTO main_module (module_name, module_sname, module_major, module_uin, module_date2, module_icon)
			VALUES ('$module_name', '$module_sname', $module_major, '$module_uin', '".date('Y-m-d H:i:s')."', '$module_icon')",3,'main_module');
	getrow($db,"SELECT LAST_INSERT_ID() as sid");
	$sid=$db->Record["sid"];
	global $user;
	if(!$user->super){
		$db->query("INSERT INTO auth_perm (perm_target,perm_type,perm_auth,perm_object,perm_view,perm_edit)
						VALUES	(1,1,$user->id,$sid,2,2)",3,"auth_perm");
	}		

	$db->query("INSERT INTO ex_module (ex_module, ex_name, ex_sname, ex_uin)
			VALUES ($sid, 'основной','base', '".uuin()."')",3,'ex_module');
	getrow($db,"SELECT LAST_INSERT_ID() as sid2");
	$sid2=$db->Record["sid2"];
	$cex=$sid2;
	SetCookie('cex'.$sid,$sid2, time()+60*60*24*30);
	global $user;
	if(!$user->super){
		$db->query("INSERT INTO auth_perm (perm_target,perm_type,perm_auth,perm_object,perm_view,perm_edit,perm_del)
						VALUES	(1,2,$user->id,$sid,2,2,2)",3,"auth_perm");
	}		

	$action='';
	$mod=getall($db,"SELECT * FROM main_module ORDER BY module_name",2,'main_module');
}

//=======================
//  Удаление - подтверждение
//=======================
if(!empty($action) && $action=='del'){
	getrow($db,"SELECT * FROM main_module WHERE module_id=$id",1,"main_module");
	$name=' "'.$db->Record["module_name"].'"';
	echo '<div style="padding: 10px; border: 1px solid #999999; background-color: #F9F9F9; width: 400px;">';
	echo '<b>Внимание!</b><br>Вы действительно хотите удалить модуль '.$name.'?<br>После этой операции будет невозможно восстановить информацию из этого модуля.';
	echo '<form method="post" action="modules?id='.$id.'&action=del_confirm">';
	echo get_form_protection_key('modules',1,1);
	/*$groups=getall($db,"SELECT * FROM main_auth WHERE auth_type=1 AND group_module=$id");
	if(!empty($groups)){
		echo '<br><div><b>Что делать с группами пользователей модуля?</b></div>';
		foreach($groups AS $group){
			 echo '<br><div style="background-color: #FFFFFF; padding: 5px; border: 1px solid #AAAAAA; ">Группа "<b>'.$group['group_name'].'</b>"<br>';
			 echo '<br><label style="cursor: pointer;"><input type="checkbox" class="checkbox" name="del['.$group['group_id'].']" checked> Удалить</label>';
			 echo '<br><label style="cursor: pointer;"><input type="checkbox" class="checkbox" name="flush['.$group['group_id'].']"> Удалить тех пользователей, которые присутствуют только в этой группе</label>';
			 echo '</div>';
		}
		echo '<br>';
	}
	*/
	echo '<input type="submit" name="smb1" value="Да" class="button" style="width: 100px;">	  <input type="submit" name="smb1" value="Нет" class="button" style="margin-left: 50px; width: 100px;">';
	echo '</form>';
	echo '</div><br>';
	unset($action);
}

//=======================
//  Удаление модуля
//=======================
if(!empty($action) && $action=='del_confirm' && check_mod(0,'del') && !empty($smb1) && (!empty($confirm) || (!empty($smb1) && $smb1=='Да')) && check_form_protection_key($_POST['key'],'modules',1)){
	del_module($id);
	$action='';
	$mod=getall($db,"SELECT * FROM main_module ORDER BY module_name",2,'main_module');	
}

//=======================
//  Редактирование
//=======================
if(!empty($action) && $action=='edit' && !empty($id) && check_mod($id,'edit')){
	update_module_state($id);
	if(!empty($module_major)) $module_major=1; else $module_major=0;	
	$mi=getrowval("SELECT module_icon FROM main_module WHERE module_id=$id","module_icon");
	if(!empty($_FILES['module_icon']) && !empty($_FILES['module_icon']['name'])) $module_icon='module_icon';
	if(empty($module_icon)) $module_icon='';
	if(!empty($module_icon) && $mi){
		if(file_exists(DOCUMENT_ROOT.$mi)) unlink(DOCUMENT_ROOT.$mi);
	}
	if(!empty($module_icon)){
		$module_icon=upload_file('module_icon','/files/editor/modules/','module-icon',1);
	} else $module_icon=$mi;
	if(empty($module_public_ex)) $module_public_ex=0;
	$db->query("UPDATE main_module SET
				module_name='$module_name',
				module_sname='$module_sname',
				module_major=$module_major,
				module_icon='$module_icon',
				module_public_ex=$module_public_ex
			WHERE module_id=$id",3,'main_module');
	$action='';
	$mod=getall($db,"SELECT * FROM main_module ORDER BY module_name",2,'main_module');
}

//=======================
//  Вывод таблицы модулей
//=======================

echo '<table width="100%" cellpadding="0" cellspacing="0"><tr><td>';//чтобы функции не заезжали на форму добавления
if($user->super==1 && $use_crosstables) 
	echo '<table id="records" cellpadding="3" cellspacing="1"><tr>
	<td><a href="'.$GLOBALS["zone_url2"].'/parts?type=0">Функции</a></td>
	<td><a href="'.$GLOBALS["zone_url2"].'/parts?type=1">Отображения</a></td>
	<td><a href="'.$GLOBALS["zone_url2"].'/parts?type=2">Компоненты</a></td>
	<td><a href="'.$GLOBALS["zone_url2"].'/parts?type=3">Формы</a></td>
	<td><a href="'.$GLOBALS["zone_url2"].'/tbltpl">Шаблоны таблиц</a></td>
	</tr></table>'; else echo '<br>';
	
if($user->super==1) 
	echo '<div style="float: right; width: 200px;">
	<style>
		.bbox{
			margin-bottom: 10px;
			padding: 5px;
			border: 2px solid #EEEEEE;
		}
	</style>
		<div class="bbox"><a href="'.$GLOBALS["zone_url2"].'/parts?type=0">Функции</a></div>
		<div class="bbox"><a href="'.$GLOBALS["zone_url2"].'/parts?type=1">Отображения</a></div>
		<div class="bbox"><a href="'.$GLOBALS["zone_url2"].'/parts?type=2">Компоненты</a></div>
		<div class="bbox"><a href="'.$GLOBALS["zone_url2"].'/parts?type=3">Формы</a></div>
		<div class="bbox"><a href="'.$GLOBALS["zone_url2"].'/tbltpl">Шаблоны таблиц</a></div>
		<div class="bbox"><a href="'.$GLOBALS["zone_url2"].'/mod_col">Шаблоны переменных</a></div>
	</div>';

if(!empty($mod)){
	//Проверка модулей на наличие обновлений
	if(!empty($GLOBALS["update_server"])){
		$check_update=Array();
		$check_update2=Array();
		$need_update=Array();
		$cd=date('Y-m-d');
		foreach($mod AS $cm) if($cm["module_lastcheck"]!=$cd && $cm["module_lastcheck"]!='0001-01-01' && !empty($cm["module_uin"])){
			$check_update[$cm["module_uin"]]=$cm["module_uin"];
			$check_update2[$cm["module_uin"]]=$cm["module_date"];
		}
		if(!empty($check_update)){
			$uins="'".implode("','",$check_update)."'";
			$uins2=implode(',',$check_update);
			$db->query("UPDATE main_module SET module_lastcheck='".date('Y-m-d')."' WHERE module_uin IN (".$uins.")",3,"main_module");
			$mdates=explode(',',loadserv($GLOBALS["update_server"].'?type=moddates&uins='.$uins2));
			if(!empty($mdates)) foreach($mdates AS $mdate){
				$mdate=explode(';',$mdate);
				$m_uin=$mdate[0];
				if(isset($check_update[$m_uin])){
					$m_date=$mdate[1];		
					if($mdate!=$check_update2[$m_uin]) $need_update[$m_uin]=$m_uin;
				}
			}
		}
		if(!empty($need_update)){
			//echo "UPDATE main_module SET module_lastcheck='0001-01-01' WHERE module_uin IN ('".implode("','",$need_update)."')";
			$db->query("UPDATE main_module SET module_lastcheck='0001-01-01' WHERE module_uin IN ('".implode("','",$need_update)."')",3,"main_module");
		}
	}
	
	echo '<table><tr><td>';
	foreach($mod AS $cm) if(check_mod($cm["module_id"],'view') && !strstr($cm["module_sname"],'_for_uninstall')){
		echo '<div style="float: left; width: 90px; height: 85px; padding: 5px; margin: 5px; margin-right: 20px; margin-bottom: 30px;" align="center">';
		$module_icon=$cm['module_icon'];
		if(empty($module_icon)) $module_icon='/files/editor/module.png';
		echo '<div style="min-height: 64px;"><a href="'.$GLOBALS["zone_url2"].'/mod_main?id='.$cm["module_id"].'" class="ablack"><img src="'.$base_root.$module_icon.'" border="0"></a></div>';
		echo '<div style="margin-top: 5px;"><a href="'.$GLOBALS["zone_url2"].'/mod_main?id='.$cm["module_id"].'" class="ablack">'.$cm["module_name"].'</a></div>';
		$start=false;
		if(check_mod($cm["module_id"],'edit')){
			echo '<div style="margin-top: 3px;">';
			 $start=true;
 			 echo ' <a href="'.$GLOBALS["zone_url2"].'/modules?id='.$cm["module_id"].'&amp;action=edit_form#form">'.se('edit','','','',0,2,0,' height="13" ').'</a>';
 			 if($cm["module_lastcheck"]=='0001-01-01' || isset($need_update[$cm["module_uin"]]) || !empty($_GET['force'])) echo ' <a href="'.$GLOBALS["zone_url2"].'/mod_update?id='.$cm["module_id"].'&amp;action=check">'.se('update','','','',0,2,0,' height="13" ').'</a>';
		}
		if(check_mod(0,'del')){
			if(!$start) echo '<div style="margin-top: 3px;">';
			 $start=true;
			 echo ' <a href="'.$GLOBALS["zone_url2"].'/modules?id='.$cm["module_id"].'&amp;action=del">'.se('del','','','',0,2,0,' height="13" ').'</a>';
		}
		if($start) echo '</div>';
		echo '</div>';
	}
	echo '</td></tr></table><br><br>';
}

echo '</td></tr></table>';

//=======================
//  Форма редактирования
//=======================
if(!empty($action) && $action=='edit_form' && !empty($id) && check_mod($id,'edit')){
	echo '<a name="form"></a>';
	echo '<h2>'.si('edit2').'Изменить модуль</h2>';
	echo '<form action="modules" method="post" enctype="multipart/form-data">
		<input type="hidden" name="action" value="edit">
		<input type="hidden" name="id" value="'.$id.'">';
	getrow($db,"SELECT * FROM main_module WHERE module_id=$id",1,'main_module');
	foreach($db->Record AS $var=>$value)$$var=$value;
	echo '<p>Название<br><input name="module_name" type="text" value="'.$module_name.'"></p>';
	echo '<p>Уникальное спец. название на английском<br><input name="module_sname" type="text" value="'.$module_sname.'"></p>';
	if($module_icon){
		echo '<br><img src="'.$base_root.$module_icon.'">';
	}
	echo '<p>Файл иконки (64x64 png)<br><input name="module_icon" type="file" value=""></p>';
	$add='';
	if(!empty($module_major)) $add=' checked';		
	echo '<p><input type="checkbox" name="module_major" class="button"'.$add.'> Можно использовать как главный модуль для сайта</p>';
	$exs=getall($db,"SELECT * FROM ex_module WHERE ex_module=$id");
	if(!empty($exs)){
		echo '<p>Разрешать загружать этот экземпляр вместе с модулем (при обновлении с удалённого сервера):<br><select name="module_public_ex">';
		echo '<option value="0">Нет</option>';
		foreach($exs AS $ex){
			if($module_public_ex==$ex['ex_id']) $add=' selected'; else $add='';
			echo '<option value="'.$ex['ex_id'].'"'.$add.'>'.$ex['ex_name'].'</option>';
		}
		echo '</select></p>';
	}
	if(!empty($GLOBALS['update_server']) && getrowval("SELECT module_date FROM main_module WHERE module_id=$id AND module_date!='0000-00-00 00:00:00' AND module_date!=''","module_date")){
		echo '<div align="right"><a href="mod_update?id='.$module_id.'&action=check">Перейти к обновлению</a></div>';
	}
	echo '<input class="button" type="submit" value="Сохранить"> или <a href="modules">вернуться назад</a>';
	echo '</form>';
}

//=======================
//  Форма добавления
//=======================
if(empty($action) && check_mod(0,'add')){
	echo '<h3 OnClick="showhide(\'module_add\');" style="cursor: pointer; width: 180px;">'.si('add').'Добавить модуль</h3><div id="module_add" style="display: none;">';
	echo '<form action="modules" method="post" enctype="multipart/form-data">
		<input type="hidden" name="action" value="add">';
	echo '<p>Название<br><input name="module_name" type="text" value="" OnBlur="translate2(this,module_sname);"></p>';
	echo '<p>Уникальное спец. название на английском<br><input name="module_sname" type="text" value=""></p>';
	echo '<p>Файл иконки (64x64 png)<br><input name="module_icon" type="file" value=""></p>';
	echo '<p><input type="checkbox" name="module_major" class="button"> Можно использовать как главный модуль для сайта</p>';
	echo '<input class="button" type="submit" value="Добавить">';
	echo '</form></div>';
}

?>