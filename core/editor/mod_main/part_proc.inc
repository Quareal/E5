<?php

//=======================
//  Добавление части модуля
//=======================
if(!empty($action) && $action=='add_part' && check_mod($id,'edit')){
	update_module_state($id);
	if(!empty($part_iowner)) $part_iowner=1; else $part_iowner=0;
	if(!empty($part_sowner) && empty($part_iowner)) $part_sowner=1; else $part_sowner=0;
	//if(!empty($part_ignore)) $part_ignore=0; else $part_ignore=1;//реверс чека
	if($part_ignore==5) $part_404=1; else $part_404=0;
	if($part_ignore!=0) $part_ignore=0; else $part_ignore=1;
	//if(!empty($part_404)) $part_404=1; else $part_404=0;
	if(!empty($part_unsafe)) $part_unsafe=1; else $part_unsafe=0;
	if(!empty($part_skipurl)) $part_skipurl=1; else $part_skipurl=0;
	//if(!empty($part_auth)){ if($user->super==1) $part_auth=0; else $part_auth=$user->id; if($user->id==0) $part_auth=-1;}
	//else $part_auth=-1;
	//if(!empty($part_auth)){ if($user->super==1) $part_auth=0; else $part_auth=$user->id; if($user->id==0 && $user->super!=1) $part_auth=-1;}
	//else $part_auth=-1;
	if($part_auth==0 && !$user->super) $part_auth=-1;
	$part_uin=uuin();
	$db->query("INSERT INTO main_part (part_name, part_sname, part_access, part_type, part_table, part_iowner, part_sowner, part_module, part_url, part_ignore, part_parse, part_ex, part_file, part_cur, part_owner, part_uin, part_auth, part_404, part_pic, part_unsafe, part_ifcase, part_ifrow, part_ifdetect, part_folder, part_date, part_date2, part_skipurl)
			VALUES ('$part_name', '$part_sname', $part_access, $part_type, $part_table, $part_iowner, $part_sowner, $id, '$part_url', $part_ignore, $part_parse, $part_ex, '$part_file', '$part_cur', $part_owner, '$part_uin', $part_auth, $part_404, '$part_pic', $part_unsafe, '$part_ifcase', '$part_ifrow', '$part_ifdetect', '$part_folder', '".date('Y-m-d H:i:s')."', '".date('Y-m-d H:i:s')."', $part_skipurl)",3,'main_part');
	//update_part_links($id,'part',$part_uin,'');
}

//=======================
//  Редактирование части модуля
//=======================
if(!empty($action) && $action=='edit_part' && !empty($id2) && check_mod($id,'edit')){
	update_module_state($id);
	//update_part_links($id,'part',$part_uin,'');
	if(!empty($part_iowner)) $part_iowner=1; else $part_iowner=0;
	//if(!empty($part_sowner)) $part_sowner=1; else $part_sowner=0;
	//if(!empty($part_ignore)) $part_ignore=0; else $part_ignore=1;//тут реверс этого чека, для удобства
	if(!empty($part_sowner) && empty($part_iowner)) $part_sowner=1; else $part_sowner=0;
	if($part_ignore==5) $part_404=1; else $part_404=0;
	if($part_ignore!=0) $part_ignore=0; else $part_ignore=1;
	//if(!empty($part_404)) $part_404=1; else $part_404=0;
	if(!empty($part_unsafe)) $part_unsafe=1; else $part_unsafe=0;
	if(!empty($part_skipurl)) $part_skipurl=1; else $part_skipurl=0;
	//if(!empty($part_auth)){ if($user->super==1) $part_auth=0; else $part_auth=$user->id; if($user->id==0 && $user->super!=1) $part_auth=-1;}
	//else $part_auth=-1;
	if($part_auth==0 && !$user->super){
		$old_part_auth=getrowval("SELECT part_auth FROM main_part WHERE part_id=$part_id","part_auth");
		if($old_part_auth!=0) $part_auth=-1;
	}
	update_part_state($id2);
	$db->query("UPDATE main_part SET 
		part_name='$part_name',
		part_sname='$part_sname',
		part_access=$part_access,
		part_type=$part_type,
		part_table=$part_table,
		part_iowner=$part_iowner,
		part_sowner=$part_sowner,
		part_url='$part_url',
		part_ignore=$part_ignore,
		part_parse=$part_parse,
		part_ex=$part_ex,
		part_file='$part_file',
		part_cur='$part_cur',
		part_owner=$part_owner,
		part_auth=$part_auth,
		part_404=$part_404,
		part_pic='$part_pic',
		part_unsafe=$part_unsafe,
		part_ifcase='$part_ifcase',
		part_ifdetect='$part_ifdetect',
		part_folder='$part_folder',
		part_ifrow='$part_ifrow',
		part_skipurl=$part_skipurl
		WHERE part_module=$id AND part_id=$id2",3,'main_part');
	$action='';
}

//=======================
//  Настройка таймера части модуля
//=======================
if(!empty($action) && $action=='edit_timer' && !empty($id2) && check_mod($id,'edit')){
	update_module_state($id);
	$db->query("UPDATE main_part SET 
		timer_type=$timer_type,
		timer_time='$timer_time',
		timer_date='$timer_date',
		timer_x=$timer_x,
		timer_y=$timer_y
		WHERE part_module=$id AND part_id=$id2",3,'main_part');
	$action='';
}

//=======================
//  Удаление части модуля - подтверждение
//=======================
if(!empty($action) && !empty($id2) && $action=='del_part'){
	getrow($db,"SELECT * FROM main_part WHERE part_id=$id2",1,"main_part");
	$name=' "'.$db->Record["part_name"].'"';
	$buff.='<div style="margin-top: 15px; margin-bottom: -10px; padding: 10px; border: 1px solid #999999; background-color: #F9F9F9; width: 400px;"><b>Внимание!</b><br>Вы действительно хотите удалить часть '.$name.'?
	<form method="post" action="mod_main?id='.$id.'&id2='.$id2.'&action=del_part_confirm">
	'.get_form_protection_key('mod_main',1,1).'
	  <input type="submit" name="smb1" value="Да" class="button" style="width: 100px;">	  <input type="submit" name="smb1" value="Нет" class="button" style="margin-left: 50px; width: 100px;">
	</form></div><br>';
	unset($action);
}

//=======================
//  Удаление части модуля
//=======================
if(!empty($action) && $action=='del_part_confirm' && check_mod($id,'edit') && !empty($smb1) && (!empty($confirm) || (!empty($smb1) && $smb1=='Да')) && check_form_protection_key($_POST['key'],'mod_main',1)){
	update_module_state($id);
	del_part_links($id,'part',getrowval("SELECT part_id, part_uin FROM main_part WHERE part_id=$id2","part_uin"));
	del_part($id2);
	$action='';
}

//=======================
//  Деактивация части модуля
//=======================
if(!empty($action) && $action=='deactivate_part' && !empty($id2) && check_mod($id,'edit') && check_form_protection_key($_GET['key'],'mod_main',1)){
	$db->query("UPDATE main_part SET part_enable=0 WHERE part_id=".$id2,3,"main_part");
}

//=======================
//  Активация части модуля
//=======================
if(!empty($action) && $action=='activate_part' && !empty($id2) && check_mod($id,'edit') && check_form_protection_key($_GET['key'],'mod_main',1)){
	$db->query("UPDATE main_part SET part_enable=1 WHERE part_id=".$id2,3,"main_part");
}

//=======================
//  Выполнение части модуля
//=======================
if(!empty($action) && $action=='shell' && !empty($id2) && check_mod($id,'edit') && check_form_protection_key((!empty($_GET['key'])?$_GET['key']:$_POST['key']),'mod_main',1)){
	$GLOBALS["import"]='';
	if(!empty($_FILES["part_file"]["tmp_name"])) $GLOBALS["import"]=str_replace("'","''",implode('',file($_FILES["part_file"]["tmp_name"])));
	$GLOBALS["cur_module"]=$id;
	$GLOBALS["cur_ex"]=$cex;
	echo run_part($id2);
	$action='';
}

//=======================
//  Очистка таймера
//=======================
if(!empty($action) && $action=='cleartimer' && !empty($id2) && check_mod($id,'edit')){
	$db->query("UPDATE main_part SET
			timer_last='0000-00-00 00:00:00'
		WHERE part_id=$id2",3,"main_part");
	$action='';
}

//=======================
//  Преобразование части в шаблон
//=======================
if(!empty($action) && !empty($id2) && $action=='create_template' && $user->super){
	getrow($db,"SELECT * FROM main_part WHERE part_id=$id2");
	$part_sname=$db->Record["part_sname"];
	$part_name=$db->Record["part_name"];
	$part_body=$db->Record["part_body"];
	$true_name='Шаблон '.$part_name;
	$true_name2='Шаблоны '.$part_name;
	$true_sname='tbl_'.$part_sname;
	getrow($db,"SELECT * FROM main_table WHERE table_sname='$true_sname' AND table_module=$id");
	if(!empty($db->Record)){
		//игнорируем операцию, т.к. такая таблица уже существует
		$buff.='<div style="margin-top: 15px; margin-bottom: -10px; padding: 10px; border: 1px solid #999999; background-color: #F9F9F9; width: 400px;"><b>Внимание!</b><br>Операция по превращению части в шаблон завершилась неудачей, т.к. таблица с именем "'.$true_sname.'" уже существует</div>';
	} else {
		$db->query("INSERT INTO main_table (table_uin, table_module, table_multy, table_name, table_sname, table_cansub, table_bold, table_extype)
						VALUES ('".uuin()."', $id, 0, '$true_name2', '$true_sname', 0, 0, 2)");
		$table_id=getrowval("SELECT LAST_INSERT_ID() as sid","sid");
	
		$module_exes=getall3($db,"SELECT * FROM ex_module WHERE ex_module=$id","ex_id");
		$db->query("INSERT INTO ex_table (ex_module, ex_table, ex_name)
						VALUES ($id, $table_id, '$true_name2')");
		$table_ex=getrowval("SELECT LAST_INSERT_ID() as sid","sid");
		if(!empty($module_exes)) foreach($module_exes AS $module_ex) $db->query("INSERT INTO ex_group (ex_module, ex_table, ex_ex1, ex_ex2)
																				VALUES ($id, $table_id, $table_ex, $module_ex)");
		
		$db->query("INSERT INTO main_col (col_uin, col_module, col_table, col_target, col_name, col_sname, col_cat, col_type, col_filter, col_onform, col_pos, col_bold, col_fastedit, col_inform)
								VALUES ('".uuin()."', $id, $table_id, 0, 'Имя', 'name', 0, 0, 0, '', 1, 1, 1, 1)");
		$namecol_id=getrowval("SELECT LAST_INSERT_ID() as sid","sid");
		$db->query("UPDATE main_table SET major_col=$namecol_id WHERE table_id=$table_id");
		$db->query("INSERT INTO main_col (col_uin, col_module, col_table, col_target, col_name, col_sname, col_cat, col_type, col_filter, col_onform, col_pos, col_bold, col_fastedit, col_inform)
								VALUES ('".uuin()."', $id, $table_id, 0, 'Шаблон', 'tpl', 0, 0, 0, '[form.textarea height=500]', 2, 0, 0, 1)");	
		$bodycol_id=getrowval("SELECT LAST_INSERT_ID() as sid","sid");
		
		$db->query("INSERT INTO main_row (row_uin, row_module, row_table, row_ex, row_sub, row_enable, row_user)
								VALUES ('".uuin()."', $id, $table_id, $table_ex, 0, 1, 0)");
		$row_id=getrowval("SELECT LAST_INSERT_ID() as sid","sid");	
		$db->query("INSERT INTO row_owner (ro_pos, ro_enable, ro_ex, ro_sub, row_id, row_module, row_table, owner_id, owner_table, owner_module, ro_user, ro_users)
								VALUES (1, 1, $table_ex, 0, $row_id, $id, $table_id, 0, 0, 0, 0, 0)");
		$db->query("INSERT INTO row_value (value_module, value_table, value_row, value_col, value_value)
								VALUES ($id, $table_id, $row_id, $namecol_id, 'Шаблон 1')");	
		$part_body=prepend_value($part_body);
		$db->query("INSERT INTO row_value (value_module, value_table, value_row, value_col, value_value)
								VALUES ($id, $table_id, $row_id, $bodycol_id, '$part_body')");

		$max_col=getrowval("SELECT max(col_pos) AS imax FROM main_col WHERE col_module=$id AND col_table=0","imax")+1;
		$db->query("INSERT INTO main_col (col_uin, col_module, col_table, col_target, col_name, col_sname, col_cat, col_type, col_filter, col_onform, col_pos, col_bold, col_fastedit, col_link, col_link2, col_inform)
								VALUES ('".uuin()."', $id, 0, 1, '$true_name', '$true_sname', 0, 1, 0, '', $max_col, 1, 1, $table_id, 0, 1)");
		$exparam_id=getrowval("SELECT LAST_INSERT_ID() as sid","sid");
		
		if(!empty($module_exes)) foreach($module_exes AS $module_ex) $db->query("INSERT INTO row_value (value_module, value_table, value_row, value_col, value_value)
								VALUES ($id, 0, $module_ex, $exparam_id, $row_id)");
																			
		$db->query("UPDATE main_part SET part_body='[cex.param.".$true_sname.".tpl.shell]' WHERE part_id=$id2");
		clean_sql();
	}
}

?>