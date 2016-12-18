<?php

global $id,$id2,$id3,$type;
if(isset($type) && $user->super==0){include('main.php'); exit;}
if(isset($id2) && isset($id) && !check_mod($id,'edit')){include('main.php'); exit;}
if(empty($id2)){include('main.php'); exit;}
getrow($db,"SELECT * FROM main_part WHERE part_id=$id2");
if(!empty($db->Record)) foreach($db->Record AS $var=>$value) $$var=$value;
if($use_titles) echo '<h1>'.$part_name.' — Переменные</h1>';
if(isset($type)){
	if($type==0) $pn2='функций';
	if($type==1) $pn2='отображений';
	if($type==2) $pn2='компонентов';
	if($type==3) $pn2='форм';
	if($use_titles){
		echo '<div align="right"><a href="parts?type='.$type.'">Назад к списку '.$pn2.'</a></div>';
		echo '<br><div align="right"><a href="mod_part?type='.$type.'&amp;id2='.$id2.'">Содержание</a></div>';
	}
} else {
	getrow($db,"SELECT * FROM main_module WHERE module_id=$id");
	if(!empty($db->Record)) foreach($db->Record AS $var=>$value) $$var=$value;
	if($use_titles){
		echo '<div align="right"><a href="mod_main?id='.$id.'">Назад к модулю '.$module_name.'</a></div>';
		echo '<br><div align="right"><a href="mod_part?id='.$id.'&amp;id2='.$id2.'">Содержание</a></div>';
	}
}
echo '<br>';

