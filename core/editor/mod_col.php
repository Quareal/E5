<?php

global $id,$id2,$major_col;
if($id!=0){
	getrow($db,"SELECT * FROM main_module WHERE module_id=$id",1,'main_module');
	if(empty($db->Record)){ include('modules.php'); exit;}
	foreach($db->Record AS $var=>$value) $$var=$value;
}
if(!empty($id2)){
	getrow($db,"SELECT * FROM main_table WHERE table_id=$id2",1,'main_table');
	foreach($db->Record AS $var=>$value) $$var=$value;
	if(!check_mod($table_module,'edit')){include('main.php'); exit;}
} else {
	$table_id=0;$id2=0;
	if(!empty($id) && !check_mod($id,'edit') ){include('main.php'); exit;}
}
if(empty($id)) $id=0;
if(empty($id2)) $id2=0;

// Чтобы обезопасить от переменных объявленных в foreach
//foreach($_POST AS $var=>$value) $$var=$value;
//foreach($_GET AS $var=>$value) $$var=$value;

if(!empty($GLOBALS["cex".$id])) $cex=$GLOBALS["cex".$id]; else $cex=0;

if($use_titles){
	if(!empty($table_name)) echo '<h1>Переменные таблицы «'.$table_name.'»</h1>';
	else 		 echo '<h1>Переменные модуля «'.$module_name.'»</h1>';
	if(!empty($module_name)) echo '<h2 align="center">Модуль «'.$module_name.'»</h2>';
	if($id!=0){
		echo '<div align="right"><a href="mod_main?id='.$id.'">Назад к модулю «'.$module_name.'»</a></div>';
		echo '<div align="right"><a href="modules">Назад к списку модулей</a></div>';
	} else echo '<div align="right"><a href="tbltpl">Назад к шаблонам таблиц</a></div>';
} else echo '<br>';

//=======================
//  Сортировка переменных
//=======================
if(!empty($action) && $action=='sort'){
	update_module_state($id);
	update_table_state($id2);
	foreach($p AS $id3=>$pos){
		$db->query("UPDATE main_col SET col_pos=$pos WHERE col_id=$id3",3,'main_col');
	}
	$action='';
}

//=======================
//  Удаление - подтверждение
//=======================
if(!empty($action) && !empty($id3) && $action=='del'){
	getrow($db,"SELECT * FROM main_col WHERE col_id=$id3",1,"main_col");
	$name=' "'.$db->Record["col_name"].'"';
	echo '<div style="padding: 10px; border: 1px solid #999999; background-color: #F9F9F9; width: 400px;"><b>Внимание!</b><br>Вы действительно хотите удалить столбец '.$name.'?<br>После этой операции будет невозможно восстановить информацию из этой переменной.
	<form method="post" action="mod_col?id='.$id.'&id2='.$id2.'&id3='.$id3.'&action=del_confirm">
	  '.get_form_protection_key('mod_col',1,1).'
	  <input type="submit" name="smb1" value="Да" class="button" style="width: 100px;">	  <input type="submit" name="smb1" value="Нет" class="button" style="margin-left: 50px; width: 100px;">
	</form></div><br>';
	unset($action);
}

//=======================
//  Удаление переменной
//=======================
if(!empty($action) && $action=='del_confirm' && !empty($smb1) && (!empty($confirm) || (!empty($smb1) && $smb1=='Да')) && check_form_protection_key($_POST['key'],'mod_col',1)){
	update_module_state($id);
	update_table_state($id2);
	del_col($id3);
	del_cache2(1,$id3);
}

//=======================
//  Установка главного параметра
//=======================
if(!empty($action) && $action=='set_major'){
	update_module_state($id);
	update_table_state($id2);
	$db->query("UPDATE main_table SET major_col=$id3 WHERE table_id=$id2",3,'main_table');
	$major_col=$id3;
}

//=======================
//  Перестроить индекс
//=======================
if(!empty($action) && $action=='refresh'){
	remove_col_index($id3);	
	add_job_col($id3);
}

