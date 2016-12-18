<?php

//=======================
//  Удаление экземпляра модуля - подтверждение
//=======================
if(!empty($action) && !empty($id2) && $action=='del_ex'){
	if(count(getall($db,"SELECT ex_name FROM ex_module WHERE ex_module=".$id))==1){
		echo '<h2>Невозможно удалить последний раздел модуля</h2>';
	} else {
		getrow($db,"SELECT * FROM ex_module WHERE ex_id=$id2",1,"ex_module");
		$name=' "'.$db->Record["ex_name"].'"';
		$buff.='<div style="margin-top: 15px; margin-bottom: -10px; padding: 10px; border: 1px solid #999999; background-color: #F9F9F9; width: 400px;"><b>Внимание!</b><br>Вы действительно хотите удалить раздел '.$name.'?<br>После этой операции будет невозможно восстановить информацию из этого раздела.
		<form method="post" action="mod_main?id='.$id.'&id2='.$id2.'&action=del_ex_confirm">
		  '.get_form_protection_key('mod_main',1,1).'
		  <input type="submit" name="smb1" value="Да" class="button" style="width: 100px;">	  <input type="submit" name="smb1" value="Нет" class="button" style="margin-left: 50px; width: 100px;">
		</form></div><br>';
		unset($action);
	}
}

//=======================
//  Удаление экземпляра модуля
//=======================
if(!empty($action) && $action=='del_ex_confirm' && (/*check_mod($id,'edit') || */check_ex(0,'del',$id)) && !empty($smb1) && (!empty($confirm) || (!empty($smb1) && $smb1=='Да') && check_form_protection_key($_POST['key'],'mod_main',1))){
	if(count(getall($db,"SELECT ex_name FROM ex_module WHERE ex_module=".$id))==1){
		echo '<h2>Невозможно удалить последний раздел модуля</h2>';
	} else {
		del_ex($id2);
		if($cex==$id2){
			$cex=-1;SetCookie('cex'.$id,0, time()+60*60*24*30,'/','',false);
		}
		reload_exs();
		$action='';
	}
}

//=======================
//  Установка текущего экземпляра
//=======================
if(!empty($action) && $action=='set_cex' && isset($id2) && (check_mod($id,'edit') || check_ex($id2,'view'))){
	$cex=$id2;
	SetCookie('cex'.$id,$id2, time()+60*60*24*30,'/','',false);
	$_COOKIE["cex".$id]=$cex;
	$action='';
}
if(!empty($action) && $action=='set_cex' && $id2==-1 && check_mod($id,'edit')){
	$cex=0;
	SetCookie('cex'.$id,-1, time()+60*60*24*30,'/','',false);
	$_COOKIE["cex".$id]=$cex;
	$action='';
}

