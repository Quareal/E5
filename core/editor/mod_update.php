<?php

/*global $user; if($user->super==0) {include('main.php'); exit;}*/
include_once(DOCUMENT_ROOT.'/core/update/functions.inc');
global $db;

$uin=getrowval("SELECT * FROM main_module WHERE module_id=$id","module_uin");
if(empty($uin)){
	echo '<h2>Проблемы с обновлением модуля</h2>';
}
if($action=='check' && !empty($id) && !empty($GLOBALS["update_server"]) && check_mod($id,'edit') && !empty($uin)){
	$del_m=getall($db,"SELECT * FROM main_module WHERE module_sname LIKE '%_for_uninstall'");
	if(!empty($del_m)) foreach($del_m AS $dm) del_module($dm["module_id"]);	
	$uin2=$uin.'_update';
	
	$GLOBALS["silent"]=true;
	$GLOBALS["do_not_addon_groups"]=true;	
	$mdls=explode(',',loadserv($GLOBALS["update_server"].'?type=modlinks&uin='.$uin));
	prepend_mod_install();
	
	if(!empty($mdls)) foreach($mdls AS $mdl)if(!empty($mdl)){
		getrow($db,"SELECT * FROM main_module WHERE module_uin='".$mdl."'");
		if(empty($db->Record)){		
			$new=import_module(0,$mdl);//тихая загрузка дополнительных модулей
			$name=getrowval("SELECT module_name FROM main_module WHERE module_id=$new","module_name");
			echo '<h2>Догружен новый модуль "'.$name.'"</h2>';
		}
	}

	$sid=import_module(0,'-'.$uin,1);
	after_mod_install();
	$GLOBALS["silent"]=false;
	if(empty($sid)){
		echo '<h2 align="center">Обновление не удалось. Не удалось найти указанный модуль на сервере</h2>';
	} else {
		$db->query("UPDATE main_module SET module_sname='".getrowval("SELECT * FROM main_module WHERE module_id=$sid","module_sname")."_for_uninstall' WHERE module_id=$sid");		
	}
	
	if(!empty($sid)){
		global $zone_url;
		echo '<form action="'.$zone_url.'/mod_update" method="post">
			<input type="hidden" name="action" value="update">
			<input type="hidden" name="id" value="'.$id.'">
			<input type="hidden" name="sid" value="'.$sid.'">';
		
		$old_tables=getall4($db,"SELECT * FROM main_table WHERE table_module=$id","table_uin");
		$new_tables=getall4($db,"SELECT * FROM main_table WHERE table_module=$sid","table_uin");
		
		$old_colsB=getall($db,"SELECT * FROM main_col WHERE col_module=$id");
		$old_cols=Array();
		$old_cols2=Array();
		if(!empty($old_colsB)) foreach($old_colsB AS $col){
			$old_cols[$col["col_table"]][$col["col_uin"]]=$col;
			$old_cols2[$col["col_uin"]]=$col;
		}
		
		$new_colsB=getall($db,"SELECT * FROM main_col WHERE col_module=$sid");
		$new_cols=Array();
		$new_cols2=Array();
		if(!empty($new_colsB)) foreach($new_colsB AS $col){
			$new_cols[$col["col_uin"]]=$col;
			$new_cols2[$col["col_table"]][$col["col_uin"]]=$col;
		}
		
		//Запрос на упразднение таблиц
		$first=true; $first2=Array();
	  	if(!empty($old_tables)) foreach($old_tables AS $tbl_uin=>$tbl) if(empty($new_tables[$tbl_uin])){
	  		if($first) echo '<h2>Удалить упразднённые таблицы</h2><p>Если вы планируете пользоваться данными, находящимися в них, не удаляйте</p>';
	  		$first=false;
	  		$add=' checked';
	  		if($tbl['table_date2']!='0000-00-00 00:00:00' && $tbl['table_date2']!='') $add='';
	  		echo '<div><input type="checkbox" class="checkbox" name="remove_tbl['.$tbl["table_id"].']"'.$add.'> '.$tbl["table_name"].'</div>';
	  	}
		//Запрос на упразднение столбцов
	  	if(!empty($old_tables)) foreach($old_tables AS $tbl_uin=>$tbl) if(!empty($new_tables[$tbl_uin])){
	 		$first=true;
	 		if(!empty($old_cols[$tbl["table_id"]])) foreach($old_cols[$tbl["table_id"]] AS $col) if(empty($new_cols[$col["col_uin"]])){
	 	  		if($first) echo '<h2>Удалить упразднённые переменные таблицы "'.$tbl["table_name"].'"</h2>';
		  		$first=false;
		  		$add='checked';
		  		if($col['col_date2']!='0000-00-00 00:00:00' && $col['col_date2']!='') $add='';
		  		echo '<div><input type="checkbox" class="checkbox" name="remove_col['.$col["col_id"].']"'.$add.'> '.$col["col_name"].'</div>';
	 		}
	  	}
	  	//Перечень модифицированных таблиц
	  	$first=true;
	  	if(!empty($new_tables)) foreach($new_tables AS $tbl_uin=>$tbl) if(!empty($old_tables[$tbl_uin])){
		  	//echo $tbl["table_name"].'<br>';
		  	//echo $tbl["major_col"].' = '.$old_tables[$tbl_uin]["major_col"].'<br>';
	  		if(!empty($tbl["major_col"])){		
		  		$tbl["major_col"]=getrowval("SELECT * FROM main_col WHERE col_id=".$tbl["major_col"],"col_uin");
		  		if(!empty($old_tables[$tbl_uin]["major_col"])) $old_tables[$tbl_uin]["major_col"]=getrowval("SELECT * FROM main_col WHERE col_id=".$old_tables[$tbl_uin]["major_col"],"col_uin");
		  	}
		  	//echo $tbl["major_col"].'<br>';
		  	//echo $old_tables[$tbl_uin]["major_col"].'<br>';
	  		if(!check_ident($tbl,$old_tables[$tbl_uin],"table")){
		  		if($first) echo '<h2>Обновить таблицы</h2>';
		  		$first=false;
		  		echo '<div><input type="checkbox" class="checkbox" name="update_tbl['.$old_tables[$tbl_uin]["table_id"].']" checked> '.$old_tables[$tbl_uin]["table_name"].'</div>';
		  	}
	  	}
	  	//Перечень модифицированных столбцов
	  	if(!empty($new_tables)) foreach($new_tables AS $tbl_uin=>$tbl) if(!empty($old_tables[$tbl_uin])){
		  	$first=true;
			if(!empty($new_cols2[$tbl["table_id"]]))  foreach($new_cols2[$tbl["table_id"]] AS $col)if(!empty($old_cols2[$col["col_uin"]])){
				if(!empty($col["col_link"])){
					$col["col_link"]=getrowval("SELECT * FROM main_table WHERE table_id=".$col["col_link"],"table_uin");
					if(!empty($old_cols2[$col["col_uin"]]["col_link"])) $old_cols2[$col["col_uin"]]["col_link"]=getrowval("SELECT * FROM main_table WHERE table_id=".$old_cols2[$col["col_uin"]]["col_link"],"table_uin");
				}
				//echo $col["col_link"].'<br>'.$old_cols2[$col["col_uin"]]["col_link"].'<br><br>';
				if(!check_ident($col,$old_cols2[$col["col_uin"]],"col")){
					if($first) echo '<h2>Обновить переменные у таблицы "'.$tbl["table_name"].'"</h2>';
					$first=false;
					echo '<div><input type="checkbox" class="checkbox" name="update_col['.$old_cols2[$col["col_uin"]]["col_id"].']" checked> '.$old_cols2[$col["col_uin"]]["col_name"].'</div>';
				}
			}
	  	}
	  	//Переменные экземпляра для удаления
 		$first=true;
 		if(!empty($old_cols[0])) foreach($old_cols[0] AS $col) if(empty($new_cols[$col["col_uin"]])){
 	  		if($first) echo '<h2>Удалить упразднённые переменные модуля</h2>';
	  		$first=false;
	  		$add=' checked';
	  		if($col['col_date2']!='0000-00-00 00:00:00' && $col['col_date2']!='') $add='';
	  		echo '<div><input type="checkbox" class="checkbox" name="remove_col['.$col["col_id"].']"'.$add.'> '.$col["col_name"].'</div>';
 		}
 		if(!empty($new_cols2[0])) foreach($new_cols2[0] AS $col){
 			if(isset($old_cols[0][$col["col_uin"]])){
 				echo '<input type="hidden" name="update_col['.$old_cols2[$col["col_uin"]]["col_id"].']" value="1">';
 			} else {
 				echo '<input type="hidden" name="add_col['.$col["col_id"].']" value="-1">';
 			}
 		}
	  	
	  	
	  	//Перечень новых таблиц
	  	if(!empty($new_tables)) foreach($new_tables AS $tbl_uin=>$tbl){
	  		if(empty($old_tables[$tbl_uin])){
		  		echo '<input type="hidden" name="add_tbl['.$tbl["table_id"].']" value="1">';
		  	}
			if(!empty($new_cols2[$tbl["table_id"]]))  foreach($new_cols2[$tbl["table_id"]] AS $col)if(empty($old_cols2[$col["col_uin"]])){
				echo '<input type="hidden" name="add_col['.$col["col_id"].']" value="'.$tbl["table_uin"].'">';
			}
	  	}

		//Запрос на упразднение частей
		$old_parts=getall4($db,"SELECT * FROM main_part WHERE part_module=$id AND part_type IN (0,1,3,4,5)","part_uin");
		$new_parts=getall4($db,"SELECT * FROM main_part WHERE part_module=$sid AND part_type IN (0,1,3,4,5)","part_uin");
		$first=true;
		if(!empty($old_parts)) foreach($old_parts AS $part) if(empty($new_parts[$part["part_uin"]])){
			if($first) echo '<h2>Удалить упразднённые части</h2>';
			$add=' checked';
	  		if($part['part_date2']!='0000-00-00 00:00:00' && $part['part_date2']!='') $add='';
			echo '<div><input type="checkbox" class="checkbox" name="remove_part['.$part["part_id"].']"'.$add.'> '.$part["part_name"].'</div>';
			$first=false;
		}
		//Запрос на добавление частей
		if(!empty($new_parts)) foreach($new_parts AS $part) if(!isset($old_parts[$part["part_uin"]])){
			echo '<input type="hidden" name="add_part['.$part["part_id"].']" value="1">';
			$new_param=getall4($db,"SELECT * FROM part_param WHERE param_part=".$part["part_id"],"param_sname");
			if(!empty($new_param)) foreach($new_param AS $param) echo '<input type="hidden" name="add_param'.$part["part_id"].'['.$param["param_uin"].']" value="1">';
		}
		//Запрос на изменение частей
		$first=true;
		if(!empty($old_parts)) foreach($old_parts AS $part) if(!empty($new_parts[$part["part_uin"]])){	
			if(!empty($part["part_owner"])){
				$part["part_owner"]=getrowval("SELECT * FROM main_part WHERE part_id=".$part["part_owner"],"part_uin");
				if(!empty($new_parts[$part["part_uin"]]["part_owner"])) $new_parts[$part["part_uin"]]["part_owner"]=getrowval("SELECT * FROM main_part WHERE part_id=".$new_parts[$part["part_uin"]]["part_owner"],"part_uin");
			}
			if(!empty($part["part_table"])){
				$part["part_table"]=getrowval("SELECT * FROM main_table WHERE table_id=".$part["part_table"],"table_uin");
				if(!empty($new_parts[$part["part_uin"]]["part_table"])) $new_parts[$part["part_uin"]]["part_table"]=getrowval("SELECT * FROM main_table WHERE table_id=".$new_parts[$part["part_uin"]]["part_table"],"table_uin");
			}
			if(!empty($part["part_major"])){
				$part["part_major"]=getrowval("SELECT * FROM part_param WHERE param_id=".$part["part_major"],"param_uin");
				if(!empty($new_parts[$part["part_uin"]]["part_major"])) $new_parts[$part["part_uin"]]["part_major"]=getrowval("SELECT * FROM part_param WHERE param_id=".$new_parts[$part["part_uin"]]["part_major"],"param_uin");
			}
			$xident=!check_ident($part,$new_parts[$part["part_uin"]],'part');
			$part_id=$part["part_id"];
			$old_param=getall4($db,"SELECT * FROM part_param WHERE param_part=".$part_id,"param_sname");
			$new_param=getall4($db,"SELECT * FROM part_param WHERE param_part=".$new_parts[$part["part_uin"]]["part_id"],"param_sname");
			if(!empty($new_param)) foreach($new_param AS $sname=>$param){
				if(empty($old_param[$sname])){
					echo '<input type="hidden" name="add_param'.$part["part_id"].'['.$param["param_uin"].']" value="1">';
					$xident=true;
				} else {
					if(!empty($param["param_link"])) $param["param_link"]=getrowval("SELECT * FROM main_table WHERE table_id=".$param["param_link"],"table_uin");
					if(!empty($old_param[$sname]["param_link"])) $old_param[$sname]["param_link"]=getrowval("SELECT * FROM main_table WHERE table_id=".$old_param[$sname]["param_link"],"table_uin");
					//echo $param["param_link"].'<br>'.$old_param[$sname]["param_link"].'<br><br>';
					if(!check_ident($old_param[$sname],$param,'param')){
						echo '<input type="hidden" name="update_param'.$part["part_id"].'['.$param["param_uin"].']" value="1">';
						$xident=true;
					}
				}
			}
			if(!empty($old_param)) foreach($old_param AS $sname=>$param) if(empty($new_param[$sname])){
				echo '<input type="hidden" name="remove_param'.$part["part_id"].'['.$param["param_uin"].']" value="1">';
				$xident=true;
			}
			if($xident){
				if($part["part_date"]==$part["part_date2"]){
					echo '<input type="hidden" name="update_part['.$part["part_id"].']" value="1">';
				} else {
					if($first){
						echo '<h2>Обновить части</h2>';
						$first=false;
					}
					echo '<div><input type="checkbox" class="checkbox" name="update_part['.$part["part_id"].']" checked> '.$part["part_name"].'</div>';
				}				
			}
		}		
		
		//Запрос на упразднение групп
		$old_groups=getall4($db,"SELECT * FROM main_auth WHERE group_module=$id","group_uin");
		$new_groups=getall4($db,"SELECT * FROM main_auth WHERE group_module=$sid","group_uin");
		$first=true;
		if(!empty($old_groups)) foreach($old_groups AS $group) if(empty($new_groups[$group["group_uin"]])){
			if($first) echo '<h2>Удалить упразднённые группы доступа</h2>';
			$first=false;
			echo '<div><input type="checkbox" class="checkbox" name="remove_group['.$group["group_id"].']" checked> '.$group["group_name"].'</div>';
		}
		//Добавление новых групп
		if(!empty($new_groups)) foreach($new_groups AS $group) if(empty($old_groups[$group["group_uin"]])){
			echo '<input type="hidden" name="add_group['.$group["auth_id"].']" value="1">';
		} else {
			//Принудительное изменение существующих групп
			echo '<input type="hidden" name="update_group['.$group["auth_id"].']" value="1">';
		}
		
		//Упразднение не нужных подтаблиц	
		$all_tbls=getall3($db,"SELECT * FROM main_table WHERE table_module=$id","table_id");
		$first=true;
		if(!empty($all_tbls)){
			$all_tbls=implode(',',$all_tbls);
			$osubs=getall($db,"SELECT * FROM table_sub WHERE sub_table1 IN ($all_tbls)");
			if(!empty($osubs)) foreach($osubs AS $osub){
				$owner=getrowval("SELECT * FROM main_table WHERE table_id=".$osub["sub_table1"],"table_uin");
				$owner=getrowval("SELECT * FROM main_table WHERE table_uin='$owner' AND table_id!=".$osub["sub_table1"],"table_id");
				$sub=getrowval("SELECT * FROM main_table WHERE table_id=".$osub["sub_table2"],"table_uin");
				$sub=getrowval("SELECT * FROM main_table WHERE table_uin='$sub' AND table_id!=".$osub["sub_table2"],"table_id");
				if(!empty($sub) && !empty($owner) && !getrowval("SELECT * FROM table_sub WHERE sub_table1=$owner AND sub_table2=$sub","sub_id")){
					if($first) echo '<h2>Удалить упразднённые связки между таблицами (данные в подтаблицах будут удалены)</h2>';
					$first=false;
					$name='Подтаблица "'.getrowval("SELECT * FROM main_table WHERE table_id=".$osub["sub_table2"],"table_name").'"';
					$name.=' для таблицы "'.getrowval("SELECT * FROM main_table WHERE table_id=".$osub["sub_table1"],"table_name").'"';
					echo '<div><input type="checkbox" class="checkbox" name="remove_sub['.$osub["sub_id"].']" checked> '.$name.'</div>';
				}
			}
		}
		
		//Запрос на догрузку компонентов частей
		//$module_parts=getrowval("SELECT * FROM main_module WHERE module_id=$sid","module_parts");
		$module_parts=loadserv($GLOBALS["update_server"].'?type=mod_part_links&uin='.$uin);
		if(!empty($module_parts)){
			$module_parts=unserialize($module_parts);
			$need_parts=Array();
			$need_parts2=Array();
			$parts_data=Array();
			$tmp=explode('///',loadserv($GLOBALS["update_server"].'?type=part_list'));
			if(!empty($tmp)) foreach($tmp AS $t){
				$t=explode('|||',$t);
				$parts_data[$t[2]]->id=$t[4];
				$parts_data[$t[2]]->name=$t[1];
			}
			if(!empty($module_parts)) foreach($module_parts AS $type=>$needles)
					if(!empty($needles)) foreach($needles AS $part_sname)if(empty($need_parts[$part_sname]) && !empty($parts_data[$part_sname])){
						getrow($db,"SELECT part_sname, part_proc, part_type FROM main_part WHERE part_type=2 AND part_proc=$type AND part_sname='$part_sname'");
						if(empty($db->Record)){
							$need_parts2[$part_sname]=$parts_data[$part_sname]->id;
							$need_parts[$part_sname]=$parts_data[$part_sname]->name;
						}
					}
			if(!empty($need_parts)){
				echo '<h2>Загрузка дополнительных компонентов</h2>';
				foreach($need_parts AS $part_sname=>$part_name){
					$part_id=$need_parts2[$part_sname];
					echo '<div><input type="checkbox" class="checkbox" name="upload_component['.$part_id.']" checked> '.$part_name.'</div>';
				}
			}
		}
		
		
		echo '<input type="submit" value="Завершить обновление модуля" class="button">  или <a href="mod_update?id='.$id.'&amp;sid='.$sid.'&amp;action=remove">вернуться назад</a>
		<br><p><b>Внимание!</b> Если закрыть данную страницу на этом этапе возможна нестабильная работа модуля. Если Вы хотите завершить обновление - нажмите кнопку, если хотите отказаться от него - нажмите на ссылку "вернуться назад".</p>
		</form>';
	}
}

