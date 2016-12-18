<?php

//=======================
//  Вывод экземпляров модуля
//=======================

//$t=getall($db,"SELECT * FROM ex_module WHERE ex_module=$id ORDER BY ex_name",1,'ex_module');
//$c_ex=count($t);
$c_ex=count($aex);
if(!empty($aex)){

	echo '<div style="clear: both; height: 30px; margin-top: 13px;"><div style="float: left;"><h2>Разделы (Экземпляры)</h2></div><div style="float: right;">';
	if($eid!=-1)	echo se('check_off','mod_main?id='.$id.'&amp;id2=-1&amp;action=set_cex','Показать все разделы','',true,5,0,'','margin-top: 13px; margin-right: 11px;');
	else			echo se('check_on','','Показаны все разделы','',false,5,0,'','margin-top: 13px; margin-right: 11px;');
	echo '</div></div>';

	//echo '<h2>Разделы (Экземпляры)</h2>';
	echo '<table id="records" cellpadding="3" cellspacing="1">';
	echo '<tr>';
	echo '<th>Название</th>';
	echo '<th width="120">Действия</th>';
	echo '</tr>';
	$ezo_all=getall5($db,"SELECT * FROM ex_zone","ex_module");
	foreach($aex AS $ct)/*if(check_ex($ct["ex_id"],'view'))*/{
		$id2=$ct["ex_id"];
		echo '<tr>';
		echo '<td>'.$ct["ex_name"].' <span style="color: #777777;">('.$ct["ex_sname"].')</span></td>';
		echo '<td align="right"> ';
		if(check_ex($id2,'edit')/* || check_mod($id,'edit')*/) echo se('edit','mod_main?id='.$id.'&amp;id2='.$ct["ex_id"].'&amp;action=edit_ex_form#edit_ex');//echo '<a href="mod_main?id='.$id.'&amp;id2='.$ct["ex_id"].'&amp;action=edit_ex_form#edit_ex">Изменить</a> ';
		if(/*check_mod($id,'edit') || */check_ex(0,'del',$id) || check_ex($id2,'del')) echo se('del','mod_main?id='.$id.'&amp;id2='.$ct["ex_id"].'&amp;action=del_ex'); //echo '<a href="mod_main?id='.$id.'&amp;id2='.$ct["ex_id"].'&amp;action=del_ex">Удалить</a> ';
		if(check_ex($id2,'edit')/* || check_mod($id,'edit')*/) echo se('zone','','Связь с сайтами','',false,5,0,'OnClick="showhide(\'szo'.$ct["ex_id"].'\')"','cursor: pointer');//echo '<span OnClick="showhide(\'szo'.$ct["ex_id"].'\')" class="link">Сайты</span> ';
		if($cex!=$ct["ex_id"]){
			 if(check_ex($id2,'view')) echo se('check_off','mod_main?id='.$id.'&amp;id2='.$ct["ex_id"].'&amp;action=set_cex','Выбрать раздел');//echo ' <a href="mod_main?id='.$id.'&amp;id2='.$ct["ex_id"].'&amp;action=set_cex">Выбрать</a> ';
		 } else echo se('check_on','','Текущий раздел','',false);//echo ' <b>Текущий</b>';
		if(check_ex($id2,'edit')/* || check_mod($id,'edit')*/){
			 echo '<div id="szo'.$ct["ex_id"].'" style="display: none;">';
			//$ezo=getall($db,"SELECT * FROM ex_zone WHERE ex_module=".$ct["ex_id"],1,"ex_zone");			
			$ezo=Array(); if(!empty($ezo_all[$ct["ex_id"]])) $ezo=$ezo_all[$ct["ex_id"]];
			$tzo=Array(); foreach($ezo AS $zo) $tzo[$zo["ex_zone"]]=1;
			if(!empty($ezo)){
				echo '<br><div>удалить:</div>';
				foreach($ezo AS $zo) if(!empty($zone2[$zo["ex_zone"]])) echo '<div>- <a href="mod_main?id='.$id.'&amp;action=del_exzone&amp;id2='.$zo["ex_id"].'">'.$zone2[$zo["ex_zone"]]["zone_name"].'</a></div>';
			}
			$selz='';
			foreach($zone AS $z) if(empty($tzo[$z["zone_id"]]) && empty($z["zone_redirect"]) && check_zone($z["zone_id"],'view') && $z["zone_module"]!=-1 && $z["zone_module"]!=-2){
				$selz.='<option value="'.$z["zone_id"].'">'.$z["zone_name"].'</option>';
			}
			if(!empty($selz)) echo '<br><form action="mod_main"><input type="hidden" name="id" value="'.$id.'"><input type="hidden" name="action" value="add_exzone"><input type="hidden" name="module_ex" value="'.$ct["ex_id"].'">Добавить связь с сайтом:<br><select name="new_zone">'.$selz.'</select><input type="submit" value="добавить" class="button"></form>';
			echo '</div>';
		} else {
			echo '<div id="szo'.$ct["ex_id"].'" style="display: none;">Отсутствуют права доступа на текущий раздел</div>';
		}
		echo '</td></tr>';
	}
	echo '</table>';
	echo '<div align="right">';
	//if(check_mod($id,'edit')){
	//	echo '<a href="mod_col?id='.$id.'">Переменные модуля</a> &nbsp; ';
	//	if($cex!=0) echo '<a href="mod_main?id='.$id.'&amp;id2=0&amp;action=set_cex">Выбрать все</a>';
	//	else echo '<b>Выбраны все</b>';
	//}
	echo '</div>';
}

