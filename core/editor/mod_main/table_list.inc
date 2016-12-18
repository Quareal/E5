<?php

//=======================
//  Вывод таблиц		
//=======================

$tmp=getall($db,"SELECT * FROM main_table WHERE table_module=$id AND table_bold=1 ORDER BY table_name",1,'main_table');
$tmp=getall2($db,"SELECT * FROM main_table WHERE table_module=$id AND table_bold=0 ORDER BY table_name",$tmp,1,'main_table');
$tmp=getall2($db,"SELECT * FROM main_table WHERE table_module=$id AND table_bold=2 ORDER BY table_name",$tmp,1,'main_table');
$t=Array();
$have_hide2=false;
if(!empty($tmp)) foreach($tmp AS $tm) if(check_tbl($tm["table_id"],"view")){
	$t[]=$tm;
	if($tm["table_bold"]==2) $have_hide2=true;
}

$have_hide='';
$module_major=0;
if(!empty($t)){
	//echo '<div style="clear: both; height: 30px; margin-top: 13px;"><div style="float: left;"><h2>Таблицы данных</h2></div><div style="float: right;">';
	
	echo '<br><table cellpadding="0" cellspacing="0" width="100%"><tr><td valign="center" align="left"><h2>Таблицы данных</h2></td><td align="right" valign="center" width="310">';
	echo '<form action="mod_main" method="post" style="margin: 0px; padding: 0px;">
			<input type="hidden" name="id" value="'.$id.'">
			<input type="hidden" name="action" value="table_search">
			<input type="hidden" name="search_ex" id="search_ex" value="1">
			'.si('search').'
			<input type="text" name="query" value="" style="width: 220px;">
			<input type="button" title="по текущему разделу" class="arrow_btn" align="center" OnClick="fclick3(this,\'search_ex\');" value="E">
			<input type="submit" value=">" class="arrow_btn">
		</form>';
	echo '</td></tr></table>';

	//echo '</div></div>';
	echo '<table id="records" cellpadding="3" cellspacing="1">';
	echo '<tr>';
	echo '<th>Таблица</th>';
	echo '<th width="170">Действия</th>';
	echo '</tr>';
	$stbl=getall($db,"SELECT table_id, table_module, table_uin, table_name, table_sname, table_cansub FROM main_table WHERE table_cansub=1 ORDER BY table_module",1,'main_table');$st=Array();if(!empty($stbl)) foreach($stbl AS $value) $st[$value["table_id"]]=$value;
	$ma=getall($db,"SELECT * FROM main_module",1,'main_module');$m=Array();if(!empty($ma)) foreach($ma AS $value) $m[$value["module_id"]]=$value;
	foreach($t AS $ct){
		if($ct["table_bold"]!=2) echo '<tr>'; else{
			echo '<tr id="hh'.$ct["table_id"].'" style="display: none;">';
			$have_hide.='showhide(\'hh'.$ct["table_id"].'\');';
		}
		if($ct["table_cansub"]) $als=se('global','','Может подчиняться другим таблицам','',false); else $als='';

		if($cex==-1){
			echo '<td>'.$ct["table_name"].$als.' <span style="color: #777777;">('.$ct["table_sname"].')</span>';
			echo se('view2','','Показать все экземпляры таблицы','',false,5,0,' OnClick="document.getElementById(\'view2_t'.$ct['table_id'].'\').style.display=\'\'; this.style.display=\'none\'; showhide(\'table_exes'.$ct['table_id'].'\');" id="view1_t'.$ct['table_id'].'"','cursor: pointer; margin-top: 0px; margin-right: 11px;');
			echo se('view','','Скрыть все экземпляры таблицы','',false,5,0,' OnClick="document.getElementById(\'view1_t'.$ct['table_id'].'\').style.display=\'\'; this.style.display=\'none\'; showhide(\'table_exes'.$ct['table_id'].'\');" id="view2_t'.$ct['table_id'].'"','cursor: pointer; margin-top: 0px; display: none; margin-right: 11px;');
			echo '<div style="display: none;" id="table_exes'.$ct['table_id'].'">';
			$ex=getall($db,"SELECT * FROM ex_table WHERE ex_table=".$ct["table_id"]." ORDER BY ex_name",1,'ex_table');
			$first=true;
			foreach($ex AS $e){
				$first=false;
				$eex2=0;
				$exgs=getall($db,"SELECT * FROM ex_group WHERE ex_table=".$ct["table_id"]." AND ex_ex1=".$e["ex_id"],1,'ex_group');
				$tmp='';if(!empty($exgs)){
					foreach($exgs AS $exg) {
						$eex2=$exg["ex_ex2"];
						if(!empty($aex[$exg["ex_ex2"]]["ex_name"])){
							if(!empty($tmp)) $tmp.=', '; $tmp.=$aex[$exg["ex_ex2"]]["ex_name"];
						}
					}
					$tmp=' (для: '.$tmp.')';
				}
				if(!empty($eex2) && check_ex($eex2,'view')){
					echo '<br> - <a href="mod_table?id='.$id.'&amp;id2='.$e["ex_id"].'">'.$e["ex_name"].$tmp.'</a> ';
					if(check_mod($id,'edit')) echo '(<a href="mod_main?id='.$id.'&amp;id2='.$e["ex_id"].'&amp;action=del_table_ex">удалить</a> / <a href="mod_main?id='.$id.'&amp;id2='.$e["ex_id"].'&amp;id3='.$ct["table_id"].'&amp;action=throw_table_ex">отвязать</a>)';
				}
			}
			if($first) echo '<i><a href="#ex" OnClick="show(\'ex_add\');">Необходимо добавить раздел (либо прикрепите существующий)</a></i>';
			echo '</div>';
			echo '</td>';
		} else {
			$ex=getall($db,"SELECT * FROM ex_group WHERE ex_ex2=$cex AND ex_table=".$ct["table_id"],1,'ex_group');
			if(count($ex)==1 && !empty($ex[0]["ex_ex1"])){
				getrow($db,"SELECT * FROM ex_table WHERE ex_id=".$ex[0]["ex_ex1"],1,'ex_table');
				echo '<td><a href="mod_table?id='.$id.'&amp;id2='.$db->Record["ex_id"].'">'.$ct["table_name"].'</a>'.$als.' <span style="color: #777777;">('.$ct["table_sname"].')</span></td>';
			} else if(count($ex)>1) {
				echo '<td>'.$ct["table_name"].$als.' <span style="color: #777777;">('.$ct["table_sname"].')</span>: ';
				foreach($ex AS $e){
					getrow($db,"SELECT * FROM ex_table WHERE ex_id=".$e["ex_ex1"],1,'ex_table');
					echo ' - <a href="mod_table?id='.$id.'&amp;id2='.$db->Record["ex_id"].'">'.$db->Record["ex_name"].'</a>';
				}
				echo '</td>';
			} else if(count($ex)==0){
				if(check_mod($id,'edit')){
					echo '<td><div><b>'.$ct["table_name"].'</b>'.$als.' <span style="color: #777777;">('.$ct["table_sname"].')</span>: нет раздела для этого экз.модуля. <span class="link" OnClick="showhide(\'add_table_ex'.$ct["table_id"].'\');">Добавить?</span><br>';
					echo '<form action="mod_main" method="post" name="exadd'.$ct["table_id"].'" id="add_table_ex'.$ct["table_id"].'" style="display: none;">
					<input type="hidden" name="id" value="'.$id.'">
					<input type="hidden" name="action" value="add_table_ex">
					<input type="hidden" name="id2" value="'.$ct["table_id"].'">';
					$tb=$ct;
					$ext=getall($db,"SELECT * FROM ex_table WHERE ex_table=".$tb["table_id"],1,'ex_table');
					$sid=count($ext);
					echo '<div align="right">добавить новый <input class="button" type="text" name="nex'.$tb["table_id"].'" value="'.$tb["table_name"].' '.($sid+1).'" OnClick="var rb=document.getElementById(\'erb'.$tb["table_id"].'\');rb.checked=true;"><input type="radio" class="checkbox" id="erb'.$tb["table_id"].'" name="erb'.$tb["table_id"].'" value="0" checked>';
					$opt='';
					foreach($ext AS $ex) $opt.='<option value="'.$ex["ex_id"].'">'.$ex["ex_name"].'</option>';
					if(!empty($opt)) echo '<br> или использовать существующий: <select name="oex'.$tb["table_id"].'" class="button" OnClick="var rb=document.getElementById(\'e2rb'.$tb["table_id"].'\');rb.checked=true;">'.$opt.'</select><input type="radio" class="checkbox" id="e2rb'.$tb["table_id"].'" name="erb'.$tb["table_id"].'" value="1">';
					echo '<br><input class="button" type="submit" value="добавить"></div></form>';
					echo '</td>';
				} else echo '<td><b>'.$ct["table_name"].'</b> <span style="color: #777777;">('.$ct["table_sname"].')</span></td>';
			}
		}

		echo '<td align="right">';
		if($ct["table_bold"]!=1){
			if(check_mod($ct["table_module"],'edit')){
				// echo '<a href="mod_main?id='.$id.'&amp;id2='.$ct["table_id"].'&amp;action=major_table"  onclick="return(confirm(\'Вы уверены?\'))">Сделать основной</a> ';
				echo se('check_off','mod_main?id='.$id.'&amp;id2='.$ct["table_id"].'&amp;action=major_table','Сделать основной','',1,5,0,'onclick="return(confirm(\'Вы уверены?\'))"');
			}
		} else {
			$module_major=$ct["table_id"];
			//echo '<b>Основная</b> ';
			echo si('check_on',5,0,'Основная');
		}
		if(check_mod($ct["table_module"],'edit')) echo se('edit','mod_main?id='.$id.'&amp;id2='.$ct["table_id"].'&amp;action=edit_table_form#edit_table');//echo ' <a href="mod_main?id='.$id.'&amp;id2='.$ct["table_id"].'&amp;action=edit_table_form#edit_table">Изменить</a> ';
		if(check_mod($ct["table_module"],'edit')) echo se('sub_tables','','','',0,5,0,' OnClick="showhide(\'pan'.$ct["table_id"].'\');"','cursor: pointer;');//echo '<span class="link" OnClick="showhide(\'pan'.$ct["table_id"].'\');">Подтаблицы</span> ';
		if(check_mod($ct["table_module"],'edit')) echo se('back_table','mod_col?id='.$id.'&amp;id2='.$ct["table_id"],'Переменные таблицы');//echo '<a href="mod_col?id='.$id.'&amp;id2='.$ct["table_id"].'">Переменные</a> ';
		if(check_mod($ct["table_module"],'edit')) echo se('del','mod_main?id='.$id.'&amp;id2='.$ct["table_id"].'&amp;action=del_table');//'<a href="mod_main?id='.$id.'&amp;id2='.$ct["table_id"].'&amp;action=del_table">Удалить</a> ';
		if(check_mod($ct["table_module"],'edit')) {
			if($ct["table_bold"]!=2) echo se('view','mod_main?id='.$id.'&amp;id2='.$ct["table_id"].'&amp;action=hide_table','Скрыть таблицу','',1,5,0,'onclick="return(confirm(\'Вы уверены?\'))"');//echo ' <a href="mod_main?id='.$id.'&amp;id2='.$ct["table_id"].'&amp;action=hide_table"  onclick="return(confirm(\'Вы уверены?\'))">Скрыть</a> ';
			else echo se('view2','mod_main?id='.$id.'&amp;id2='.$ct["table_id"].'&amp;action=show_table','Показать таблицу','',1,5,0,'onclick="return(confirm(\'Вы уверены?\'))"');//echo ' <a href="mod_main?id='.$id.'&amp;id2='.$ct["table_id"].'&amp;action=show_table" onclick="return(confirm(\'Вы уверены?\'))">Показать</a> ';
		}
		if(check_mod($ct["table_module"],'edit')){
			echo '<div id="pan'.$ct["table_id"].'" align="left" style="display: none;">';
			$cts=getall($db,"SELECT * FROM table_sub WHERE sub_table1=".$ct["table_id"],1,'table_sub');
			echo '<br><div><b>Подтаблицы</b></div>';
			//if(!empty($cts)) echo '<br><div>удалить:</div>';
			$flag=Array();
			if(!empty($cts)) foreach($cts AS $value)if(isset($st[$value["sub_table2"]]["table_name"])){
				$flag[$value["sub_table2"]]=1;
				$subtable[$st[$value["sub_table2"]]["table_module"]]=$st[$value["sub_table2"]]["table_name"];
				echo '<div>- '.$st[$value["sub_table2"]]["table_name"].' (<a href="mod_main?id='.$id.'&amp;action=del_sub&amp;sub_id='.$value["sub_id"].'">удал.</a>)</div>';
			}
			$opt='';
			foreach($stbl AS $value) if(empty($flag[$value["table_id"]]) && $value["table_id"]!=$ct["table_id"] && check_tbl($ct["table_id"],'view',$ct["table_module"]) && check_mod($ct["table_module"],'view')) $opt.='<option value="'.$value["table_id"].'">'.$m[$value["table_module"]]["module_name"].': '.$value["table_name"].'</option>';
			if(!empty($opt)){
				echo '<br>Добавить подтаблицу:<br><select id="tid'.$ct["table_id"].'" class="button">';
				echo $opt;
				echo '</select>';
				echo '<br><input type="button" class="button" value="подчинить" OnClick="document.location.href=\'mod_main?id='.$id.'&action=add_sub&table1='.$ct["table_id"].'&table2=\'+document.getElementById(\'tid'.$ct["table_id"].'\').value;">';
			} else echo '<br>Нет доступных подтаблиц';
			echo '</div>';
		}
		echo '</td>';
		echo '</tr>';
	}
	echo '</table>';
	
	if($have_hide2){
		echo '<div align="right" style="margin-top: -5px; margin-bottom: -24px;">';
		echo se('view2','','Показать спрятанные таблицы','',false,5,0,' OnClick="document.getElementById(\'view2\').style.display=\'\'; this.style.display=\'none\'; showhide_tbl();" id="view1"','cursor: pointer; margin-top: 13px; margin-right: 11px;');
		echo se('view','','Скрыть спрятанные таблицы','',false,5,0,' OnClick="document.getElementById(\'view1\').style.display=\'\'; this.style.display=\'none\'; showhide_tbl();" id="view2"','cursor: pointer; margin-top: 13px; display: none; margin-right: 11px;');
		echo '</div>';
	}
	
	if($have_hide2){
		echo '<script>
			function showhide_tbl(){
			'.$have_hide.'
			}
		</script>';
	}
	/*if(!empty($have_hide)){
		echo '<div align="right" id="ss1"><span class="link" OnClick="showhide(\'ss1\');showhide(\'ss2\');'.$have_hide.'">Показать скрытые</span></div>';
		echo '<div align="right" id="ss2" style="display: none;"><span class="link" OnClick="showhide(\'ss1\');showhide(\'ss2\');'.$have_hide.'">Убрать скрытые</span></div>';
	}*/
}


