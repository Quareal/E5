<?php

if($user->super==0){include('main.php'); exit;}

if($use_titles){
	echo '<h1>Шаблоны таблиц</h1>';
	echo '<div align="right"><a href="modules">Назад к списку модулей</a></div>';
} else echo '<br>';

//=======================
//  Добавление
//=======================
if(!empty($action) && $action=='add'){
	$table_multy=0;
	$table_cansub=0;
	$table_uin=uuin();
	$bb=0;
	$db->query("INSERT INTO main_table (table_module, table_name, table_sname, table_multy, table_cansub, table_onedit, table_bold, table_extype, table_uin, table_bottom, table_top)
			VALUES (0, '$table_name', '$table_sname', $table_multy, $table_cansub,'$table_onedit', $bb, $table_extype, '$table_uin', '$table_bottom','$table_top')",3,'main_table');
	getrow($db,"SELECT LAST_INSERT_ID() as sid");
	$id=$db->Record['sid'];
	
	update_part_links($id,'table_onedit',$table_uin,$table_onedit,'table');
	update_part_links($id,'table_top',$table_uin,$table_top,'table');
	update_part_links($id,'table_bottom',$table_uin,$table_bottom,'table');
	
	$action='';
}

//=======================
//  Удаление шаблона таблицы
//=======================
if(!empty($action) && $action=='del_confirm' && !empty($id) && $smb1=='Да'){
	del_table($id,0);
	update_table_state($id);
	$action='';
}

//=======================
//  Редактирование
//=======================
if(!empty($action) && $action=='edit' && !empty($id)){
	$table_multy=0;
	$table_cansub=0;
	$db->query("UPDATE main_table SET table_name='$table_name', table_sname='$table_sname', table_multy=$table_multy, table_cansub=$table_cansub, table_onedit='$table_onedit', table_extype=$table_extype, table_bottom='$table_bottom', table_top='$table_top' WHERE table_module=0 AND table_id=$id",3,'main_table');
	update_table_state($id);
	
	$table_uin=getrowval("SELECT table_uin FROM main_table WHERE table_id=$id","table_uin");
	update_part_links($id,'table_onedit',$table_uin,$table_onedit,'table');
	update_part_links($id,'table_top',$table_uin,$table_top,'table');
	update_part_links($id,'table_bottom',$table_uin,$table_bottom,'table');
	
	$action='';	
}

//=======================
//  Удаление - подтверждение
//=======================
if(!empty($action) && !empty($id) && $action=='del'){
	getrow($db,"SELECT * FROM main_table WHERE table_id=$id",1,"main_table");
	$name=' "'.$db->Record["table_name"].'"';
	echo '<div style="padding: 10px; border: 1px solid #999999; background-color: #F9F9F9; width: 400px;"><b>Внимание!</b><br>Вы действительно хотите удалить шаблон таблицы '.$name.'?
	<form method="post" action="tbltpl?id='.$id.'&action=del_confirm">
	  <input type="submit" name="smb1" value="Да" class="button" style="width: 100px;">	  <input type="submit" name="smb1" value="Нет" class="button" style="margin-left: 50px; width: 100px;">
	</form></div><br>';
	unset($action);
}

$tbls=getall($db,"SELECT * FROM main_table WHERE table_module=0 ORDER BY table_name",2,'main_table');

//=======================
//  Вывод таблицы шаблонов
//=======================

if(!empty($tbls)){
	echo '<table id="records" cellpadding="3" cellspacing="1">';
	echo '<tr>';
	echo '<th width="30%">Таблица</th>';
	echo '<th width="30%">Столбцы</th>';
	echo '<th>Действия</th>';
	echo '</tr>';
	foreach($tbls AS $tbl){
		echo '<tr>';
		echo '<td>'.$tbl["table_name"].'</td>';
		echo '<td><a href="mod_col?id=0&amp;id2='.$tbl["table_id"].'">Редактировать</a></td>';
		echo '<td>';
		
		//echo ' <a href="tbltpl?id='.$tbl["table_id"].'&amp;action=edit_form#edit_form">Изменить</a>';
		//echo ' <a href="tbltpl?id='.$tbl["table_id"].'&amp;action=del">Удалить</a></td>';
		
		echo se('edit','tbltpl?id='.$tbl['table_id'].'&amp;action=edit_form#edit_form');
		echo se('del','tbltpl?id='.$tbl['table_id'].'&amp;action=del');
		
		echo '</td>';
		echo '</tr>';
	}
	echo '</table>';
}
echo '<br>';

//=======================
//  Форма редактирования
//=======================
if(!empty($action) && $action=='edit_form' && !empty($id)){
	echo '<a name="edit_form"></a>';
	echo '<h2>'.si('edit').' Изменить таблицу</h2>';
	echo '<form action="tbltpl" method="post">
		<input type="hidden" name="action" value="edit">
		<input type="hidden" name="id" value="'.$id.'">';
	getrow($db,"SELECT * FROM main_table WHERE table_id=$id",1,'main_table');
	foreach($db->Record AS $var=>$value)$$var=$value;
	echo '<p>Название<br><input name="table_name" type="text" value="'.$table_name.'"></p>';
	echo '<p>Уникальное спец. название на английском<br><input name="table_sname" type="text" value="'.$table_sname.'"></p>';
	
	echo '<input type="hidden" name="table_extype" value=0>';
	echo '<input type="hidden" name="table_slice" value=0>';
	echo '<h2 style="cursor: pointer" OnClick="showhide(\'addition_table\');">Дополнительные параметры</h2><div id="addition_table" style="display: none;">';
	/*echo '<p>Обработка при добавлении/редактировании строки (текущая строка - cow):<br><textarea name="table_onedit">'.$table_onedit.'</textarea></p>';
	echo '<p>Шапка таблицы (спец.кнопки и т.д.):<br><textarea name="table_top">'.$table_top.'</textarea></p>';
	echo '<p>Подвал таблицы (форма мультизагрузки и т.д.):<br><textarea name="table_bottom">'.$table_top.'</textarea></p>';*/
	
	echo '<p>Обработка при добавлении/редактировании строки:<br>';//<textarea name="table_onedit"></textarea></p>';
	ide($table_onedit,-1,0,$id,-1,'table_onedit','table_part',$GLOBALS['use_ace']);
	echo '</p>';
	echo '<p>Шапка таблицы:<br>';//<textarea name="table_top"></textarea></p>';
	ide($table_onedit,-1,0,$id,-1,'table_top','table_part',$GLOBALS['use_ace']);
	echo '</p>';
	echo '<p>Подвал таблицы:<br>';//<textarea name="table_bottom"></textarea></p>';
	ide($table_onedit,-1,0,$id,-1,'table_bottom','table_part',$GLOBALS['use_ace']);
	echo '</p>';	
	
	echo '</div>';	
	
	echo '<input class="button" type="submit" value="Сохранить"> или <a href="tbltpl">вернуться назад</a>';
	echo '</form>';
}

//=======================
//  Форма добавления
//=======================
if(empty($action) || $action!='edit_form'){
	echo '<h3 OnClick="showhide(\'table_add\');" style="cursor: pointer;">'.si('add').' Добавить таблицу</h3><div id="table_add" style="display: none;">';
	echo '<form action="tbltpl" method="post">
		<input type="hidden" name="action" value="add">';
	echo '<p>Название<br><input name="table_name" type="text" value="" OnBlur="translate2(this,table_sname);"></p>';
	echo '<p>Уникальное спец. название на английском<br><input name="table_sname" type="text" value=""></p>';
	echo '<input type="hidden" name="table_extype" value=0>';
	echo '<input type="hidden" name="table_slice" value=0>';
	echo '<h2 style="cursor: pointer" OnClick="showhide(\'addition_table\');">Дополнительные параметры</h2><div id="addition_table" style="display: none;">';
	echo '<p>Обработка при добавлении/редактировании строки:<br>';//<textarea name="table_onedit"></textarea></p>';
	ide('',-1,0,0,-1,'table_onedit','table_part',$GLOBALS['use_ace']);
	echo '</p>';
	echo '<p>Шапка таблицы:<br>';//<textarea name="table_top"></textarea></p>';
	ide('',-1,0,0,-1,'table_top','table_part',$GLOBALS['use_ace']);
	echo '</p>';
	echo '<p>Подвал таблицы:<br>';//<textarea name="table_bottom"></textarea></p>';
	ide('',-1,0,0,-1,'table_bottom','table_part',$GLOBALS['use_ace']);
	echo '</p>';
	echo '</div>';
	echo '<input class="button" type="submit" value="Добавить">';
	echo '</form></div>';
}

?>