//=======================
//  Форма добавления экземпляра
//=======================
if((empty($action) || $action!='edit_ex_form') && check_mod($id,'edit')){
	echo '<a name="ex"></a><h3 OnClick="showhide(\'ex_add\');" style="cursor: pointer;">'.si('add').'Добавить раздел</h3><div id="ex_add" style="display: none;">';
	echo '<form action="mod_main" method="post" name="exadd" enctype="multipart/form-data">
		<input type="hidden" name="id" value="'.$id.'">
		<input type="hidden" name="action" value="add_ex">';
		
	add_ex_form($id,1);
	
	echo '<br><input class="button" type="submit" value="Добавить">';
	echo '</form></div>';
	
	if(!empty($GLOBALS['update_server']) && getrowval("SELECT module_date FROM main_module WHERE module_id=$id AND module_date!='0000-00-00 00:00:00' AND module_date!=''","module_date")){
		echo '<h3 OnClick="showhide(\'ex_upload\'); $(\'#load_container\').load(jsq2+\'/ajax?action=get_remote_ex&m='.$id.'\');" style="cursor: pointer; width: 250px;">'.si('update').' Загрузить с сервера</h3><div id="ex_upload" style="display: none;">';
		echo '<form action="mod_main" method="post" enctype="multipart/form-data">';
		echo '<input type="hidden" name="id" value="'.$id.'">';
		echo '<input type="hidden" name="action" value="upload_ex">';
		
		global $db;
		getrow($db,"SELECT * FROM main_module WHERE module_id=$id",1,"main_module");
		$module_name=$db->Record["module_name"];
		$module_sname=$db->Record["module_sname"];
		getrow($db,"SELECT count(ex_id) AS count FROM ex_module WHERE ex_module=$id",1,"main_ex");
		if(empty($db->Record["count"])) $module_count=1; else $module_count=$db->Record["count"]+1;
		$prefix='';
		$vars=Array();
		$vars['only_body']=1;
		$vars['section']['main']['fields'][]=Array('type'=>'text','name'=>$prefix.'ex_name','value'=>$module_name.' '.$module_count,'title'=>lng('Name'),'addon'=>' OnBlur="translate2(this,'.$prefix.'ex_sname);"');
		$vars['section']['main']['fields'][]=Array('type'=>'text','name'=>$prefix.'ex_sname','value'=>latinica($module_name).$module_count,'title'=>lng('Unique special name in English'));
		$vars['section']['main']['fields'][]=Array('type'=>'checkbox','name'=>$prefix.'ex_public','value'=>0,'caption'=>lng('Allow open access to data (for updates child systems)'));
		echo shell_tpl_admin('block/form', $vars);
		
		echo '<div id="load_container">Ожидайте загрузки данных</div><br>';
		
		echo '<div><input type="submit" class="button" value="Загрузить"></div>';
		echo '</form></div>';
	}	
	
}

//=======================
//  Форма редактирования экземпляра
//=======================
if(!empty($_GET["id2"])) $id2=$_GET["id2"];
if(!empty($action) && $action=='edit_ex_form' && !empty($id2) && (check_mod($id,'edit') || check_ex($id2,'edit'))){
	echo '<a name="edit_ex"></a>';
	echo '<h2>'.si('edit2').'Изменить раздел</h2>';
	echo '<form action="mod_main" method="post" enctype="multipart/form-data">
		<input type="hidden" name="action" value="edit_ex">
		<input type="hidden" name="id2" value="'.$id2.'">
		<input type="hidden" name="id" value="'.$id.'">';
	getrow($db,"SELECT * FROM ex_module WHERE ex_id=$id2 AND ex_module=$id",1,'ex_module');
	foreach($db->Record AS $var=>$value)$$var=$value;
	echo '<p>Название<br><input name="ex_name" type="text" value="'.$ex_name.'"></p>';
	echo '<p>Уникальное спец. название на английском<br><input name="ex_sname" type="text" value="'.$ex_sname.'"></p>';
	echo '<p><input type="checkbox" class="button" name="ex_public"'.($ex_public?' checked':'').'> '.lng('Allow open access to data (for updates child systems)').'</p>';
	
	echo '<p>Основная таблица для этого раздела:<br>';
	echo '<select name="ex_major"><option value="0">Совпадает с основной таблицей модуля</option>';	
	$tbl=getall($db,"SELECT * FROM main_table WHERE table_module=$id ORDER BY table_name",1,'main_table');
	if(!empty($tbl)) foreach($tbl AS $tb){
		if($ex_major==$tb["table_id"]) $add=' selected'; else $add='';
		if($tb["table_id"]!=$module_major) echo '<option value="'.$tb["table_id"].'"'.$add.'>'.$tb["table_name"].'</option>';
	}
	echo '</select></p>';
	
	global $ex_ex2;
	$ex_ex2=$id2;
	echo input_form(0,$id,$id2,0,0,$ex_ex2);
	$tid=0;
	
	$all_subtables=get_crossmodule_subtable($id,1);
	
	if(!empty($all_subtables)) foreach($all_subtables AS $var=>$value)if($var!=$id){
		//$GLOBALS["eef_modex"]=getrowval("SELECT * FROM ex_module WHERE ex_module=$var","ex_id");
		//$GLOBALS["eef_tex"]=get_tex(0,$GLOBALS["eef_modex"],);	
		$z=input_form(0,$var,$id2,0,0,$ex_ex2,0,1);
		if($z!=''){
			$tid++;
			echo '<h2 OnClick="showhide(\'additional_table'.$tid.'\')" style="cursor: pointer;">Параметры подтаблицы «'.$value.'»</h2>';
			echo '<div id="additional_table'.$tid.'" style="display: none;">';
			echo $z;
			echo '</div>';
		}
	}
	echo '<input class="button" type="submit" value="Сохранить"> или <a href="mod_main?id='.$id.'">вернуться назад</a>';
	echo '</form>';
}

?>