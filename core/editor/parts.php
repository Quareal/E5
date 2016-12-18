<?php

// Чтобы обезопасить от переменных объявленных в foreach
//foreach($_POST AS $var=>$value) $$var=$value;
//foreach($_GET AS $var=>$value) $$var=$value;

if($user->super==0){include('main.php'); exit;}

global $type;

if($type==0) $pn2='функцию';
if($type==1) $pn2='отображение';
if($type==2) $pn2='компонент';
if($type==3) $pn2='форму';
if($type==0) $pn='Функции';
if($type==1) $pn='Отображения';
if($type==2) $pn='Компоненты';
if($type==3) $pn='Формы';
if($type==0) $pn3='функций';
if($type==1) $pn3='отображений';
if($type==2) $pn3='компонентов';
if($type==3) $pn3='форм';
if($use_titles){
	echo '<h1>'.$pn.'</h1>';
	echo '<div align="right"><a href="modules">Назад к списку модулей</a></div>';
} else echo '<br>';

//=======================
//  Добавление части
//=======================
if(!empty($action) && $action=='add_part'){
	reset_components_json();
	$part_uin=uuin();
	if(!empty($part_auth)){ if($user->super==1) $part_auth=0; else $part_auth=$user->id; if($user->id==0) $part_auth=-1;}
	else $part_auth=-1;
	if(empty($part_unsafe)) $part_unsafe=0; else $part_unsafe=1;
	$db->query("INSERT INTO main_part (part_name, part_sname, part_proc, part_type, part_parse, part_cat, part_about, part_uin, part_folder, part_auth, part_unsafe, part_date, part_date2)
			VALUES ('$part_name', '$part_sname', $type, 2, $part_parse, $part_cat, '$part_about', '$part_uin', '$part_folder', $part_auth, $part_unsafe, '".date('Y-m-d H:i:s')."', '".date('Y-m-d H:i:s')."')",3,'main_part');
}

//=======================
//  Добавление категории
//=======================
if(!empty($action) && $action=='add_cat'){
	$cat_uin=uuin();
	$db->query("INSERT INTO part_cat (cat_name, cat_pre, cat_after, cat_type, cat_uin)
			VALUES ('$cat_name', '$cat_pre', '$cat_after', $type, '$cat_uin')",3,'part_cat');
}


//=======================
//  Удаление части
//=======================
if(!empty($action) && $action=='del_part' && !empty($part_id) && check_form_protection_key($_GET['key'],'parts',1)){
	reset_components_json();
	del_part($part_id);
	$action='';
}

//=======================
//  Удаление категории
//=======================
if(!empty($action) && $action=='del_cat' && !empty($cat_id) && check_form_protection_key($_GET['key'],'parts',1)){
	$db->query("DELETE FROM part_cat WHERE cat_id=$cat_id",3,"part_cat");
	$prts=getall($db,"SELECT * FROM main_part WHERE part_cat=$cat_id",1,"main_part");
	if(!empty($prts)) foreach($prts AS $prt) del_part($prt["part_id"]);
	$action='';
}


//=======================
//  Редактирование части
//=======================
if(!empty($action) && $action=='edit_part' && !empty($part_id)){
	reset_components_json();
	if(!empty($part_auth)){ if($user->super==1) $part_auth=0; else if($user->id!=0) $part_auth=$user->id; else $part_auth=-1;}
	else $part_auth=-1;
	if(empty($part_unsafe)) $part_unsafe=0; else $part_unsafe=1;
	update_part_state($part_id);
	$db->query("UPDATE main_part SET 
		part_name='$part_name',
		part_sname='$part_sname',
		part_about='$part_about',
		part_parse=$part_parse,
		part_cat=$part_cat,
		part_folder='$part_folder',
		part_auth=$part_auth,
		part_unsafe=$part_unsafe
		WHERE part_id=$part_id",3,'main_part');
	$action='';
}

//=======================
//  Редактирование категории
//=======================
if(!empty($action) && $action=='edit_cat' && !empty($cat_id)){
	$db->query("UPDATE part_cat SET 
		cat_name='$cat_name',
		cat_pre='$cat_pre',
		cat_after='$cat_after'
		WHERE cat_id=$cat_id",3,'part_cat');
	$action='';
}

//=======================
//  Вывод частей
//=======================

function echo_cat($cat_id,$cat_name){
	global $type,$db;
	$t=getall($db,"SELECT * FROM main_part WHERE part_type=2 AND part_proc=$type AND part_cat=$cat_id ORDER BY part_name",1,'main_part');
	$show=true;
	if(empty($t) && $cat_id==0) $show=false;
	if($show){
		echo '<tr><td><span class="link" OnClick="showhide(\'qq'.$cat_id.'\');"><b>'.$cat_name.'</b></span></td><td>';
		if($cat_id!=0){
			echo '<a href="parts?type='.$type.'&amp;cat_id='.$cat_id.'&amp;action=edit_cat_form">Изменить</a> ';
			echo '<a href="parts?type='.$type.'&amp;cat_id='.$cat_id.'&amp;action=del_cat&amp;key='.get_form_protection_key('parts',1,0).'"  onclick="return(confirm(\'Вы уверены?\'))">Удалить</a>';
		}
		echo '&nbsp;</td></tr>';
		echo '<tr id="qq'.$cat_id.'" style="display: none;"><td colspan="2">';
		echo '<table id="records" cellpadding="3" cellspacing="1">';
	}
	if(!empty($t)){
		foreach($t AS $ct){
			echo '<tr>';
			echo '<td>'.$ct["part_name"].'</td>';
			echo '<td width="240">
				<a href="mod_part?type='.$type.'&amp;id2='.$ct["part_id"].'">Содержимое</a> 
				<a href="parts_param?type='.$type.'&amp;id2='.$ct["part_id"].'">Переменные</a> 
				<a href="parts?type='.$type.'&amp;part_id='.$ct["part_id"].'&amp;action=edit_part_form">Изменить</a>
				<a href="parts?type='.$type.'&amp;part_id='.$ct["part_id"].'&amp;action=del_part&amp;key='.get_form_protection_key('parts',1,0).'"  onclick="return(confirm(\'Вы уверены?\'))">Удалить</a>';
			echo '</td></tr>';
		}
	}
	if($show) echo '</table></td></tr>';
}

	if($use_titles) echo '<br><h2>'.$pn.'</h2>';
	echo '<table id="records" cellpadding="3" cellspacing="1">';
	echo '<tr>';
	echo '<th>Название</th>';
	echo '<th width="250">Действия</th>';
	echo '</tr>';

	$pcs=getall($db,"SELECT * FROM part_cat WHERE cat_type=$type ORDER BY cat_name",1,"part_cat");
	foreach($pcs AS $pc) echo_cat($pc["cat_id"],$pc["cat_name"]);

	echo_cat(0,"Без категории");

	echo '</table>';

$edit=false;
if(!empty($cat_id) && !empty($action) && $action=='edit_cat_form'){
	getrow($db,"SELECT * FROM part_cat WHERE cat_id=$cat_id",2,'part_cart');
	foreach($db->Record AS $var=>$value)$$var=$value;
	$edit=true;
	echo '<h3>Изменить категорию</h3>';
	echo '<form action="parts" method="post">
	<input type="hidden" name="type" value="'.$type.'">
	<input type="hidden" name="cat_id" value="'.$cat_id.'">
	<input type="hidden" name="action" value="edit_cat">';

	echo '<p>Название<br><input name="cat_name" type="text" value="'.$cat_name.'"></p>';
	echo '<p>Pre-обработчик<br>(срабатывает перед вызовом каждого из дочерних '.$pn.')<br><textarea name="cat_pre" style="height: 150px;">'.htmlspecialchars($cat_pre,ENT_QUOTES).'</textarea></p>';
	echo '<p>Post-обработчик<br>(срабатывает после вызовом каждого из дочерних '.$pn.')<br><textarea name="cat_after" style="height: 150px;">'.htmlspecialchars($cat_after,ENT_QUOTES).'</textarea></p>';
	echo '<input class="button" type="submit" value="Сохранить"> или <a href="parts?type='.$type.'">вернуться назад</a>';
	echo '</form>';
}

//==========================
//  Форма редактирования части
//==========================
if(!empty($part_id) && !empty($action) && $action=='edit_part_form'){
	getrow($db,"SELECT * FROM main_part WHERE part_id=$part_id",1,'main_part');
	foreach($db->Record AS $var=>$value)$$var=$value;
	echo '<h3>Редактировать '.$pn2.'</h3>';
	echo '<form action="parts" method="post">
		<input type="hidden" name="type" value="'.$type.'">
		<input type="hidden" name="action" value="edit_part">
		<input type="hidden" name="part_id" value="'.$part_id.'">';
	echo '<p>Название<br><input name="part_name" type="text" value="'.$part_name.'"></p>';
	echo '<p>Уникальное спец. название на английском<br><input name="part_sname" type="text" value="'.$part_sname.'"></p>';
	$add1='';$add2='';$add3='';
	if($part_parse==0) $add1=' selected';
	if($part_parse==1) $add2=' selected';
	echo '<p>Обработчик<br><select name="part_parse">
		<option value="0"'.$add1.'>Язык шаблонов</option>
		<option value="1"'.$add2.'>Язык PHP</option>
	</select></p>';

	$pcs=getall($db,"SELECT * FROM part_cat WHERE cat_type=$type ORDER BY cat_name ASC",1,"part_cat");
	echo '<p>Категория:<br><select name="part_cat">';
	if($part_cat==0) echo '<option value="0">Без категории</option>';
	if(!empty($pcs)) foreach($pcs AS $pc){
		$add='';
		if($pc["cat_id"]==$part_cat) $add=' selected';
		echo '<option value="'.$pc["cat_id"].'"'.$add.'>'.$pc["cat_name"].'</option>';
	}
	echo '</select>';
	
	echo '<p>Описание:<br><textarea style="width: 300px; height: 200px;" name="part_about">'.$part_about.'</textarea></p>';
	
	echo '<p>Директория с связными файлами (начинать с /files, должна начинаться и заканчиваться слешем, если нужно указать несколько папок - указывайте через запятую)<br><input name="part_folder" type="text" value="'.$part_folder.'"></p>';
	
	if($part_auth!=-1) $add='checked'; else $add='';
	echo '<p><input type="checkbox" class="button" name="part_auth" '.$add.'> Всегда запускать с правами текущего пользователя (если установлено, то только текущий пользователь и СуперПользователь смогут редактировать эту часть)</p>';
	
	if($part_unsafe!=0) $add='checked'; else $add='';
	echo '<p><input type="checkbox" class="button" name="part_unsafe" '.$add.'> Не проверять входящие GET переменные (для AJAX вызовов), а также разрешить вызов через ?ajax=type.part</p>';
	
	echo '<input class="button" type="submit" value="Сохранить"> или <a href="parts?type='.$type.'">вернуться назад</a>';
	echo '</form>';
	$edit=true;
}

//==========================
//  Форма добавление части
//==========================
if(!$edit) {
$pcs=getall($db,"SELECT * FROM part_cat WHERE cat_type=$type ORDER BY cat_name ASC",1,"part_cat");
if(count($pcs)>0){
	echo '<h3 style="cursor: pointer;" OnClick="showhide(\'add_form\')">+ Добавить '.$pn2.'</h3>';
	echo '<form action="parts" method="post" id="add_form" style="display: none;">
	<input type="hidden" name="type" value="'.$type.'">
	<input type="hidden" name="action" value="add_part">';

	echo '<p>Название<br><input name="part_name" type="text" value="" OnBlur="translate2(this,part_sname);"></p>';
	echo '<p>Уникальное спец. название на английском<br><input name="part_sname" type="text" value=""></p>';
	echo '<p>Обработчик<br><select name="part_parse">
		<option value="0">Язык шаблонов</option>
		<option value="1">Язык PHP</option>
	</select></p>';

	echo '<p>Категория:<br><select name="part_cat">';
	//'<option value="0">Без категории</option>';
	if(!empty($pcs)) foreach($pcs AS $pc) echo '<option value="'.$pc["cat_id"].'">'.$pc["cat_name"].'</option>';
	echo '</select>';
	
	echo '<p>Описание:<br><textarea style="width: 300px; height: 200px;" name="part_about"></textarea></p>';

	echo '<p>Директория с связными файлами (начинать с /files, должна начинаться и заканчиваться слешем, если нужно указать несколько папок - указывайте через запятую)<br><input name="part_folder" type="text" value=""></p>';
	
	echo '<p><input type="checkbox" class="button" name="part_auth"> Всегда запускать с правами текущего пользователя (если установлено, то только текущий пользователь и СуперПользователь смогут редактировать эту часть)</p>';
	
	echo '<p><input type="checkbox" class="button" name="part_unsafe"> Не проверять входящие GET переменные (для AJAX вызовов), а также разрешить вызов через ?ajax=type.part</p>';

	echo '<input class="button" type="submit" value="Добавить">';
	echo '</form>';
}

echo '<br><h3 style="cursor: pointer;" OnClick="showhide(\'add_cat\')">+ Добавить категорию</h3>';
echo '<form action="parts" method="post" id="add_cat" style="display: none;">
	<input type="hidden" name="type" value="'.$type.'">
	<input type="hidden" name="action" value="add_cat">';

	echo '<p>Название<br><input name="cat_name" type="text" value=""></p>';
	echo '<p>Pre-обработчик<br>(срабатывает перед вызовом каждого из дочерних '.$pn.')<br><textarea name="cat_pre" style="height: 150px;"></textarea></p>';
	echo '<p>Post-обработчик<br>(срабатывает после вызовом каждого из дочерних '.$pn.')<br><textarea name="cat_after" style="height: 150px;"></textarea></p>';

echo '<input class="button" type="submit" value="Добавить">';
echo '</form>';
}


?>