//=======================
//  Форма редактирования таблицы
//=======================
if(!empty($action) && $action=='edit_table_form' && !empty($id2) && check_mod($id,'edit')){
	echo '<a name="edit_table"></a>';
	echo '<h2>'.si('edit2').'Изменить таблицу</h2>';
	echo '<form action="mod_main" method="post" enctype="multipart/form-data">
		<input type="hidden" name="action" value="edit_table">
		<input type="hidden" name="id2" value="'.$id2.'">
		<input type="hidden" name="id" value="'.$id.'">';
	getrow($db,"SELECT * FROM main_table WHERE table_id=$id2 AND table_module=$id",1,'main_table');
	foreach($db->Record AS $var=>$value)$$var=$value;
	echo '<p>Название<br><input name="table_name" type="text" value="'.$table_name.'"></p>';
	echo '<p>Уникальное спец. название на английском<br><input name="table_sname" type="text" value="'.$table_sname.'"></p>';
	$check='';if($table_multy==1) $check=' checked';
	echo '<p><input type="checkbox" class="button" name="table_multy"'.$check.'> Мультивложенная таблица</p>';
	$check='';if($table_cansub==1) $check=' checked';
	echo '<p><input type="checkbox" class="button" name="table_cansub"'.$check.'> Может подчиняться другим таблицам</p>';
	echo '<p><input type="checkbox" class="button" name="table_public"'.($table_public?' checked':'').'> '.lng('Allow open access to data (for updates child systems)').'</p>';
	if($table_icon){
		echo '<br><img src="'.$base_root.$table_icon.'">';
	}
	echo '<p>Файл иконки (64x64 png)<br><input name="table_icon" type="file" value=""></p>';
	$add1='';$add2='';$add3='';
	if($table_extype==0) $add1=' selected';
	if($table_extype==1) $add2=' selected';
	if($table_extype==2) $add3=' selected';
	echo '<p>Отношение к разделам модуля:<br><select name="table_extype">
			<option value="0"'.$add1.'>Определяется вручную для каждого конкретного случая</option>
			<option value="1"'.$add2.'>Уникальная для каждого раздела</option>
			<option value="2"'.$add3.'>Общая для всех разделов</option>
		</select></p>';
	echo '<h2 style="cursor: pointer" OnClick="showhide(\'addition_table\');">'.si('point').'Дополнительные параметры</h2>';
	if(empty($table_top) && empty($table_bottom) && empty($table_onedit)) echo '<div id="addition_table" style="display: none;">';
	else echo '<div id="addition_table">';
	echo '<p>Обработка при добавлении/редактировании строки:<br>';
	//echo '<textarea name="table_onedit" style="height: 150px;">'.$table_onedit.'</textarea>';
	global $use_ace;
	ide($table_onedit,-1,$id,$id2,-1,'table_onedit','table_part',$use_ace);
	echo '</p>';
	//echo '<p>(для дочерних таблиц) счётчик новых/активных сообщений в строке родителя (обработчик - в стадии разработке): <br><textarea name="table_counter">'.$table_counter.'</textarea></p>';
	echo '<p>Шапка таблицы:<br>';
	//<textarea name="table_top" style="height: 150px;">'.$table_top.'</textarea>
	ide($table_top,-1,$id,$id2,-1,'table_top','table_part',$use_ace);
	echo '</p>';
	echo '<p>Подвал таблицы:<br>';
	ide($table_bottom,-1,$id,$id2,-1,'table_bottom','table_part',$use_ace);
	//<textarea name="table_bottom" style="height: 150px;">'.$table_bottom.'</textarea>
	echo '</p>';
	echo '</div>';
	echo '<input class="button" type="submit" value="Сохранить"> или <a href="mod_main?id='.$id.'">вернуться назад</a>';
	echo '</form>';
}