//=======================
//  Редактирование экземпляра модуля
//=======================
if(!empty($action) && $action=='edit_ex' && !empty($id2) && (/*check_mod($id,'edit') || */check_ex($id2,'edit'))){
	//del_cache('row',$id2);
	$backup=backup_ex($id2);
	del_vals_pre($id2,0);
	//$db->query("DELETE FROM row_value WHERE value_module=$id AND value_table=0 AND value_row=$id2",3,'row_value');
	$GLOBALS["cancel"]=false;
	echo insert_values(0,$module_id,$id2);
	
	$tbls=getall($db,"SELECT * FROM main_table WHERE table_module=$id");
	$tid=''; if(!empty($tbls)) foreach($tbls AS $tbl){if(!empty($tid)) $tid.=','; $tid.=$tbl["table_id"];}
	if(!empty($tid)) $tbls2=getall($db,"SELECT * FROM table_sub WHERE sub_table1 IN ($tid)");
	$tid=''; if(!empty($tbls2)) foreach($tbls2 AS $tbl2) {if(!empty($tid)) $tid.=','; $tid.=$tbl2["sub_table2"];}
	if(!empty($tid)) $tbls3=getall($db,"SELECT * FROM main_table WHERE table_module!=$id AND table_id IN ($tid)");
	$subtable=Array();if(!empty($tbls3)) foreach($tbls3 AS $tbl3) $subtable[$tbl3["table_module"]]=$tbl3["table_name"];
	$all_subtables=get_crossmodule_subtable($id,1);
	if(!empty($all_subtables)) foreach($all_subtables AS $var=>$value){
		echo insert_values(0,$var,$id2);
	}
	if(empty($ex_public)) $ex_public=0; else $ex_public=1;
	
	if($GLOBALS["cancel"]){
		del_vals(" value_table=0 AND value_row=$id2");
		copy_vars($backup,$id2);
		$buff.='<div align="center"><h2 style="color:#FF0000;">Изменение не удалось по причине: '.$GLOBALS["cancel"].'</h2></div>';
	} else {
		//del_row($backup);
		del_vals(" value_table=0 AND value_row=$backup");
		$db->query("UPDATE ex_module SET ex_name='$ex_name', ex_sname='$ex_sname', ex_major=$ex_major, ex_public=$ex_public WHERE ex_module=$id AND ex_id=$id2",3,'ex_module');
	}
	del_cache2(0,$id2);
	$action='';
	reload_exs();
}

//=======================
//  Загрузка экземпляра с сервера
//=======================
if(!empty($action) && $action=='upload_ex'){
	include_once(DOCUMENT_ROOT.'/core/update/functions.inc');
	include_once(DOCUMENT_ROOT.'/core/update/objects.inc');
	if(empty($_POST['ex_public'])) $ex_public=0; else $ex_public=1;

	start_export();
	$old=prepend_mod_install();
	$sid=import_ex($ex_upload,'',true,$id,$ex_public,$ex_name,$ex_sname);
	after_mod_install($old);
	end_export();
		
	reload_exs();
	
}

//=======================
//  Добавление экземпляра модуля
//=======================
if(!empty($action) && $action=='add_ex'){
	$cex=add_ex($id,$ex_name,$ex_sname,$ex_major,$new_zone);
	reload_exs();
}

//=======================
//  Удаление экземпляра таблицы - подтверждение
//=======================
if(!empty($action) && !empty($id2) && $action=='del_table_ex'){
	getrow($db,"SELECT * FROM ex_table WHERE ex_id=$id2",1,"ex_table");
	$name=' "'.$db->Record["ex_name"].'"';
	$buff.='<div style="margin-top: 15px; margin-bottom: -10px; padding: 10px; border: 1px solid #999999; background-color: #F9F9F9; width: 400px;"><b>Внимание!</b><br>Вы действительно хотите удалить раздел таблицы '.$name.'?<br>После этой операции будет невозможно восстановить информацию из этого раздела.
	<form method="post" action="mod_main?id='.$id.'&id2='.$id2.'&action=del_table_ex_confirm">
	 '.get_form_protection_key('mod_main',1,1).'
	  <input type="submit" name="smb1" value="Да" class="button" style="width: 100px;">	  <input type="submit" name="smb1" value="Нет" class="button" style="margin-left: 50px; width: 100px;">
	</form></div><br>';
	unset($action);
}

//=======================
//  Удаление экземпляра таблицы
//=======================
if(!empty($action) && $action=='del_table_ex_confirm' && check_mod($id,'edit') && !empty($smb1) && (!empty($confirm) || (!empty($smb1) && $smb1=='Да')) && check_form_protection_key($_POST['key'],'mod_main',1)){
	del_table_ex($id2);
	$action='';
	reload_exs();
}

