<?php

//=======================
//  Добавление таблицы
//=======================

if(!empty($action) && $action=='add_table' && check_mod($id,'edit')){
	update_module_state($id);
	if(empty($table_multy)) $table_multy=0; else $table_multy=1;
	if(empty($table_cansub)) $table_cansub=0; else $table_cansub=1;
	if(empty($table_public)) $table_public=0; else $table_public=1;
	if(!empty($_FILES['table_icon']) && !empty($_FILES['table_icon']['name'])) $table_icon='table_icon';
	if(!empty($table_icon)) $table_icon=upload_file('table_icon','/files/editor/modules/','table-icon',1);
	if(empty($table_icon)) $table_icon='';
	$table_uin=uuin();
	$bb=0;
	getrow($db,"SELECT COUNT(table_id) AS ctb FROM main_table WHERE table_module=$id",1,"main_table");
	if($db->Record["ctb"]==0) $bb=1;
	
	if(!empty($table_tpl)){
		getrow($db,"SELECT * FROM main_table WHERE table_id=$table_tpl AND table_module=0",1,"main_table");
		$table_bottom=$db->Record["table_bottom"];
		$table_top=$db->Record["table_top"];
		$table_onedit=$db->Record["table_onedit"];
		mprep($table_onedit);
		mprep($table_bottom);
		mprep($table_top);
		$major_col=$db->Record["major_col"];
		$valid=true;//на всякий случай, а то можно будет указывать в table_tpl левые чужие таблицы
	} else $valid=false;
	
	update_part_links($id,'table_onedit',$table_uin,$table_onedit);
	update_part_links($id,'table_top',$table_uin,$table_top);
	update_part_links($id,'table_bottom',$table_uin,$table_bottom);
	
	$db->query("INSERT INTO main_table (table_module, table_name, table_sname, table_multy, table_cansub, table_onedit, table_bold, table_extype, table_uin, table_bottom, table_top, table_icon, table_public)
			VALUES ($id, '$table_name', '$table_sname', $table_multy, $table_cansub,'$table_onedit', $bb, $table_extype, '$table_uin', '$table_bottom','$table_top','$table_icon',$table_public)",3,'main_table');
	getrow($db,"SELECT LAST_INSERT_ID() as sid");
	$sid=$db->Record["sid"];
	
	if(!empty($table_tpl) && $valid){
		$cols=getall($db,"SELECT * FROM main_col WHERE col_table=$table_tpl",1,"main_col");
		if(!empty($cols)) foreach($cols AS $c){
			$col_uin=uuin();//он должен быть новым			
			$col_name=$c["col_name"];
			$col_sname=$c["col_sname"];
			$col_type=$c["col_type"];
			$col_pos=$c["col_pos"];
			$col_order=$c["col_order"];
			$col_default=$c["col_default"];
			$col_unique=$c["col_unique"];
			$col_url=$c["col_url"];
			$col_tpl=$c["col_tpl"];
			$col_bold=$c["col_bold"];
			$col_fastedit=$c["col_fastedit"];
			$col_required=$c["col_required"];
			
			$col_deflist=$c["col_deflist"];
			$col_index=$c["col_index"];
			$col_filter=$c["col_filter"];
			$col_paramlink=$c["col_paramlink"];
			$file_genname=$c["file_genname"];
			$col_speclink=$c["col_speclink"];
			$col_link4=$c["col_link4"];
			$col_cat=$c["col_cat"];									
			$col_link=$c["col_link"];
			$col_link2=$c["col_link2"];
			$col_link3=$c["col_link3"];
			$module_url=$c["module_url"];
			$module_type=$c["module_type"];
			$file_dir=$c["file_dir"];
			$file_maxsize=$c["file_maxsize"];
			$file_prefix=$c["file_prefix"];
			$file_types=$c["file_types"];
			$file_totalmax=$c["file_totalmax"];
			$col_inform=$c["col_inform"];
			$col_part=$c["col_part"];
			$col_onshow=$c["col_onshow"];
			//mprep($col_onshow);
			$col_onform=$c["col_onform"];
			//mprep($col_onform);
			$col_oninsert=$c["col_oninsert"];
			//mprep($col_oninsert);
			$col_hint=$c["col_hint"];
			//mprep($col_hint);
			$col_deep=$c["col_deep"];
			
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
						
			$db->query("INSERT INTO main_col (col_module, col_table, col_name, col_sname, col_type, col_pos, col_order, col_default, col_unique, col_url, col_bold, col_fastedit, col_required, col_link, col_link2, col_link3, module_url, module_type, file_dir, file_maxsize, file_prefix, file_types, file_totalmax, col_inform, col_part,col_onshow,col_onform,col_oninsert,col_hint,col_uin, col_deep, col_tpl, col_deflist, col_index, col_filter, col_paramlink, file_genname, col_speclink, col_link4, col_cat, col_date)
						VALUES ($id, $sid, '$col_name', '$col_sname', $col_type, $col_pos, $col_order, '$col_default', $col_unique, $col_url, $col_bold, $col_fastedit, $col_required, $col_link, $col_link2, $col_link3, $module_url, $module_type, '$file_dir', '$file_maxsize', '$file_prefix', '$file_types', '$file_totalmax', $col_inform, $col_part,'$col_onshow','$col_onform','$col_oninsert','$col_hint','$col_uin',$col_deep,$col_tpl,'$col_deflist', $col_index, $col_filter, $col_paramlink, $file_genname, '$col_speclink', $col_link4, $col_cat, '".date('Y-m-d H:i:s')."')",3,'main_col');
			getrow($db,"SELECT LAST_INSERT_ID() as sid");
			$sid2=$db->Record["sid"];
			update_part_links($sid2,'col_onshow',$col_uin,$col_onshow);
			update_part_links($sid2,'col_onform',$col_uin,$col_onform);
			update_part_links($sid2,'col_oninsert',$col_uin,$col_oninsert);
			if($c["col_id"]==$major_col){
				$db->query("UPDATE main_table SET major_col=$sid2 WHERE table_id=$sid",3,"main_table");
			}
		}
	}

	$exm=getall($db,"SELECT * FROM ex_module WHERE ex_module=$id",1,'ex_module');
	if(!empty($table_slice) || count($exm)<=1){
		$db->query("INSERT INTO ex_table (ex_table, ex_module, ex_name)
				VALUES ($sid, $id, 'основная')",3,'ex_table');
		getrow($db,"SELECT LAST_INSERT_ID() as sid2");
		$sid2=$db->Record["sid2"];	
		foreach($exm AS $ex){
			$db->query("INSERT INTO ex_group (ex_module, ex_table, ex_ex1, ex_ex2)
					VALUES ($id, $sid, $sid2, ".$ex["ex_id"].")",3,'ex_group');
		}
	} else {
		foreach($exm AS $ex){
			$db->query("INSERT INTO ex_table (ex_table, ex_module, ex_name)
					VALUES ($sid, $id, 'для «".$ex["ex_name"]."»')",3,'ex_table');
			getrow($db,"SELECT LAST_INSERT_ID() as sid2");
			$sid2=$db->Record["sid2"];	
			$db->query("INSERT INTO ex_group (ex_module, ex_table, ex_ex1, ex_ex2)
					VALUES ($id, $sid, $sid2, ".$ex["ex_id"].")",3,'ex_group');
		}		
	}
	
	update_table_state($sid);

	$action='';
}

//=======================
//  Удаление таблицы - подтверждение
//=======================
if(!empty($action) && !empty($id2) && $action=='del_table'){
	getrow($db,"SELECT * FROM main_table WHERE table_id=$id2",1,"main_table");
	$name=' "'.$db->Record["table_name"].'"';
	$buff.='<div style="margin-top: 15px; margin-bottom: -10px; padding: 10px; border: 1px solid #999999; background-color: #F9F9F9; width: 400px;"><b>Внимание!</b><br>Вы действительно хотите удалить таблицу '.$name.'?<br>После этой операции будет невозможно восстановить информацию из этой таблицы.
	<form method="post" action="mod_main?id='.$id.'&id2='.$id2.'&action=del_table_confirm">
	'.get_form_protection_key('mod_main',1,1).'
	  <input type="submit" name="smb1" value="Да" class="button" style="width: 100px;">	  <input type="submit" name="smb1" value="Нет" class="button" style="margin-left: 50px; width: 100px;">
	</form></div><br>';
	unset($action);
}

//=======================
//  Удаление таблицы
//=======================
if(!empty($action) && $action=='del_table_confirm' && check_mod($id,'edit') && !empty($smb1) && (!empty($confirm) || (!empty($smb1) && $smb1=='Да')) && check_form_protection_key($_POST['key'],'mod_main',1)){
	update_module_state($id);
	del_table($id2,1,$id);
	$action='';
}

//=======================
//  Редактирование таблицы
//=======================
if(!empty($action) && $action=='edit_table' && !empty($id2) && check_mod($id,'edit')){
	update_module_state($id);
	if(empty($table_multy)) $table_multy=0; else $table_multy=1;
	if(empty($table_cansub)) $table_cansub=0; else $table_cansub=1;
	if(empty($table_public)) $table_public=0; else $table_public=1;
	
	$table_uin=getrowval("SELECT table_id, table_uin FROM main_table WHERE table_id=$id2","table_uin");
	update_part_links($id,'table_onedit',$table_uin,$table_onedit);
	update_part_links($id,'table_top',$table_uin,$table_top);
	update_part_links($id,'table_bottom',$table_uin,$table_bottom);
	
	$ti=getrowval("SELECT table_icon FROM main_table WHERE table_id=$id","table_icon");
	if(!empty($_FILES['table_icon'])) $table_icon='table_icon';
	if(empty($table_icon)) $table_icon='';
	if(!empty($table_icon) && $ti){
		if(file_exists(DOCUMENT_ROOT.$ti)) unlink(DOCUMENT_ROOT.$ti);
	}
	if(!empty($table_icon)){
		$table_icon=upload_file('table_icon','/files/editor/modules/','table-icon',1);
	} else $table_icon=$ti;
	
	$db->query("UPDATE main_table SET table_name='$table_name', table_sname='$table_sname', table_multy=$table_multy, table_cansub=$table_cansub, table_onedit='$table_onedit', table_extype=$table_extype, table_bottom='$table_bottom', table_top='$table_top', table_icon='$table_icon', table_public=$table_public WHERE table_module=$id AND table_id=$id2",3,'main_table');
	
	update_table_state($id2);
	
	$action='';
}

//=======================
//  Сделать основной таблицей
//=======================
if(!empty($action) && $action=='major_table' && !empty($id2) && check_mod($id,'edit')){
	update_module_state($id);
	$db->query("UPDATE main_table SET table_bold=0 WHERE table_module=$id AND table_bold=1",3,'main_table');
	$db->query("UPDATE main_table SET table_bold=1 WHERE table_module=$id AND table_id=$id2",3,'main_table');
	$action='';
}

//=======================
//  Скрыть таблицу
//=======================
if(!empty($action) && $action=='hide_table' && !empty($id2) && check_mod($id,'edit')){
	update_module_state($id);
	$db->query("UPDATE main_table SET table_bold=2 WHERE table_module=$id AND table_id=$id2",3,'main_table');
	$action='';
}

//=======================
//  Показать таблицу
//=======================
if(!empty($action) && $action=='show_table' && !empty($id2) && check_mod($id,'edit')){
	update_module_state($id);
	$db->query("UPDATE main_table SET table_bold=0 WHERE table_module=$id AND table_id=$id2",3,'main_table');
	$action='';
}


//=======================
//  Удаление подчинённой таблицы - подтверждение
//=======================
if(!empty($action) && !empty($sub_id) && $action=='del_sub'){
	getrow($db,"SELECT * FROM table_sub WHERE sub_id=$sub_id",1,"table_sub");
	getrow($db,"SELECT * FROM main_table WHERE table_id=".$db->Record["sub_table2"],1,"main_table");
	$name=' "'.$db->Record["table_name"].'"';
	$buff.='<div style="margin-top: 15px; margin-bottom: -10px; padding: 10px; border: 1px solid #999999; background-color: #F9F9F9; width: 400px;"><b>Внимание!</b><br>Вы действительно хотите удалить подчинённую таблицу '.$name.'?<br>После этой операции будет невозможно восстановить информацию из этой подтаблицы.
	<form method="post" action="mod_main?id='.$id.'&sub_id='.$sub_id.'&action=del_sub_confirm">
	'.get_form_protection_key('mod_main',1,1).'
	  <input type="submit" name="smb1" value="Да" class="button" style="width: 100px;">	  <input type="submit" name="smb1" value="Нет" class="button" style="margin-left: 50px; width: 100px;">
	</form></div><br>';
	unset($action);
}

//=======================
//  Удаление подчинённой таблицы
//=======================
if(!empty($action) && $action=='del_sub_confirm' && check_mod($id,'edit') && !empty($smb1) && (!empty($confirm) || (!empty($smb1) && $smb1=='Да')) && check_form_protection_key($_POST['key'],'mod_main',1)){
	update_module_state($id);
	del_sub($sub_id);
	$action='';
}

//=======================
//  Добавление подчинённой таблицы
//=======================
if(!empty($action) && $action=='add_sub' && check_mod($id,'edit')){
	update_module_state($id);	
	$db->query("SELECT * FROM table_sub WHERE sub_table1=$table1 AND sub_table2=$table2");
	if(!$db->num_rows()){
		$db->query("INSERT INTO table_sub (sub_table1, sub_table2)
							VALUES ($table1, $table2)",3,'table_sub');
	}
}

?>