//=======================
//  Форма добавления таблицы
//=======================
if((empty($action) || $action!='edit_table_form') && check_mod($id,'edit')){
echo '<h3 OnClick="showhide(\'table_add\');" style="cursor: pointer;">'.si('add').'Добавить таблицу</h3><div id="table_add" style="display: none;">';
echo '<form action="mod_main" method="post" enctype="multipart/form-data">
	<input type="hidden" name="id" value="'.$id.'">
	<input type="hidden" name="action" value="add_table">';
echo '<p>Название<br><input name="table_name" type="text" value="" OnBlur="translate2(this,table_sname);"></p>';
echo '<p>Уникальное спец. название на английском<br><input name="table_sname" type="text" value=""></p>';
echo '<p><input type="checkbox" class="button" name="table_multy"> Мультивложенная таблица</p>';
echo '<p><input type="checkbox" class="button" name="table_cansub"> Может подчиняться другим таблицам</p>';
echo '<p><input type="checkbox" class="button" name="table_public"> '.lng('Allow open access to data (for updates child systems)').'</p>';
echo '<p>Файл иконки (64x64 png)<br><input name="table_icon" type="file" value=""></p>';

$tpls=getall($db,"SELECT * FROM main_table WHERE table_module=0 ORDER BY table_name",1,"main_table");
if(empty($tpls)) echo '<input type="hidden" name="table_tpl" value="0">';
else {
	echo '<p>Использовать шаблон:<br><select name="table_tpl" OnChange="var obj=document.getElementById(\'addition_table2\');if (this.selectedIndex==0) obj.style.display=\'\'; else obj.style.display=\'none\';"><option value="0">Нет</option>';
	foreach($tpls AS $tpl) echo '<option value="'.$tpl["table_id"].'">'.$tpl["table_name"].'</option>';
	echo '</select>';
}

$add1='';$add2=' selected';$add3='';
echo '<p>Отношение к разделам модуля:<br><select name="table_extype">
		<option value="0"'.$add1.'>Определяется вручную для каждого конкретного случая</option>
		<option value="1"'.$add2.'>Уникальная для каждого раздела</option>
		<option value="2"'.$add3.'>Общая для всех разделов</option>
</select></p>';
$exm=getall($db,"SELECT * FROM ex_module WHERE ex_module=$id",1,'ex_module');
if(count($exm)>1) echo '<p><input type="checkbox" class="button" name="table_slice"> Общая для всех текущих разделов</p>';
echo '<div id="addition_table2">';
echo '<h2 style="cursor: pointer" OnClick="showhide(\'addition_table\');">'.si('point').'Дополнительные параметры</h2><div id="addition_table" style="display: none;">';
echo '<p>Обработка при добавлении/редактировании строки:<br><textarea name="table_onedit" style="height: 150px;"></textarea></p>';
//echo '<p>(для дочерних таблиц) счётчик новых/активных сообщений в строке родителя (обработчик): <br><textarea name="table_counter"></textarea></p>';
//echo '<p>(для дочерних таблиц) счётчик новых/активных сообщений в строке родителя (обработчик - в стадии разработке): <br><textarea name="table_counter"></textarea></p>';
echo '<p>Шапка таблицы:<br><textarea name="table_top" style="height: 150px;"></textarea></p>';
echo '<p>Подвал таблицы:<br><textarea name="table_bottom" style="height: 150px;"></textarea></p>';
echo '</div>';
echo '</div>';
echo '<input class="button" type="submit" value="Добавить">';
echo '</form></div>';
}

?>