//=======================
//  Отвязка экземпляра таблицы - подтверждение
//=======================
if(!empty($action) && !empty($id2) && $action=='throw_table_ex'){
	getrow($db,"SELECT * FROM ex_table WHERE ex_id=$id2",1,"ex_table");
	$name=' "'.$db->Record["ex_name"].'"';
	$buff.='<div style="margin-top: 15px; margin-bottom: -10px; padding: 10px; border: 1px solid #999999; background-color: #F9F9F9; width: 400px;"><b>Внимание!</b><br>Вы действительно хотите отвязать раздел таблицы '.$name.'?<br>После этой операции вы сможете привязать данный раздел при создании нового экземпляра или в экземпляр, который ещё не имеет таблицы данного типа.
	<form method="post" action="mod_main?id='.$id.'&id2='.$id2.'&id3='.$id3.'&action=throw_table_ex_confirm">
	  '.get_form_protection_key('mod_main',1,1).'
	  <input type="submit" name="smb1" value="Да" class="button" style="width: 100px;">	  <input type="submit" name="smb1" value="Нет" class="button" style="margin-left: 50px; width: 100px;">
	</form></div><br>';
	unset($action);
}

//=======================
//  Отвязка экземпляра таблицы
//=======================
if(!empty($action) && $action=='throw_table_ex_confirm' && check_mod($id,'edit') && !empty($smb1) && (!empty($confirm) || (!empty($smb1) && $smb1=='Да')) && check_form_protection_key($_POST['key'],'mod_main',1)){	
	$db->query("DELETE FROM ex_group WHERE ex_ex1=$id2",3,"ex_group");
	$action='';
	reload_exs();
}

//=======================
//  Добавление экземпляра таблицы
//=======================
if(!empty($action) && $action=='add_table_ex' && !empty($id2) && check_mod($id,'edit')){
	if($_POST["erb".$id2]==0){
		$db->query("INSERT INTO ex_table (ex_table, ex_module, ex_name)
		VALUES (".$id2.", $id, '".$_POST["nex".$id2]."')",3,'ex_table');
		getrow($db,"SELECT LAST_INSERT_ID() as sid2");
		$sid2=$db->Record["sid2"];
	} else $sid2=$_POST["oex".$id2];
	if(empty($ex_public)) $ex_public=0; else $ex_public=1;
	$db->query("INSERT INTO ex_group (ex_module, ex_table, ex_ex1, ex_ex2)
	VALUES ($id, ".$id2.", $sid2, $cex)",3,'ex_group');
	$action='';
}

//=======================
//  Добавление экземпляра зоны к экземпляру модуля
//=======================
if(!empty($action) && $action=='add_exzone'){
	if(/*check_mod($id,'edit') || */check_ex($module_ex,'edit'))
	$db->query("INSERT INTO ex_zone (ex_zone,ex_module,ex_module2)
		VALUES($new_zone,$module_ex,$id)",3,"ex_zone");
}

//=======================
//  Удаление экземпляра зоны
//=======================
if(!empty($action) && $action=='del_exzone'){
	getrow($db,"SELECT * FROM ex_zone WHERE ex_id=$id2",1,"ex_zone");
	if(/*check_mod($id,'edit') || */check_ex($db->Record["ex_module"],'edit')) //именно ex_module, т.к. она обозначает экземпляр
		$db->query("DELETE FROM ex_zone WHERE ex_id=$id2",3,"ex_zone");
}

//=======================
//  Удаление файла
//=======================
if(!empty($action) && !empty($id2) && !empty($file) && $action=='delfile'){
	if(check_ex($id2,'edit')){
		getrow($db,"SELECT * FROM row_value WHERE value_row=$id2 AND value_table=0 AND value_col=$file",1,"row_value");
		if(!empty($db->Record["value_value"]) && file_exists(DOCUMENT_ROOT.$db->Record["value_value"])){
			$dbval=$db->Record["value_value"];
			del_file($dbval);
			//getrow($db,"SELECT count(*) AS cnt FROM row_value WHERE value_value='".$dbval."'",1,"row_value");
			//if($db->Record["cnt"]<=1) unlink(DOCUMENT_ROOT.$dbval);
		}
		$db->query("UPDATE row_value SET value_value='' WHERE value_row=$id2 AND value_table=0 AND value_col=$file",3,'row_value');
	}
	unset($act2);
}


?>