if($action=='remove' && !empty($sid)){
	echo '<h2>Обновление модуля прервано</h2>';
	echo '<div><a href="'.$zone_url.'/modules">Вернуться в список модулей</a></div>';
	$del_m=getall($db,"SELECT * FROM main_module WHERE module_sname LIKE '%_for_uninstall'");
	if(!empty($del_m)) foreach($del_m AS $dm) del_module($dm["module_id"]);
}

if($action=='update' && !empty($id) && !empty($GLOBALS["update_server"]) && check_mod($id,'edit') && !empty($uin) && !empty($sid)){
	//Переносим права для группы "по умолчанию"
	del_current_perm(0,$id,false);
	//replace_current_perm($sid,$id,0);
	
	//Обновляем дату обновления и изменения модуля (первую с сервера, вторую - текущую)
	$m_date=explode(';',loadserv($GLOBALS["update_server"].'?type=modates&uins='.$uin));
	$m_date=$m_date[1];
	$db->query("UPDATE main_module SET module_date='".$m_date."' WHERE module_id=$id");
	$db->query("UPDATE main_module SET module_date2='".date('Y-m-d H:i:s')."' WHERE module_id=$id");
	$db->query("UPDATE main_module SET module_lastcheck='".date('Y-m-d')."' WHERE module_id=$id");
	
	//Обновляем модуль
	do_ident(getrow($db,"SELECT * FROM main_module WHERE module_id=$sid"),$id,"edit","module");
	
	//Удаляем таблицы
	if(!empty($_POST["remove_tbl"])) foreach($_POST["remove_tbl"] AS $del_id=>$checked) if($checked) del_table($del_id,1);
	
	//Добавляем таблицы
	$tbl_major=Array();
	$first=true;
	if(!empty($_POST["add_tbl"])) foreach($_POST["add_tbl"] AS $add_id=>$checked) if($checked){
		if($first){
			$first=false;
			$exm=getall($db,"SELECT * FROM ex_module WHERE ex_module=$id",1,'ex_module');
		}
		$tbl=getrow($db,"SELECT * FROM main_table WHERE table_id=$add_id");
		$x=do_ident($tbl,0,"add","table",$id);
		if(!empty($tbl["major_col"])) $tbl_major[$x]=getrowval("SELECT col_id FROM main_col WHERE col_id=".$tbl["major_col"],"col_uin");
		//$new_tbl[getrowval("SELECT table_id,table_uin FROM main_table WHERE table_id=$add_id","table_uin")]=$x;
		
		//Добавляем экземпляры таблиц
		if($tbl['table_extype']==2 || count($exm)<=1){
			$db->query("INSERT INTO ex_table (ex_table, ex_module, ex_name)
					VALUES ($x, $id, 'основная')",3,'ex_table');
			getrow($db,"SELECT LAST_INSERT_ID() as sid2");
			$sid2=$db->Record["sid2"];	
			foreach($exm AS $ex){
				$db->query("INSERT INTO ex_group (ex_module, ex_table, ex_ex1, ex_ex2)
						VALUES ($id, $x, $sid2, ".$ex["ex_id"].")",3,'ex_group');
			}
		} else {
			foreach($exm AS $ex){
				$db->query("INSERT INTO ex_table (ex_table, ex_module, ex_name)
						VALUES ($x, $id, 'для «".$ex["ex_name"]."»')",3,'ex_table');
				getrow($db,"SELECT LAST_INSERT_ID() as sid2");
				$sid2=$db->Record["sid2"];	
				$db->query("INSERT INTO ex_group (ex_module, ex_table, ex_ex1, ex_ex2)
						VALUES ($id, $x, $sid2, ".$ex["ex_id"].")",3,'ex_group');
			}		
		}
	}
	
	//Удаляем столбцы
	if(!empty($_POST["remove_col"])) foreach($_POST["remove_col"] AS $col_id=>$checked) if($checked) del_col($col_id);

	//Добавляем столбцы
	$col_part=Array();
	$col_param=Array();
	if(!empty($_POST["add_col"])) foreach($_POST["add_col"] AS $col_id=>$table_uin){
		$col=getrow($db,"SELECT * FROM main_col WHERE col_id=$col_id");
		if(!empty($col["col_link"])){
			$l=getrowval("SELECT * FROM main_table WHERE table_id=".$col["col_link"],'table_uin');
			if(!empty($l)) $l=getrowval("SELECT * FROM main_table WHERE table_uin='$l' AND table_module!=$sid","table_id");
			else $l=0;
			$col["col_link"]=$l;
			//если col_link небыл найден, нужно делать запрос на сервер за дообновлением нужной таблицы
		}
		if($table_uin!=-1) $table_id=getrowval("SELECT table_id,table_uin,table_module FROM main_table WHERE table_uin='$table_uin' AND table_module=$id","table_id");
		else $table_id=0;//параметр экземпляра
		$x=do_ident($col,0,"add","col",$id,$table_id);
		if(!empty($col["col_part"])) $col_part[$x]=getrowval("SELECT * FROM main_part WHERE part_id=".$col["col_part"],"part_uin");
		if(!empty($col["col_paramlink"])) $col_param[$x]=getrowval("SELECT * FROM part_param WHERE param_id=".$col["col_paramlink"],"param_uin").'|'.$col["col_paramlink"];
		//$new_col[getrowval("SELECT col_id,col_uin FROM main_col WHERE col_id=$col_id","col_uin")]=
	}
	
	//Изменяем столбцы
	if(!empty($_POST["update_col"])) foreach($_POST["update_col"] AS $col_id=>$checked)if($checked){
		$old_col=getrow($db,"SELECT * FROM main_col WHERE col_id=$col_id");
		$col=getrow($db,"SELECT * FROM main_col WHERE col_uin='".$old_col["col_uin"]."' AND col_module=$sid");
		if(!empty($col["col_link"])){
			$l=getrowval("SELECT * FROM main_table WHERE table_id=".$col["col_link"],'table_uin');
			if(!empty($l))
				$l=getrowval("SELECT * FROM main_table WHERE table_uin='$l' AND table_module!=$sid","table_id");
			else $l=0;
			$col["col_link"]=$l;
			//если col_link небыл найден, нужно делать запрос на сервер за дообновлением нужной таблицы
		}
		$table_id=$old_col["col_table"];
		$x=do_ident($col,$old_col["col_id"],"edit","col",$id,$table_id);
		if(!empty($col["col_part"])) $col_part[$x]=getrowval("SELECT * FROM main_part WHERE part_id=".$col["col_part"],"part_uin");
		if(!empty($col["col_paramlink"])) $col_param[$x]=getrowval("SELECT * FROM part_param WHERE param_id=".$col["col_paramlink"],"param_uin").'|'.$col["col_paramlink"];
	}
	
	//Изменяем таблицы
	if(!empty($_POST["update_tbl"])) foreach($_POST["update_tbl"] AS $update_id=>$checked) if($checked){
		$old_tbl=getrow($db,"SELECT * FROM main_table WHERE table_id=$update_id");
		$new_tbl=getrow($db,"SELECT * FROM main_table WHERE table_uin='".$old_tbl["table_uin"]."' AND table_module=$sid");		
		$x=do_ident($new_tbl,$old_tbl["table_id"],"edit","table",$id);
		if(!empty($new_tbl["major_col"])){
			$tbl_major[$x]=getrowval("SELECT col_id,col_uin FROM main_col WHERE col_id=".$new_tbl["major_col"],"col_uin");
			//echo $x.' = '.$new_tbl["major_col"].' = '.$tbl_major[$x].'<br>';
		}
	}
	
	//Удаляем части
	if(!empty($_POST["remove_part"])) foreach($_POST["remove_part"] AS $part=>$checked) if($checked) del_part($part);
	
	//Добавляем части
	$part_table=Array();
	$part_owner=Array();
	$part_major=Array();
	if(!empty($_POST["add_part"])) foreach($_POST["add_part"] AS $part=>$checked) if($checked){
		$new_part=getrow($db,"SELECT * FROM main_part WHERE part_id=$part");
		$x=do_ident($new_part,0,"add","part",$id);
		if(!empty($new_part["part_table"])) $part_table[$x]=getrowval("SELECT * FROM main_table WHERE table_id=".$new_part["part_table"],"table_uin");
		if(!empty($new_part["part_owner"])) $part_owner[$x]=getrowval("SELECT * FROM main_part WHERE part_id=".$new_part["part_owner"],"part_uin");
		if(!empty($new_part["part_major"])) $part_major[$x]=getrowval("SELECT * FROM part_param WHERE param_id=".$new_part["part_major"],"param_uin");
	}
	
	//Изменяем части
	if(!empty($_POST["update_part"])) foreach($_POST["update_part"] AS $part=>$checked) if($checked){
		$opart=getrow($db,"SELECT * FROM main_part WHERE part_id=$part");
		$new_part=getrow($db,"SELECT * FROM main_part WHERE part_uin='".$opart["part_uin"]."' AND part_module=$sid");
		$x=do_ident($new_part,$part,"edit","part",$id);
		if(!empty($new_part["part_table"])) $part_table[$x]=getrowval("SELECT * FROM main_table WHERE table_id=".$new_part["part_table"],"table_uin");
		if(!empty($new_part["part_owner"])) $part_owner[$x]=getrowval("SELECT * FROM main_part WHERE part_id=".$new_part["part_owner"],"part_uin");
		if(!empty($new_part["part_major"])) $part_major[$x]=getrowval("SELECT * FROM part_param WHERE param_id=".$new_part["part_major"],"param_uin");
		//файлы в папках обновляются в момент загрузки модуля на предыдущем этапе
	}
	
	//Параметры частей	
	$param_link=Array();
	$nparts=getall4($db,"SELECT * FROM main_part WHERE part_module=$sid","part_id");
	if(!empty($nparts)) foreach($nparts AS $npart_id=>$npart){
		$part_id=getrowval("SELECT * FROM main_part WHERE part_uin='".$npart["part_uin"]."' AND part_module=$id","part_id");
		//Добавляем параметры частей созданных
		if(!empty($_POST["add_param".$part_id])) foreach($_POST["add_param".$part_id] AS $param_uin=>$checked){
			$param=getrow($db,"SELECT * FROM part_param WHERE param_uin='".$param_uin."' AND param_part=$npart_id");
			$x=do_ident($param,0,"add","param",$id,$part_id);
			if(!empty($param["param_link"])) $param_link[$x]=getrowval("SELECT * FROM main_table WHERE table_module!=$id AND table_id=".$param["param_link"],"table_uin");
		}
		if(!empty($_POST["add_param".$npart_id])) foreach($_POST["add_param".$npart_id] AS $param_uin=>$checked){
			$param=getrow($db,"SELECT * FROM part_param WHERE param_uin='".$param_uin."' AND param_part=$npart_id");
			$x=do_ident($param,0,"add","param",$id,$part_id);
			if(!empty($param["param_link"])) $param_link[$x]=getrowval("SELECT * FROM main_table WHERE table_module!=$id AND table_id=".$param["param_link"],"table_uin");
		}
	}	
	$parts=getall4($db,"SELECT * FROM main_part WHERE part_module=$id","part_id");
	if(!empty($parts)) foreach($parts AS $part_id=>$part){
		//Обновляем параметры частей
		$npart_id=getrowval("SELECT * FROM main_part WHERE part_module=$sid AND part_uin='".$part["part_uin"]."'","part_id");
		if(!empty($_POST["update_param".$part_id])) foreach($_POST["update_param".$part_id] AS $param_uin=>$checked){
			testfile($param_uin.' = '.$npart_id.' = '.$part_id);		
			$nparam=getrow($db,"SELECT * FROM part_param WHERE param_uin='".$param_uin."' AND param_part=$npart_id");
			$param=getrowval("SELECT * FROM part_param WHERE param_uin='".$param_uin."' AND param_part=$part_id",'param_id');
			$x=do_ident($nparam,$param,"edit","param",$id,$part_id);
			if(!empty($nparam["param_link"])) $param_link[$x]=getrowval("SELECT * FROM main_table WHERE table_module!=$id AND table_id=".$nparam["param_link"],"table_uin");
		}
		//Удаление параметров частей
		if(!empty($_POST["remove_param".$part_id])) foreach($_POST["remove_param".$part_id] AS $param_uin=>$checked){
			$db->query("DELETE FROM part_param WHERE param_part=$part_id AND param_uin!='' AND param_uin='$param_uin'");
		}
	}
	//Связываем param_link
	if(!empty($param_link)) foreach($param_link AS $param_id=>$table_uin){
		$table_id=getrowval("SELECT * FROM main_table WHERE table_uin='$table_uin' AND table_module!=$sid","table_id");
		if(!empty($table_id)){
			$db->query("UPDATE part_param SET param_link=".$table_id." WHERE param_id=$param_id");
		}
	}	

	//Связываем col_major с столбцами
	if(!empty($tbl_major))foreach($tbl_major AS $table_id=>$col_uin){
		$col_id=getrowval("SELECT * FROM main_col WHERE col_uin='$col_uin' AND col_table=$table_id","col_id");
		if(empty($col_id)) $col_id='0';
		$db->query("UPDATE main_table SET major_col=$col_id WHERE table_id=$table_id");
	}
	
	//Упразднение групп доступа
	if(!empty($_POST["remove_group"])) foreach($_POST["remove_group"] AS $group_id=>$checked) if($checked) del_group($group_id);
	
	//Добавление новых групп доступа
	if(!empty($_POST["add_group"])) foreach($_POST["add_group"] AS $ngroup_id=>$checked) if ($checked){
		$group=getrow($db,"SELECT * FROM main_auth WHERE auth_id=$ngroup_id");
		$x=do_ident($group,0,"add","group",$id);
		add_perms($sid,$id,$ngroup_id,$x);
	}
	//Изменение существующих групп доступа
	if(!empty($_POST["update_group"])) foreach($_POST["update_group"] AS $ngroup_id=>$checked) if ($checked){
		$group=getrow($db,"SELECT * FROM main_auth WHERE auth_id=$ngroup_id");
		$ogroup=getrowval("SELECT * FROM main_auth WHERE group_uin='".$group["group_uin"]."' AND group_module!=$sid","auth_id");
		$x=do_ident($group,$ogroup,"edit","group",$id);
		del_current_perm($x,$id,false);
		add_perms($sid,$id,$group["auth_id"],$x);
	}
	replace_current_perm($sid,$id,0);
		
	//Проверить связки таблиц
	$all_tbls=getall3($db,"SELECT * FROM main_table WHERE table_module=$sid","table_id");
	if(!empty($all_tbls)){
		$all_tbls=implode(',',$all_tbls);
		$nsubs=getall($db,"SELECT * FROM table_sub WHERE sub_table1 IN ($all_tbls)");
		if(!empty($nsubs)) foreach($nsubs AS $nsub){
			$owner=getrowval("SELECT * FROM main_table WHERE table_id=".$nsub["sub_table1"],"table_uin");
			$owner=getrowval("SELECT * FROM main_table WHERE table_uin='$owner' AND table_id!=".$nsub["sub_table1"],"table_id");
			$sub=getrowval("SELECT * FROM main_table WHERE table_id=".$nsub["sub_table2"],"table_uin");
			$sub=getrowval("SELECT * FROM main_table WHERE table_uin='$sub' AND table_id!=".$nsub["sub_table2"],"table_id");
			if(!empty($owner) && empty($sub)){
				//Для связей с другими модулями
				$sub=getrow($db,"SELECT * FROM main_table WHERE table_id=".$nsub["sub_table2"]);
				if($sub['table_module']!=$sid && $sub['table_module']!=$id){
					$sub=$sub['table_id'];
				}
			}
			if(!empty($sub) && !empty($owner)){
				if(!getrowval("SELECT * FROM table_sub WHERE sub_table1=$owner AND sub_table2=$sub","sub_id")){
					$db->query("INSERT INTO table_sub (sub_table1,sub_table2) VALUES ($owner,$sub)");
				}
			}
		}
	}
	//Удаление ненужных связок таблиц
	if(!empty($_POST["remove_sub"])) foreach($_POST["remove_sub"] AS $sub_id=>$checked) if($checked){
		del_sub($sub_id);
	}	
	
	//Синхронизация col_part через col_part
	if(!empty($col_part)) foreach($col_part AS $col_id=>$part_uin){
		$col_part=getrowval("SELECT * FROM main_part WHERE part_uin='$part_uin' AND part_module!=$sid","part_id");
		$db->query("UPDATE main_col SET col_part=$col_part WHERE col_id=$col_id");	
	}
	//Синхронизация col_paramlink через col_param
	if(!empty($col_param)) foreach($col_param AS $col_id=>$param_uin){
		$param_uin=explode('|',$param_uin);
		$col_paramlink=getrowval("SELECT * FROM part_param WHERE param_uin='".$param_uin[0]."' AND param_id!=".$param_uin[1],"part_id");
		if(!empty($col_paramlink)) $db->query("UPDATE main_col SET col_paramlink=$col_paramlink WHERE col_id=$col_id");	
	}
	//Синхронизация part_table
	if(!empty($part_table)) foreach($part_table AS $part_id=>$table_uin){
		$table_id=getrowval("SELECT * FROM main_table WHERE table_uin='".$table_uin."' AND table_module!=$sid","table_id");
		$db->query("UPDATE main_part SET part_table=".$table_id." WHERE part_id=$part_id");
	}
	//Синхронизация part_major
	if(!empty($part_major)) foreach($part_major AS $part_id=>$param_uin){
		$param_id=getrowval("SELECT * FROM part_param WHERE param_uin='".$param_uin."' AND param_part=$part_id","param_id");
		if(!empty($param_id)){
			$db->query("UPDATE main_part SET part_major=".$param_id." WHERE part_id=$part_id");
		}
	}
	//Синхронизация part_owner
	if(!empty($part_owner)) foreach($part_owner AS $part_id=>$part_uin){
		$part_id2=getrowval("SELECT * FROM main_part WHERE part_uin='".$part_uin."' AND part_module!=$sid","part_id");
		$db->query("UPDATE main_part SET part_owner=".$part_id2." WHERE part_id=$part_id");
	}
	
	//Загружаем дополнительные компоненты
	if(!empty($_POST["upload_component"])) foreach($_POST["upload_component"] AS $var=>$value) if(!empty($value)){
		import_part($var,0);
	}
	
	//Удаляем ненужный модуль
	del_module($sid);
	
	//Обновляем дату нашего модуля
	$db->query("UPDATE main_module SET module_date='".date('Y-m-d H:i:s')."' WHERE module_id=$id");
	
	echo '<h2>Если вы не видите никаких ошибок - модуль успешно обновлён</h2>';
	global $zone_url;
	echo '<p><a href="'.$zone_url.'/modules">Вернуться назад к списку модулей</a></p>';
}

?>