//=======================
//  Добавление переменной
//=======================
if(!empty($action) && $action=='add_col'){
	update_module_state($id);
	update_table_state($id2);
	
	if(empty($use_tpl)){
		if(empty($col_bold)) $col_bold=0; else $col_bold=1;
		if(empty($col_fastedit)) $col_fastedit=0; else $col_fastedit=1;
		if(empty($col_required)) $col_required=0; else $col_required=1;
		if(empty($col_url)) $col_url=0; else $col_url=1;
		if(empty($col_tpl)) $col_tpl=0; else $col_tpl=1;
		if(empty($col_unique)) $col_unique=0; else $col_unique=1;
		if(!empty($col_unique) && !empty($col_unique2)) $col_unique=$col_unique2;
		if(empty($module_url)) $module_url=0; else $module_url=1;
		if(empty($col_inform)) $col_inform=0; else $col_inform=1;
		if(empty($col_part)) $col_part=0;
		if(empty($col_index)) $col_index=0; else $col_index=1;
		if(empty($col_force_onshow)) $col_force_onshow=0; else $col_force_onshow=1;
		if(empty($col_order1)) $col_order=0; else{
			$col_order=$col_order2*$col_order3;
		}
		if($col_type==5 && !empty($col_linkG)) $col_link=$col_linkG;
		if($col_type==5 && empty($col_linkG)) $col_link=0;
		if(empty($col_link)) $col_link=0;
		$col_filter=0;
		if(!empty($col_filterA) && $col_type==0) $col_filter=1;
		if(!empty($col_filterB) && $col_type==1) $col_filter=$col_filterB;
		if(!empty($col_filterC) && $col_type==2) $col_filter=1;
		if(!empty($file_typesB) && $col_type==6) $file_types=$file_typesB;
		if(!empty($file_dirB) && $col_type==6) $file_dir=$file_dirB;
		if(!empty($file_totalmaxB) && $col_type==6) $file_totalmax=$file_totalmaxB;
		$col_default=safe_sql_input($col_default);
	} else {
		$tpl=getrow($db,"SELECT * FROM main_col WHERE col_id=$use_tpl AND col_module=0 AND col_table=0");
		if(!empty($tpl)){
			foreach($tpl AS $var=>$value) $$var=$value;
			if(empty($col_type)) $col_type=0;
			if(empty($col_filter)) $col_filter=0;
			if(empty($file_totalmax)) $file_totalmax=0;
			if(empty($col_link)) $col_link=0;
			if(empty($col_link2)) $col_link2=0;
			if(empty($col_link3)) $col_link3=0;
			if(empty($col_link4)) $col_link4=0;
			if(empty($col_force_onshow)) $col_force_onshow=0;
			if(empty($col_part)) $col_part=0;
			if(empty($col_fastedit)) $col_fastedit=0;
			if(empty($col_inform)) $col_inform=0;
			if(empty($col_genname)) $col_genname=0;
			if(empty($col_maxsize)) $col_maxsize=0;
			if(empty($module_type)) $module_type=0;
			if(empty($module_url)) $module_url=0;
			if(empty($col_tpl)) $col_tpl=0;
			if(empty($col_url)) $col_url=0;
			if(empty($col_required)) $col_required=0;
			if(empty($col_unique)) $col_unique=0;
			if(empty($col_paramlink)) $col_paramlink=0;
			if(empty($col_bold)) $col_bold=0;
			if(empty($col_order)) $col_order=0;
			if(empty($col_index)) $col_index=0;
			if(empty($col_target)) $col_target=0;
			if(empty($col_cat)) $col_cat=0;
			foreach(Array('col_hint','col_name','col_default','col_deflist','col_onform','col_onshow','col_oninsert') AS $var=>$value) $$value=safe_sql_input($$value);
		}
	}		
	
	$col_uin=uuin();
	if($id2>0){
		getrow($db,"SELECT MAX(col_pos) AS cid FROM main_col WHERE col_table=$id2",1,'main_col');
	} else getrow($db,"SELECT MAX(col_pos) AS cid FROM main_col WHERE col_module=$id AND col_table=0",1,'main_col');
	if(!empty($db->Record)) $col_pos=$db->Record["cid"]+1; else $col_pos=1;

	//col_module, col_table, col_name, col_sname, col_type, col_pos, col_order, col_default, col_unique, col_url, col_bold, col_required, col_link, col_link2, col_link3, module_url, module_type, file_dir, file_maxsize, file_prefix, file_types, file_totalmax, col_inform
	
	if(!empty($id2)){
		update_part_links($id2,'col_oninsert',$col_uin,$col_oninsert,'table');
		update_part_links($id2,'col_onform',$col_uin,$col_onform,'table');
		update_part_links($id2,'col_onshow',$col_uin,$col_onshow,'table');
	}

	$db->query("INSERT INTO main_col (col_module, col_table, col_name, col_sname, col_type, col_pos, col_order, col_default, col_unique, col_url, col_bold, col_fastedit, col_required, col_link, col_link2, col_link3, col_link4, module_url, module_type, file_dir, file_maxsize, file_prefix, file_types, file_totalmax, col_inform, col_part,col_onshow,col_onform,col_oninsert,col_hint,col_uin, col_deep, col_paramlink, col_speclink, col_filter, col_index, col_force_onshow, file_genname, col_deflist, col_cat, col_date, col_tpl)
			VALUES ($id, $id2, '$col_name', '$col_sname', $col_type, $col_pos, $col_order, '$col_default', $col_unique, $col_url, $col_bold, $col_fastedit, $col_required, $col_link, $col_link2, $col_link3, $col_link4, $module_url, $module_type, '$file_dir', '$file_maxsize', '$file_prefix', '$file_types', '$file_totalmax', $col_inform, $col_part,'$col_onshow','$col_onform','$col_oninsert','$col_hint','$col_uin','$col_deep', $col_paramlink, '$col_speclink', $col_filter, $col_index, $col_force_onshow, $file_genname, '$col_deflist', $col_cat, '".date('Y-m-d H:i:s')."', $col_tpl)",3,'main_col');
	getrow($db,"SELECT LAST_INSERT_ID() as sid");
	$sid=$db->Record["sid"];
	if(empty($id2)){
		update_part_links($sid,'col_oninsert',$col_uin,$col_oninsert,'col');
		update_part_links($sid,'col_onform',$col_uin,$col_onform,'col');
		update_part_links($sid,'col_onshow',$col_uin,$col_onshow,'col');
	}
	global $major_col;
	if($major_col==0){
		$major_col=$sid;
		$db->query("UPDATE main_table SET major_col=$sid WHERE table_id=$id2",3,'main_table');
	}
	if(!empty($col_part)){
		getrow($db,"SELECT * FROM main_part WHERE part_id=$col_part",1,'main_part');
		if($db->Record["part_table"]==0){
			$db->query("UPDATE main_part SET part_table=$id2 WHERE part_id=$col_part",3,'main_part');
		}
	}

	$action='';
}

//=======================
//  Редактирование переменной
//=======================
if(!empty($action) && $action=='edit_col'){
	update_module_state($id);
	update_table_state($id2);	
	if(empty($col_bold)) $col_bold=0; else $col_bold=1;
	if(empty($col_fastedit)) $col_fastedit=0; else $col_fastedit=1;
	if(empty($col_required)) $col_required=0; else $col_required=1;
	if(empty($col_url)) $col_url=0; else $col_url=1;
	if(empty($col_tpl)) $col_tpl=0; else $col_tpl=1;
	if(empty($col_unique)) $col_unique=0; else $col_unique=1;
	if(!empty($col_unique) && !empty($col_unique2)) $col_unique=$col_unique2;
	if(empty($module_url)) $module_url=0; else $module_url=1;
	if(empty($col_inform)) $col_inform=0; else $col_inform=1;
	if(empty($col_part)) $col_part=0;
	if(empty($col_index)) $col_index=0; else $col_index=1;
	if(empty($col_force_onshow)) $col_force_onshow=0; else $col_force_onshow=1;
	if(empty($col_order1)) $col_order=0; else{
		$col_order=$col_order2*$col_order3;
	}
	$col_filter=0;
	if(!empty($col_filterA) && $col_type==0) $col_filter=1;
	if(!empty($col_filterB) && $col_type==1) $col_filter=$col_filterB;
	if(!empty($col_filterC) && $col_type==2) $col_filter=1;
	if(!empty($file_typesB) && $col_type==6) $file_types=$file_typesB;
	if(!empty($file_dirB) && $col_type==6) $file_dir=$file_dirB;
	if(!empty($file_totalmaxB) && $col_type==6) $file_totalmax=$file_totalmaxB;
	if($col_type==5 && !empty($col_linkG)) $col_link=$col_linkG;
	if($col_type==5 && empty($col_linkG)) $col_link=0;
	$col_default=safe_sql_input($col_default);
	
	if($col_index!=0 && getrowval("SELECT col_id, col_index FROM main_col WHERE col_id=$id3","col_index")==0) add_job_col($id3);
	if($col_index==0 && getrowval("SELECT col_id, col_index FROM main_col WHERE col_id=$id3","col_index")==1) remove_col_index($id3);

	$col_uin=getrowval("SELECT col_id, col_uin FROM main_col WHERE col_id=$id3","col_uin");
	if(!empty($id2)){
		update_part_links($id2,'col_oninsert',$col_uin,$col_oninsert,'table');
		update_part_links($id2,'col_onform',$col_uin,$col_onform,'table');
		update_part_links($id2,'col_onshow',$col_uin,$col_onshow,'table');
	} else {
		update_part_links($id3,'col_oninsert',$col_uin,$col_oninsert,'col');
		update_part_links($id3,'col_onform',$col_uin,$col_onform,'col');
		update_part_links($id3,'col_onshow',$col_uin,$col_onshow,'col');
	}

	$db->query("UPDATE main_col SET
		col_name='$col_name',
		col_sname='$col_sname',
		col_type=$col_type,
		col_order=$col_order,
		col_default='$col_default',
		col_unique=$col_unique,
		col_url=$col_url,
		col_bold=$col_bold,
		col_fastedit=$col_fastedit,
		col_required=$col_required,
		col_link=$col_link,
		col_link2=$col_link2,
		col_link3=$col_link3,
		col_link4=$col_link4,
		col_index=$col_index,
		col_force_onshow=$col_force_onshow,
		col_part=$col_part,
		col_onform='$col_onform',
		col_onshow='$col_onshow',
		col_oninsert='$col_oninsert',
		col_deep='$col_deep',
		col_paramlink=$col_paramlink,
		col_speclink='$col_speclink',
		col_filter=$col_filter,
		col_deflist='$col_deflist',
		module_url=$module_url,
		module_type=$module_type,
		file_dir='$file_dir',
		file_maxsize='$file_maxsize',
		file_prefix='$file_prefix',
		file_types='$file_types',
		file_totalmax='$file_totalmax',
		file_genname=$file_genname,
		col_inform=$col_inform,
		col_cat=$col_cat,
		col_hint='$col_hint',
		col_date2='".date('Y-m-d H:i:s')."',
		col_tpl=$col_tpl
		WHERE col_id=$id3",3,'main_col');
	if(!empty($col_part)){
		getrow($db,"SELECT * FROM main_part WHERE part_id=$col_part",1,'main_part');
		if($db->Record["part_table"]==0){
			$db->query("UPDATE main_part SET part_table=$id2 WHERE part_id=$col_part",3,'main_part');
		}
	}
	del_cache2(1,$id3);

	$action='';
}

//=======================
//  Вывод таблицы переменных
//=======================

$c=getall($db,"SELECT * FROM main_col WHERE col_table=$id2 AND col_module=$id ORDER BY col_pos",1,'main_col');
if(!empty($c)){
	echo '<form action="mod_col" method="post">
		<input type="hidden" name="id" value="'.$id.'">
		<input type="hidden" name="id2" value="'.$id2.'">
		<input type="hidden" name="action" value="sort">';
	echo '<table id="records" cellpadding="3" cellspacing="1">';
	echo '<tr>';
	echo '<th width="30%">Переменная</th>';
	echo '<th width="20%">Тип</th>';
	echo '<th width="20%">Свойства</th>';
	echo '<th>Действия</th>';
	echo '</tr>';
	foreach($c AS $cc){
		echo '<tr>';
		echo '<td><input type="text" name="p['.$cc["col_id"].']" value="'.$cc["col_pos"].'" style="width: 30px;"> '.$cc["col_name"].' / '.$cc["col_sname"].'</td>';
		echo '<td>';
		if($cc["col_type"]==0) echo 'Значение';
		if($cc["col_type"]==1){
			$tt=$cc["col_link"];
			
			if($cc["col_link"]!=0){
				if($tt<0) {echo 'Связь с подтаблицей «'; $tt=-$tt;} else echo 'Связь с таблицей «';
				getrow($db,"SELECT * FROM main_table WHERE table_id=".$tt,1,'main_table');
				echo $db->Record["table_name"].'»';
			} else echo 'Связь с специальным набором строк';
		}
		if($cc["col_type"]==2) echo 'Логический';
		if($cc["col_type"]==3) echo 'Файл';
		if($cc["col_type"]==4) echo 'Ссылка на модуль';
		if($cc["col_type"]==5) echo 'Ссылка на пользователя';
		if($cc["col_type"]==6) echo 'Папка с файлами';
		echo '</td>';
		echo '<td>';
		if($cc["col_url"]) echo si('url');
		if($cc["col_bold"]) echo si('bold');
		if($cc["col_required"]) echo si('require');
		if($cc["col_fastedit"]) echo si('fastedit');
		if($cc["col_filter"]!=0) echo si('filter2',5,0,'Участвует в фильтрации');
		if($cc["col_unique"]) echo si('unique');
		if($cc["col_type"]==1 && $cc["col_link2"]==1) echo si('checkbox');
		if($cc["col_type"]==1 && $cc["col_link2"]==0) echo si('selectbox');
		if($cc["col_index"]==1){
			$cnt=getrowval("SELECT count(*) AS cnt FROM row_index WHERE index_col=".$cc["col_id"]." AND index_word=0","cnt");
			if(!empty($cnt)){
				$cnt2=getrowval("SELECT count(*) AS cnt2 FROM main_row WHERE row_table=".$cc["col_table"],"cnt2");
				$one=$cnt2/100;
				if(!empty($one)) $prc=floor($cnt/$one); else $prc=100;
				echo si('search2').'<span style="color: #009900;">'.(100-$prc).'%</span> <span style="color: #999999; font-size: 10px;">'.$cnt.'</span>';
			} else echo si('search');
		}
		echo '</td>';
		echo '<td>';
		if($id2!=0){
			if($major_col==$cc["col_id"]) echo si('check_on',5,0,'Главный параметр');
			else if($cc["col_type"]==0 || $cc["col_type"]==1 || $cc["col_type"]==5) echo se('check_off','mod_col?id='.$id.'&amp;id2='.$id2.'&amp;id3='.$cc["col_id"].'&amp;action=set_major','Назначить главным');//echo '<a href="mod_col?id='.$id.'&amp;id2='.$id2.'&amp;id3='.$cc["col_id"].'&amp;action=set_major">Назначить</a> ';
		}
		echo se('edit','mod_col?id='.$id.'&amp;id2='.$id2.'&amp;id3='.$cc["col_id"].'&amp;action=edit_col_form#edit');
		echo se('del','mod_col?id='.$id.'&amp;id2='.$id2.'&amp;id3='.$cc["col_id"].'&amp;action=del');
		if($cc["col_index"]==1/* && $id2!=0*/ && $id!=0) echo se('refresh','mod_col?id='.$id.'&amp;id2='.$id2.'&amp;id3='.$cc["col_id"].'&amp;action=refresh');
		echo '</tr>';
	}
	echo '</table>';
	echo '<input type="submit" value="Пересортировать" class="button"></form>';
	echo show_se();
	//echo '<div>Расшифровка свойств: B - важный, Uniq - уникальный, Url - используется для разбора URL, M - множественная выборка, O - только одно значение, FE - доступно для быстрого редактирования из таблицы</div>';
}

function show_subt($tid,$aval=0,$alt=Array(),$step='-  '){
	global $db;
	$stbl=getall($db,"SELECT * FROM table_sub WHERE sub_table1=$tid",1,"table_sub");
	$nalt=$alt; $nalt[$tid]=1;
	if(!empty($stbl)) foreach($stbl AS $stb) if(empty($alt[$stb["sub_table2"]])){
		getrow($db,"SELECT * FROM main_table WHERE table_id=".$stb["sub_table2"],1,"main_table");
		$tb=$db->Record;
		if($aval==-$tb["table_id"]) $add=' selected'; else $add='';
		echo '<option value="-'.$tb["table_id"].'"'.$add.'>'.$step.$tb["table_name"].'</option>';
		show_subt($tb["table_id"],$aval,$nalt,'&nbsp;&nbsp;'.$step);
	}
}

function seek_subtables($table_id,$current=0,$space='',$included=Array(),$owners=Array()){
	global $db;
	$res='';
	if(empty($owners)) $owners[]=$table_id;
	$tbls=getall($db,"SELECT * FROM table_sub WHERE sub_table1=$table_id",1,"table_sub");
	$xn=implode('.',$owners);
	if(!empty($xn)) $xn.='.';
	foreach($tbls AS $tbl){
		getrow($db,"SELECT * FROM main_table WHERE table_id=".$tbl["sub_table2"],1,"main_table");
		$add='';
		$x=$owners;
		$x[]=$tbl["sub_table2"];
		if($current==/*$tbl["sub_table2"]*/implode('.',$x)) $add=' selected';
		$res.='<option value="'.$xn.$db->Record["table_id"].'"'.$add.'>'.$space.$db->Record["table_name"].'</option>';
		if(empty($included[$tbl["sub_table2"]])){
			$tinc=$included;
			$tinc[$tbl["sub_table2"]]=1;
			$res.=seek_subtables($tbl["sub_table2"],$current,$space.'&nbsp;&nbsp;&nbsp;',$tinc,$x);
		}
	}
	return $res;
}

//==========================
//  Форма добавления переменной
//==========================
if(empty($action) || $action!='edit_col_form'){
echo '<h3 OnClick="showhide(\'col_add\');" style="cursor: pointer;">'.si('add').' Добавить переменную</h3><div id="col_add" style="display: none;">';
echo '<form action="mod_col" method="post" onsubmit="return check_fields(\'mod_col\');">
	<input type="hidden" name="id" value="'.$id.'">
	<input type="hidden" name="id2" value="'.$id2.'">
	<input type="hidden" name="action" value="add_col">';
echo '<p>Название<br><input name="col_name" type="text" value="" OnBlur="translate2(this,col_sname);"></p>';
echo '<p>Описание/подсказка:<br><textarea name="col_hint"></textarea></p>';
echo '<p>Уникальное спец. название на английском<br><input name="col_sname" type="text" value=""></p>';
if(empty($c)) echo '<p><input type="checkbox" class="button" name="col_bold" checked> Важный (показывается в таблице)</p>';
else echo '<p><input type="checkbox" class="button" name="col_bold"> Важный (показывается в таблице)</p>';
echo '<p><input type="checkbox" class="button" name="col_required"> Обязательно к заполнению/выбору</p>';
echo '<p><input type="checkbox" class="button" name="col_inform" checked> Активное поле (участвует в добавлении)</p>';// / если поле не активное, то оно будет обнуляться при изменении строки  (чтобы оно не обнулялось используйте backrow)</p>';

$add1='';$add2='';
echo '<p>Категория:<br><SELECT name="col_cat">
	<option value="0"'.$add1.'>Общая</option><option value="-1"'.$add2.'>Дополнительная</option>
</SELECT></p>';

echo '<p><input type="checkbox" class="button" name="col_fastedit"> Доступно для быстрого редактирования прямиком из таблицы</p>';
echo '<p>Значение по умолчанию (если тип переменной "связь с таблицей", то можно указать название нужного элемента (выбирается по главному полю связной таблицы) или его ID, если можно выбрать несколько значений, то выбранные можно указать через запятую)<br><input name="col_default" type="text" value=""></p>';

echo '<p>Тип:<br><SELECT name="col_type" OnChange="
	var obj1=document.getElementById(\'type1\');
	var obj2=document.getElementById(\'type2\');
	var obj4=document.getElementById(\'type4\');
	var obj5=document.getElementById(\'type5\');
	var obj6=document.getElementById(\'type6\');
	var obj7=document.getElementById(\'type7\');
	var obj8=document.getElementById(\'type8\');
	obj1.style.display=\'none\';obj2.style.display=\'none\';obj4.style.display=\'none\';obj5.style.display=\'none\';obj6.style.display=\'none\';obj7.style.display=\'none\';obj8.style.display=\'none\';
	if(this.value==0) obj1.style.display=\'\';
	if(this.value==1) obj2.style.display=\'\';
	if(this.value==2) obj7.style.display=\'\';
	if(this.value==3) obj4.style.display=\'\';
	if(this.value==4) obj5.style.display=\'\';
	if(this.value==5) obj6.style.display=\'\';
	if(this.value==6) obj8.style.display=\'\';
	">
	<option value="0" selected>Значение</option>';
	if($id!=0) echo '<option value="1">Ссылка на таблицу</option>';
	echo '<option value="2">Логический (да/нет)</option><option value="3">Файл</option><option value="4">Ссылка на модуль</option>';
	
$grp=getall($db,"SELECT * FROM main_auth WHERE auth_type=1 AND group_module=$id ORDER BY group_name",1,"main_auth");
/*if(!empty($grp)) */echo '<option value="5">	Ссылка на пользователя</option>';

	echo '<option value="6">	Папка с файлами</option>';

echo '</SELECT></p>';

//папка
echo '<div id="type8" style="display: none;" class="subdiv">';
echo '<p>Корневой путь для папок (с обеих сторон выделять /, начинать с /files)<br><input name="file_dirB" type="text" value=""></p>';
//echo '<p>Максимальный размер папки (в килобайтах, 0 - неограничено)<br><input name="file_totalmaxB" type="text" value="0"></p>';
//echo '<p>Разрешённые типы файлов (через запятую)<br><input name="file_typesB" type="text" value=""></p>';
echo '</div>';

//группа пользователей
echo '<div id="type6" style="display: none;">';
echo '<p>Группы:<br><SELECT name="col_linkG">';
foreach($grp AS $g){
	echo '<option value="'.$g["auth_id"].'">'.$g["group_name"].'</option>';
}
echo '<option value="0">Без привязки к группе</option>';
echo '</SELECT>';
echo '</div>';

// значение
echo '<div id="type1" class="subdiv">';

echo '<p><input type="checkbox" class="button" name="col_unique" OnClick="if(this.checked) document.getElementById(\'col_unique2\').style.display=\'\'; else document.getElementById(\'col_unique2\').style.display=\'none\';"> Значение должно быть уникальным';
$add1='display: none;';
echo '&nbsp; <select name="col_unique2" id="col_unique2" class="button" style="'.$add1.'">';
$add1=' selected'; $add2='';
echo '<option value="1"'.$add1.'>в рамках текущего родителя</option>';
echo '<option value="2"'.$add2.'>в рамках экземпляра модуля</option>';
echo '</select></p>';

echo '<p style="display: none;"><input type="checkbox" class="button" name="col_order1"> Участвует в сортировке, в порядке <input type="text" class="button" name="col_order2" value="1"> по возрастанию <input type="radio" class="button" name="col_order3" value="1" checked> по убыванию <input type="radio" class="button" name="col_order3" value="-1"></p>';

echo '<p>Связано с параметром части: <select name="col_paramlink"><option value="0">не связано</option>';
echo param_select(0);
echo '</select></p>';

echo '<p><input type="checkbox" class="button" name="col_url"> Предназначается для разбора URL</p>';
echo '<input type="hidden" name="col_part" value="0">';

echo '<p><input type="checkbox" class="button" name="col_filterA"> Участвует в фильтрации</p>';

echo '<p><input type="checkbox" class="button" name="col_index"> Индексировать для быстрого поиска</p>';

echo '<p><input type="checkbox" class="button" name="col_tpl"> Содержит шаблон</p>';

echo '<p>Предустановленный список значений (каждая строчка - значение):<br><textarea name="col_deflist" style="height: 150px;"></textarea></p>';

echo '</div>';

// логический

echo '<div id="type7" style="display: none;">';

echo '<p><input type="checkbox" class="button" name="col_filterC"> Участвует в фильтрации</p>';

echo '</div>';

// ссылка
echo '<div id="type2" style="display: none" class="subdiv">';
echo '<p>Таблица:<br><SELECT name="col_link" OnChange="JavaScript: if(this.options[this.selectedIndex].value==0) document.getElementById(\'mylist\').style.display=\'\'; else document.getElementById(\'mylist\').style.display=\'none\';">';
$tbl=getall($db,"SELECT * FROM main_table WHERE table_module=$id",1,'main_table');
foreach($tbl AS $tb){
	echo '<option value="'.$tb["table_id"].'">'.$tb["table_name"].'</option>';
	show_subt($tb["table_id"]);
}
echo '<option value="0">Свой список</option>';
echo '</SELECT>';
echo '<div id="mylist" style="display: none;" class="subdiv">
	<p>Свой список (в формате шаблонизатора Е5 - результат = массив)<br><input type="text" name="col_speclink"></p>
</div>';
$help=show_help('Значение подтаблиц с префиксом "-" будут менять набор своих элементов в зависимости от текущего родителя.<br><br>
<b>Пример использования</b>:<br>
Есть три таблицы - категории, параметры и товары.<br>
Параметры и товары это подтаблицы категории. Т.е. для каждой категории можно указать свои параметры и свои товары.<br>
Далее, в настройках таблицы Товары мы указываем связь с таблицей Параметров с префиксом "-".
Теперь, при добалвении товаров какой-то конкретной категории, вам будет предложен набор параметров из подтаблицы Параметры этой же категории.<br>
Таким образом, для товаров из разных категорий можно будет указать разные наборы параметров. Однако, такие товары нельзя будет переносить или клонировать в другие категории.');
echo '<p>'.se('help','','',' OnClick="showhide(\'help1\')" style="cursor: pointer;"',0,5,0).'Подтаблицы с префиксом "-"<br>';
echo '<div id="help1" style="display: none;">'.$help.'</div>';
//echo '<em>Значение подтаблиц с префиксом "-" будут меняться в зависимости от родительского элемента. Если вы не понимаете о чём речь - игнорируйте. Область применения: разный набор критериев для категорий товаров интернет-магазина</em>';



echo '<p>Выборка:<br><SELECT name="col_link3">
	<option value="0" selected>Можно выбирать любое значение</option><option value="1">Можно выбирать только значения, не имеющие потомков</option><option value="3">Можно выбирать только значения на максимально-глубоком уровне</option><option value="2">Не предлагать элементы подтаблиц</option>
</SELECT></p>';
echo '<p>Какая подтаблица будет участвовать в выборке? (значения глубже указанной подтаблицы отображаться не будут)<br><SELECT name="col_deep"><option value="0">Все</option>';

//echo seek_subtables($table_id);
$tbls=getall($db,"SELECT * FROM main_table WHERE table_module=$id",1,"main_table");
if(!empty($tbls)) foreach($tbls AS $tbl){
	echo seek_subtables($tbl["table_id"]);
}

echo '</SELECT>';
echo '<p>Множественность:<br><SELECT name="col_link2">
	<option value="0" selected>Можно выбирать одно значение</option><option value="1">Можно выбирать несколько значений</option>
</SELECT></p>';

$add1=' selected';$add2='';$add3='';
echo '<p>Особые условия:<br><SELECT name="col_link4">
	<option value="0"'.$add1.'>Нет</option>
	<option value="1"'.$add2.'>Выбирать только те строки, которые ссылаются на текущую</option>
	<option value="2"'.$add3.'>Выбирать абсолютно все объекты данной таблицы</option>
</SELECT></p>';

echo '<p>Участие в фильтрации:<br><SELECT name="col_filterB">
	<option value="0">Не участвует</option><option value="2">Поиск по одному значению (выпадающий список)</option><option value="3">Поиск по нескольким значениям</option>
</SELECT></p>';

echo '</div>';

// файл
echo '<div id="type4" style="display: none" class="subdiv">';
if(!empty($module_sname)) $postfix=$module_sname.'/'; else $postfix='';
echo '<p>Путь хранения файла (с обеих сторон выделять /)<br><input name="file_dir" type="text" value="/files/uploads/'.$postfix.'"></p>';
echo '<p>Максимальный размер файла (в килобайтах, 0 - неограничено)<br><input name="file_maxsize" type="text" value="0"></p>';
echo '<p>Максимальный размер папки (в килобайтах, 0 - неограничено)<br><input name="file_totalmax" type="text" value="0"></p>';
echo '<p>Префикс названия<br><input name="file_prefix" type="text" value=""></p>';
echo '<p>Разрешённые типы файлов (через запятую)<br><input name="file_types" type="text" value=""></p>';
echo '<p>Тип генерирования имени:<br><SELECT name="file_genname">
	<option value="0" selected>Порядковая 1..N</option><option value="1">Случайная</option><option value="2">Брать название из имени загружаемого файла</option>
</SELECT></p>';
echo '</div>';

// ссылка на модуль
echo '<div id="type5" style="display: none" class="subdiv">';
echo '<p><input type="checkbox" class="button" name="module_url">  Автоматически выбирать часть модуля по аппендиксу URL</p>';
echo '<p>Или выбирать вручную:<br><SELECT name="module_type">
	<option value="0" selected>Только части общего вывода</option><option value="1">Только виджеты</option><option value="2">Все</option>
</SELECT></p>';
echo '</div>';


echo '<h2 onclick="showhide(\'additional\');"  style="cursor: pointer;">Дополнительные обработчики</h2>';
echo '<div id="additional" class="subdiv" style="display: none;">';
echo '<p>Компонент формы:<br>';//<textarea name="col_onform" style="height: 200px;"></textarea></p>';
//ide($col_onform,-1,$id,$id2,$id3,'col_onform','table_col',$use_ace);
ide('',-1,$id,$id2,0,'col_onform','table_col',$use_ace);
echo '</p>';
echo '<p>Компонент вывода (<input type="checkbox" class="button" name="col_force_onshow"> вызывать обработчик при каждом обращении):';//<br><textarea name="col_onshow" style="height: 200px;"></textarea></p>';
//ide($col_onshow,-1,$id,$id2,$id3,'col_onshow','table_col',$use_ace);
ide('',-1,$id,$id2,0,'col_onshow','table_col',$use_ace);
echo '</p>';
echo '<p>Обработчик при добавлении/редактировании:<br>';//<textarea name="col_oninsert" style="height: 200px;"></textarea></p>';
//ide($col_oninsert,-1,$id,$id2,$id3,'col_oninsert','table_col',$use_ace);
ide('',-1,$id,$id2,0,'col_oninsert','table_col',$use_ace);
echo '</p>';
echo '</div>';

echo '<input class="button" type="submit" value="Добавить">';
echo '</form></div>';

$cols=getall($db,"SELECT col_name, col_id FROM main_col WHERE col_module=0 AND col_table=0 ORDER BY col_pos, col_name");
if(!empty($cols)){
	echo '<h3 OnClick="showhide(\'col_tpl\');" style="cursor: pointer;">'.si('add_tpl').' Использовать шаблон</h3><div id="col_tpl" style="display: none;">';
	echo '<form action="mod_col" method="post">
		<input type="hidden" name="id" value="'.$id.'">
		<input type="hidden" name="id2" value="'.$id2.'">
		<input type="hidden" name="action" value="add_col">';
	echo '<p>Шаблон:<br><SELECT name="use_tpl">';
	foreach($cols AS $c) echo '<option value="'.$c['col_id'].'">'.$c['col_name'].'</option>';
	echo '</SELECT></p>';
	echo '<input class="button" type="submit" value="Добавить">';
	echo '</form></div>';
}

}

echo '<br>';

//==========================
//  Форма редактирования переменной
//==========================
if(!empty($action) && $action=='edit_col_form'){
echo '<a name="edit"></a><h3>'.si('edit').'Редактировать переменную</h3>';
echo '<form action="mod_col" method="post" OnSubmit="return check_fields(\'mod_col\');">
	<input type="hidden" name="id" value="'.$id.'">
	<input type="hidden" name="id2" value="'.$id2.'">
	<input type="hidden" name="id3" value="'.$id3.'">
	<input type="hidden" name="action" value="edit_col">';
getrow($db,"SELECT * FROM main_col WHERE col_id=$id3",1,'main_col');
foreach($db->Record AS $var=>$value) $$var=$value;
echo '<p>Название<br><input name="col_name" type="text" value="'.htmlspecialchars($col_name).'"></p>';
echo '<p>Описание/подсказка:<br><textarea name="col_hint">'.htmlspecialchars($col_hint).'</textarea></p>';
echo '<p>Уникальное спец. название на английском<br><input name="col_sname" type="text" value="'.$col_sname.'"></p>';
if($col_bold) $add=' checked'; else $add='';
echo '<p><input type="checkbox" class="button" name="col_bold"'.$add.'> Важный (показывается в таблице)</p>';
if($col_required) $add=' checked'; else $add='';
echo '<p><input type="checkbox" class="button" name="col_required"'.$add.'> Обязательно к заполнению/выбору</p>';
if($col_inform) $add=' checked'; else $add='';
echo '<p><input type="checkbox" class="button" name="col_inform"'.$add.'> Активное поле (участвует в добавлении)</p>';// / если поле не активное, то оно будет обнуляться при изменении строки (чтобы оно не обнулялось используйте backrow)</p>';

$add1='';$add2='';
if(empty($col_cat)) $add1=' selected';
if($col_cat==-1) $add2=' selected';
echo '<p>Категория:<br><SELECT name="col_cat">
	<option value="0"'.$add1.'>Общая</option><option value="-1"'.$add2.'>Дополнительная</option>
</SELECT></p>';

if($col_fastedit) $add=' checked'; else $add='';
echo '<p><input type="checkbox" class="button" name="col_fastedit"'.$add.'> Доступно для быстрого редактирования прямиком из таблицы</p>';
echo '<p>Значение по умолчанию (если тип переменной "связь с таблицей", то можно указать название нужного элемента (выбирается по главному полю связной таблицы) или его ID, если можно выбрать несколько значений, то выбранные можно указать через запятую)<br><input name="col_default" type="text" value="'.htmlspecialchars($col_default).'"></p>';

$sel1='';$sel2='';$sel3='';$sel4='';$sel5='';
if($col_type==0) $sel1=' selected';
if($col_type==1) $sel2=' selected';
if($col_type==2) $sel3=' selected';
if($col_type==3) $sel4=' selected';
if($col_type==4) $sel5=' selected';
if($col_type==5) $sel6=' selected';
if($col_type==6) $sel7=' selected';
$inc=3;
if($id==0) $inc--;
echo '<p>Тип:<br><SELECT name="col_type" OnChange="
	var obj1=document.getElementById(\'type1\');
	var obj2=document.getElementById(\'type2\');
	var obj4=document.getElementById(\'type4\');
	var obj5=document.getElementById(\'type5\');
	var obj6=document.getElementById(\'type6\');
	var obj7=document.getElementById(\'type7\');
	var obj8=document.getElementById(\'type8\');
	obj1.style.display=\'none\';obj2.style.display=\'none\';obj4.style.display=\'none\';obj5.style.display=\'none\';obj6.style.display=\'none\';obj7.style.display=\'none\';obj8.style.display=\'none\';
	if(this.value==0) obj1.style.display=\'\';
	if(this.value==1) obj2.style.display=\'\';
	if(this.value==2) obj7.style.display=\'\';
	if(this.value==3) obj4.style.display=\'\';
	if(this.value==4) obj5.style.display=\'\';
	if(this.value==5) obj6.style.display=\'\';
	if(this.value==6) obj8.style.display=\'\';
	">';
	$l2='<option value="1"'.$sel2.'>Ссылка на таблицу</option>';
	if($id==0) $l2='';
	if($col_type==3 || $user->super==1) echo '<option value="0"'.$sel1.'>Значение</option>'.$l2.'<option value="2"'.$sel3.'>Логический (да/нет)</option><option value="3"'.$sel4.'>Файл</option><option value="4"'.$sel5.'>Ссылка на модуль</option>';
	else echo '<option value="0"'.$sel1.'>Значение</option>'.$l2.'<option value="2"'.$sel3.'>Логический (да/нет)</option><option value="4"'.$sel5.'>Ссылка на модуль</option>';

	$grp=getall($db,"SELECT * FROM main_auth WHERE auth_type=1 AND group_module=$id ORDER BY group_name",1,"main_auth");
	/*if(!empty($grp))*/ echo '<option value="5"'.$sel6.'>	Ссылка на пользователя</option>';
	
	if($col_type==6 || $user->super==1) echo '<option value="6"'.$sel7.'>Папка с файлами</option>';

echo '</SELECT></p>';

//папка

if($col_type!=6) $add=' style="display: none;"'; else $add='';
echo '<div id="type8"'.$add.' class="subdiv">';
echo '<p>Корневой путь для папок (с обеих сторон выделять /, начинать с /files)<br><input name="file_dirB" type="text" value="'.$file_dir.'"></p>';
//echo '<p>Максимальный размер папки (в килобайтах, 0 - неограничено)<br><input name="file_totalmaxB" type="text" value="'.$file_totalmax.'"></p>';
//echo '<p>Разрешённые типы файлов (через запятую)<br><input name="file_typesB" type="text" value="'.$file_types.'"></p>';
echo '</div>';

//группа пользователей
if($col_type==5) echo '<div id="type6" class="subdiv">';
else echo '<div id="type6" style="display: none;" class="subdiv">';
echo '<p>Группы:<br><SELECT name="col_linkG">';
foreach($grp AS $g){
	$add='';
	if($col_link==$g["auth_id"]) $add=' selected';
	echo '<option value="'.$g["auth_id"].'"'.$add.'>'.$g["group_name"].'</option>';
}
if(empty($col_link)) $add=' selected'; else $add='';
echo '<option value="0"'.$add.'>Без привязки к группе</option>';
echo '</SELECT>';
echo '</div>';

// значение
if($col_type!=0) $add=' style="display: none;"'; else $add='';
echo '<div id="type1"'.$add.' class="subdiv">';
if($col_unique) $add=' checked'; else $add='';
//echo '<p><input type="checkbox" class="button" name="col_unique"'.$add.'> Значение должно быть уникальным</p>';

echo '<p><input type="checkbox" class="button" name="col_unique" OnClick="if(this.checked) document.getElementById(\'col_unique2\').style.display=\'\'; else document.getElementById(\'col_unique2\').style.display=\'none\';"'.$add.'> Значение должно быть уникальным';
$add1='display: none;'; if($col_unique) $add1='';
echo '&nbsp; <select name="col_unique2" id="col_unique2" class="button" style="'.$add1.'">';
$add1=''; $add2='';
if($col_unique==1) $add1=' selected';
if($col_unique==2) $add2=' selected';
echo '<option value="1"'.$add1.'>в рамках текущего родителя</option>';
echo '<option value="2"'.$add2.'>в рамках экземпляра модуля</option>';
echo '</select></p>';


if($col_order!=0) $add=' checked'; else $add='';
if($col_order<0) $col_order2=$col_order*-1; else $col_order2=$col_order;
$add2='';$add3='';
if($col_order>0) $add2=' checked'; else $add3=' checked';
echo '<p style="display: none;"><input type="checkbox" class="button" name="col_order1"'.$add.'> Участвует в сортировке, в порядке <input type="text" class="button" name="col_order2" value="'.$col_order2.'"> по возрастанию <input type="radio" class="button" name="col_order3" value="1"'.$add2.'> по убыванию <input type="radio" class="button" name="col_order3" value="-1"'.$add3.'></p>';

echo '<p>Связано с параметром части: <select name="col_paramlink"><option value="0">не связано</option>';
echo param_select($col_paramlink);
echo '</select></p>';

if($col_url) $add=' checked'; else $add='';
echo '<p><input type="checkbox" class="button" name="col_url"'.$add.'> Предназначается для разбора URL</p>';
echo '<input type="hidden" name="col_part" value="0">';

if($col_filter==1) $add=' checked'; else $add='';
echo '<p><input type="checkbox" class="button" name="col_filterA"'.$add.'> Участвует в фильтрации</p>';

if($col_index==1) $add=' checked'; else $add='';
echo '<p><input type="checkbox" class="button" name="col_index"'.$add.'> Индексировать для быстрого поиска</p>';

if($col_tpl==1) $add=' checked'; else $add='';
echo '<p><input type="checkbox" class="button" name="col_tpl"'.$add.'> Содержит шаблон</p>';

echo '<p>Предустановленный список значений (каждая строчка - значение):<br><textarea name="col_deflist" style="height: 150px;">'.$col_deflist.'</textarea></p>';

echo '</div>';

//логический
if($col_type!=2) $add=' style="display: none;"'; else $add='';
echo '<div id="type7"'.$add.' class="subdiv">';

if($col_filter==1) $add=' checked'; else $add='';
echo '<p><input type="checkbox" class="button" name="col_filterC"'.$add.'> Участвует в фильтрации</p>';

echo '</div>';


// ссылка
if($col_type!=1) $add=' style="display: none;"'; else $add='';
echo '<div id="type2"'.$add.' class="subdiv">';
echo '<p>Таблица:<br><SELECT name="col_link" OnChange="JavaScript: if(this.options[this.selectedIndex].value==0) document.getElementById(\'mylist\').style.display=\'\'; else document.getElementById(\'mylist\').style.display=\'none\';">';
$tbl=getall($db,"SELECT * FROM main_table WHERE table_module=$id",1,'main_table');
foreach($tbl AS $tb){
	if($tb["table_id"]==$col_link) $add=' selected'; else $add='';
	echo '<option value="'.$tb["table_id"].'"'.$add.'>'.$tb["table_name"].'</option>';
	show_subt($tb["table_id"],$col_link);
}
if($col_link==0 && $col_speclink!='')	echo '<option value="0" selected>Свой список</option>';
else	 			echo '<option value="0">Свой список</option>';
echo '</SELECT>';
if($col_link!=0 || $col_speclink=='') echo '<div id="mylist" style="display: none;" class="subdiv">'; else echo '<div id="mylist" class="subdiv">';
echo '<p>Свой список (в формате шаблонизатора Е5 - результат = массив)<br><input type="text" name="col_speclink" value="'.htmlspecialchars($col_speclink).'"></p>
</div>';
$help=show_help('Значение подтаблиц с префиксом "-" будут менять набор своих элементов в зависимости от текущего родителя.<br><br>
<b>Пример использования</b>:<br>
Есть три таблицы - категории, параметры и товары.<br>
Параметры и товары это подтаблицы категории. Т.е. для каждой категории можно указать свои параметры и свои товары.<br>
Далее, в настройках таблицы Товары мы указываем связь с таблицей Параметров с префиксом "-".
Теперь, при добалвении товаров какой-то конкретной категории, вам будет предложен набор параметров из подтаблицы Параметры этой же категории.<br>
Таким образом, для товаров из разных категорий можно будет указать разные наборы параметров. Однако, такие товары нельзя будет переносить или клонировать в другие категории.');
echo '<p>'.se('help','','',' OnClick="showhide(\'help1\')" style="cursor: pointer;"',0,5,0).'Подтаблицы с префиксом "-"<br>';
echo '<div id="help1" style="display: none;">'.$help.'</div>';
//echo $help.' Префикс "-"';
//echo '<em>Значение подтаблиц с префиксом "-" будут меняться в зависимости от родительского элемента. Если вы не понимаете о чём речь - игнорируйте. Область применения: разный набор критериев для категорий товаров интернет-магазина</em>';

$add1='';$add2='';$add3='';$add4='';
if($col_link3==0) $add1=' selected';
if($col_link3==1) $add2=' selected';
if($col_link3==2) $add3=' selected';
if($col_link3==3) $add4=' selected';
echo '<p>Выборка:<br><SELECT name="col_link3">
	<option value="0"'.$add1.'>Можно выбирать любое значение</option><option value="1"'.$add2.'>Можно выбирать только значения, не имеющие потомков</option><option value="3"'.$add4.'>Можно выбирать только значения на максимально-глубоком уровне</option><option value="2"'.$add3.'>Не предлагать элементы подтаблиц</option>
</SELECT></p>';

echo '<p>Какая подтаблица будет участвовать в выборке? (значения глубже указанной подтаблицы отображаться не будут):<br><SELECT name="col_deep"><option value="0">Все</option>';
$tbls=getall($db,"SELECT * FROM main_table WHERE table_module=$id",1,"main_table");
if(!empty($tbls)) foreach($tbls AS $tbl){
	echo seek_subtables($tbl["table_id"],$col_deep);
}
echo '</SELECT>';

$add1='';$add2='';
if($col_link2==0) $add1=' selected';
if($col_link2==1) $add2=' selected';
echo '<p>Множественность:<br><SELECT name="col_link2">
	<option value="0"'.$add1.'>Можно выбирать одно значение</option><option value="1"'.$add2.'>Можно выбирать несколько значений</option>
</SELECT></p>';

$add1='';$add2='';$add3='';
if($col_link4==0) $add1=' selected';
if($col_link4==1) $add2=' selected';
if($col_link4==2) $add3=' selected';
echo '<p>Особые условия:<br><SELECT name="col_link4">
	<option value="0"'.$add1.'>Нет</option>
	<option value="1"'.$add2.'>Выбирать только те строки, которые ссылаются на текущую</option>
	<option value="2"'.$add3.'>Выбирать абсолютно все объекты данной таблицы</option>
</SELECT></p>';

$add1='';$add2='';
if($col_filter==2) $add1=' selected';
if($col_filter==3) $add2=' selected';
echo '<p>Участие в фильтрации:<br><SELECT name="col_filterB">
	<option value="0">Не участвует</option><option value="2"'.$add1.'>Поиск по одному значению (выпадающий список)</option><option value="3"'.$add2.'>Поиск по нескольким значениям</option>
</SELECT></p>';

echo '</div>';

// файл
if($col_type!=3) $add=' style="display: none;"'; else $add='';
echo '<div id="type4"'.$add.' class="subdiv">';
echo '<p>Путь хранения файла (с обеих сторон выделять /)<br><input name="file_dir" type="text" value="'.$file_dir.'"></p>';
echo '<p>Максимальный размер файла (в килобайтах, 0 - неограничено)<br><input name="file_maxsize" type="text" value="'.$file_maxsize.'"></p>';
echo '<p>Максимальный размер папки (в килобайтах, 0 - неограничено)<br><input name="file_totalmax" type="text" value="'.$file_totalmax.'"></p>';
echo '<p>Префикс названия<br><input name="file_prefix" type="text" value="'.$file_prefix.'"></p>';
echo '<p>Разрешённые типы файлов (через запятую)<br><input name="file_types" type="text" value="'.$file_types.'"></p>';
$add1='';$add2='';$add3='';
if($file_genname==0) $add1=' selected';
if($file_genname==1) $add2=' selected';
if($file_genname==2) $add3=' selected';
echo '<p>Тип генерирования имени:<br><SELECT name="file_genname">
	<option value="0"'.$add1.'>Порядковая 1..N</option><option value="1"'.$add2.'>Случайная (uid)</option><option value="2"'.$add3.'>Брать название из имени загружаемого файла</option>
</SELECT></p>';
echo '</div>';

// ссылка на модуль
if($col_type!=4) $add=' style="display: none;"'; else $add='';
echo '<div id="type5"'.$add.' class="subdiv">';
if($module_url) $add=' checked'; else $add='';
echo '<p><input type="checkbox" class="button" name="module_url"'.$add.'> Автоматически выбирать часть модуля по аппендиксу URL</p>';
$add1='';$add2='';$add3='';
if($module_type==0) $add1=' selected';
if($module_type==1) $add2=' selected';
if($module_type==2) $add3=' selected';
echo '<p>Или выбирать вручную:<br><SELECT name="module_type">
	<option value="0"'.$add1.'>Только части общего вывода</option><option value="1"'.$add2.'>Только виджеты</option><option value="2"'.$add3.'>Все</option>
</SELECT></p>';
echo '</div>';

echo '<h2 onclick="showhide(\'additional\');"  style="cursor: pointer;">Дополнительные обработчики</h2>';
echo '<div id="additional" class="subdiv"';
if(empty($col_onshow) && empty($col_onform) && empty($col_oninsert)) echo ' style="display: none;"';
echo '>';
echo '<p>Компонент формы:<br>';
//<textarea name="col_onform" style="height: 200px;">'.$col_onform.'</textarea>
global $use_ace;
ide($col_onform,-1,$id,$id2,$id3,'col_onform','table_col',$use_ace);
echo '</p>';
$add='';
if($col_force_onshow) $add=' checked';
echo '<p>Компонент вывода (<input type="checkbox" class="button" name="col_force_onshow"'.$add.'> вызывать обработчик при каждом обращении):<br>';
//<textarea name="col_onshow" style="height: 200px;">'.$col_onshow.'</textarea>'
ide($col_onshow,-1,$id,$id2,$id3,'col_onshow','table_col',$use_ace);
echo '</p>';
echo '<p>Обработчик при добавлении/редактировании:<br>';
//<textarea name="col_oninsert" style="height: 200px;">'.$col_oninsert.'</textarea>
ide($col_oninsert,-1,$id,$id2,$id3,'col_oninsert','table_col',$use_ace);
echo '</p>';
echo '</div>';

echo '<input class="button" type="submit" value="Сохранить"> или <a href="mod_col?id='.$id.'&amp;id2='.$id2.'">вернуться назад</a>';
echo '</form>';
}

?>