//=======================
//  Добавление
//=======================
if(!empty($action) && $action=='add'){
	if(!isset($type)) update_module_state($module_id);
	if(isset($type)) reset_components_json();
	update_part_state($id2);
	if(!empty($param_array)) $param_array=1; else $param_array=0;
	if(!empty($param_get)) $param_get=1; else $param_get=0;
	if(!empty($param_hide)) $param_hide=1; else $param_hide=0;
	if(empty($param_link)) $param_link=0;
	$param_uin=uuin();	
	$db->query("INSERT INTO part_param (param_part, param_name, param_sname, param_default, param_array, param_get, param_type, param_list, param_link, param_uin, param_hide)
			VALUES ($id2, '$param_name', '$param_sname', '$param_default', $param_array, $param_get, $param_type, '$param_list', $param_link, '$param_uin', $param_hide)",3,'part_param');
	$action='';
}

//=======================
//  Сделать главным
//=======================
if(!empty($action) && $action=='major'){
	if(!isset($type)) update_module_state($module_id);
	if(isset($type)) reset_components_json();
	update_part_state($id2);
	$db->query("UPDATE main_part SET part_major=$id3 WHERE part_id=$id2");
	$part_major=$id3;
	$action='';
}

//=======================
//  Удаление
//=======================
if(!empty($action) && $action=='del' && !empty($id3)){
	if(!isset($type)) update_module_state($module_id);
	if(isset($type)) reset_components_json();
	update_part_state($id2);
	$db->query("DELETE FROM part_param WHERE param_id=$id3",3,'part_param');
	$action='';
}

//=======================
//  Редактирование
//=======================
if(!empty($action) && $action=='edit' && isset($id3)){
	if(!isset($type)) update_module_state($module_id);
	if(isset($type)) reset_components_json();
	update_part_state($id2);
	if(!empty($param_array)) $param_array=1; else $param_array=0;
	if(!empty($param_get)) $param_get=1; else $param_get=0;
	if(!empty($param_hide)) $param_hide=1; else $param_hide=0;
	if(empty($param_link)) $param_link=0;
	$db->query("UPDATE part_param SET
		param_name='$param_name',
		param_sname='$param_sname',
		param_default='$param_default',
		param_array=$param_array,
		param_get=$param_get,
		param_type='$param_type',
		param_list='$param_list',
		param_link='$param_link',
		param_hide=$param_hide
	WHERE param_id=$id3",3,'part_param');
	$action='';
}


//=======================
//  Вывод таблицы параметров
//=======================

$m=getall($db,"SELECT * FROM part_param WHERE param_part=$id2 ORDER BY param_id",1,'part_param');
if(!empty($m)){
	echo '<table id="records" cellpadding="3" cellspacing="1">';
	echo '<tr>';
	echo '<th>Имя</th>';
	echo '<th>Тип</th>';
	echo '<th>Действия</th>';
	echo '</tr>';
	$ul='parts_param?';
	if(isset($type)) $ul.='type='.$type; else $ul.='id='.$id;
	$ul.='&amp;id2='.$id2.'&amp;';
	foreach($m AS $cm){
		echo '<tr>';
		echo '<td>'.$cm["param_name"].'<span style="color: #999999;">  ('.$cm["param_sname"].')</span></td>';
		echo '<td>';
		$param_type=$cm["param_type"];
		if($param_type==0) echo 'Текст/значение';
		if($param_type==1) echo 'Значение из списка';
		if($param_type==2) echo 'Значение из таблицы';
		if($param_type==4) echo 'Логический элемент';
		if($param_type==5) echo 'Строка данных';
		if($param_type==6) echo 'Строки данных';
		if($param_type==7) echo 'Столбец данных';
		if($param_type==8) echo 'Экземпляр таблицы или подтаблица';
		if($param_type==9) echo 'Группа пользователей';
		if($param_type==10) echo 'Часть модуля';
		echo '</td>';
		echo '<td>';
		if($part_major!=$cm["param_id"]) echo '<a href="'.$ul.'&amp;id3='.$cm["param_id"].'&amp;action=major">Сделать главным</a> '; else echo '<b>Главный</b> ';
		echo '	<a href="'.$ul.'&amp;id3='.$cm["param_id"].'&amp;action=edit_form">Изменить</a>
			<a href="'.$ul.'&amp;id3='.$cm["param_id"].'&amp;action=del"  onclick="return(confirm(\'Вы уверены?\'))">Удалить</a></td>';
		echo '</tr>';
	}
	echo '</table>';
}

//=======================
//  Форма редактирования
//=======================
if(!empty($action) && $action=='edit_form' && isset($id)){
	echo '<h2>Изменить переменную</h2>';
	echo '<form action="parts_param" method="post">
		<input type="hidden" name="action" value="edit">';
	if(isset($type)) echo '<input type="hidden" name="type" value="'.$type.'">';
	else echo '<input type="hidden" name="id" value="'.$id.'">';
	echo '<input type="hidden" name="id2" value="'.$id2.'">';

	getrow($db,"SELECT * FROM part_param WHERE param_id=$id3");
	if(!empty($db->Record)) foreach($db->Record AS $var=>$value) $$var=$value;

	echo '<input type="hidden" name="id3" value="'.$param_id.'">';

	echo '<p>Название<br><input name="param_name" type="text" value="'.htmlspecialchars($param_name).'"></p>';
	echo '<p>Уникальное спец. название на английском<br><input name="param_sname" type="text" value="'.$param_sname.'"></p>';
	echo '<p>Значение по умолчанию<br><input name="param_default" type="text" value="'.htmlspecialchars($param_default).'"></p>';
	if($param_get==0) $add=''; else $add=' checked';
	echo '<p><input type="checkbox" class="button" name="param_get"'.$add.'> Брать значение из GET/POST</p>';
	if($param_array==0) $add=''; else $add=' checked';
	echo '<p><input type="checkbox" class="button" name="param_array"'.$add.'> Массив значений</p>';
	if($param_hide==0) $add=''; else $add=' checked';
	echo '<p><input type="checkbox" class="button" name="param_hide"'.$add.'> Не показывать в редакторе модуля</p>';
	$add1='';$add2='';$add3='';$add4='';$add5='';$add6='';$add7='';$add8='';$add9='';$add10='';
	if($param_type==0) $add1=' selected';
	if($param_type==1) $add2=' selected';
	if($param_type==2) $add3=' selected';
	if($param_type==4) $add4=' selected';
	if($param_type==5) $add5=' selected';
	if($param_type==6) $add6=' selected';
	if($param_type==7) $add7=' selected';
	if($param_type==8) $add8=' selected';
	if($param_type==9) $add9=' selected';
	if($param_type==10) $add10=' selected';
	echo '<p>Тип:<br><select name="param_type" OnChange="
	var obj1=document.getElementById(\'t1\');
	var obj2=document.getElementById(\'t2\');
	obj1.style.display=\'none\';obj2.style.display=\'none\';
	if(this.selectedIndex==1) obj1.style.display=\'\';
	if(this.selectedIndex==2) obj2.style.display=\'\';">
		<option value="0"'.$add1.'>Текст/значение</option>
		<option value="1"'.$add2.'>Значение из списка</option>
		<option value="2"'.$add3.'>Значение из таблицы</option>
		<option value="4"'.$add4.'>Логический элемент</option>
		<option value="5"'.$add5.'>Строка данных</option>
		<option value="6"'.$add6.'>Строки данных</option>
		<option value="7"'.$add7.'>Столбец данных</option>
		<option value="8"'.$add8.'>Экземпляр таблицы или подтаблица</option>
		<option value="9"'.$add9.'>Группа пользователей</option>
		<option value="10"'.$add10.'>Часть модуля</option>
		
	</select></p>';

	$add='display: none;';
	if($param_type==1) $add='';
	echo '<div id="t1" style="'.$add.'"><p>Впишите список значений по формату: название=значение (каждому значению отдельная строка)<br>
	<textarea name="param_list" style="height: 150px;">'.$param_list.'</textarea></p></div>';

	$add='display: none;';
	if($param_type==2) $add='';
	echo '<div id="t2" style="'.$add.'"><p>Выберите таблицу для связи:<br><select name="param_link">';
	$tbl=getall($db,"SELECT * FROM main_table WHERE table_module!=0 ORDER BY table_module",1,'main_table');
	$mod2=getall($db,"SELECT * FROM main_module",1,'main_module');
	if(!empty($mod2)) foreach($mod2 AS $m2) $mod[$m2["module_id"]]=$m2["module_name"];
	foreach($tbl AS $tb)if(check_tbl($tb["table_id"],'view') && check_mod($tb["table_mod"],'view')){
		if($tb["table_id"]==$param_link) $add=' selected'; else $add='';
		echo '<option value="'.$tb["table_id"].'"'.$add.'>'.$mod[$tb["table_module"]].' — '.$tb["table_name"].'</option>';
	}
	echo '</select></p></div>';

	$ul='parts_param?';
	if(isset($type)) $ul.='type='.$type; else $ul.='id='.$id;
	$ul.='&amp;id2='.$id2;

	echo '<input class="button" type="submit" value="Сохранить"> или <a href="'.$ul.'">вернуться назад</a>';
	echo '</form>';
}

//=======================
//  Форма добавления
//=======================
if(empty($action)){
	echo '<h2>Добавить параметр</h2>';
	echo '<form action="parts_param" method="post">
		<input type="hidden" name="action" value="add">';
	if(isset($type)) echo '<input type="hidden" name="type" value="'.$type.'">';
	else echo '<input type="hidden" name="id" value="'.$id.'">';
	echo '<input type="hidden" name="id2" value="'.$id2.'">';

	echo '<p>Название<br><input name="param_name" type="text" value="" OnBlur="translate2(this,param_sname);"></p>';
	echo '<p>Уникальное спец. название на английском<br><input name="param_sname" type="text" value=""></p>';
	echo '<p>Значение по умолчанию<br><input name="param_default" type="text" value=""></p>';
	echo '<p><input type="checkbox" class="button" name="param_get"> Брать значение из GET/POST</p>';
	echo '<p><input type="checkbox" class="button" name="param_array"> Массив значений</p>';
	echo '<p><input type="checkbox" class="button" name="param_hide"> Не показывать в редакторе модуля</p>';
	echo '<p>Тип:<br><select name="param_type" OnChange="
	var obj1=document.getElementById(\'t1\');
	var obj2=document.getElementById(\'t2\');
	obj1.style.display=\'none\';obj2.style.display=\'none\';
	if(this.selectedIndex==1) obj1.style.display=\'\';
	if(this.selectedIndex==2) obj2.style.display=\'\';">
		<option value="0">Текст/значение</option>
		<option value="1">Значение из списка</option>
		<option value="2">Значение из таблицы</option>
		<option value="4">Логический элемент</option>
		<option value="5">Строка данных</option>
		<option value="6">Строки данных</option>
		<option value="7">Столбец данных</option>
		<option value="8">Экземпляр таблицы или подтаблица</option>
		<option value="9">Группа пользователей</option>
		<option value="10">Часть модуля</option>
	</select></p>';

	echo '<div id="t1" style="display: none;"><p>Впишите список значений по формату: название=значение (каждому значению отдельная строка)<br>
	<textarea name="param_list" style="height: 150px;"></textarea></p></div>';

	echo '<div id="t2" style="display: none;"><p>Выберите таблицу для связи:<br><select name="param_link">';
	$tbl=getall($db,"SELECT * FROM main_table WHERE table_module!=0 ORDER BY table_module",1,'main_table');
	$mod2=getall($db,"SELECT * FROM main_module",1,'main_module');
	if(!empty($mod2)) foreach($mod2 AS $m2) $mod[$m2["module_id"]]=$m2["module_name"];
	foreach($tbl AS $tb)if(check_tbl($tb["table_id"],'view') && check_mod($tb["table_mod"],'view')){
		echo '<option value="'.$tb["table_id"].'">'.$mod[$tb["table_module"]].' — '.$tb["table_name"].'</option>';
	}
	echo '</select></p></div>';


	echo '<input class="button" type="submit" value="Добавить">';
	echo '</form>';
}

?>