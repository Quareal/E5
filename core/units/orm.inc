<?php

class rlink{
	var $owner=0;
	var $owners=Array();
	var $ownert=0;
	var $rsub=0;
	var $rid=0;
	var $id=0;
	var $tex=0;
	var $table=0;
	var $module=0;
	var $pos=0;
	var $enable=true;
	//var $sub=Array();
	var $user=0;
	var $users=Array();
}

function seek_rlink2($ro){
	global $db,$rolink;
	getrow($db,"SELECT * FROM row_owner WHERE ro_id=$ro",1,"row_owner");
	if(!empty($db->Record["row_id"])){
		$rolink[$ro]=$db->Record["row_id"];
		seek_rlink($db->Record["row_id"]);
	}
}

function get_rlink($row){
	global $rlink;
	if(empty($row)){//опасно
		$x->id=0;
		$x->table=0;
		return $x;
	}
	if(is_object($row) && isset($row->id)) $row=$row->id;
	seek_rlink($row);
	if(is_array($row) && isset($row["row_id"])) $row=$row["row_id"];
	if(!is_numeric($row)) return false;
	if(isset($rlink[$row])) return $rlink[$row]; else return false;
}

function seek_rlink($row,$force=0,$ignore_sub=0){
	global $db,$rlink,$tables,$rolink;
	prep_tables();
	if(empty($row)){ return;}
	if(is_array($row) || (is_numeric($row) && !isset($rlink[$row])) || $force!=0){
		if(is_array($row) && !isset($row["row_id"])) return false;
		if(is_array($row)){
			$db->Record=$row;
			$row=$row["row_id"];
		} else {
			//getrow($db,"SELECT row_id,row_enable FROM main_row WHERE row_id=$row",1,"main_row");
			global $rlink_cache;
			if(!isset($rlink_cache[$row]) || $force) {			
				if(is_numeric($row)){//тут какой-то мистический косяк... из-за которого не загружались фотографии. видимо это связано с методом =>

					getrow($db,"SELECT * FROM row_owner WHERE row_id=$row",1,"row_owner",__LINE__,__FILE__);//данный метод не будет определять множественных родителей строки
																										//эта вторая по затратности строка-запрос
					//если ROW отсутствует в списках зависимости (например, в Backrow), то ищем в main_row
					if(empty($db->Record)){
						getrow($db,"SELECT * FROM main_row WHERE row_id=$row",1,"main_row",__LINE__,__FILE__);
						if(!empty($db->Record)){
							$db->Record["ro_enable"]=$db->Record["row_enable"];
							$db->Record["owner_id"]=0;
							$db->Record["owner_table"]=0;
							$db->Record["ro_id"]=0;
							$db->Record["ro_ex"]=$db->Record["row_ex"];
							$db->Record["ro_pos"]=0;
							$db->Record["ro_sub"]=$db->Record["row_sub"];
							$db->Record["ro_users"]=0;
							$db->Record["ro_user"]=$db->Record["row_user"];
						}
					}
					$rlink_cache[$row]=$db->Record;
				}
			} else $db->Record=$rlink_cache[$row];
			global $no_cache;
			if($no_cache) unset($rlink_cache);
		}
		$rlink[$row]=new rlink();
		if(!empty($db->Record)){
			$ro=$db->Record;
			$rlink[$row]->enable=$ro["ro_enable"];
			$rlink[$row]->owner=$ro["owner_id"];//$own_link;
			$rlink[$row]->owners[$ro["ro_id"]]=$ro["owner_id"];
			$rlink[$row]->ownert=$ro["owner_table"];//$own_link;
			$rlink[$row]->rid=$ro["ro_id"];
			$rolink[$ro["ro_id"]]=$row;
			$rlink[$row]->id=$ro["row_id"];
			$rlink[$row]->tex=$ro["ro_ex"];
			$rlink[$row]->table=$ro["row_table"];
			$rlink[$row]->type='row';
			$rlink[$row]->module=$ro["row_module"];
			$rlink[$row]->pos=$ro["ro_pos"];
			$rlink[$row]->rsub=$ro["ro_sub"];
			$rlink[$row]->user=$ro["ro_user"];
			$rlink[$row]->enable=$ro["ro_enable"];//$rcache2[$atable_id][$ro["row_id"]];
			$rlink[$row]->users=Array();
			if(!empty($ro["ro_users"])){//это потом надо будет закешировать
				$usrs=getall($db,"SELECT * FROM row_user WHERE ru_row=".$rlink[$row]->id,1,"row_user");
				if(!empty($usrs)) foreach($usrs AS $usr) $rlink[$row]->users[$usr["ru_user"]]=$usr["ru_user"];
			}
			$ppro[$rlink[$row]->id]=$rlink[$row]->table;
			
			// ВНИМАНИЕ!
			// в случае ЧП необходимо
			// обратить внимание на то, что теперь отменена подгрузка дочерних подтаблиц для строк
			// ...
			// естественно всё начало косипорить в админке
			// начало SUB-а
			// ...
			// теперь ваще всё виснет... короче сношу опять
			/*if(!$ignore_sub && !empty($tables[$rlink[$row]->table]->multy) && $tables[$rlink[$row]->table]->multy==1){
				global $ex2cache,$ex2cache2;
				if(!isset($ex2cache[$rlink[$row]->table][$rlink[$row]->tex]) && !empty($rlink[$row]->table)){//неведомый косяк. оказывается иногда, каким-то образом (при selpart.cow) оно может быть пустым...
					getrow($db,"SELECT * FROM ex_group WHERE ex_table=".$rlink[$row]->table." AND ex_ex1=".$rlink[$row]->tex,1,"ex_group",__LINE__,__FILE__);
					if(empty($db->Record["ex_ex2"])) $db->Record["ex_ex2"]=0;
					$ex2cache[$rlink[$row]->table][$rlink[$row]->tex]=$db->Record["ex_ex2"];
					if(!empty($db->Record["ex_ex2"])) $ex2cache2[$rlink[$row]->table][$rlink[$row]->tex]=$db->Record["ex_ex2"];//на свой страх и риск
					//if(!isset($ex2cache[$rlink[$row]->table][$rlink[$row]->tex])) $ex2cache[$rlink[$row]->table][$rlink[$row]->tex]=0;
				}
				$ex2=$ex2cache[$rlink[$row]->table][$ro["ro_ex"]];
				//если тут убрать seek_ex то кол-во запросов возрастёт в тысячи раз (кол-во затронутых рядов и соответственно кол-во оперативной памяти)
				
				//тут параметр задаётся отрицательным, чтобы понять что запрос идёт именно из rlink
				//$rlink[$row]->sub=get_sub(-$ro["row_id"],$rlink[$row]->table,1,1,0,0,0,$ro["ro_ex"],$ex2,$rlink[$row]->table);				
				
				$rlink[$row]->sub=get_sub(-$ro["row_id"],$rlink[$row]->table,1,1,0,0,0,$ro["ro_ex"],$ex2,$rlink[$row]->table);
			}*/
			//КОНЕЦ SUB-а
			
			
			
			//if(!$ignore_sub && !empty($tables[$rlink[$row]->table]->multy) && $tables[$rlink[$row]->table]->multy==1) $rlink[$row]->sub=get_sub(-$ro["row_id"],$rlink[$row]->table,1,1/*? =0*/,0,0,/*1*/0,$ro["ro_ex"],$ex2,$rlink[$row]->table);
			
			// последнее было это $rlink[$row]->sub=get_sub($ro["row_id"],$rlink[$row]->table,1,1/*? =0*/,0,0,1,$ro["ro_ex"],$ex2,$rlink[$row]->table);
			//$rlink[$row]->sub=get_sub($ro["row_id"],$atable_id,$seek_enable,$seek_ex,$ignore_table/*,$res[$i]*/,$own_link,$seek_subt);
		} else $rlink[$row]->enable=-1;
	}
}

function del_module($id){
	global $db;
	getrow($db,"SELECT * FROM main_module WHERE module_id=$id");
	
	$module_icon=$db->Record['module_icon'];
	if(!empty($module_icon) && file_exists(DOCUMENT_ROOT.$module_icon)){
		$emi=getrowval("SELECT module_icon FROM main_module WHERE module_icon='".$module_icon."' AND module_id!=$id","module_icon");
		if(empty($emi)) unlink(DOCUMENT_ROOT.$module_icon);
	}	
	
	$db->query("DELETE FROM main_module WHERE module_id=$id",3,'main_module',__LINE__,__FILE__);
	$db->query("DELETE FROM ex_module WHERE ex_module=$id",3,'ex_module',__LINE__,__FILE__);
	$db->query("DELETE FROM ex_table WHERE ex_module=$id",3,'ex_table',__LINE__,__FILE__);
	$db->query("DELETE FROM ex_group WHERE ex_module=$id",3,'ex_group',__LINE__,__FILE__);	
	$db->query("DELETE FROM ex_zone WHERE ex_module2=$id",3,'ex_zone',__LINE__,__FILE__);	
	$tbl=getall($db,"SELECT * FROM main_table WHERE table_module=$id",1,'main_table',__LINE__,__FILE__);
	if(!empty($tbl)) foreach($tbl AS $tb) del_table($tb["table_id"],1,$id);
	$tco=getall($db,"SELECT * FROM main_col WHERE col_module=$id AND col_table=0",1,'main_col',__LINE__,__FILE__);
	if(!empty($tсo)) foreach($tсo AS $tс) del_col($tc["col_id"]);
	del_vals(" value_module=$id AND value_table=0");
	//$db->query("DELETE FROM row_value WHERE value_module=$id AND value_table=0",3,'row_value',__LINE__,__FILE__);
	$db->query("DELETE FROM main_col WHERE col_table=0 AND col_module=$id",3,'main_col',__LINE__,__FILE__);
	$prt=getall($db,"SELECT * FROM main_part WHERE part_module=$id",1,'main_part',__LINE__,__FILE__);
	if(!empty($prt)) foreach($prt AS $pr) del_part($pr["part_id"]);
	$grps=getall($db,"SELECT * FROM main_auth WHERE group_module=$id",1,'main_auth',__LINE__,__FILE__);
	if(!empty($grps)) foreach($grps AS $grp) del_group($grp["auth_id"]);
	//$db->query("UPDATE main_auth SET group_module=0 WHERE group_module=$id",3,"main_auth",__LINE__,__FILE__);
	del_current_perm(0,$id);
}

function del_table($id,$data=1,$module_id=0){//data = удалять данные (1 - да, 0 - нет)
	global $db;
	
	if(empty($module_id)) $module_id=getrowval("SELECT table_id, table_uin,table_module FROM main_table WHERE table_id=$id","table_module");
	$table_uin=getrowval("SELECT table_id, table_uin FROM main_table WHERE table_id=$id","table_uin");
	
	getrow($db,"SELECT * FROM main_table WHERE table_id=$id");
	$table_icon=$db->Record['table_icon'];
	if(!empty($table_icon) && file_exists(DOCUMENT_ROOT.$table_icon)){
		$emi=getrowval("SELECT table_icon FROM main_table WHERE table_icon='".$table_icon."' AND table_id!=$id","table_icon");
		if(empty($emi)) unlink(DOCUMENT_ROOT.$table_icon);
	}
	
	if(!empty($module_id)){
		del_part_links($module_id,'table_onedit',$table_uin);
		del_part_links($module_id,'table_top',$table_uin);
		del_part_links($module_id,'table_bottom',$table_uin);	
	}
	
	$db->query("DELETE FROM main_table WHERE table_id=$id",3,'main_table',__LINE__,__FILE__);
	$tco=getall($db,"SELECT * FROM main_col WHERE col_table=$id",1,'main_col',__LINE__,__FILE__);
	if(!empty($tсo)) foreach($tсo AS $tс){
		del_col($tc["col_id"]);
	}
	$db->query("DELETE FROM main_col WHERE col_table=$id",3,'main_col',__LINE__,__FILE__);
	if($data==1){
		$tro=getall($db,"SELECT * FROM main_row WHERE row_table=$id",1,'main_row',__LINE__,__FILE__);
		if(!empty($tro)) foreach($tro AS $tr) del_row($tr["row_id"]);
	}
	$db->query("DELETE FROM table_sub WHERE sub_table2=$id",3,'table_sub',__LINE__,__FILE__);
	$db->query("DELETE FROM table_sub WHERE sub_table1=$id",3,'table_sub',__LINE__,__FILE__);
	$db->query("DELETE FROM ex_table WHERE ex_table=$id",3,"ex_table",__LINE__,__FILE__);
	$db->query("DELETE FROM ex_group WHERE ex_table=$id",3,"ex_table",__LINE__,__FILE__);
}

function del_ex($id){
	global $db;
	$db->query("DELETE FROM ex_module WHERE ex_id=$id",3,'ex_module',__LINE__,__FILE__);
	$db->query("DELETE FROM ex_zone WHERE ex_module=$id",3,'ex_module',__LINE__,__FILE__);
	$exe=getall($db,"SELECT * FROM ex_group WHERE ex_ex2=$id",1,'ex_group',__LINE__,__FILE__);
	$db->query("DELETE FROM ex_group WHERE ex_ex2=$id",3,'ex_group',__LINE__,__FILE__);
	if(!empty($exe)) foreach($exe AS $ex){
		//Проверка на использование экземпляра таблиц в другом экземпляре модуля
		getrow($db,"SELECT * FROM ex_group WHERE ex_ex1=".$ex["ex_ex1"]." AND ex_ex2!=$id",1,'ex_group',__LINE__,__FILE__);
		if(empty($db->Record)){
			del_table_ex($ex["ex_ex1"]);
		}
	}
	del_vals(" value_row=$id AND value_table=0");
	
	//Удаление ссылок на экземпляр
	$cols=getall3($db,"SELECT col_id, col_type FROM main_col WHERE col_type=4","col_id");
	if(!empty($cols)) $vals=getall4($db,"SELECT value_value, value_col, value_id FROM row_value WHERE value_col IN (".implode(',',$cols).") AND value_value!=''","value_id");
	$sv=Array();
	if(!empty($vals)) foreach($vals AS $val){
		$t=explode(':',$val["value_value"]);
		if(isset($t[1]) && $t[1]==$id) $sv[$val["value_id"]]=$val["value_id"];
	}
	if(!empty($sv)){		
		$db->query("UPDATE row_value SET value_value='' WHERE value_id IN (".implode(',',$sv).")");
	}
}

function del_table_ex($id){
	global $db;
	$db->query("DELETE FROM ex_table WHERE ex_id=$id",3,'ex_table',__LINE__,__FILE__);
	$ro=getall($db,"SELECT * FROM main_row WHERE row_ex=".$id,1,'main_row',__LINE__,__FILE__);
	if(!empty($ro)) foreach($ro AS $r) del_row($r["row_id"]);
	$db->query("DELETE FROM ex_group WHERE ex_ex1=$id",3,'ex_group',__LINE__,__FILE__);
}


function del_row($id,$reset_autoincrement=0){
	global $db;
	if(is_object($id) && isset($id->rows)) $id=$id->rows;
	if(!is_array($id)) $id=Array($id);
	$rows=$id;
	if(empty($rows)) return false;
	foreach($rows AS $r){
		if(is_object($r)) $rows=collect_rows($rows,Array(),true);
		break;
	}
	$ids=implode(',',$rows);
	if(empty($ids)) return false;
		
	del_row_index($rows);
	
	$forc=getall($db,"SELECT owner_id FROM row_owner WHERE row_id IN ($ids) AND owner_id!=0",1,"row_owner");
	if(!empty($forc)) foreach($forc AS $forcc) del_cache('row',$forcc["owner_id"]);
	$chields=getall3($db,"SELECT row_id FROM row_owner WHERE owner_id IN ($ids)",'row_id');
	if(!empty($chields)){
		$exceptions=getall3($db,"SELECT row_id FROM row_owner WHERE row_id IN (".implode(',',$chields).")","row_id");
		if(!empty($exceptions)) foreach($exceptions AS $e) unset($chields[$e]);
	}
	if(!empty($chields)) del_row($chields);
	
	$db->query("DELETE FROM row_owner WHERE row_id IN ($ids)",3,'row_owner',__LINE__,__FILE__);
	$db->query("DELETE FROM row_owner WHERE owner_id IN ($ids)",3,'row_owner',__LINE__,__FILE__);
	$db->query("DELETE FROM main_news WHERE news_row IN ($ids) AND news_row!=0",3,'main_news',__LINE__,__FILE__);
	$db->query("DELETE FROM main_row WHERE row_id IN ($ids)",3,'main_row',__LINE__,__FILE__);
	del_vals(" value_table!=0 AND value_row IN ($ids)");	

	foreach($ids AS $id){
		del_cache('row',$id);
		global $rlink,$gsres, $rcache;
		if(!isset($rlink[$id])) seek_rlink($id);
		//чистим кеш
		if(isset($rlink[$id]->table)){
			if(isset($gsres[$rlink[$id]->owner][$rlink[$id]->table])) unset($gsres[$rlink[$id]->owner][$rlink[$id]->table]);
			if(isset($rcache[$rlink[$id]->table])) unset($rcache[$rlink[$id]->table]);
			if(isset($rcache[-$rlink[$id]->table-10])) unset($rcache[-$rlink[$id]->table-10]);
		}
		unset($rlink[$id]);
	}
	if($reset_autoincrement){
		$db->query("ALTER TABLE main_row AUTO_INCREMENT=".(getrowval("SELECT max(row_id) AS mri FROM main_row","mri")+1));
	}
	
}

/*function del_row_old($id,$reset_autoincrement=0){
	global $db;
	del_row_index($id);
	getrow($db,"SELECT * FROM main_row WHERE row_id=$id",1,"main_row",__LINE__,__FILE__);
	if(empty($db->Record)) return false;//если этого не сделать удалятся вообще все row если строки id не будет существовать, уж не знаю почему...	
	
	$forc=getall($db,"SELECT * FROM row_owner WHERE row_id=$id AND owner_id!=0",1,"row_owner");
	if(!empty($forc)) foreach($forc AS $forcc) del_cache('row',$forcc["owner_id"]);
	$own=getall($db,"SELECT * FROM row_owner WHERE owner_id=$id",1,'row_owner',__LINE__,__FILE__);
	if(!empty($own)) foreach($own AS $ow){
		getrow($db,"SELECT * FROM row_owner WHERE row_id=".$ow["row_id"]." AND owner_id!=".$ow["owner_id"],1,'row_owner',__LINE__,__FILE__);
		if(empty($db->Record)) del_row($ow["row_id"]);
	}
	$db->query("DELETE FROM row_owner WHERE row_id=$id",3,'row_owner',__LINE__,__FILE__);
	$db->query("DELETE FROM row_owner WHERE owner_id=$id",3,'row_owner',__LINE__,__FILE__);
	$db->query("DELETE FROM main_news WHERE news_row=$id AND news_row!=0",3,'main_news',__LINE__,__FILE__);
	$db->query("DELETE FROM main_row WHERE row_id=$id",3,'main_row',__LINE__,__FILE__);
	del_vals(" value_table!=0 AND value_row=$id");	


	
	//по идее тут бы неплохо занулять value все col_link у которых col_type=1 (можно просто удалять) и потом делать del_cache(у занулённого row)
	del_cache('row',$id);
	global $rlink,$gsres, $rcache;
	if(!isset($rlink[$id])) seek_rlink($id);
	//чистим кеш
	if(isset($rlink[$id]->table)){
		if(isset($gsres[$rlink[$id]->owner][$rlink[$id]->table])) unset($gsres[$rlink[$id]->owner][$rlink[$id]->table]);
		if(isset($rcache[$rlink[$id]->table])) unset($rcache[$rlink[$id]->table]);
		if(isset($rcache[-$rlink[$id]->table-10])) unset($rcache[-$rlink[$id]->table-10]);
	}
	if($reset_autoincrement){
		$db->query("ALTER TABLE main_row AUTO_INCREMENT=".(getrowval("SELECT max(row_id) AS mri FROM main_row","mri")+1));
	}
	
	unset($rlink[$id]);
}*/

function del_sub($id){
	global $db;
	getrow($db,"SELECT * FROM table_sub WHERE sub_id=$id",1,'table_sub',__LINE__,__FILE__);
	$sub=$db->Record;
	$db->query("DELETE FROM table_sub WHERE sub_id=$id",3,'table_sub',__LINE__,__FILE__);
	if(!empty($sub["sub_table1"])){
		$rows=getall($db,"SELECT * FROM main_row WHERE row_table=".$sub["sub_table1"],1,'main_row',__LINE__,__FILE__);
		if(!empty($rows)) foreach($rows AS $row){
			$rows2=getall($db,"SELECT * FROM row_owner WHERE owner_table=".$sub["sub_table1"]." AND owner_id=".$row["row_id"]." AND row_table=".$sub["sub_table2"],1,'row_owner',__LINE__,__FILE__);
			if(!empty($rows2)) foreach($rows2 AS $row2) del_row($row2["row_id"]);
		}
	}
}

function del_col($id){
	global $db;

	if(empty($id)) return false;		
	$col=getrow($db,"SELECT col_id, col_uin, col_module, col_table, col_tpl FROM main_col WHERE col_id=$id");
	$col_uin=$col["col_uin"];
	if(!empty($col['col_module'])) $module_id=$col["col_module"]; else $module_id=0;
	if(!empty($col['col_table'])) $table_id=$col["col_table"]; else $table_id=0;
	$col_id=$col["col_id"];
	if(!empty($module_id)){
		del_part_links($module_id,'col_oninsert',$col_uin);
		del_part_links($module_id,'col_onform',$col_uin);
		del_part_links($module_id,'col_onshow',$col_uin);	
	} else if(!empty($table_id)){
		del_part_links($table_id,'col_oninsert',$col_uin,'table');
		del_part_links($table_id,'col_onform',$col_uin,'table');
		del_part_links($table_id,'col_onshow',$col_uin,'table');	
	} else {
		//del_part_links($col_id,'col_oninsert',$col_uin,'col');
		//del_part_links($col_id,'col_onform',$col_uin,'col');
		//del_part_links($col_id,'col_onshow',$col_uin,'col');
	}
	
	del_vals(" value_col=$id");
	//$db->query("DELETE FROM row_value WHERE value_col=$id",3,'row_value',__LINE__,__FILE__);
	remove_col_index($id);
	$db->query("DELETE FROM main_col WHERE col_id=$id",3,'main_col',__LINE__,__FILE__);
	if(!empty($col['col_tpl'])){
		$rows=getall3($db,"SELECT row_id FROM main_row WHERE row_table=$table_id AND row_module=$module_id","row_id");
		if(!empty($rows)) foreach($rows AS $row) del_part_links($row,'val',$col_uin,'row');
	}
}

function del_vals($where){
	if(empty($where)) return'';//на всякий случай
	global $db,$pcl;
	$v=getall($db,"SELECT * FROM row_value WHERE ".$where,1,"row_value",__LINE__,__FILE__);
	if(!empty($v)) foreach($v AS $cv)if(!empty($cv['value_col'])){
		if(!isset($pcl[$cv["value_col"]])){
			getrow($db,"SELECT * FROM main_col WHERE col_id=".$cv["value_col"],1,"main_col",__LINE__,__FILE__);
			$pcl[$cv["value_col"]]=$db->Record;
		}
		$ccol=$pcl[$cv["value_col"]];
		if(!empty($ccol["col_type"]) && $ccol["col_type"]==3 && $cv["value_value"]!='' && file_exists(DOCUMENT_ROOT.$cv["value_value"])){
			del_file($cv["value_value"]);
			//getrow($db,"SELECT count(*) AS cnt FROM row_value WHERE value_value='".$cv["value_value"]."'",1,"row_value");
			//if($db->Record["cnt"]<=1) unlink(DOCUMENT_ROOT.$cv["value_value"]);
		}
	}
	$db->query("DELETE FROM row_value WHERE ".$where,3,"row_value",__LINE__,__FILE__);
}

function del_group($id){
	global $db;
	$subs=getall($db,"SELECT * FROM main_auth WHERE group_owner=$id",1,"main_auth",__LINE__,__FILE__);
	if(!empty($subs)) foreach($subs AS $sub) del_group($sub["auth_id"]);
	$db->query("DELETE FROM auth_link WHERE link_user=$id",3,"auth_link",__LINE__,__FILE__);
	$db->query("DELETE FROM auth_link WHERE link_group=$id",3,"auth_link",__LINE__,__FILE__);
	$db->query("DELETE FROM auth_perm WHERE perm_auth=$id",3,"auth_perm",__LINE__,__FILE__);
	$db->query("DELETE FROM main_auth WHERE auth_id=$id",3,"main_auth",__LINE__,__FILE__);
	$rusers=getall($db,"SELECT * FROM row_user WHERE ru_user=$id",1,"row_user",__LINE__,__FILE__);
	if(!empty($rusers)) foreach($rusers AS $ruser){
		getrow($db,"SELECT * FROM row_user WHERE ru_row=".$ruser["ru_row"]." AND ru_user!=$id",1,"row_user",__LINE__,__FILE__);
		if(!empty($db->Record)) $db->query("UPDATE row_owner SET ro_users=0 WHERE row_id=".$ruser["ru_row"],3,"row_owner",__LINE__,__FILE__);
	}
	$db->query("DELETE FROM row_user WHERE ru_user=$id",3,"row_user",__LINE__,__FILE__);
	$db->query("UPDATE row_owner SET ro_user=0 WHERE ro_user=$id",3,"row_owner",__LINE__,__FILE__);
	$db->query("UPDATE main_row SET row_user=0 WHERE row_user=$id",3,"row_owner",__LINE__,__FILE__);
}

function del_vals_pre($id3,$table_id){
	global /*$col_cv,*/$db,$reset,$pcl;
	// Исключения, чтобы сохранялись файлы при изменении строки
	$exc='';$exc2=Array();
	$v=getall($db,"SELECT * FROM row_value WHERE value_row=$id3 AND value_table=$table_id",1,'row_value',__LINE__,__FILE__);	
	if(!empty($v)) foreach($v AS $cv)if(!empty($cv["value_col"])){
		if(!isset($pcl[$cv["value_col"]])){
			getrow($db,"SELECT * FROM main_col WHERE col_id=".$cv["value_col"],1,"main_col",__LINE__,__FILE__);
			$pcl[$cv["value_col"]]=$db->Record;
		}
		$ccol=$pcl[$cv["value_col"]];
		if(!isset($reset[$ccol["col_id"]])){
			if($ccol["col_type"]==3 && $cv["value_value"]!='' && file_exists(DOCUMENT_ROOT.$cv["value_value"]) && empty($_FILES["col".$ccol["col_id"]]['tmp_name']) && empty($_FILES[$ccol["col_sname"]]['tmp_name']) && empty($_POST["col".$ccol["col_id"]."_serv"]) && $ccol["col_inform"]==1){
				if(!empty($exc)) $exc.=',';
				$exc.=$cv["value_id"];
			}
			/*if($ccol["col_type"]==3){
				if($cv["value_value"]!='') echo '1';
				if(file_exists(DOCUMENT_ROOT.$cv["value_value"])) echo '2';
				if(!empty($_FILES["col".$ccol["col_id"]]['tmp_name'])) echo '3';
				if(!empty($_FILES[$ccol["col_sname"]]['tmp_name'])) echo '4';
				if(!empty($_POST["col".$ccol["col_id"]."_serv"])) echo '5';
				if($ccol["col_inform"]==0) echo '6';
			}*/
			if($ccol["col_type"]==3 && $cv["value_value"]!='' && file_exists(DOCUMENT_ROOT.$cv["value_value"]) && (!empty($_FILES["col".$ccol["col_id"]]['tmp_name']) || !empty($_FILES[$ccol["col_sname"]]['tmp_name']) || !empty($_POST["col".$ccol["col_id"]."_serv"]) || $ccol["col_inform"]==0)){
				if(isset($_FILES["col".$ccol["col_id"]]['tmp_name']) || isset($_FILES[$ccol["col_sname"]]['tmp_name']) || !empty($_POST["col".$ccol["col_id"]."_serv"])){
					//echo '<br>'.$cv["value_value"].'<br>';
					del_file($cv["value_value"]);
					//getrow($db,"SELECT count(*) AS cnt FROM row_value WHERE value_value='".$cv["value_value"]."'",1,"row_value");
					//if($db->Record["cnt"]<=1) unlink(DOCUMENT_ROOT.$cv["value_value"]);
				}
			}
		} else $exc2[]=$ccol["col_id"];
	}
	if(!empty($exc2)) $exc2=" AND NOT (value_col IN (".implode(',',$exc2)."))"; else $exc2='';
	if(!empty($exc)) $exc=" AND NOT (value_id IN ($exc))";//интересная конструкция :)
	$db->query("DELETE FROM row_value WHERE value_row=$id3 AND value_table=$table_id".$exc.$exc2,3,'row_value',__LINE__,__FILE__);
}

function backup_row($src){
	global $db;
	getrow($db,"SELECT * FROM main_row WHERE row_id=$src",1,"main_row",__LINE__,__FILE__);
	if(empty($db->Record)) return ''; else foreach($db->Record AS $var=>$value) $$var=$value;
	$db->query("INSERT INTO main_row (row_module, row_table, row_ex, row_sub, row_enable, backup_row, backup_date)
			VALUES ($row_module, $row_table, $row_ex, $row_sub, $row_enable, $src, '".date("Y-m-d H:i:s")."')",3,"main_row",__LINE__,__FILE__);
	getrow($db,"SELECT LAST_INSERT_ID() as sid");
	$sid=$db->Record["sid"];
	copy_vars($src,$sid);
	return $sid;
}

function backup_ex($src){
	global $db;
	getrow($db,"SELECT * FROM ex_module WHERE ex_id=$src",1,"ex_module",__LINE__,__FILE__);
	if(empty($db->Record)) return ''; else foreach($db->Record AS $var=>$value) $$var=$value;
	$db->query("INSERT INTO main_row (row_module, row_table, row_ex, row_sub, row_enable, backup_row, backup_date)
			VALUES ($ex_module, 0, 0, 0, 0, $src, '".date("Y-m-d H:i:s")."')",3,"main_row",__LINE__,__FILE__);
	getrow($db,"SELECT LAST_INSERT_ID() as sid");
	$sid=$db->Record["sid"];
	copy_vars($src,$sid);
	return $sid;
}

function copy_vars($src,$dst){
	global $db,/*$col_cv*/$pcl;
	$vars=getall($db,"SELECT * FROM row_value WHERE value_row=$src AND value_table!=0",1,'row_value',__LINE__,__FILE__);
	if(!empty($vars)) foreach($vars AS $var){
		//if(!isset($pcl[$var["value_col"]])){
		//	getrow($db,"SELECT * FROM main_col WHERE col_id=".$var["value_col"],1,"main_col",__LINE__,__FILE__);
		//	$pcl[$var["value_col"]]=$db->Record;
		//}
		//$ccol=$pcl[$var["value_col"]];
		//if(empty($ccol['col_type'])){
		//	var_dump($ccol);
		//	echo '<br><br>';
		//}
		//if($ccol["col_type"]==3 && $var["value_value"]!='' && file_exists(DOCUMENT_ROOT.$var["value_value"])){
			//опасный ход, связанный с отключением удаления дубликата. физически у нас один файл - в базе две записи о нём, если одна из них замещается другим файлом, текущий файл не трогается и сохраняется для второго экземпляра файла
			//$var["value_value"]=clone_file($var["value_value"],$ccol["file_dir"],$ccol["file_prefix"]);
		//}
		$var["value_value"]=addslashes($var["value_value"]);
		$db->query("INSERT INTO row_value (value_module, value_table, value_row, value_col, value_value)
		VALUES (".$var["value_module"].", ".$var["value_table"].", ".$dst.", ".$var["value_col"].", '".$var["value_value"]."')",3,'row_value',__LINE__,__FILE__);
	}
}

function copy_row($row_id,$ro_id,$owner_id,$ov_ro_ex=0,$ov_row_module=0,$ov_row_table=0,$ov_owner_table=0,$ov_owner_module=0,$include_subtables=1,$id6=0,$step=0){
	global $db,$id2,$user;
	if(empty($id6)) global $id6;
	if(empty($ov_row_module)) global $id; else $id=$ov_row_module;
	if(empty($ov_row_table)) global $table_id; else $table_id=$ov_row_table;
	$ro_ex=$id2;
	if($ov_ro_ex!=0) $ro_ex=$ov_ro_ex;
	$ttable=$table_id;
	if($ov_row_table!=0) $ttable=$ov_row_table;	
	//echo "INSERT INTO main_row (row_module, row_table, row_ex, row_sub, row_user)
	//		VALUES ($id, ".$ttable.", $ro_ex, $id6, ".$user->id.")".'<br>';
	if($ov_row_module) $id=$ov_row_module;
	$db->query("INSERT INTO main_row (row_module, row_table, row_ex, row_sub, row_user, row_uin)
			VALUES ($id, ".$ttable/*$ov_row_table*//*$table_id*/.", $ro_ex, $id6, ".$user->id.", '".uuin()."')",3,'main_row',__LINE__,__FILE__);
	//echo 'table_id= '.$table_id.', ro_ex= '.$ro_ex.', row_sub='.$id6.'<br>';
	getrow($db,"SELECT LAST_INSERT_ID() as sid");
	$sid=$db->Record["sid"];
	copy_vars($row_id,$sid);
	if($step==0){
		getrow($db,"SELECT MAX(ro_pos) AS mid FROM row_owner WHERE ro_ex IN (".get_exes($owner_id,$ro_ex).") AND owner_id=$owner_id AND ro_sub=$id6 AND row_table=$table_id",1,'row_owner',__LINE__,__FILE__);
		if(!empty($db->Record["mid"])) $pos=$db->Record["mid"]+1; else $pos=1;
	}
	getrow($db,"SELECT * FROM row_owner WHERE ro_id=$ro_id",1,'row_owner',__LINE__,__FILE__);
	$owner_id2=$owner_id;
	$row_id2=$row_id;
	foreach($db->Record AS $var=>$value) $$var=$value;
	//оверрайд
	$rtt=$row_table;
	if(!empty($ov_ro_ex)) $ro_ex=$ov_ro_ex;
	if(!empty($ov_row_module)) $row_module=$ov_row_module;
	if(!empty($ov_row_table)) $row_table=$ov_row_table;
	if(!empty($ov_owner_module)) $owner_module=$ov_owner_module;
	if(!empty($ov_owner_table)) $owner_table=$ov_owner_table;
	if($owner_id==0){
		$owner_table=0;
		$owner_module=0;
	}
	if($step>0) $pos=$ro_pos;
	//вставка экземпляра
	if($ro_users){
		$ru=getall($db,"SELECT * FROM row_user WHERE ru_row=$row_id");
		if(!empty($ru)) foreach($ru AS $r){
			$db->query("INSERT INTO row_user (ru_row, ru_user) VALUES ($sid, ".$r["ru_user"].")",3,"row_user");
		}
	}
	if(empty($ro_users)) $ro_users='0';
	//echo 'ro_ex= '.$ro_ex.', row_sub='.$id6.', row_module='.$row_module.', row_table='.$row_table.', owner_id='.$owner_id2.', owner_table='.$owner_table.', owner_module='.$owner_module.'<br><br>';
	//echo "INSERT INTO row_owner (ro_pos, ro_ex, row_id, row_module, row_table, owner_id, owner_table, owner_module, ro_sub, ro_enable, ro_user, ro_users)
	//	VALUES ($pos, $ro_ex, $sid, $row_module, $row_table, $owner_id2, $owner_table, $owner_module, $id6, $ro_enable, ".$user->id.", $ro_users)".'<br>';
	$db->query("INSERT INTO row_owner (ro_pos, ro_ex, row_id, row_module, row_table, owner_id, owner_table, owner_module, ro_sub, ro_enable, ro_user, ro_users)
		VALUES ($pos, $ro_ex, $sid, $row_module, $row_table, $owner_id2, $owner_table, $owner_module, $id6, $ro_enable, ".$user->id.", $ro_users)",3,'row_owner',__LINE__,__FILE__);
	getrow($db,"SELECT LAST_INSERT_ID() as sid");
	$sid2=$db->Record["sid"];
	$id6b=seek_rowsub($sid,$rtt);
	if($id6b!=0){
		seek_rlink($id6b);
		global $rlink;
		$id6b=$rlink[$id6b]->owner;
	}
	if($id6b!=$id6 && $id6b!=0){
		$db->query("UPDATE main_row SET row_sub=$id6b WHERE row_id=$sid",3,"main_row");
		$db->query("UPDATE row_owner SET ro_sub=$id6b WHERE ro_id=$sid2",3,"row_owner");
	}
	
	$childs=getall($db,"SELECT * FROM row_owner WHERE owner_id=$row_id2"/* AND ro_sub=$id6"*/,1,'row_owner',__LINE__,__FILE__);
	if(!empty($childs)) foreach($childs AS $child){
		//copy_row($child["row_id"],$child["ro_id"],$sid);//нужно ли сюда добавлять переменные на оверрайд?
		//ниже всё таки вставлено перекрытие экземпляров и родительская таблица (насчёт правильности вставок не уверен)
		if($include_subtables==1 || $row_table==$child["row_table"]) copy_row($child["row_id"],$child["ro_id"],$sid,$ov_ro_ex,$ov_row_module,/*$ov_row_table*/$child["row_table"],/*$row_table*/$rtt,$owner_module,$include_subtables,$id6,$step+1);//=0,$ov_row_table=0,$ov_owner_table=0,$ov_owner_module=0
		if($include_subtables==2 /*||*/ && $row_table!=$child["row_table"]){
			//клонирование
			clone_row($sid2,$child["ro_id"]);
		}
	}
	add_row_index($sid);
	return $sid;
}

function clone_row($dst,$src,$type=0){//0 - dst,src is ro_id, 1 - dst,src is row_id
	global $db;
	if($type==0){
		getrow($db,"SELECT * FROM row_owner WHERE ro_id=$src",1,"row_owner");
		$src=$db->Record;
		getrow($db,"SELECT * FROM row_owner WHERE ro_id=$dst",1,"row_owner");
		$dst=$db->Record;
	}
	if($type==1){
		getrow($db,"SELECT * FROM row_owner WHERE row_id=$src",1,"row_owner");
		$src=$db->Record;
		getrow($db,"SELECT * FROM row_owner WHERE row_id=$dst",1,"row_owner");
		$dst=$db->Record;
	}
	getrow($db,"SELECT MAX(ro_pos) AS mid FROM row_owner WHERE ro_ex IN (".get_exes($dst["row_id"],$dst["ro_ex"]).") AND owner_id=".$dst["row_id"]." AND owner_table=".$dst["row_table"]." AND owner_module=".$dst["row_module"],1,'row_owner');
	if(!empty($db->Record["mid"])) $pos=$db->Record["mid"]+1; else $pos=1;
	$db->query("INSERT INTO row_owner (ro_pos, ro_ex, ro_sub, row_id, row_module, row_table, owner_id, owner_table, owner_module, ro_user)
		VALUES ($pos, ".$dst["ro_ex"].", ".$dst["ro_sub"].", ".$src["row_id"].", ".$src["row_module"].", ".$src["row_table"].", ".$dst["row_id"].", ".$dst["row_table"].", ".$dst["row_module"].", ".$dst["ro_user"].")",3,'row_owner');	
}

function clone_to($row_id,$ro_ex,$table_id,$module_id,$ro_owner=0,$ro_sub=0){
	global $db,$rlink;
	if($ro_owner==0) $cur_own=$ro_sub; else $cur_own=$ro_owner;
	getrow($db,"SELECT MAX(ro_pos) AS mid FROM row_owner WHERE ro_ex IN (".get_exes($cur_own,$ro_ex).") AND owner_id=$cur_own AND row_table=$table_id",1,'row_owner');
	if(!empty($db->Record["mid"])) $pos=$db->Record["mid"]+1; else $pos=1;
	if($ro_owner==0){
		if($ro_sub!=0){
			seek_rlink($ro_sub);
			$owner_table=$rlink[$ro_sub]->table;
		} else $owner_table=0;
	} else {
		seek_rlink($ro_owner);
		$owner_table=$rlink[$ro_owner]->table;
	}
	getrow($db,"SELECT * FROM row_owner WHERE row_id=$row_id",1,"row_owner");
	$ro_users=$db->Record["ro_users"];
	if(empty($ro_users)) $ro_users='0';
	$ro_user=$db->Record["ro_user"];
	$ro_enable=$db->Record["ro_enable"];
	$db->query("INSERT INTO row_owner (ro_pos, ro_enable, ro_ex, ro_sub, row_id, row_module, row_table, owner_id, owner_table, owner_module, ro_user, ro_users)
			VALUES ($pos, $ro_enable, $ro_ex, $ro_sub, $row_id, $module_id, $table_id, $cur_own, $owner_table, $module_id, $ro_user , $ro_users)",3,'row_owner');
}

function table_cache($table,$spec=0){
	global $db,$ta_cache,$tables;
	//здесь было db2 поменял на db
	if(empty($table)) return false;
	if(isset($tables[$table]) && $spec==0) return table_to_db($tables[$table]);
	if(!isset($ta_cache[$table])){
		getrow($db,"SELECT * FROM main_table WHERE table_id=$table",1,"main_table",__LINE__,__FILE__);
		$ta_cache[$table]=$db->Record;
	}
	return $ta_cache[$table];
}

function get_basename($row,$table=0){
	global $db;
	if($table==0){
		getrow($db,"SELECT * FROM row_owner WHERE row_id=$row");
		$table=$db->Record["row_table"];
	}
	$major=seek_major($table);
	if(empty($major)) return "Строка №".$row;	
	getrow($db,"SELECT * FROM row_value WHERE value_row=$row AND value_table=$table AND value_col=".$major['col_id'],1,"row_value");
	$val=$db->Record["value_value"];
	if($major["col_type"]==1){
		$val=get_basename($val,$major["col_link"]);
	}
	if($major["col_type"]==4){
		if(strstr($val,':')){
			$bval=explode(':',$val);
			if(count($bval)==2){
				getrow($db,"SELECT * FROM main_module WHERE module_id=$bval[0]",1,"main_module");
				if(!empty($db->Record)) $val=$db->Record["module_name"];
				getrow($db,"SELECT * FROM ex_module WHERE ex_id=$bval[1]",1,"ex_module");
				if(!empty($db->Record)) $val.=' - '.$db->Record["ex_name"];
			} else if(count($bval)==3){
				getrow($db,"SELECT * FROM main_module WHERE module_id=$bval[0]",1,"main_module");
				if(!empty($db->Record)) $val=$db->Record["module_name"];
				getrow($db,"SELECT * FROM ex_module WHERE ex_id=$bval[1]",1,"ex_module");
				if(!empty($db->Record)) $val.=' - '.$db->Record["ex_name"];
				getrow($db,"SELECT * FROM main_part WHERE part_id=$bval[2]",1,"main_part");
				if(!empty($db->Record)) $val.=' - '.$db->Record["part_name"];	
			}
		} else {
			getrow($db,"SELECT * FROM main_module WHERE module_id=$val",1,"main_module");
			if(!empty($db->Record)) $val=$db->Record["module_name"];
		}
	}
	if($major["col_type"]==5){
		getrow($db,"SELECT * FROM main_auth WHERE auth_id=$val",1,"main_auth");
		if(!empty($db->Record)) $val=$db->Record["user_name"].' ('.$db->Record["user_login"].')';
	}
	return $val;
}

function seek_major($table){
	global $db,$tmj;
	if(empty($table)) return 0;
	if(empty($tmj[$table])){
		//getrow($db,"SELECT * FROM main_table WHERE table_id=$table",2,'main_table',__LINE__,__FILE__);
		$db->Record=table_cache($table);
		if($db->Record["major_col"]!=0) {
			getrow($db,"SELECT * FROM main_col WHERE col_id=".$db->Record["major_col"],1,'main_col',__LINE__,__FILE__);
			$tmj[$table]=$db->Record;
			return $db->Record;
		} else{
			getrow($db,"SELECT * FROM main_col WHERE col_table=$table AND col_bold=1 ORDER BY col_pos LIMIT 1",1,'main_col',__LINE__,__FILE__);
			if(!empty($db->Record)){
				$tmj[$table]=$db->Record;
				return $db->Record;
			}
		}
	} else return $tmj[$table];
	return 0;
}

function db_to_table($rec){
	global $tables,$tables2;
	$r=new stdClass();
	$r->id=$rec["table_id"];
	$r->module=$rec["table_module"];
	$r->name=$rec["table_name"];
	$r->sname=$rec["table_sname"];
	$r->major=$rec["major_col"];
	$r->multy=$rec["table_multy"];
	$tables[$r->id]=$r;
	$tables2[$r->module][$r->sname]=$r;
}

function table_to_db($tbl){
	$rec["table_id"]=$tbl->id;
	$rec["table_module"]=$tbl->module;
	$rec["table_name"]=$tbl->name;
	$rec["table_sname"]=$tbl->sname;
	$rec["major_col"]=$tbl->major;
	return $rec;
}

function prep_tables($force=false){
	global $tables,$tables2,$cached_tbls,$stables,$stables2;
	//if(!isset($tables)){
	if(empty($cached_tbls) || $force){
		global $db;
		$tbles=getall($db,"SELECT table_sname, table_id, table_module, table_name, major_col, table_multy FROM main_table",1,'main_table',__LINE__,__FILE__);
		$tables=Array();$tables2=Array();
		if(!empty($tbles)) foreach($tbles AS $t){
			db_to_table($t);
		}
		$tbles2=getall($db,"SELECT * FROM table_sub",1,'table_sub',__LINE__,__FILE__);
		$stables=Array();$stables2=Array();
		if(!empty($tbles2)) foreach($tbles2 AS $t){
			$stables[$t["sub_table1"]][$t["sub_table2"]]=$t["sub_table2"];
			$stables2[$t["sub_table2"]]=$t["sub_table1"];
		}
	}
	$cached_tbls=1;
}

function seek_usercol($table,$col_sname=''){
	global $db;
	$sqla='';
	if($col_sname!='') $sqla=" AND col_sname='".$col_sname."'";
	$sql="SELECT * FROM main_col WHERE col_table=$table".$sqla." AND col_type=5 LIMIT 1";
	getrow($db,$sql,1,"main_col");
	return $db->Record;
}

function seek_usercols($table,$col_sname=''){
	global $db;
	$sqla='';
	if($col_sname!='') $sqla=" AND col_sname='".$col_sname."'";
	$sql="SELECT * FROM main_col WHERE col_table=$table".$sqla." AND col_type=5";
	if(!empty($col_sname)){
		getrow($db,$sql,1,"main_col");
		return Array($db->Record['col_id']);
	} else {
		return getall3($db,$sql,"col_id");
	}
}

function seek_vars($rows){
	global $res,$tmj;
	if(!empty($rows)) foreach($rows AS $row){
		$res[$row->id]=1;
		if(empty($tmj[$row->table])){	
			$tmj[$row->table]=seek_major($row->table);
		}
		if(!empty($row->sub)) seek_vars($row->sub);
	}
}

function set_vars($rows,$vals){
	if(!empty($rows)) foreach($rows AS $var=>$value){
		if(!empty($vals[$rows[$var]->id])) $rows[$var]->value=$vals[$rows[$var]->id];
		else $rows[$var]->value='';
		if(!empty($rows[$var]->sub)) $rows[$var]->sub=set_vars($rows[$var]->sub,$vals);
	}
	return $rows;
}

function get_vars($rows){
	global $db,$res,$vcacheG,$tmj;
	$res=Array();
	seek_vars($rows);
	if(!empty($res)){
		$res2='';
		$vals=Array();
		foreach($res AS $var=>$value){
			if(!empty($vcacheG[$var])){
				$vals[$var]=$vcacheG[$var];
			} else {
				if(!empty($res2))$res2.=',';
				$res2.=$var;
			}
		}
		if(!empty($res2)){
			$mjc2='';
			if(!empty($tmj)) foreach($tmj AS $var=>$value)if(!empty($value["col_id"])){ if(!empty($mjc2)) $mjc2.=','; $mjc2.=$value["col_id"]; }
			if(!empty($res2) && !empty($mjc2)) $vals2=getall($db,"SELECT value_row, value_col, value_value FROM row_value WHERE value_row IN ($res2) AND value_col IN ($mjc2)",1,'row_value',__LINE__,__FILE__);
			else $vals2=Array();
			if(!empty($vals2)) foreach($vals2 AS $val2){
				$vals[$val2["value_row"]]=$val2["value_value"];
				$vcacheG[$val2["value_row"]]=$val2["value_value"];//тут вероятно напутано, т.к. vcache теперь это совершенно другое
			}
		}
		$rows=set_vars($rows,$vals);
	}
	return $rows;
}

function collect_rows($rows,$x=Array(),$ignore_sub=false){
	if(!empty($rows)) foreach($rows AS $var=>$row){
		if(is_object($row)){
			$x[$row->id]=$row->id;
			if(!empty($row->sub) && !$ignore_sub) $x=collect_rows($row->sub,$x);
		} else {
		//	$x[$var]=$row;
		}
	}
	return $x;
}

function collect_rows2($rows,$x=Array()){
	if(!empty($rows)) foreach($rows AS $var=>$row){
		if(is_object($row)){
			if(!empty($row->sub)) $x=collect_rows2($row->sub,$x);
		} else {
			$x[$var]=$row;
		}
	}
	return $x;
}

function collect_rows_cross($rows,$x=Array()){
	if(!empty($rows)) foreach($rows AS $var=>$row){
		if(is_object($row)){
			$x[$var]=$row->id;
			if(!empty($row->sub)) $x=collect_rows_cross($row->sub,$x);
		} else if(is_array($row)){
			$x=collect_rows_cross($row,$x);
		} else {
			$x[$var]=$row;
		}
	}
	return $x;
}

function collect_rows3($rows,$x=Array()){
	if(!empty($rows)) foreach($rows AS $var=>$row){
		if(is_object($row)){
			$x[$row->id]=$row;
			if(!empty($row->sub)) $x=collect_rows3($row->sub,$x);
		} else {
		//	$x[$var]=$row;
		}
	}
	return $x;
}

function collect_tables($rows,$x=Array()){
	if(!empty($rows)) foreach($rows AS $row){
		if(is_object($row)){
			$x[$row->table]=$row->table;
			if(!empty($row->sub)) $x=collect_tables($row->sub,$x);
		}
	}
	return $x;
}

function seek_filter_sub($rows,$filter){
	$r=collect_rows($rows);
	foreach($r AS $rs) if(isset($filter[$rs])) return true;
	return false;
}

function filter_rows($rows,$filter,$in_sub=0){//filter - массив, содержащий перечень row которых нужно оставить в rows, in_sub=1 будет оставлять rows если результат найден у её потомка
	$result=Array();$i=0;
	if(!empty($rows)) foreach($rows AS $row){
		if(!empty($filter[$row->id]) || ($in_sub && !empty($row->sub) && seek_filter_sub($row->sub,$filter))){
			$i++;
			$result[$i]=$row;//возможно тут надо будет ввести DC(), т.к. дальше будут урезаться sub
			if(!empty($result[$i]->sub)) $result[$i]->sub=filter_rows($result[$i]->sub,$filter);
		}
	}
	return $result;
}

function filter_rows_except($rows,$filter){//filter - массив, содержащий перечень row которых нужно исключить из rows
	$result=Array();$i=0;
	if(!empty($rows)) foreach($rows AS $row){
		if(empty($filter[$row->id])){
			$i++;
			$result[$i]=$row;//возможно тут надо будет ввести DC(), т.к. дальше будут урезаться sub
			if(!empty($result[$i]->sub)) $result[$i]->sub=filter_rows_except($result[$i]->sub,$filter);
		}
	}
	return $result;
}

function set_collect($rows,$val,$ycollect){
	$result=Array();
	if(!empty($rows)) foreach($rows AS $i=>$row){
		if(is_object($row)){
			if(!isset($val[$row->id])) $val[$row->id]='';
			$result[$i]->id=$row->id;
			$result[$i]->rid=$row->rid;
			$result[$i]->value=$val[$row->id];
			if(!empty($row->sub)) $result[$i]->sub=set_collect($row->sub,$val,$ycollect);
		} else {
			if(isset($ycollect[$i])) $result[$i]=$ycollect[$i];
			else if(isset($val[$i])) $result[$i]=$val[$i];
			else $result[$i]='';
			//$val[$i]=$row;
		}
	}
	return $result;
}

function get_vars2($rows,$linear=false){//linear - если true, то возвращает результаты в едином массиве rows[id]->value включая sub
	//->value
	//->sub
	global $db;
	if(empty($rows)) return Array();
	//foreach($rows AS $row){$table=$row->table;break;}
	$collect=collect_rows($rows);
	$ycollect=collect_rows2($rows);
	$collect3=collect_tables($rows);
	$xcollect=$collect;
	if(empty($collect)) return Array(); else $collect=implode(',',$collect);
	$cols=Array();
	foreach($collect3 AS $table){
		$tmp=seek_major($table);
		if(!empty($tmp["col_id"])){
			$xcol[$tmp["col_id"]]=$tmp;
			$cols[$tmp["col_id"]]=$tmp["col_id"];
		}
	}	
	//&& !empty(implode(',',$cols))
	if(!empty($collect) && !empty($cols)) $tvals=getall($db,"SELECT * FROM row_value WHERE value_row IN ($collect) AND value_col IN (".implode(',',$cols).")",1,"row_value");
	if(empty($tvals)) return Array();	
	foreach($tvals AS $tval){
		$val2[$tval["value_col"]][$tval["value_row"]]=$tval["value_value"];//это исключает попадания сюда строк с мультивыборкой (оно пока и не нужно, но потом может пригодиться)
		$val3[$tval["value_row"]]=$tval["value_value"];
	}
	$collect2b=Array();
	foreach($val2 AS $col=>$v2){
		$collect2b[$col]=implode(',',$v2);
	}
	foreach($val2 AS $c=>$val){
		$collect2=$collect2b[$c];
		$col=$xcol[$c];
		if($col["col_type"]==4){
			$ms=Array(); $ps=Array(); $es=Array();
			foreach($val AS $v){
				if(strstr($v,':')){
					$v=explode(':',$v);
					if(count($val)==2){
						$ms[$v[0]]=$v[0];
						$es[$v[1]]=$v[1];
					}
					if(count($val)==3){
						$ms[$v[0]]=$v[0];
						$es[$v[1]]=$v[1];
						$ps[$v[2]]=$v[2];
					}
				} else {
					$ms[$v]=$v;
				}
			}
			if(!empty($ms)){
				$ms=implode(',',$ms);
				$tms=getall($db,"SELECT * FROM main_module WHERE module_id IN ($ms)",1,"main_module");
				$ms=Array();
				foreach($tms AS $tm) $ms[$tm["module_id"]]=$tm["module_name"];
			}
			if(!empty($es)){
				$es=implode(',',$es);
				$tes=getall($db,"SELECT * FROM ex_module WHERE ex_id IN ($es)",1,"ex_module");
				$es=Array();
					foreach($tes AS $te) $es[$te["ex_id"]]=$te["ex_name"];
			}
			if(!empty($ps)){
				$es=implode(',',$ps);
				$tps=getall($db,"SELECT * FROM main_part WHERE part_id IN ($es)",1,"main_part");
				$ps=Array();
				foreach($tps AS $tp) $es[$tp["part_id"]]=$tp["part_name"];
			}		
			foreach($val AS $row=>$v){
				if(strstr($v,':')){
					$v=explode(':',$v);
					if(!isset($ms[$v[0]])) $ms[$v[0]]='';
					if(count($val)==2){
						if(!isset($es[$v[1]])) $es[$v[1]]='';
						$val3[$row]=$ms[$v[0]].' - '.$es[$v[1]];
					}
					if(count($val)==3){
						if(!isset($es[$v[1]])) $es[$v[1]]='';
						if(!isset($ps[$v[2]])) $ps[$v[2]]='';
						$val3[$row]=$ms[$v[0]].' - '.$es[$v[1]].' - '.$ps[$v[2]];
					}
				} else {
					if(!isset($ms[$v])) $ms[$v]='';
					$val3[$row]=$ms[$v];
				}
			}
		}
		if($col["col_type"]==5){
			$tus=getall($db,"SELECT * FROM main_auth WHERE auth_id IN ($collect2)",1,"main_auth");
			$us=Array();
			foreach($tus AS $tu) $us[$tu["auth_id"]]=$tu["user_name"].' ('.$tu["user_login"].')';
			foreach($val AS $row=>$v) if(isset($us[$v])) $val3[$row]=$us[$v];
		}
		if($col["col_type"]==1){
			$col2=seek_major($col["col_link"]);
			$rows2=Array();
			$i=0;
			foreach($val AS $row=>$v){
				$rows2[$i]->table=$col["col_link"];
				$rows2[$i]->id=$v;
				$i++;
			}
			$rows2=get_vars2($rows2,true);//тут почему-то было rows2b ?
			foreach($val AS $row=>$v){
				if(empty($rows2[$v]->value)) $rows2[$v]->value='';
				$val3[$row]=$rows2[$v]->value;
			}
		}	
	}
	if($linear){
		$result=Array();
		foreach($xcollect AS $row)if(isset($val[$row])) $result[$row]->value=$val3[$row];
		if(!empty($ycollect)) foreach($ycollect AS $var=>$value) $result[$var]=$value;
	} else {
		$result=set_collect($rows,$val3,$ycollect);
	}
	return $result;
}

function scan_enable($owner){
	global $db,$rlink;
	if($owner==0) return true;
	if(empty($rlink[$owner])) seek_rlink($owner);
	if(!$rlink[$owner]->enable) return false;
	if(!empty($rlink[$owner]->owner)) return scan_enable($rlink[$owner]->owner);
	return true;
}

function prep_exx($ex_ex2,$module_id){
	global $exx,$db;
	if(!isset($exx[$ex_ex2])){
		if(!empty($ex_ex2)) $texxs=getall($db,"SELECT * FROM ex_group WHERE ex_ex2=$ex_ex2",1,'ex_group',__LINE__,__FILE__);
		else if(!empty($module_id)) $texxs=getall($db,"SELECT * FROM ex_group WHERE ex_module=$module_id",1,'ex_group',__LINE__,__FILE__);
		if(!empty($texxs)) foreach($texxs AS $texx) $exx[$ex_ex2][$texx["ex_ex1"]]=1;
		else $exx[$ex_ex2]=Array();
	}
}

function search_subs(&$res,$sub1,$sub2,$seek_enable,$seek_ex,$ignore_table,$own_link,$seek_subt,$ex,$modex,$table,$seek_top_table,$stop_table,$check_auth,$check_user,$seek_osub,$step){
	global $rlink;
	//$scan_tables=false;
	$stop_table2=$stop_table;
	if(!empty($stop_table) && strpos($stop_table,'.')!=0){
		$tmp=explode('.',$stop_table);
		$stop_table=$tmp[count($tmp)-1];
		/*array_pop($tmp);
		$scan_tables=true;
		$allow_tables=array_flip($tmp);
		foreach($allow_tables AS $var=>$value) $allow_tables[$var]=1;
		$allow_tables[$stop_table]=1;*/
	}
	/*if(!empty($stop_table)){
		$scan_tables=true;
		$allow_tables[$stop_table]=1;
	}*/
	if(!empty($sub1)) foreach($sub1 AS $atable_id=>$rows)/*if(!$scan_tables || !empty($allow_tables[$atable_id]))*/{
		$st=Array();
		foreach($rows AS $owner=>$row){
			foreach($row AS $rid=>$ro){
				//$sub1a[$ro]=$owner;
				$st[$ro]=$ro;
			}
		}
		//$st=implode(',',$st);//теперь get_sub поддерживает массивы на вход
		
		$rs=get_sub($st,$atable_id,$seek_enable,$seek_ex,$ignore_table,$own_link,$seek_subt,$ex,$modex,$table,$seek_top_table,$stop_table2,$check_auth,Array(),$check_user,$seek_osub,$step);

		//if($st=='46629,46630') echo count($rs);
		/*foreach($rs AS $r)if(isset($sub1a[$r->owner])){
			$res[$sub1a[$r->owner]]->sub[]=$r;
		}*/
		$zrows=Array();
		foreach($rs AS $r) foreach($r->owners AS $rid=>$own) if($r->rid==$rid) $zrows[$own][$rid]=$r;
		foreach($rows AS $owner=>$row){
			foreach($row AS $rid=>$ro){
				if(!empty($zrows[$ro])) foreach($zrows[$ro] AS $r) $res[$owner]->sub[]=$r;
			}
			//if($st=='46629,46630') echo $owner.' - '.count($res[$owner]->sub).'<br>';
		}
	}
	if(!empty($sub2)) foreach($sub2 AS $atable_id=>$rows)/*if(!$scan_tables || !empty($allow_tables[$atable_id]))*/{
		$st=Array();
		foreach($rows AS $owner=>$row){
			foreach($row AS $rid=>$ro){
				//$sub2a[$ro]=$owner;
				$st[$ro]=$ro;
			}
		}
		//$st=implode(',',$st);
		$rs=get_sub($st,$atable_id,$seek_enable,$seek_ex,0,$own_link,$seek_subt,$ex,$modex,$table,$seek_top_table,$stop_table2,$check_auth,Array(),$check_user,$seek_osub,$step);
		/*foreach($rs AS $r) if(isset($sub2a[$r->owner])){
			$res[$sub2a[$r->owner]]->sub[]=$r;
			break;
		}*/
		$zrows=Array();
		foreach($rs AS $r) foreach($r->owners AS $rid=>$own) if($r->rid==$rid) $zrows[$own][$rid]=$r;
		foreach($rows AS $owner=>$row){
			foreach($row AS $rid=>$ro){
				if(!empty($zrows[$ro])) foreach($zrows[$ro] AS $rid2=>$r){
					//echo '1: '.$owner.' - '.$r->id.'<br>';
					$res[$owner]->sub[]=$r;
				}
			}
		}
	}
	//корректная правка next и prev
	foreach($res AS $owner=>$row){
		if(empty($rlink[$row->id]->sub) && !empty($row->sub)) $rlink[$row->id]->sub=$row->sub;
		if(!empty($row->sub)) foreach($row->sub AS $i=>$subrow){
			if(isset($point)){
				$rlink[$point]->next=&$rlink[$subrow->id];
				$rlink[$subrow->id]->prev=&$rlink[$point];
			} else {
				unset($rlink[$subrow->id]->next);// тут вопрос, т.к. unset может сработать не на ссылке next, а на объекте, на который происходит ссылка
				unset($rlink[$subrow->id]->prev);
			}
			$point=$subrow->id;
		}
		unset($point);
	}
}

function rlink_to_res($rl){
	$res=new rlink();
	$res->owner=$rl->owner;
	$res->owners=&$rl->owners;
	$res->rid=$rl->rid;
	$res->id=$rl->id;
	$res->tex=$rl->tex;
	$res->table=$rl->table;
	$res->ownert=$rl->ownert;
	$res->rsub=$rl->rsub;
	$res->user=$rl->user;
	$res->users=$rl->users;
	$res->module=$rl->module;
	$res->pos=$rl->pos;
	$res->enable=$rl->enable;
	return $res;
}

function get_sub($owner,$atable_id,$seek_enable,$seek_ex,$ignore_table=0,$own_link=0,$seek_subt=1,$ex=0,$modex=0,$table=0,$seek_top_table=0,$stop_table=0,$check_auth=0/*1*/,$res=Array(),$check_user=0,$seek_osub=1,$step=0){//ex - экземпляр таблицы, modex - экземпляр модуля, seek_top_table - сравнивает с table_id только если step=0, stop_table - таблица, при которой поиск дочерних элементов идёт только в этой же таблце (исключает дальнейшие подтаблицы), check_auth = проверять доступ, seek_osub - искать в sub своей же подтаблицы (т.е. в своих же потомках)

	$bowner=$owner;//just for tests

	//подготавливаем переменные
	global $db,$exx,$table_id,$rcache,$rcache2,$module_id,$rlink,$rlink2,$ppro,$no_cache,$tables,$stables,$rolink,$gsres;
	prep_tables();
	prep_exx($modex,$module_id);
	if(is_object($owner) && isset($owner->id) && !isset($owner->rows)) $owner=$owner->id;
	if(is_object($owner) && isset($owner->rows)) $owner=$owner->rows;
	$multy_own=(is_array($owner) || (strpos($owner,',')!==false));
	$from_rlink=($owner<0);
	if(!$multy_own) $owner=abs($owner);
	else {
		if(!is_array($owner)) $owners=explode(',',$owner);
		else {
			$x=current($owner);
			if($x && is_object($x) && isset($x->id)){
				$owners=Array();
				foreach($owner AS $r) $owners[]=$r->id;
			} else $owners=$owner;
			$owner=current($owners);
		}
	}
	$ignore_table2=$ignore_table;
	if(empty($table)) $table=$table_id;
	$i=count($res);
	$rs=Array();
	$sub1=Array(); $sub2=Array();
	if($step==0 && $seek_top_table) $ignore_table2=0;
	if($step>0 && $seek_top_table){
		//это необходимо для отображения товаров подтаблиц с префиксом '-'
		$ignore_table2=1;
		$ignore_table=1;
	}
	if($seek_ex==2){
		$seek_ex=0;
		$double_cache=1;
	} else $double_cache=0;
	$ust='';
	
	//FOR TESTS
	$double_cache=0;
	//ошибка заключалась в следующем
	//т.к. gsres в качестве последнего ключа использует не row_id, а просто ID, 
	//то при двойном вызове с разным значением seek_ex возможно перекрытие кеша (т.е. добавление старых значений повторно сверху в кеш)
	//выхода два - 1ый убрать double_cache (что и было сделано), второй - сделать ключём ro_id, но в этом случае придётся пределывать логику обработки строк, как массивов
	
	$stop_table2=$stop_table;
	if(!empty($stop_table) && strpos($stop_table,'.')!=0){
		$tmp=explode('.',$stop_table);
		$stop_table=$tmp[count($tmp)-1];
		/*array_pop($tmp);
		$allow_tables=array_flip($tmp);
		foreach($allow_tables AS $var=>$value) $allow_tables[$var]=1;
		$allow_tables[$stop_table]=1;*/
		$allow_tables=$tmp;
		if($ignore_table && $owner!=0){
			$ust=' AND row_table IN ('.implode(',',$allow_tables).')';
		}
	}
	if(!$multy_own && !isset($gsres[$owner][$atable_id][$seek_enable][$seek_ex][$modex][$ex][$ignore_table][$check_auth])) $gsres[$owner][$atable_id][$seek_enable][$seek_ex][$modex][$ex][$ignore_table][$check_auth]=Array();
	if($multy_own){
		foreach($owners AS $own) if(!isset($gsres[$own][$atable_id][$seek_enable][$seek_ex][$modex][$ex][$ignore_table][$check_auth])) $gsres[$own][$atable_id][$seek_enable][$seek_ex][$modex][$ex][$ignore_table][$check_auth]=Array();
	}
	if($double_cache){//рискованный ход
		if(!$multy_own && !isset($gsres[$owner][$atable_id][$seek_enable][1][$modex][$ex][$ignore_table][$check_auth])) $gsres[$owner][$atable_id][$seek_enable][1][$modex][$ex][$ignore_table][$check_auth]=Array();
		if($multy_own){
			foreach($owners AS $own) if(!isset($gsres[$own][$atable_id][$seek_enable][1][$modex][$ex][$ignore_table][$check_auth])) $gsres[$own][$atable_id][$seek_enable][1][$modex][$ex][$ignore_table][$check_auth]=Array();
		}
	}
	
	//сооружаем массив экземпляров таблиц, присоединённых к текущему модулю
	if(!empty($exx[$modex])) foreach($exx[$modex] AS $var=>$value) if(empty($exxs)) $exxs=$var; else $exxs.=','.$var;
	
	//поиск кеша
	$cstr=$seek_enable.'-'.$check_user.'-'.$check_auth;
	if($owner==0/* || $multy_own*/)	$cache=&$rcache[$atable_id][$ex][$cstr];	//из-за этого multy_own многое кешировалось не там где нужно. дважды подумать если потребуется раскомментировать
	else {
		if($ignore_table){	
				$cache=&$rcache[-$ignore_table-1][$cstr];
				//$cache2=&$rcache[-$atable_id-10][$cstr];
		} else {
				$cache=&$rcache[-$atable_id-10][$cstr];//- и -10 сделано для исключения попаданий в единый массив с -ignore_table-1 и atable_id
				//$cache2=&$rcache[-$ignore_table-1][$cstr];
		}
	}
	
	//создаём SQL запрос
	//часть 1. активность
	if($seek_enable==1) $ena=" AND ro_enable=1";
	else if($seek_enable==0) $ena='';
	else if($seek_enable==2) $ena=" AND ro_enable=0";
	//часть 2. экземплярность
	if($seek_ex/* && $table==$atable_id*/ && !empty($ex)){
		//$exa="ro_ex=".$ex." AND ";
		$exa="ro_ex=".get_exes($owner,$ex)." AND ";
	} else $exa="";
	//часть 3. владелец
	if($check_user!=0) $usa=" AND ro_user=$check_user";
	else $usa='';
	//часть 4. родители
	if(!$multy_own) $owa="owner_id=".$owner;
	else {
		$s=Array();
		foreach($owners AS $ow){
			if(isset($cache[$ow])){
				$rs=array_merge_recursive($rs,$cache[$ow]);
			} else {
				$s[$ow]=$ow;
			}
		}
		if(!empty($s)){
			$bs=$s;
			if(count($s)==1) $owa="owner_id=".implode('',$s);
			else {
				$s=implode(',',$s);
				$owa="owner_id IN ($s)";
			}
		} else $owa='';
	}
	
	//компиляция запроса
	if($owner==0) 			$sql="SELECT * FROM row_owner WHERE ".$exa.$usa."row_table=$atable_id AND owner_id=0".$ena." ORDER BY ro_pos";
	else if($owa!=""){
		if($ignore_table)	$sql="SELECT * FROM row_owner WHERE ".$owa.$ena.$usa.$ust." ORDER BY ro_pos";
		else				$sql="SELECT * FROM row_owner WHERE ".$owa.$ena.$usa." AND row_table=$atable_id ORDER BY ro_pos";
	}
	//выполняем SQL запрос
	if($owner==0 && isset($cache)) foreach($cache AS $var) $rs=array_merge($rs,$var);
	else if($owner!=0 && !$multy_own && isset($cache[$owner])) $rs=$cache[$owner];
	else if($owner!=0 && $multy_own && $owa="") {}//тут всё хорошо, значит все элементы multy_own закешированы и взялись из кеша выше
	else if(!empty($sql)){
		/*if($sql=="SELECT * FROM row_owner WHERE owner_id=16365 AND ro_enable=1 AND row_table=683 ORDER BY ro_pos"){
			fire();
		}*/
		//if($sql=="SELECT * FROM row_owner WHERE owner_id=22495 AND ro_enable=1 AND row_table=14 ORDER BY ro_pos") fire();
		$rs=getAll2($db,$sql,$rs,1,"row_owner",__LINE__,__FILE__);	
		//echo $sql.' - '.count($rs).'<br>';
		//if(count($rs)>800){debug_wayback(); exit;}
		
		//if($owner==46629) echo $sql.'<br>';
		//echo count($rs).'<br>';
		//if($sql=='SELECT * FROM row_owner WHERE owner_id=46630 AND row_table=76 ORDER BY ro_pos'){
		//	echo count($rs);
		//	debug_wayback(); exit;
		//}
	}	
	
	//if(isset($_GET["debug"])) echo $sql;
	
	/*if($sql=='SELECT * FROM row_owner WHERE owner_id=826 AND ro_enable=1 AND row_table=296 ORDER BY ro_pos'){
		$trace=debug_backtrace();
		foreach($trace AS $tr) echo $tr["file"].' - '.$tr["line"].'<br>';
	}*/
	
	//if($sql=="SELECT * FROM row_owner WHERE ro_ex=171 AND row_table=697 AND owner_id=0 AND ro_enable=1 ORDER BY ro_pos") echo count($rs);
	//echo $sql.' - '.$seek_ex.' - '.$ex.' - '.$atable_id.' - '.$table.'<br>';
	
	//в случае пустого результата, кешируем и его
	if(empty($rs)){
		if($owner!=0 && !$multy_own) $cache[$owner]=Array();
		if($owner!=0 && $multy_own && !empty($bs)) foreach($bs AS $var) $cache[$var]=Array();
		if($owner==0) $cache=Array();
	}

	//перебираем полученные ряды
	if(!empty($rs)) foreach($rs AS $ro){
	
		//кешируем массив rlink_cache и rcache
		if(!$GLOBALS["is_admin"]) $rlink_cache[$ro["row_id"]]=$ro;
		if(!$multy_own) $own=$owner; else $own=$ro["owner_id"];
		$cache[$own][$ro["ro_id"]]=$ro;
		
		$rid=$ro["row_id"];
		$rid2=$ro["ro_id"];	

		if(isset($rlink[$rid])){
			$rlink[$rid]->owner=$own;//перекрываем родителя на самого последнего
			$rlink[$rid]->owners[$rid2]=$own;
			$rlink[$rid]->ownert=$ro["owner_table"];
			$rlink[$rid]->rsub=$ro["ro_sub"];
			$rlink[$rid]->module=$ro["row_module"];
			$rlink[$rid]->pos=$ro["ro_pos"];
			$rlink[$rid]->enable=$ro["ro_enable"];
			//$rlink[$rid]->rid=$rid2;//в этом нет смысла, они все будут одинаковые  //если убрать этот комментарий, то в сводной таблице все клоны будут иметь один и тот же RID (последний)
			$rolink[$rid2]=$rid;
		} else {
			$rlink[$rid]=new rlink();
			//присваивание родителя
			$rlink[$rid]->owner=$own;//по идее тут может быть массив значений
			$rlink[$rid]->owners[$rid2]=$own;//нужно помнить, что этот массив не будет полным, он будет хранить только затронутые родители ROW
			//прочие поля
			$rlink[$rid]->rid=$rid2;//в этом нет смысла, они все будут одинаковые
			$rolink[$rid2]=$rid;
			$rlink[$rid]->id=$ro["row_id"];
			$rlink[$rid]->tex=$ro["ro_ex"];
			$rlink[$rid]->table=$ro["row_table"];
			$rlink[$rid]->ownert=$ro["owner_table"];
			$rlink[$rid]->rsub=$ro["ro_sub"];
			$rlink[$rid]->user=$ro["ro_user"];
			$rlink[$rid]->module=$ro["row_module"];
			$rlink[$rid]->pos=$ro["ro_pos"];
			$rlink[$rid]->user=$ro["ro_user"];
			$rlink[$rid]->type='row';
			$rlink[$rid]->enable=$ro["ro_enable"];
			$rlink[$rid]->users=Array();
			if(!empty($ro["ro_users"])){//это потом надо будет закешировать
				$usrs=getall($db,"SELECT * FROM row_user WHERE ru_row=".$rlink[$rid]->id,1,"row-user");
				if(!empty($usrs)) foreach($usrs AS $usr) $rlink[$rid]->users[$usr["ru_user"]]=$usr["ru_user"];
			}
		}
		
		//проверяем настройки безопасности		
		if(!$check_auth || check_row($ro["row_id"],$ro["row_table"],get_ex($ro["row_id"],$ro["row_table"],$ro["ro_ex"]),'view',$ro["ro_user"],$rlink[$rid]->users)!=0){
	
			//если не задан поиск подтаблиц, то выходим
			if(!$seek_subt && $ro["row_table"]!=$atable_id){
				if(!empty($sub1) || !empty($sub2)) search_subs($res,$sub1,$sub2,$seek_enable,$seek_ex,$ignore_table,$own_link,$seek_subt,$ex,$modex,$table,$seek_top_table,$stop_table2,$check_auth,$check_user,$seek_osub,$step+1);
				return $res;
			}

			//подконтейнер
			$res[$i]=rlink_to_res($rlink[$rid]);
			$res[$i]->owners=&$rlink[$rid]->owners;
			$ppro[$res[$i]->id]=$res[$i]->table;
			//$res[$i]->sub=Array();
			if($stop_table!=$res[$i]->table || ($stop_table!=0 && $stop_table==$ro["row_table"] && $tables[$res[$i]->table]->multy==1)){
				if($stop_table!=0 && $stop_table==$ro["row_table"]){

				} else {
					if($seek_subt){
						if(!empty($stables[$ro["row_table"]]))
							$sub1[$res[$i]->table][$i][$rid2]=$rid;
					}
				}
			}
			if($tables[$res[$i]->table]->multy==1 && $seek_osub){
				$sub2[$res[$i]->table][$i][$rid2]=$rid;
			}

			//связь с рлинк и связи next и prev
			$rlink2=true;
			if(isset($point)){
				$rlink[$point]->next=&$rlink[$rid];
				$rlink[$rid]->prev=&$rlink[$point];
			}
			$point=$rid;
			
			$gsres[$own][$ro["row_table"]][$seek_enable][$seek_ex][$modex][$ex][$ignore_table][$check_auth][$i]=$res[$i];
			//if($bowner=='20354,20397,20456' && $res[$i]->id==20457){
			//	echo $own.' = '.$res[$i]->id.'<br>';
			//	echo 'row_table='.$ro['row_table'].'<br>seek_enable='.$seek_enable.'<br>seek_ex='.$seek_ex.'<br>modex='.$modex.'<br>ex='.$ex.'<br>ignore_table='.$ignore_table.'<br>check_auth='.$check_auth.'<br>i='.$i;
			//	echo '<br><br>';
			//}
			if($double_cache) $gsres[$own][$ro["row_table"]][$seek_enable][1][$modex][$ex][$ignore_table][$check_auth][$i]=$res[$i];			
			
			$i++;			
		}
	}

	if(!empty($res) && (!empty($sub1) || !empty($sub2))){
		search_subs($res,$sub1,$sub2,$seek_enable,$seek_ex,$ignore_table,$own_link,$seek_subt,$ex,$modex,$table,$seek_top_table,$stop_table2,$check_auth,$check_user,$seek_osub,$step+1);
		//echo count($res[0]->sub).'<br>';
	}
	return $res;
}

function options($rows,$space,$selected=0,$id=1,$show=1,$except=0,$sexpect=0,$stxt='',$owners=Array(),$lim=10000,$clevel=0,$col_link3=0,$level=0,$ntbl=0,$add_meta=0){
	$res='';
	$level2=$level-1;
	if($show==2) $res=Array();
	$ignore=0;
	if(!empty($ntbl)){
		if(!empty($rows)) foreach($rows AS $var=>$value){
			if(is_object($value)){
				$vai=$value->id;
			} else {
				$vai=$var;
			}
			seek_rlink($vai);
			global $rlink;
			$tbl=$rlink[$vai]->table;
			if($tbl!=$ntbl) $ignore=1;
			break;
		}
	}
	if(!empty($rows)) foreach($rows AS $var=>$value){
		if(is_object($value)){
			if($id==1) $vai=$value->id; else $vai=$value->rid;
		} else {
			$vai=$var;
			$tmp->value=$value;
			$tmp->id=$var;
			$tmp->rid=$var;
			$value=$tmp;
		}
		if((string)$except!=(string)$vai && (string)$sexpect!=(string)$value->id){
			if($id!=3){if(($selected!=0 || !is_numeric($selected)) && ($selected==$vai || $selected==$value->value)) $add=' selected'; else $add='';}
			if($id==2 || $id==3) $vai=$value->id;
			if($id==3){if(($selected!=0 || !is_numeric($selected)) && ($selected==$vai || $selected==$value->value) && $id==3) $add=' selected'; else $add='';}
			$value->value=str_replace(chr(10),'',$value->value);
			$value->value=str_replace(chr(13),'',$value->value);
			if(empty($value->value)) $value->value='Нет значения';
			//$value->value.=' (vai = '.$vai.', value_id = '.$value->id.', selected = '.$selected.')';
			if($lim!=10000 && strlen5($value->value)>$lim){
				$value->value=mb_substr($value->value,0,$lim,'UTF-8');//substr5($value->value,0,$lim);
			}
			$dis=0;
			if($level2!=$clevel && $level!=0){ $add.=' disabled'; $dis=1;}
			if($col_link3==1 && !empty($value->sub)){ $add.=' disabled'; $dis=1;}
			if($ignore){ $add.=' disabled'; $dis=1;}
			
			if(!empty($value->sub)){
				$tmp[$vai]=1;
				$d=options($value->sub,$space.'&nbsp;&nbsp;',$selected,$id,$show,$except,$sexpect,$stxt,$tmp,$lim,$clevel+1,$col_link3,$level,$ntbl,$add_meta);
			} else $d=Array();
			
			if(!$dis || !empty($d)){
				if($add_meta){
					$r=get_rlink($vai);
					if(is_numeric($vai) && check_row($vai,$r->table,get_ex2($r->tex),"edit",$r->user,$r->users)){
						$add=' meta="'.get_admin_url($vai).'"'.$add;
					}
				}
				if($show==1) echo '<option value="'.$vai.'"'.$add.'>'.$stxt.$space.safe_sql_input($value->value,2).'</option>';
				else if($show==2){
					$i=count($res);
					$res[$i]->own=$owners;				
					$res[$i]->data='<option value="'.$vai.'"'.$add.'>'.$stxt.$space.safe_sql_input($value->value,2).'</option>';
					$res[$i]->index=$vai;
				}
				else $res.='<option value="'.$vai.'"'.$add.'>'.$stxt.$space.safe_sql_input($value->value,2).'</option>';			
			}
			if(!empty($d)){
				if($show==0) $res.=$d;
				if($show==1) echo $d;
				if($show==2) $res=array_merge_recursive($res,$d);
			}
		}
	}
	return $res;
}

function get_top_row($id){
	global $rlink;
	if($id==0) return 0;
	seek_rlink($id);
	while($rlink[$id]->owner!=0){
		$id=$rlink[$id]->owner;
		seek_rlink($id);
	}
	return $rlink[$id];
}

function get_module_name($id){
	global $m,$m2,$db;
	if(!is_array($m2)){
		$m=getall($db,"SELECT * FROM main_module ORDER BY module_name",1,"main_module");
		$m2=Array();
		if(!empty($m)) foreach($m AS $mm){
			$m2[$mm["module_id"]]=$mm;
		}
	}
	if(!empty($m2[$id])) return $m2[$id]["module_name"]; else return '-';
}

function seek_max_level($rows){
	$l=1;$z=0;
	foreach($rows AS $r){
		if(!empty($r->sub)){
			$t=seek_max_level($r->sub);
			if($t>$z) $z=$t;
		}
	}
	$l+=$z;
	return $l;
}

function checkbox($rows,$space,$name,$type,$selected=Array(),$echo=1,$level=0,$ntable=0,$step=0){
	$res='';
	$level2=$level-1;
	$ignore=0;
	if(!empty($ntable)){
		if(!empty($rows)) foreach($rows AS $var=>$value){
			seek_rlink($value->id);
			global $rlink;
			$tbl=$rlink[$value->id]->table;
			if($tbl!=$ntable) $ignore=1;
			break;
		}
	}
	if(!empty($rows)) foreach($rows AS $var=>$value){
		//if($type==0 || ($type==1 && empty($value->sub))){
		$add=''; $add2=' style="display: none;"';
		if(!empty($value->description)){
			$add=' OnChange="if(this.checked) $(\'#description'.$value->id.'\').show(); else $(\'#description'.$value->id.'\').hide();"';
		}
		if(in_array($value->id,$selected) || in_array($value->value,$selected)){
			$add=' checked';
			$add2='';
		}
		$chkbx='<input type="checkbox" name="'.$name.'['.$value->id.']" class="button"'.$add.'>';
		if($type==1 && !empty($value->sub)) $chkbx='';
		if($level2!=$step && $level!=0) $chkbx='';
		if($ignore) $chkbx='';
		//if(is_array($value->value)) echo serialize($value->value).'<br>';
		if(!empty($value->sub)) $sub=checkbox($value->sub,$space.'&nbsp;&nbsp;',$name,$type,$selected,$echo,$level,$ntable,$step+1); else $sub='';
		if(!empty($chkbx) || !empty($sub)) $res.=$space.'<label style="cursor: pointer;">'.$chkbx.clean_html($value->value).'</label><br>';
		if(!empty($value->description)){
			$res.='<div id="description'.$value->id.'"'.$add2.'>'.$value->description.'</div>';
		}
		$res.=$sub;
		//}
	}
	if($echo) echo $res; else return $res;
}

// превращает обычный массив ROWS в такой же массив, в который вставляются все используемые компоненты
function get_rows_part_links(&$rows){
	foreach($rows AS $index=>$row){
		$parts=get_row_parts_links($row->id);
		$parts=unserialize($parts);
		if(!empty($parts)){
			$rows[$index]->components=$parts;
			//foreach($parts AS $part_proc=>$tmp) foreach($tmp AS $part_sname=>$t)
		}
		if(!empty($row->sub)) get_rows_part_links($rows[$index]->sub);
	}
}

function collect_part_links_from_rows($rows,$parts=Array()){
	foreach($rows AS $index=>$row){
		$parts=get_row_parts_links($row->id,$parts,0);
		$parts=$parts;
		if(!empty($row->sub)) $parts=collect_part_links_from_rows($rows[$index]->sub,$parts);
	}
	return $parts;
}

function seek_row_part_links(&$rows,$src,$dst){
	foreach($rows AS $index=>$row){
		if(!empty($row->components)){
			$r='';
			$ids=Array();
			foreach($row->components AS $part_proc=>$tmp) foreach($tmp AS $part_sname=>$t) if(empty($dst[$part_proc][$part_sname])){
				//if(!empty($r)) $r.=', ';
				$r.='<br>- '.$src[$part_sname]->name;
				$ids[]=$src[$part_sname]->id;
			}
			if(!empty($r)){
				$rows[$index]->description='<div style="background-color: #EFEFEF; padding: 5px; margin-bottom: 5px;">';
				$rows[$index]->description.='<input type="hidden" name="install_components['.$row->id.']" value="'.implode('^^',$ids).'">';
				$rows[$index]->description.='Автоматически будут загружены следующие компоненты:'.$r;
				$rows[$index]->description.='</div>';
				//echo $rows[$index]->description;
			}
		}
		if(!empty($row->sub)) seek_row_part_links($rows[$index]->sub,$src,$dst);
	}
}

function user_select($group,$sel=0,$type=0){
	global $db,$cuser2;
	$r='';
	if(!isset($cuser2[$group])){
		$tmp=getall($db,"SELECT * FROM auth_link WHERE link_group=$group",1,"auth_link",__LINE__,__FILE__);
		$s=Array();
		if(!empty($tmp)) foreach($tmp AS $t) $s[]=$t["link_user"];
		$s=implode(',',$s);
		if(!empty($s)) $cuser2[$group]=getall($db,"SELECT * FROM main_auth WHERE auth_id IN ($s) ORDER BY user_login",1,"main_auth",__LINE__,__FILE__);
		else $cuser2[$group]=Array();
	}
	$have_select=false;
	foreach($cuser2[$group] AS $u)if(check_user(-$u["auth_id"],"view",$u["auth_owner"])){
		$add=''; if($u["auth_id"]==$sel){
			$add=' selected';
			$have_select=true;
		}
		if($type==0){
			$r.='<option value="'.$u["auth_id"].'"'.$add.'>'.$u["user_login"].' ('.$u["user_name"].')</option>';
		} else if($type==1 && $u["auth_id"]==$sel){
			$r.=$u["user_login"].' ('.$u["user_name"].')';
		}
	}
	if(!$have_select && $sel && is_numeric($sel)){
		if($sel==-1) $name='Суперпользователь';
		elseif($sel==0) $name='Гость';
		else{
			if(!check_user(-$sel,'view')) $name='Скрытый пользователь';
			else {
				getrow($db,"SELECT user_name, user_login FROM main_auth WHERE auth_id=$sel AND auth_type=0");
				if(!empty($db->Record)) $name=$db->Record['user_login'].' ('.$db->Record['user_name'].')';
				else $name='Пользователь был удалён';
			}
		}
		$r.='<option value="'.$sel.'" selected>'.$name.'</option>';
	}
	return $r;
}

function echo_params($parts,&$params,$select=0){
	$res='';
	foreach($parts AS $part_id=>$part_name){
		if(!empty($params[$part_id])) foreach($params[$part_id] AS $param){
			$add='';
			if($select==$param["param_id"]) $add=' selected';
			$res.='<option value="'.$param["param_id"].'"'.$add.'>'.$part_name.' - '.$param["param_name"].'</option>';
		}
	}
	return $res;
}

function param_select($select=0){
	global $db,$m2;
	$tmp=getall($db,"SELECT param_name, param_part, param_type, param_id FROM part_param WHERE param_type=1",1,"part_param");
	$params=Array();
	if(!empty($tmp)) foreach($tmp AS $tm) $params[$tm["param_part"]][]=$tm;

	$tmp=getall($db,"SELECT part_id, part_type, part_proc, part_module, part_name FROM main_part",1,"main_part");
	$parts=Array();
	if(!empty($tmp)) foreach($tmp AS $tm) if(!empty($params[$tm["part_id"]])){
		foreach($tm AS $var=>$value) $$var=$value;
		if($part_type==2) $parts[1][$part_proc][$part_id]=$part_name;
		else $parts[2][$part_module][$part_id]=$part_name;
	}
	
	$res='';
	if(!empty($parts[1])){
		if(!empty($parts[1][0])){
			$res.='<option disabled>Функции</option>';			
			$res.=echo_params($parts[1][0],$params,$select);
		}
		if(!empty($parts[1][1])){
			$res.='<option disabled>Отображения</option>';			
			$res.=echo_params($parts[1][1],$params,$select);
		}
		if(!empty($parts[1][2])){
			$res.='<option disabled>Компоненты</option>';			
			$res.=echo_params($parts[1][2],$params,$select);
		}
		if(!empty($parts[1][3])){
			$res.='<option disabled>Формы</option>';			
			$res.=echo_params($parts[1][3],$params,$select);
		}
	}

	if(!empty($parts[2])) foreach($parts[2] AS $module_id=>$part) if(check_mod($module_id,'view')){
		if(empty($m2[$module_id])) get_module_name($module_id);
		$res.='<option disabled>Модуль "'.$m2[$module_id]["module_name"].'"</option>';
		$res.=echo_params($part,$params,$select);
	}
	
	return $res;
}

function module_select($module_url,$module_type,$selected,$search_only=0,$search_type=0,$echo_type=0,$show_new=0,$only_major=0){
	global $db,$id,$ms,$ps,$mex;
	if(empty($ms)){
		$ms2=getall($db,"SELECT * FROM main_module ORDER BY module_name",1,'main_module',__LINE__,__FILE__);
		foreach($ms2 AS $m2) $ms[$m2["module_id"]]=$m2;
	}
	if(empty($mex)){
		$tmp=getall($db,"SELECT * FROM ex_module",1,'ex_module',__LINE__,__FILE__);
		if(!empty($tmp)) foreach($tmp AS $tm) $mex[$tm["ex_module"]][$tm["ex_id"]]=$tm["ex_name"];
	}
	if(empty($ps)){
		$tmp=getall($db,"SELECT * FROM main_part WHERE part_access=1",1,'main_part',__LINE__,__FILE__);
		if(!empty($tmp)) foreach($tmp AS $tm){
			$ps[$tm["part_module"]][$tm["part_type"]][$tm["part_id"]]=$tm["part_name"];
			$ps[$tm["part_module"]][2][$tm["part_id"]]=$tm["part_name"];
		}
	}
	if($search_only){
		$res='';
		$selected=explode(':',$selected);
		if(!empty($ms[$selected[0]])){
			$res=$ms[$selected[0]]["module_name"];
			if($search_type==1) return $res;
			if(!empty($mex[$selected[0]][$selected[1]])){
				$res.=' - '.$mex[$selected[0]][$selected[1]];
				if($search_type==2) return $res;
				if(!empty($selected[2]) && !empty($ps[$selected[0]][2][$selected[2]]))
					$res.=' - '.$ps[$selected[0]][2][$selected[2]];
			}
		}
		return $res;
	}
	$res='';
	if($echo_type!=1 || $echo_type==2) $res.='<option value="0">Нет</option>';
	foreach($ms AS $m) /*if($m["module_id"]!=$id || $echo_type==1)*/ if(!$only_major || $m["module_major"]){
		$first_ex=true;
		if(empty($mex[$m["module_id"]]) && check_mod($m["module_id"],'view') && $show_new){
			if(check_mod($m["module_id"],'edit') || check_ex(0,'add',$m["module_id"])) $res.='<option value="'.$m["module_id"].'n">'.$m["module_name"].' (создать новый)</option>';
		}
		if(!empty($mex[$m["module_id"]]) && check_mod($m["module_id"],'view')) foreach($mex[$m["module_id"]] AS $mex_var=>$mex_value)if(check_ex($mex_var,'view'/*,$m["module_id"]*/)){
			if($show_new && $first_ex){
				if(check_mod($m["module_id"],'edit') || check_ex(0,'add',$m["module_id"])) $res.='<option value="'.$m["module_id"].'n">'.$m["module_name"].' (создать новый)</option>';
			}
			$first_ex=false;
			if($module_url){
				if($selected!=$m["module_id"].':'.$mex_var) $add=''; else $add=' selected';	
				$res.= '<option value="'.$m["module_id"].':'.$mex_var.'"'.$add.'>'.$m["module_name"].' - '.$mex_value.'</option>';
			} else {
				if(($echo_type==1 || $echo_type==2) && $module_type!=1){
					$add=''; if($selected==$m["module_id"].':'.$mex_var) $add=' selected';
					$res.= '<option value="'.$m["module_id"].':'.$mex_var.'"'.$add.'>'.$m["module_name"].' - '.$mex_value.' (автоопределение)</option>';
				}
				if(!empty($ps[$m["module_id"]][$module_type])) foreach($ps[$m["module_id"]][$module_type] AS $ps_var=>$ps_value){
					if($selected!=$m["module_id"].':'.$mex_var.':'.$ps_var) $add=''; else $add=' selected';	
					$res.='<option value="'.$m["module_id"].':'.$mex_var.':'.$ps_var.'"'.$add.'>'.$m["module_name"].' - '.$mex_value.' - '.$ps_value.'</option>';
				}
			}
		}
	}
	return $res;
}

function force_value($v,$c){
	//тут есть вероятность прокола с неподгруженностью col_onshow
	if(empty($c["col_onshow"])){
		if(!empty($v["value_value"])) return $v["value_value"]; else return '';
	}
	if(!empty($c["col_id"]) && !empty($v["value_row"])){
		if(!empty($GLOBALS["district_force_value"][$c["col_id"]][$v["value_row"]])){
			if(!empty($v["value_value"])) return $v["value_value"]; else return '';
		}
		$GLOBALS["district_force_value"][$c["col_id"]][$v["value_row"]]=1;
	}
	global $f_type;
	$oldft=$f_type;
	$f_type='onshow';
	$b=backup_globals();
	if(!empty($v["value_module"])) $GLOBALS["cur_module"]=$v["value_module"];
	else if(!empty($c["col_module"])) $GLOBALS["cur_module"]=$c["col_module"];
	if(!empty($v["value_table"])) $GLOBALS["cur_table"]=$v["value_table"];
	else if(!empty($c["col_table"])) $GLOBALS["cur_table"]=$c["col_table"];
	if(!empty($c["col_id"])) $GLOBALS["cur_col"]->id=$c["col_id"];
	if(!empty($v["value_row"])){
		$GLOBALS["cur_row"]=$v["value_row"];
		$GLOBALS["current"]=$v["value_row"];
		if(!empty($c["col_id"])){
			$GLOBALS["cur_col"]->id=$c["col_id"];
			$GLOBALS["cur_col"]->row=$GLOBALS["cur_row"];
		}
		global $rlink;
		seek_rlink($v["value_row"]);
		$GLOBALS["cur_ex"]=get_ex2($rlink[$v["value_row"]]->tex);
		$GLOBALS["url_row"][$GLOBALS["cur_table"]]=$GLOBALS["cur_row"];//тут по идее условия на переназначение cur_table
	}
	$res=shell_tpl($c["col_onshow"],0);
	return_globals($b);
	$f_type=$oldft;
	if(!empty($c["col_id"]) && !empty($v["value_row"])){
		$GLOBALS["district_force_value"][$c["col_id"]][$v["value_row"]]=0;
	}
	return $res;
}

function debug_wayback(){
	$x=debug_backtrace();
	foreach($x AS $xs){
		echo $xs["file"].' : '.$xs["line"].'<br>';
	}
}

function del_part_history($count=1){
	global $pup;
	if($count==0) return false;
	for($i=0;$i<$count;$i++) array_pop($pup);
}

function part_history($r,$mex=0,$mid=0){
	if(empty($GLOBALS["pup"])) $GLOBALS["pup"]=Array();
	if(isset($r->part)){
		$p->part=$r->part;
		if(isset($r->data)){
			$p->cow=$r->data["value_row"];
			if($mex==0){
				global $rlink;
				seek_rlink($p->cow);
				$mex=get_ex2($rlink[$p->cow]->tex);
				$mid=$rlink[$p->cow]->module;
			}
			$p->table=$r->data["value_table"];	
		} else {
			$p->cow=0;
			$p->table=0;
		}
		$p->module=$mid;
		$p->cex=$mex;
		$GLOBALS["pup"][]=$p;
	} else if(isset($r->data)){
		$p->part=$r->data;
		$p->cow=0;
		$p->module=$mid;
		$p->table=0;
		$p->cex=$mex;
		$GLOBALS["pup"][]=$p;
	}
}

function set_cow_part($part,$cow){
	/*global $pup;
	if(is_object($cow) && isset($cow->id)) $cow=$cow->id;
	foreach($pup AS $var=>$value){
		if($value->part==$part){
			seek_rlink($cow);
			global $rlink;
			$pup[$var]->cow=$cow;
			$pup[$var]->table=$rlink[$cow]->table;
			return;
		}
	}*/
}

function prep_do_if($condition,$part,$cow=0,$ex=0,$url=Array(),$step=1,$set_case=0){
	global $case_url;
	$b_case=$case_url;	
	$tmp=backup_globals();
	$GLOBALS["cur_module"]=$part["part_module"];
	if(empty($ex) && !empty($cow)){ seek_rlink($cow); global $rlink; $ex=get_ex2($rlink[$cow]->tex); }
	$GLOBALS["cur_ex"]=$ex;
	$GLOBALS["cur_part"]=$part["part_id"];
	$GLOBALS["cur_row"]=$cow;
	if($set_case){
		if(!empty($url[$step])) $case_url=$url[$step]; else $case_url='';
	}
	cut_left_url($url,$step);// test needed
	$res=do_if($condition);
	return_globals($tmp);
	$case_url=$b_case;
	return $res;
}

function prep_parse_var($parse,$module,$ex,$part,$cow=0,$url=''){
	$tmp=backup_globals();
	$GLOBALS["cur_module"]=$module;
	if(empty($ex) && !empty($cow)){ seek_rlink($cow); global $rlink; $ex=get_ex2($rlink[$cow]->tex); }
	$GLOBALS["cur_ex"]=$ex;
	$GLOBALS["cur_part"]=$part;
	
	/*$p->part=$part;
	$p->cow=$cow;
	$p->module=$module;
	if(!empty($cow)){
		seek_rlink($cow); global $rlink;
		$p->table=$rlink[$cow]->table;
	} else $p->table=0;
	$p->cex=$ex;	
	$GLOBALS["pup"][]=$p;*/
	
	if($cow!=0) $GLOBALS["cur_row"]=$cow;
	cut_left_url($url,1);// test needed
	$res=parse_var($parse);
	//array_pop($GLOBALS["pup"]);
	return_globals($tmp);
	return $res;
}

function get_row_id($row){
	if(is_object($row) && isset($row->id)) return $row->id; else return $row;
}

function its_cron_time(){
	if($GLOBALS["cron_type"]!=1) return false;
	if(empty($GLOBALS["last_cron_shell"])){
		$GLOBALS["last_cron_shell"]=$GLOBALS["cur_min"];
		save_timing();
		return true;
	} else if($GLOBALS["cur_min"]-$GLOBALS["last_cron_shell"]>5){
		$GLOBALS["last_cron_shell"]=$GLOBALS["cur_min"];
		save_timing();
		return true;
	}
	return false;
}

function cfg_extract($name,$string=1){
	if(!file_exists(DOCUMENT_ROOT.'/core/config.inc')) return '';
	$f=file_get_contents(DOCUMENT_ROOT.'/core/config.inc');
	if(!strpos($f,'?>')) return -1;
	if($string) $r=get_tag($f,'$'.$name.'=\'','\';'); else $r=get_tag($f,'$'.$name.'=',';');
	return $r;
}

function save_timing(){
	global $last_send_report, $last_cron_shell, $su_safe, $last_cron_date, $last_cron_hour;
	if(file_exists(DOCUMENT_ROOT.'/core/timing.inc') && (!is_readable(DOCUMENT_ROOT.'/core/timing.inc') || !is_writeable(DOCUMENT_ROOT.'/core/timing.inc'))) chmod(DOCUMENT_ROOT.'/core/timing.inc',DEF_CHMOD);
	$f=fopen(DOCUMENT_ROOT.'/core/timing.inc','w');
	fwrite($f,"<?php
	\$last_start_date='".date('Y-m-d')."';
	\$last_start_hour='".date('Y-m-d H')."';
	\$last_send_report='$last_send_report';
	\$last_cron_shell='$last_cron_shell';
	\$last_cron_date='".$last_cron_date."';
	\$last_cron_hour='".$last_cron_hour."';
	\$su_safe='$su_safe';
?>");
	fclose($f);
}

function save_config($force=false){
	global $update_server,$database,$server,$username,$password,$cache_sql,$cache_func,$cache_url,$cache_tpl,$use_dnct,$debug,$debug2,$ip_debug,$correct_utf,$e5uid,$su_login,$su_pwl,$no_cache,$do_backup,$do_backup_files,$def_charset,$do_part_log;
	global $system_email,$debug_ajax,$no_update,$black_ips,$cron_type,$is_offline,/*$last_cron_shell,*/$speed_in,$speed_out,$speed_cpu,$speed_hdd,$speed_db,$cache_tpl2,$cron_su,$def_chmod,$def_drmod,$cron_jobs,/*$su_safe,*/$protect_admin_form, $check_for_xss, $use_ace;
	global $database_type,$port;
	global $del_ip_history, $del_hour_history, $del_day_history, $del_source_history, $collect_object_history, $collect_sources_history,$statistics;
	global $rewrite_upload_max_filesize, $rewrite_post_max_size, $rewrite_memory_limit, $rewrite_max_execution_time, $rewrite_max_input_time;
	global $send_error_reports, /*$last_send_report,*/ $ignore_bot, $show_404,$session_lifetime,$session_multy,$session_storage,$session_everytime,$superuser_ip,$max_cache_size_mb;
	
	if(file_exists(DOCUMENT_ROOT.'/core/config.inc') && (!is_readable(DOCUMENT_ROOT.'/core/config.inc') || !is_writeable(DOCUMENT_ROOT.'/core/config.inc'))) chmod(DOCUMENT_ROOT.'/core/config.inc',DEF_CHMOD);
	
	if(empty($e5uid)) $e5uid=get_code2(rand(10,20));
	$def_chmod2=$def_chmod;
	$def_drmod2=$def_drmod;
	if(strlen($def_chmod2)==3) $def_chmod2='0'.decoct($def_chmod2);
	if(strlen($def_drmod2)==3) $def_drmod2='0'.decoct($def_drmod2);
	if(!$force){
		if(file_exists(DOCUMENT_ROOT.'/core/config.inc')) /*if(empty($username)) */$username=cfg_extract('username');
		if(file_exists(DOCUMENT_ROOT.'/core/config.inc')) /*if(empty($password)) */$password=cfg_extract('password');
		if(file_exists(DOCUMENT_ROOT.'/core/config.inc')) /*if(empty($su_login)) */$su_login=cfg_extract('su_login');
		if(file_exists(DOCUMENT_ROOT.'/core/config.inc')) /*if(empty($su_pwl)) */$su_pwl=cfg_extract('su_pwl');
	}
	if($su_login==-1 || $su_pwl==-1 || $username==-1 || $password==-1) return false;
	if(empty($username) && file_exists(DOCUMENT_ROOT.'/core/config.inc')) fire(0,'Save config error');
	$f=fopen(DOCUMENT_ROOT.'/core/config.inc','w');
	
	if(empty($is_offline)) $is_offline='0';
	if(empty($do_backup)) $do_backup='0';
	if(empty($do_backup_files)) $do_backup_files='0';
	if(empty($do_part_log)) $do_part_log='0';
	if(empty($use_ace)) $use_ace='0';
	if(empty($protect_admin_form)) $protect_admin_form='0';
	if(empty($check_for_xss)) $check_for_xss='0';
	if(empty($ignore_bot)) $ignore_bot='0';
	if(empty($show_404)) $show_404='0';
	if(empty($session_multy)) $session_multy=0;
	if(empty($session_everytime)) $session_everytime=0;
	$nu='';
	if(!empty($no_update)) foreach($no_update AS $var=>$value){if($nu!='') $nu.='|'; $nu.=$var;}
	$nu2='';
	if(!empty($black_ips)) foreach($black_ips AS $var=>$value){if($nu2!='') $nu2.='|'; $nu2.=$var;}
	
//	\$last_start_date='".date('Y-m-d')."';
//	\$last_start_hour='".date('Y-m-d H')."';	
	
	fwrite($f,"<?php

\$database='$database';
\$server='$server';
\$username='$username';
\$password='$password';
\$update_server='$update_server';
\$cache_sql='$cache_sql';
\$cache_func='$cache_func';
\$cache_tpl2='$cache_tpl2';
\$cache_url='$cache_url';
\$cache_tpl='$cache_tpl';
\$use_dnct='$use_dnct';
\$no_cache='$no_cache';
\$debug='$debug';
\$debug2='$debug2';
\$debug_ajax='$debug_ajax';
\$ip_debug='$ip_debug';
\$correct_utf=$correct_utf;
\$protect_admin_form=$protect_admin_form;
\$check_for_xss=$check_for_xss;
\$use_ace=$use_ace;
\$do_backup=$do_backup;
\$do_backup_files=$do_backup_files;
\$do_part_log=$do_part_log;
\$e5uid='$e5uid';
\$su_login='$su_login';
\$su_pwl='$su_pwl';
\$system_email='$system_email';
\$cron_type=$cron_type;
\$cron_su=$cron_su;
\$cron_jobs=$cron_jobs;
\$is_offline=$is_offline;
\$def_chmod=$def_chmod2;
\$def_drmod=$def_drmod2;
\$def_charset='$def_charset';
\$no_update=array_flip(explode('|','$nu'));
\$black_ips=array_flip(explode('|','$nu2'));
\$speed_in='$speed_in';
\$speed_out='$speed_out';
\$speed_cpu='$speed_cpu';
\$speed_hdd='$speed_hdd';
\$speed_db='$speed_db';
\$database_type='$database_type';
\$port='$port';
\$statistics=$statistics;
\$del_ip_history=$del_ip_history;
\$del_hour_history=$del_hour_history;
\$del_day_history=$del_day_history;
\$del_source_history=$del_source_history;
\$collect_object_history=$collect_object_history;
\$collect_sources_history=$collect_sources_history;
\$rewrite_upload_max_filesize='$rewrite_upload_max_filesize';
\$rewrite_post_max_size='$rewrite_post_max_size';
\$rewrite_memory_limit='$rewrite_memory_limit';
\$rewrite_max_execution_time='$rewrite_max_execution_time';
\$rewrite_max_input_time='$rewrite_max_input_time';
\$send_error_reports='$send_error_reports';
\$ignore_bot=$ignore_bot;
\$show_404=$show_404;
\$session_lifetime=$session_lifetime;
\$session_multy=$session_multy;
\$session_storage=$session_storage;
\$session_everytime=$session_everytime;
\$max_cache_size_mb=$max_cache_size_mb;
\$superuser_ip='$superuser_ip';

?>");	
	fclose($f);
	@chmod(DOCUMENT_ROOT.'/core/config.inc',DEF_CHMOD);
}

function del_part($part_id){
	global $db;
	$db->query("DELETE FROM main_part WHERE part_id=$part_id",3,'main_part',__LINE__,__FILE__);
	$db->query("DELETE FROM part_param WHERE param_part=$part_id",3,'part_param',__LINE__,__FILE__);
	$db->query("UPDATE main_col SET col_part=0 WHERE col_part=$part_id",3,'main_col',__LINE__,__FILE__);
}

function run_part($part_id,$do_parse=0,$from_timer=0){//$do_parse - выполнять работу по допарсингу //упразднено
	global $db;
	getrow($db,"SELECT * FROM main_part WHERE part_id=$part_id",1,"main_part",__LINE__,__FILE__);
	foreach($db->Record AS $var=>$value) $$var=$value;

	if($from_timer==0) $db->query("UPDATE main_part SET part_shell='".date('Y-m-d H:i:s')."' WHERE part_id=$part_id",3,"main_part",__LINE__,__FILE__);
	else {
		$cshell=$timer_count+1;
		if($timer_type!=1) $db->query("UPDATE main_part SET timer_count=$cshell, timer_last='".date('Y-m-d H:i:s')."', part_shell='".date('Y-m-d H:i:s')."' WHERE part_id=$part_id",3,"main_part",__LINE__,__FILE__);
		else $db->query("UPDATE main_part SET timer_count=$cshell, timer_type=0, timer_last='".date('Y-m-d H:i:s')."', part_shell='".date('Y-m-d H:i:s')."' WHERE part_id=$part_id",3,"main_part",__LINE__,__FILE__);
	}
	$mcex=getall($db,"SELECT * FROM ex_module WHERE ex_module=$part_module",1,"ex_module",__LINE__,__FILE__);
	$c_ex=count($mcex);
	if($part_ex==0){ //это условие левое и вообще непонятно как оно сюда попало
		$parser_end_ex++; if($parser_end_ex>=$c_ex)$parser_end_ex=0;
		if(isset($mcex[$parser_end_ex]["ex_id"])) $cex=$mcex[$parser_end_ex]["ex_id"]; else $cex=0;
		$db->query("UPDATE main_part SET parser_end_ex=$parser_end_ex WHERE part_id=$part_id",3,"main_part",__LINE__,__FILE__);
	} else if($part_ex==-1) $cex=0; else $cex=$part_ex;
	return shell_part($part_id,$cex,0);
}

function select_users($sel=0,$superuser=0,$guest=0,$ignore_group=0){
	global $grps_cache,$usrs_cache,$links_cache,$db;
	if(empty($_GET["group"])) $_GET["group"]=0;
	if($ignore_group==0) $ignore_group=$_GET["group"];
	if(!isset($grps_cache)){
		$grps=getall($db,"SELECT auth_id, group_name FROM main_auth WHERE auth_enable=1 AND auth_type=1 ORDER BY group_name",1,"main_auth");
		$usrs2=getall($db,"SELECT auth_id, user_login, user_name FROM main_auth WHERE auth_enable=1 AND auth_type=0 ORDER BY user_name",1,"main_auth");
		$lnks2=getall($db,"SELECT * FROM auth_link",1,"auth_link");
		$grps_cache=$grps;
		$usrs_cache=$usrs2;
		$links_cache=$lnks2;
	} else {
		$grps=$grps_cache;
		$usrs2=$usrs_cache;
		$lnks2=$links_cache;
	}
	$s='';
	//в принципе может лучше будет и без этого кеша. т.к. большенство юзеров будут видеть только ограниченное число остальных юзеров
	if(!empty($lnks2)) foreach($lnks2 AS $tmp) $lnks[$tmp["link_user"]][]=$tmp["link_group"];
	if(!empty($usrs2)) foreach($usrs2 AS $tmp){
		if(!empty($lnks[$tmp["auth_id"]])) foreach($lnks[$tmp["auth_id"]] AS $value) $usrs[$value][]=$tmp;
		if(empty($lnks[$tmp["auth_id"]])) $usr2[]=$tmp;
	}
	if(!empty($grps)) foreach($grps AS $grp) if(check_group($grp["auth_id"],'view') && check_user($grp["auth_id"],'view') && $ignore_group!=$grp["auth_id"]){
		if(!empty($usrs[$grp["auth_id"]])){
			$first=true;
			foreach($usrs[$grp["auth_id"]] AS $us) if(check_user(-$us["auth_id"],'view')){
				if($first) $s.='<optgroup label="Группа '.$grp["group_name"].'">';//$s.='<option>&nbsp;&nbsp;&nbsp;Группа '.$grp["group_name"].'</option>';
				if($us["auth_id"]==$sel) $seld=' selected'; else $seld='';
				$s.='<option value="'.$us["auth_id"].'"'.$seld.'>'.$us["user_name"].' ('.$us["user_login"].')'.'</option>';
				$first=false;
			}
			if(!$first) $s.='</optgroup>';
		}
	}
	if(!empty($usr2)){
	$first=true;
		foreach($usr2 AS $us) if(check_user(-$us["auth_id"],'view')){
			if($first) $s.='<optgroup label="Пользователи без групп">';
			if($us["auth_id"]==$sel) $seld=' selected'; else $seld='';
			$s.='<option value="'.$us["auth_id"].'"'.$seld.'>'.$us["user_name"].' ('.$us["user_login"].')'.'</option>';
			$first=false;
		}
		if(!$first) $s.='</optgroup>';
	}
	if($superuser) $s='<option value="-1"'.($sel==-1?' selected':'').'>Суперпользователь</option>'.$s;
	if($guest) $s='<option value="0"'.($sel==0?' selected':'').'>Гость</option>'.$s;
	return $s;
}

function rowcol_val($row,$col,$prepare_on_show=true){//col is array
	global $db, $rowval, $vcache, $vcache2;
	//if(is_array($row) && !empty($row["row_id"])) $row=$row["row_id"];
	//if(is_object($row) && isset($row->id)) $row=$row->id;
	if(!empty($rowval[$row][$col["col_sname"]])) $res=$rowval[$row][$col["col_sname"]];
	else {
		getrow($db,"SELECT * FROM row_value WHERE value_row=$row AND value_col=".$col["col_id"],1,"row_value",__LINE__,__FILE__);
		if(empty($db->Record)){ return ''; }
		$vcache2[$col["col_id"]][$row]=$db->Record;
		$vcache2[$col["col_id"]][$row][$db->Record["value_id"]]=$db->Record;
		if(!empty($col["col_force_onshow"]) && $prepare_on_show) $db->Record["value_value"]=force_value($db->Record,$col);
		$rowval[$row][$col["col_sname"]]=$db->Record["value_value"];
		$res=$rowval[$row][$col["col_sname"]];
	}
	return $res;
}

function clone_col_file($col,$row){//col - array of col_id, col_sname..., row - integer row_id
	global $rowval,$db;
	foreach($col AS $var=>$value) $$var=$value;
	if(!empty($rowval[$row][$col_sname])) $name=$rowval[$row][$col_sname];
	else {
		getrow($db,"SELECT * FROM row_value WHERE value_row=$row AND value_col=".$col_id,1,"row_value",__LINE__,__FILE__);
		if(empty($db->Record)) return '';
		if(!empty($col_force_onshow)) $db->Record["value_value"]=force_value($db->Record,$col);
		$rowval[$row][$col_sname]=$db->Record["value_value"];
		$name=$rowval[$row][$col_sname];
	}
	$tmp_name=DOCUMENT_ROOT.$name;
	if(empty($name) || !file_exists(DOCUMENT_ROOT.$name)) return '';
	if((sizeDirRec(DOCUMENT_ROOT.$file_dir)+filesize($tmp_name))>($file_totalmax*1024) && $file_totalmax!=0){
		$GLOBALS["cancel"]='Невозможно загрузить файл «'.$col_name.'» в папку '.$file_dir.', т.к. будет превышен максимальный размер папки';
		return '';
	}
	$nname=find_name($file_dir,$file_prefix,get_ext($name),1,$file_genname,$name);
	check_dir($file_dir);
	
	$r=copy($tmp_name,DOCUMENT_ROOT.$nname);	
	if($r){
		chmod(DOCUMENT_ROOT.$nname,DEF_CHMOD);
		return $nname;
	} else return '';	
}

function seek_rlink_ex($row){
	global $db, $gbu_priorex, $zi,$rlink,$rlink_cache,$rlink_ex_cache;
	if(isset($rlink_ex_cache[$row])) return true;
	$rlink_ex_cache[$row]=1;
	if(!isset($gbu_priorex[$zi])){
	$tmp=implode(',',getall3($db,"SELECT * FROM ex_zone WHERE ex_zone=$zi","ex_module"));
	if(empty($tmp)) $gbu_priorex[$zi]='';
	else {
			$gbu_priorex[$zi]=implode(',',getall3($db,"SELECT * FROM ex_group WHERE ex_ex2 IN ($tmp)","ex_ex1"));
		}
	}
	unset($rlink[$row]);
	if(!empty($gbu_priorex[$zi])){
		getrow($db,"SELECT * FROM row_owner WHERE row_id=$row AND ro_ex IN (".$gbu_priorex[$zi].")",1,"row_owner",__LINE__,__FILE__);
		if(!empty($db->Record)) $rlink_cache[$row]=$db->Record;
	}
	seek_rlink($row);
}

function eval_e5($code){
	//$savefile=DOCUMENT_ROOT.'/core/config.inc';
	//каким-то макаром нужно защитить этот файл. flock не катит, т.к. всегда есть file_get_contents
	//и нужно забрать из эксплуатации переменные db,db2,db3 для запрета доступа к БД
	eval($code);
}

//Функция очищает кеш при изменении экземпляров (type=0, и столбцов type=1)
//необходимость столь сложного решения возникла из-за того, что кеш не хранит используемые столбцы, а только строки
function del_cache2($type,$id){
	global $db;
	if($type==0){
		$grps=getall($db,"SELECT * FROM ex_group WHERE ex_ex2=$id",1,"ex_group");
		$tmp=Array();
		if(!empty($grps)) foreach($grps AS $grp) $tmp[]=$grp["ex_ex1"];
		$tmp=implode(',',$tmp);
		if($tmp!=''){
			$rows=getall($db,"SELECT * FROM main_row WHERE row_ex IN ($tmp)",1,"main_row");
			if(!empty($rows)) foreach($rows AS $rws) del_cache('row',$rws["row_id"]);
		}
	}
	if($type==1){
		//$vls=getall($db,"SELECT value_row FROM row_value WHERE value_col=$id",1,"row_value");
		@$db->query("SET group_concat_max_len = -1");
		getrow($db,"SELECT count(*) AS cnt FROM row_value WHERE value_col=$id");
		$cnt=$db->Record["cnt"];
		getrow($db,"SELECT GROUP_CONCAT(value_row SEPARATOR ',') AS data FROM row_value WHERE value_col=$id");
		$vls=explode(',',$db->Record["data"]);
		if($cnt>count($vls)){
			$vls=getall($db,"SELECT value_row FROM row_value WHERE value_col=$id",1,"row_value");
		}
		$rows=Array();
		if(!empty($vls)) foreach($vls AS $vl) if(isset($vl["value_row"])) $rows[$vl["value_row"]]=1;
		foreach($rows AS $row=>$val) del_cache('row',$row);
	}
}

function get_rows_from_array($rws){
	global $db,$xrows_cache;
	$line='3.'.$rws;
	if(!isset($xrows_cache[$line])){
		$xrows_cache[$line]=getall($db,"SELECT * FROM row_owner WHERE row_id IN ($rws)",1,"row_owner",__LINE__,__FILE__);
		return $xrows_cache[$line];
	} else return $xrows_cache[$line];
}

function flush_cache(){
	global $rlink,$globr,$prow,$rlink2,$vcache,$vcache2,$prow2,$risk_cache,$parow,$qc,$rowval,$prowval,$rcache,$ppro,$gsres, $user_irows_cache, $linked_count_cache;
	$rlink=Array();
	$globr=Array();
	$prow=Array();
	$prow2=Array();
	$rlink2=Array();
	$vcache=Array();
	$vcache2=Array();
	$risk_cache=Array();
	$parow=Array();
	$rowval=Array();
	$prowval=Array();
	$rcache=Array();
	$ppro=Array();
	$gsres=Array();
	$user_irows_cache=Array();
	$linked_count_cache=Array();
	unset($qc);
}

function update_module_state($module){
	global $db;
	if(empty($module)) return false;
	reset_module_json($module);
	$db->query("UPDATE main_module SET module_date2='".date('Y-m-d H:i:s')."' WHERE module_id=$module",3,"main_module");
}

function update_table_state($table){
	global $db;
	if(empty($table)) return false;
	$db->query("UPDATE main_table SET table_date2='".date('Y-m-d H:i:s')."' WHERE table_id=$table",3,"main_table");
}

function update_row_state($row,$date=''){
	global $db;
	if(empty($row)) return false;
	if(empty($date)) $date=date('Y-m-d H:i:s');
	if(is_array($row)) $db->query("UPDATE main_row SET modified_date='$date' WHERE row_id IN (".implode(',',$row).")",3,"main_row");
	else $db->query("UPDATE main_row SET modified_date='$date' WHERE row_id=$row",3,"main_row");
}

function reset_row_part_links($row){
	global $rlink,$db;
	seek_rlink($row);
	$table=$rlink[$row]->table;
	$module=$rlink[$row]->module;
	$cols=getall3($db,"SELECT col_id FROM main_col WHERE col_table=$table AND col_module=$module AND col_tpl=1","col_id");
	if(empty($cols)) return false;
	$cols=implode(',',$cols);
	$vals=getall($db,"SELECT * FROM row_value WHERE value_row=$row AND value_col IN ($cols) AND value_table=$table AND value_value!=''");
	if(empty($vals)) return false;
	foreach($vals AS $val) update_part_links($row,'val',getrowval("SELECT col_uin FROM main_col WHERE col_id=".$val['value_col'],"col_uin"),$val['value_value'],'row');
}

function update_part_state($part){
	global $db;
	if(empty($part)) return false;
	$db->query("UPDATE main_part SET part_date2='".date('Y-m-d H:i:s')."' WHERE part_id=$part",3,"main_part");
}

function search_part_data($data,$key,$res=Array()){
	$len=strlen($data);
	global $allow_search_part_data;
	if(empty($allow_search_part_data)){
		$allow_tmp='qwertyuiopasdfghjklzxcvbnmQWERTYUIOPASDFGHJKLZXCVBNM_1234567890';
		for($i=0;$i<strlen($allow_tmp);$i++) $allow[$allow_tmp[$i]]=1;
		$allow_search_part_data=$allow;
	} else $allow=$allow_search_part_data;
	$x=-1;
	while($x=strpos($data,$key,$x+1)){
		$tmp='';
		for($i=$x+strlen($key);$i<$len;$i++){		
			//if($data[$i]=='.' || $data[$i]==']' || $data[$i]=='(' || $data[$i]==')' || $data[$i]==' ') break;
			if(empty($allow[$data[$i]])) break;
			else $tmp.=$data[$i];
		}
		if(!empty($tmp)) $res[$tmp]=$tmp;
	}
	return $res;
}

function update_part_links($id,$type,$uin,$data,$src='module',$table=0,$module=0){
	global $db;
	if(empty($uin)) return false;
	
	if($src=='table'){
		$tm=getrowval("SELECT table_module FROM main_table WHERE table_id=$id","table_module");
		if(!empty($tm)){
			$src='module';
			$id=$tm;
		} else $parts_tmp=getrowval("SELECT table_parts FROM main_table WHERE table_id=$id","table_parts");
	}
	if($src=='module'){
		$parts_tmp=getrowval("SELECT module_id, module_parts FROM main_module WHERE module_id=$id","module_parts");
	}
	if($src=='part'){
		$parts_tmp=getrowval("SELECT part_parts FROM main_part WHERE part_id=$id","part_parts");
	}
	if($src=='col'){
		$parts_tmp=getrowval("SELECT col_parts FROM main_col WHERE col_id=$id","col_parts");
	}
	if($src=='row'){
		if(empty($module)){
			seek_rlink($id); global $rlink;
			$table=$rlink[$id]->table;
			$module=$rlink[$id]->module;
		}
		getrow($db,"SELECT value_value FROM row_value WHERE value_row=$id AND value_col=0 AND value_table=$table AND value_module=$module");
		if(!empty($db->Record)) $parts_tmp=$db->Record["value_value"];
		else $row_insert=true;
	}
	
	// Unpack data
	if(empty($parts_tmp)) $parts=Array(); else $parts=unserialize($parts_tmp);
	$key=$type.'_'.$uin;
	if(isset($parts[$key])) unset($parts[$key]);

	// Search keys
	$components=search_part_data($data,'component.');
	$functions=search_part_data($data,'func.');
	$functions=search_part_data($data,'function.',$functions);
	$forms=search_part_data($data,'form.');
	$shows=search_part_data($data,'show.');
	if($src=='row'){
		$links=search_part_data($data,'uin:');
	} else $links=Array();
	// желательно добавить сюда поиск виджетов (но есть проблема с переброской их экземпляров)
	
	// Check keys
	$res=Array();
	if(!empty($functions)) $res=getall9($db,"SELECT part_type, part_proc, part_sname FROM main_part WHERE part_type=2 AND part_proc=0 AND part_sname IN ('".implode("','",$functions)."')","part_proc","part_sname",$res);
	if(!empty($shows)) $res=getall9($db,"SELECT part_type, part_proc, part_sname FROM main_part WHERE part_type=2 AND part_proc=1 AND part_sname IN ('".implode("','",$shows)."')","part_proc","part_sname",$res);
	if(!empty($components)) $res=getall9($db,"SELECT part_type, part_proc, part_sname FROM main_part WHERE part_type=2 AND part_proc=2 AND part_sname IN ('".implode("','",$components)."')","part_proc","part_sname",$res);
	if(!empty($forms)) $res=getall9($db,"SELECT part_type, part_proc, part_sname FROM main_part WHERE part_type=2 AND part_proc=3 AND part_sname IN ('".implode("','",$forms)."')","part_proc","part_sname",$res);
	if(!empty($links)) $res['links']=getall3($db,"SELECT row_id FROM main_row WHERE row_uin IN ('".implode("','",$links)."')",'row_id');
	
	// Attach m_links to row
	if($src=='row'){
		$widgets=search_part_data($data,'widget.');
		if(!empty($widgets)){
			foreach($widgets AS $wg){
				$tmp=search_part_data($data,'widget.'.$wg.'.');
				if(empty($tmp)) continue;
				foreach($tmp AS $t){$ex_sname=$t; break;}
				$module_uin=getrowval("SELECT module_uin FROM main_module WHERE module_sname='$wg'","module_uin");
				$module_id=getrowval("SELECT module_id FROM main_module WHERE module_sname='$wg'","module_id");
				if(!empty($module_id)){
					$ex_uin=getrowval("SELECT ex_uin FROM ex_module WHERE ex_module=$module_id AND ex_sname='$ex_sname'","ex_uin");
					if(!empty($module_uin) && !empty($ex_uin)) $res['links'][]=$module_uin.':'.$ex_uin;
				}
			}
		}
	}
	
	// Write to data
	if(!empty($res)) $parts[$key]=$res;
	
	// Save data
	$parts_tmp=safe_sql_input(serialize($parts));
	
	if($src=='module'){
		$db->query("UPDATE main_module SET module_parts='$parts_tmp' WHERE module_id=$id");
	}
	if($src=='table'){
		$db->query("UPDATE main_table SET table_parts='$parts_tmp' WHERE table_id=$id");
	}
	if($src=='part'){
		$db->query("UPDATE main_part SET part_parts='$parts_tmp' WHERE part_id=$id");
	}
	if($src=='col'){
		$db->query("UPDATE main_col SET col_parts='$parts_tmp' WHERE col_id=$id");
	}
	if($src=='row'){
		if(!empty($row_insert)){
			$db->query("INSERT INTO row_value (value_row, value_col, value_table, value_module, value_value)
									VALUES ($id, 0, $table, $module, '$parts_tmp')");
		} else {
			$db->query("UPDATE row_value SET value_value='$parts_tmp' WHERE value_row=$id AND value_table=$table AND value_module=$module AND value_col=0");
		}
	}
	
}

function get_row_uin_links($row,$is_ex=false){
	global $rlink;
	if(!$is_ex){
		seek_rlink($row);
		$tmp=getrowval("SELECT value_value FROM row_value WHERE value_row=$row AND value_table=".$rlink[$row]->table." AND value_module=".$rlink[$row]->module." AND value_col=0",'value_value');
	} else {
		$module_id=getrowval("SELECT ex_module FROM ex_module WHERE ex_id=$row","ex_module");
		$tmp=getrowval("SELECT value_value FROM row_value WHERE value_row=$row AND value_table=0 AND value_module=".$module_id." AND value_col=0",'value_value');
	}
	if(!empty($tmp)){
		$tmp=unserialize($tmp);
		foreach($tmp AS $t) if(!empty($t['links'])) return $t['links'];
	}
	return Array();
}

function get_row_parts_links($row,$parts=Array(),$need_serialize=1,$is_ex=false){
	global $rlink;
	if(!$is_ex){
		seek_rlink($row);
		$tmp=getrowval("SELECT value_value FROM row_value WHERE value_row=$row AND value_table=".$rlink[$row]->table." AND value_module=".$rlink[$row]->module." AND value_col=0",'value_value');
	} else {
		$module_id=getrowval("SELECT ex_module FROM ex_module WHERE ex_id=$row","ex_module");
		$tmp=getrowval("SELECT value_value FROM row_value WHERE value_row=$row AND value_table=0 AND value_module=".$module_id." AND value_col=0",'value_value');
	}
	if(!empty($tmp)) return get_all_part_links($tmp,$parts,!$need_serialize);
	else {	
		if($need_serialize) return serialize($parts);
		else return $parts;
	}
}

function get_all_part_links($parts,$res=Array(),$return_array=false){
	global $db;
	if(!empty($db->Record)) $odb=$db->Record;
	if(!empty($parts)) $parts=unserialize($parts);
	else {
		if($return_array) return $res;
		else return serialize($res);
	}
	
	foreach($parts AS $key=>$prts){
		foreach($prts AS $part_proc=>$tmp) if(is_numeric($part_proc) || empty($part_proc) || $part_proc!='links') foreach($tmp AS $part_sname=>$t){
			if(empty($res[$part_proc]) || empty($res[$part_proc][$part_sname])){
				$new_parts=getrowval("SELECT part_parts FROM main_part WHERE part_type=2 AND part_proc=$part_proc AND part_sname='$part_sname'",'part_parts');
				if(!empty($new_parts)){
					$res[$part_proc][$part_sname]=$part_sname;
					$res=get_all_part_links($new_parts,$res,true);
				}
			}
			$res[$part_proc][$part_sname]=$part_sname;
		}
	}
	
	if(!empty($odb)) $db->Record=$odb;
	if($return_array) return $res;
	else return serialize(/*$parts*/$res);
}

function get_all_part_links_for_cat($cat_id){
	global $db;
	if(!empty($db->Record)) $odb=$db->Record;
	$res=Array();
	$parts=getall($db,"SELECT part_parts, part_proc, part_sname FROM main_part WHERE part_cat=$cat_id AND part_type=2",'part_parts');
	if(!empty($parts)) foreach($parts AS $p){
		$res[$p['part_proc']][$p['part_sname']]=$p['part_sname'];
	}
	if(!empty($parts)) foreach($parts AS $p)if(!empty($p['part_parts'])){
		$res=get_all_part_links($p['part_parts'],$res,1);
	}
	if(!empty($parts)) foreach($parts AS $p){
		unset($res[$p['part_proc']][$p['part_sname']]);
	}
	if(!empty($odb)) $db->Record=$odb;
	return serialize($res);
}

function del_part_links($id,$type,$uin,$src='module'){
	global $db;
	if(empty($uin)) return false;
	if(empty($id)) return false;
	
	// Unpack data
	if($src=='module') $parts_tmp=getrowval("SELECT module_id, module_parts FROM main_module WHERE module_id=$id","module_parts");
	if($src=='table') $parts_tmp=getrowval("SELECT table_parts FROM main_table WHERE table_id=$id","table_parts");
	if($src=='row'){
		seek_rlink($id); global $rlink;
		$table=$rlink[$id]->table;
		$module=$rlink[$id]->module;
		getrow($db,"SELECT value_value FROM row_value WHERE value_row=$id AND value_col=0 AND value_table=$table AND value_module=$module");
		if(!empty($db->Record)) $parts_tmp=$db->Record["value_value"];
		else return false;
	}
	if(empty($parts_tmp)) $parts=Array(); else $parts=unserialize($parts_tmp);
	$key=$type.'_'.$uin;
	if(isset($parts[$key])) unset($parts[$key]);

	// Save data
	$parts_tmp=safe_sql_input(serialize($parts));
	if($src=='module') $db->query("UPDATE main_module SET module_parts='$parts_tmp' WHERE module_id=$id");
	if($src=='table') $db->query("UPDATE main_table SET table_parts='$parts_tmp' WHERE table_id=$id");
	if($src=='row') $db->query("UPDATE row_value SET value_value='$parts_tmp' WHERE value_row=$id AND value_table=$table AND value_module=$module AND value_col=0");
}

function get_protection_code(){
	global $e5uid;
	return (string)md5($e5uid);
}

function get_exes($owner,$ex){
	global $db,$exes_cache;	
	$line=$owner.'.'.$ex;
	if(isset($exes_cache[$line])) return $exes_cache[$line];
	if(!empty($owner)){
		//эта вещь жрёт запросы...
		//if(!strstr($owner,',')) $exes=getall3($db,"SELECT * FROM row_owner WHERE row_id=$owner","ro_ex");
		//else $exes=getall3($db,"SELECT * FROM row_owner WHERE row_id IN ($owner)","ro_ex");
		if(!strstr($owner,',')) $tmp=getall($db,"SELECT * FROM row_owner WHERE row_id=$owner",1,"ro_ex");
		else $tmp=getall($db,"SELECT * FROM row_owner WHERE row_id IN ($owner)",1,"ro_ex");
		foreach($tmp AS $t){
			seek_rlink($t);//для оптимизации SQL запросов
			$exes[$t["ro_ex"]]=$t["ro_ex"];
		}
	}
	if(strstr($ex,',')){
		$ex=explode(',',$ex);
		foreach($ex AS $cex) $exes[$cex]=$cex;
	} else $exes[$ex]=$ex;
	if(empty($exes)) $exes[$ex]=$ex;
	$res=implode(',',$exes);
	$exes_cache[$line]=$res;
	return $res;
}

function seek_top_owner($id){
	global $rlink;
	seek_rlink($id);//тут по идее надо поменять на seek_rlink_ex
	while($rlink[$id]->owner){
		$id=$rlink[$id]->owner;
		seek_rlink($id);
	}
	return $id;
}

//ищет tex таблицы $table, по имеющимуся $tex от другой таблицы
function seek_match_tex($tex,$table){
	$exs=get_ex_all($tex);
	if(empty($exs)) return $tex;
	$res=getrowval("SELECT * FROM ex_group WHERE ex_ex2 IN (".implode(',',$exs).") AND ex_table=$table",'ex_ex1');
	if(empty($res)) return $tex;
	else return $res;
}

//ищет активность всех строк массива $rows с учётом их активных родителей
function deep_seek_activity($rows){
	global $db;
	if(empty($rows)) return $rows;
	$r=getall8($db,"SELECT row_id, ro_enable, owner_id FROM row_owner WHERE row_id IN (".implode(',',$rows).") AND ro_enable=1","owner_id","row_id");
	$k=Array();
	$res=Array();
	foreach($r AS $owner=>$rs){
		if($owner!=0) $k[]=$owner;
		else $res=$rs;
	}
	if(!empty($k)){
		$tk=deep_seek_activity($k);
		$tk=array_flip($tk);
		foreach($r AS $owner=>$rs){
			if($owner!=0 && isset($tk[$owner])) $res=array_merge($res,$rs);
		}
	}
	return $res;
}

function get_filters($filters_tmp,$table,&$cols_preload,$from_find=0,$basic_type=''){
	$i=0;
	global $lex_equal;
	$filters=Array();
	if(is_object($filters_tmp) && isset($filters_tmp->sub)){
		$filters_tmp=$filters_tmp->sub;
	}
	if(!$from_find) check_single($filters_tmp,Array(',','|','or','and'),1);
	//var_dump($filters_tmp);echo '<br><br><br><br>';
	if(!empty($filters_tmp)) foreach($filters_tmp AS $tmp)if(is_object($tmp)){
		//var_dump($tmp);
		if($from_find) $filters[$i]->next=$basic_type;
		else $filters[$i]->next=$tmp->next_operation;
		if($from_find){
			if($tmp->data=='where_set'){
				$filters[$i]->sub=get_filters($tmp->put[1]->sub,$table,$cols_preload,1,$tmp->put[0]->data);
				$i++;
				continue;
			}
		} else {
			if($tmp->type==LEXER_SUB && !empty($tmp->sub) && !isset($lex_equal[$tmp->sub[0]->next_operation])){
				$filters[$i]->sub=get_filters($tmp->sub,$table,$cols_preload);
				$i++;
				continue;
			}
		}
		$filters[$i]->col='';
		$filters[$i]->val='';
		$filters[$i]->op='';
		$filters[$i]->reverse=false;
		$filters[$i]->lower=false;
		$filters[$i]->nullval=false;
		$filters[$i]->length=false;
		
		if($from_find){
			$o=get_real_data($tmp->put[1]);
			$filters[$i]->val='';
			if(!empty($tmp->put[2])){
				$filters[$i]->val=parse_var($tmp->put[2]);
				if(in_array($filters[$i]->val,Array('mutate_lower','mutate_length'))){
					$filters[$i]->val='';
					$tmp->put[3]=$tmp->put[2];
				}
			}
			if($o=='contain') $o='^';
			if($o=='not_contain') $o='!^';
			if($o=='contained') $o='^!';
			if($o=='not_contained') $o='!^!';
			if($o=='in_array') $o='^^';
			if($o=='not_in_array') $o='!^^';
			if($o=='empty'){
				$o='!=';
				$filters[$i]->val='';
				$filters[$i]->nullval=true;
				$filters[$i]->reverse=true;
			}
			if($o=='not_empty'){
				$o='!=';
				$filters[$i]->val='';
				$filters[$i]->nullval=true;
			}
			$filters[$i]->op=$o;
			if(!empty($tmp->put[3])){
				if(get_real_data($tmp->put[3])=='mutate_lower') $filters[$i]->lower=true;
				if(get_real_data($tmp->put[3])=='mutate_length') $filters[$i]->length=true;
			}
			$filters[$i]->col=get_real_data($tmp->put[0]);			
		} else {
			if($tmp->type==LEXER_SUB && count($tmp->sub)==2 && isset($lex_equal[$tmp->sub[0]->next_operation])){
				$filters[$i]->op=$tmp->sub[0]->next_operation;
				$filters[$i]->val=parse_var($tmp->sub[1]);
				if($tmp->sub[0]->data[0]=='_'){
					$filters[$i]->lower=true;
					$x=substr($tmp->sub[0]->data,1);
					if($x[0]=='\'' && $x[strlen($x)-1]=='\''){
						$x=substr($x,1,strlen($x)-2);
						$tmp->sub[0]->type=LEXER_STR;
					}
					if($x[0]=='"' && $x[strlen($x)-1]=='"'){
						$x=substr($x,1,strlen($x)-2);
						$tmp->sub[0]->type=LEXER_STR;
					}
					$tmp->sub[0]->data=$x;
				}
				if($tmp->sub[0]->data[0]=='#'){
					$filters[$i]->length=true;
					$x=substr($tmp->sub[0]->data,1);
					if($x[0]=='\'' && $x[strlen($x)-1]=='\''){
						$x=substr($x,1,strlen($x)-2);
						$tmp->sub[0]->type=LEXER_STR;
					}
					if($x[0]=='"' && $x[strlen($x)-1]=='"'){
						$x=substr($x,1,strlen($x)-2);
						$tmp->sub[0]->type=LEXER_STR;
					}
					$tmp->sub[0]->data=$x;
				}
				$filters[$i]->col=$tmp->sub[0];
				if(isset($tmp->sub[0]->mutator) && $tmp->sub[0]->mutator==MUTATOR_NOT){
					$filters[$i]->reverse=true;
					$filters[$i]->col->mutator=0;
				}
			} else {
				$filters[$i]->op='!=';
				$filters[$i]->val='';
				$filters[$i]->nullval=true;
				if($tmp->data[0]=='_'){
					$filters[$i]->lower=true;
					$tmp->data=substr($tmp->data,1);
				}
				$filters[$i]->col=$tmp;
				if(isset($tmp->mutator) && $tmp->mutator==MUTATOR_NOT){
					$filters[$i]->reverse=true;
					$filters[$i]->col->mutator=0;
				}
			}
			if(empty($filters[$i]->col)) break;
			$filters[$i]->col=parse_var($filters[$i]->col);
		}
		if($filters[$i]->col=='major'){
			$tmp=getrowval("SELECT major_col FROM main_table WHERE table_id=$table",'major_col');
			if(!$tmp) continue;
			$tmp=getrowval("SELECT col_sname FROM main_col WHERE col_id=$tmp",'col_sname');
			if(!$tmp) continue;
			$filters[$i]->col=$tmp;
		}
		$cols_preload[$filters[$i]->col]=$filters[$i]->col;
		$i++;
	}
	return $filters;
}

function apply_filters($filters,&$SQL_where,&$SQL_leftjoin,&$f_id,$cols){
	global $include_row;
	$a='';
	foreach($filters AS $filter)if(!empty($filter->col) || !empty($filter->sub)){
		if(!empty($filter->sub)){
			$SQL_where.=$a.'(';
			apply_filters($filter->sub,$SQL_where,$SQL_leftjoin,$f_id,$cols);
			$SQL_where.=')';
			if($filter->next=='|' || $filter->next=='or') $a=' OR '; else $a=' AND ';
			continue;
		}
		if($filter->op!='!^^' && $filter->op!='^^' && is_array($filter->val)) continue;
		if(($filter->op=='^^' || $filter->op=='!^^') && empty($filter->val)) continue;
		$SQL_leftjoin.=" LEFT JOIN row_value AS rv".$f_id." ON rv".$f_id.".value_row=ro.row_id";
		if($filter->col!='id' && $filter->col!='owner' && $filter->col!='pos' && $filter->col!='own' && $filter->col!='user' && $filter->col!='usr' && $filter->col!='creation_date' && $filter->col!='modified_date'){
			if(!empty($cols[$filter->col])) $filter->col=$cols[$filter->col];
			$SQL_leftjoin.=" AND rv".$f_id.".value_col=".$filter->col;
		}
		if($filter->col=='id') $cv="rv".$f_id.".value_row";
		else if($filter->col=='owner' || $filter->col=='own') $cv="ro.owner_id";
		else if($filter->col=='user' || $filter->col=='usr') $cv="ro.ro_user";
		else if($filter->col=='pos') $cv="ro.ro_pos";
		else if($filter->col=='creation_date'){
			$cv="row.creation_date";
			$include_row=true;
		} else if($filter->col=='modified_date'){
			$cv="row.modified_date";
			$include_row=true;
		} else $cv="rv".$f_id.".value_value";
		if($filter->lower){
			if(is_array($filter->val)){
				foreach($filter->val AS $var2=>$value) $filter->val[$var2]=strtolower($value);
			}else $filter->val=strtolower($filter->val);
			$cv="LOWER(rv".$f_id.".value_value)";
		}
		if($filter->length){
			$cv="CHAR_LENGTH(rv".$f_id.".value_value)";
		}
		$value='';
		if(is_object($filter->val) && isset($filter->val->rows)){
			$tmp=$filter->val->rows;
			unset($filter->val);
			$filter->val=$tmp;
		}
		if(is_array($filter->val)){
			foreach($filter->val AS $var2=>$value){
				if(is_object($value) && !empty($value->id)){
					$tmp=$value->id;
					unset($filter->val[$var2]);
					$filter->val[$var2]=$value->id;
				}
				else $filter->val[$var2]=prepend_value($value);
			}
		} else {
			if(is_object($filter->val) && !empty($filter->val->id)) $value=$filter->val->id;
			else $value=prepend_value($filter->val);
		}
		if($filter->op=='==') $value="='".$value."'";
		if($filter->op=='>'){
			if(is_numeric($value)) $value=">".$value;
			else $value=">'".$value."'";
		}
		if($filter->op=='>='){
			if(is_numeric($value)) $value=">=".$value;
			else $value=">='".$value."'";
		}
		if($filter->op=='<'){
			if(is_numeric($value)) $value="<".$value;
			else $value="<'".$value."'";
		}
		if($filter->op=='<='){
			if(is_numeric($value)) $value="<=".$value;
			else $value="<='".$value."'";
		}
		if($filter->op=='!=') $value="!='".$value."'";
		if($filter->op=='!^^'){
			$filter->reverse=!$filter->reverse;
			$value=" IN ('".implode("','",$filter->val)."')";
		}
		if($filter->op=='^^'){
			$value=" IN ('".implode("','",$filter->val)."')";
		}
		if($filter->op=='!^') $value=" NOT LIKE '%".$value."%'";
		if($filter->op=='^') $value=" LIKE '%".$value."%'";
		if($filter->op=='!^!'){
			$value="concat('%',concat($cv,'%')) AND $cv!=''";
			$cv="'%$value%' NOT LIKE ";
		}
		if($filter->op=='^!'){
			$value="concat('%',concat($cv,'%')) AND $cv!=''";
			$cv="'%$value%' LIKE ";
			//based on WHERE 'data' LIKE concat('%',concat(field_name,'%')) AND field_name!=''
			//conclusion
		}
		if(!$filter->nullval){		
			if($filter->reverse)  $SQL_where.=$a."NOT (".$cv.$value.')';
			else $SQL_where.=$a.$cv.$value;
		} else {
			//операция без условия
			if($filter->reverse) $SQL_where.=$a."($cv='' OR $cv='0' OR $cv IS NULL)";
			else $SQL_where.=$a."($cv!='' AND $cv!='0' AND NOT ($cv IS NULL))";
		}
		if($filter->next=='|' || $filter->next=='or') $a=' OR '; else $a=' AND ';
		$f_id++;
	}
}

function fastget(&$op,&$ops,$rws=Array(),$table=0,$tex=0,$owner=0){
	//	Два варианта вызова
	//	первый с RWS (строки, в которых будет происходить поиск / вызов - rows.fastget() из cmd_arr )
	//	второй - без RWS, но с указанием table, tex и owner (вызов table.fastget() из parse_table )	
	
	global $utpl,$db,$rlink;
	global $include_row, $include_row_backup;
	$include_row_backup=$include_row;
	//prepend
	if(empty($rws) && empty($table)) return Array();

	$from_find=false;
	$check_auth=false;
	if($op->data=='find' || $op->data=='find_count'){
		$from_find=true;
		$ignore_owner=false;
		$ignore_ex=false;
		$ignore_all_ex=false;
		$activity='act';
		$return_owners=false;
		$only_count=($op->data=='find_count');
		if(!empty($op->put)) foreach($op->put AS $p){
			if($p->data=='ignore_owner') $ignore_owner=true;
			if($p->data=='ignore_ex') $ignore_ex=true;
			if($p->data=='ignore_module') $ignore_all_ex=true;
			if($p->data=='disabled') $activity='no';
			if($p->data=='all') $activity='all';
			if($p->data=='return_owners') $return_owners=true;
			if($p->data=='check_auth') $check_auth=true;
			if(!empty($p->sub) && $p->sub[0]->data=='subtable'){
				$stable_sname=get_real_data($p->sub[1]);
			}
		}
		$filters_tmp=Array();
		$orders_tmp=Array();
		while(in_array(get_first_data_safe($ops),Array('where','where_set','sort','start','limit'))){
			$o=get_first_op($ops);
			if($o->data=='where'){
				$filters_tmp[]=$o;
			} else if($o->data=='where_set'){
				$filters_tmp[]=$o;
			} else if($o->data=='sort'){
				$orders_tmp[]=$o;
			} else if($o->data=='start'){
				$start=parse_var($o->put);
			} else if($o->data=='limit'){
				$limit=parse_var($o->put);
			}
		}
		/*
		//добавляем rows_auth
		if($check_auth){
			add_new_op($ops,create_op('rows_auth',CMD_ARRAY,'.'));
		}*/
	} else {
		if($op->data[0]=='e'){
			$ignore_ex=true;
			$op->data=substr($op->data,1);
		} else $ignore_ex=false;
		if($op->data[0]=='a'){
			$ignore_all_ex=true;
			$op->data=substr($op->data,1);
		} else $ignore_all_ex=false;
		if($op->data[0]=='i'){
			$ignore_owner=true;
			$op->data=substr($op->data,1);
		} else $ignore_owner=false;
		$var[0]=$op->data;
		if(empty($op->put) && isset($op->next_operation) && $op->next_operation=='.' /*&& !empty($ops[0]->put)*//* <- ? нельзя т.к. может быть и fastgetc:articles как пример (т.е. без скобок) */ && check_arrayB($ops)){
			$op=array_shiftA($ops);
			$stable_sname=$op->data;
		}
		$activity='act';//only active
		$i=0;
		if(!empty($var[0][7]) && $var[0][7]=='2'){
			$activity='all';//all
			$i++;
		}
		if(!empty($var[0][7]) && $var[0][7]=='3'){
			$activity='no';//only n/active
			$i++;
		}
		if(!empty($var[0][7+$i]) && $var[0][7+$i]=='c'){
			$i++;
			$only_count=true;
		} else $only_count=false;
		//Разбираем скобки
		check_single($op->put,Array(' '));
		if(shift_op($op->put,'FG_RETURN_OWNERS')) $return_owners=true;
		else $return_owners=false;
		if(!empty($op->put)){
			$orders_tmp=Array();
			$filters_tmp=Array();
			$limit=0;
			$start=0;
			$filters_tmp=$op->put[0];
			if(!empty($op->put[1])){
				$orders_tmp=$op->put[1];
				if(is_object($orders_tmp) && !empty($orders_tmp->sub) && $orders_tmp->type==LEXER_SUB) $orders_tmp=$orders_tmp->sub;
				if(count($orders_tmp)==1) $orders_tmp=Array($orders_tmp);
			}
			if(!empty($op->put[2])) $start=parse_var($op->put[2]);
			if(!empty($op->put[3])) $limit=parse_var($op->put[3]);
		}
	}

	if(!empty($start) && empty($limit)){
		$start=0;
		$limit=$start;
	}
	
	if(!empty($rws)){
		if(is_object($rws) && isset($rws->rows)) $rws=$rws->rows;
		foreach($rws AS $rw){
			$x=$rw;
			break;
		}
		if(is_numeric($x)){
			seek_rlink($x);
			$x=$rlink[$x];
			$rws=implode(',',collect_rows_cross($rws));
		} else $rws=implode(',',collect_rows_cross($rws));
		if(!isset($x->table)){
			//такой косяк после того, как row проходит через prepend_result (или prepend_type) шаблонизатора (т.к. там тип есть, и id есть, но при этом это не класс rlink)
			seek_rlink($x->id);
			$x=$rlink[$x->id];
		}
		$table=$x->table;
		$owner=$x->owner;
		$tex=$x->tex;
		if(empty($rws)){
			$include_row=$include_row_backup;
			return Array();
		}
		$SQL_zone="ro.row_id IN (".$rws.")";
	} else {
		//$ro_exes=get_exes($owner,$tex);
		//почему-то тут вылазиит периодически ошибка, что нет ro_exes
		/*if(!$ignore_owner){
			if(!empty($ro_exes)) $SQL_zone="ro.ro_ex IN (".$ro_exes.") AND ro.owner_id=".$owner;
			else $SQL_zone="ro.owner_id=".$owner;
		} else {
			if(!empty($ro_exes)) $SQL_zone="ro.ro_ex IN (".$ro_exes.")";
			else $SQL_zone="";
		}*/
		$SQL_zone='';
		if($ignore_all_ex){
			$SQL_zone='';
		} elseif(!$ignore_ex){
			$ro_exes=get_exes($owner,$tex);
			if(!empty($ro_exes)) $SQL_zone.="ro.ro_ex IN (".$ro_exes.")";
		} else {
			$ro_exes=get_all_table_ex($table,getrowval("SELECT table_module FROM main_table WHERE table_id=".$table,'table_module'),get_ex2($tex));
			if(!empty($ro_exes)) $SQL_zone.="ro.ro_ex IN (".implode(',',$ro_exes).")";
		}
		if(!$ignore_owner){
			if(!empty($SQL_zone)) $SQL_zone.=' AND ';
			$SQL_zone.='ro.owner_id='.$owner;
		}
	}
	
	//Поддержка поиска в подтаблице
	if(!empty($stable_sname)){
		$stbls=getall3($db,"SELECT * FROM table_sub WHERE sub_table1=$table","sub_table2");
		if(!empty($stbls)){
			$stable_id=getrowval("SELECT table_sname, table_id FROM main_table WHERE table_sname='$stable_sname' AND table_id IN (".implode(',',$stbls).")",'table_id');
		}
		if(empty($stable_id)){
			$include_row=$include_row_backup;
			return Array();
		}
		if(empty($rws)){
			if(!$ignore_owner){
				$rws=getall3($db,"SELECT * FROM row_owner WHERE owner_id=$owner AND row_table=$table AND ro_ex IN (".get_exes($owner,$tex).") AND ro_enable=1","row_id");
			} else {
				$rws=getall3($db,"SELECT * FROM row_owner WHERE row_table=$table AND ro_ex IN (".get_exes($owner,$tex).") AND ro_enable=1","row_id");
			}
			if(empty($rws)){
				$include_row=$include_row_backup;
				return Array();
			}
			$rws=implode(',',$rws);
		}
		$table=$stable_id;
		$SQL_zone="ro.owner_id IN (".$rws.")";
	}
	
	//work
	$utpl["rows"][$table.'.'.$tex]=1;
	
	//parse filter
	$filters=Array();
	//нужна также фильтрация по sub-ам (вроде [cex.cat.fastget("article.name"==1)], где article - подтаблица cat
	$cols_preload=Array();
	if(isset($filters_tmp)) $filters=get_filters($filters_tmp,$table,$cols_preload,$from_find,'and');
		
	//parse order
	$orders=Array();
	$i=0;
	if(!empty($orders_tmp)) foreach($orders_tmp AS $tmp){
		$orders[$i]->order='ASC';
		$orders[$i]->col='';
		
		if($from_find){
			if(!empty($tmp->put[1]) && get_real_data($tmp->put[1])=='desc') $orders[$i]->order='DESC';
			if(!empty($tmp->put[1]) && get_real_data($tmp->put[1])=='rand') $orders[$i]->order='RAND';
			$tmp=get_real_data($tmp->put[0]);
		} else {
			if(isset($tmp->mutator) && $tmp->mutator==MUTATOR_NOT){
				$orders[$i]->order='DESC';
				$tmp->mutator=0;
				$tmp=parse_var($tmp);
			} else  if(isset($tmp->mutator) && $tmp->mutator==MUTATOR_SPEC){
				$orders[$i]->order='RAND';
				$tmp->mutator=0;
				$tmp=parse_var($tmp);
			} else $tmp=parse_var($tmp);
		}
						
		if($tmp=='id'){
			$orders[$i]->type='id';
		} else if($tmp=='major'){
			$tmp=getrowval("SELECT major_col FROM main_table WHERE table_id=$table",'major_col');
			if(!$tmp) continue;
			$tmp=getrowval("SELECT col_sname FROM main_col WHERE col_id=$tmp",'col_sname');
			if(!$tmp) continue;
			
			$orders[$i]->col=$tmp;
			$orders[$i]->type='col';
			if(!empty($orders[$i]->col)){
				$cols_preload[$orders[$i]->col]=$orders[$i]->col;
			}
		} else if($tmp=='pos'){
			$orders[$i]->type='pos';
		} else if($tmp=='owner' || $tmp=='own'){
			$orders[$i]->type='owner';
		} else if($tmp=='modified_date'){
			$orders[$i]->type='modified_date';
		} else if($tmp=='creation_date'){
			$orders[$i]->type='creation_date';
		} else if($tmp=='user' || $tmp=='usr'){
			$orders[$i]->type='user';
		} else {
			$orders[$i]->col=$tmp;
			$orders[$i]->type='col';
			if(!empty($orders[$i]->col)){
				$cols_preload[$orders[$i]->col]=$orders[$i]->col;
			}
		}
		$i++;
	}
	
	//cols load
	if(!empty($cols_preload)){
		$cols=getall6($db,"SELECT col_id,col_table,col_sname FROM main_col WHERE col_table=$table AND col_sname IN ('".implode("','",$cols_preload)."')","col_sname","col_id");
		if(!empty($orders)) foreach($orders AS $var2=>$value) if(!empty($orders[$var2]->col) && !empty($cols[$orders[$var2]->col])) $orders[$var2]->col=$cols[$orders[$var2]->col];
		//if(!empty($filters)) foreach($filters AS $var2=>$value) if(!empty($filters[$var2]->col) && !empty($cols[$filters[$var2]->col])) $filters[$var2]->col=$cols[$filters[$var2]->col];
	} else $cols=Array();

	//SQL construct
	$SQL_enable='';
	if($activity=='act') $SQL_enable=' AND ro.ro_enable=1';
	if($activity=='no') $SQL_enable=' AND ro.ro_enable=0';
	//SQL filter
	$SQL_leftjoin='';
	$SQL_where='';
	$f_id=1;
	apply_filters($filters,$SQL_where,$SQL_leftjoin,$f_id,$cols);
	if(!empty($SQL_where)) $SQL_where=' AND ('.$SQL_where.')';

	//SQL order
	$SQL_sort='';
	$o_id=1;
	if(!empty($orders)) foreach($orders AS $order){
		if($order->type=='col' && !empty($order->col) && is_numeric($order->col)){
			$SQL_leftjoin.=' LEFT JOIN row_value AS rvb'.$o_id.' ON rvb'.$o_id.'.value_row=ro.row_id AND rvb'.$o_id.'.value_col='.$order->col;
			if(!empty($SQL_sort)) $SQL_sort.=' ,'; else $SQL_sort.=' ORDER BY ';				
			if($order->order=='ASC'){
				//$SQL_sort.='rvb'.$o_id.'.value_value';
				//$SQL_sort.=' DESC';
				$SQL_sort.='rvb'.$o_id.'.value_value=0, -rvb'.$o_id.'.value_value DESC, rvb'.$o_id.'.value_value';
			} else if ($order->order=='DESC') {
				$SQL_sort.='rvb'.$o_id.'.value_value!=0, -rvb'.$o_id.'.value_value, rvb'.$o_id.'.value_value DESC';
			} else if($order->order=='RAND'){
				//$SQL_sort.=' rvb'.$o_id.'.value_value RAND()';
				$SQL_sort=' ORDER BY RAND()';
			}
			$o_id++;
		} else if($order->type=='id'){
			if(!empty($SQL_sort)) $SQL_sort.=' ,'; else $SQL_sort.=' ORDER BY ';
			$SQL_sort.=' ro.ro_id';
			if($order->order=='DESC') $SQL_sort.=' DESC';
		} else if($order->type=='pos'){
			if(!empty($SQL_sort)) $SQL_sort.=' ,'; else $SQL_sort.=' ORDER BY ';
			$SQL_sort.=' ro.ro_pos';
			if($order->order=='DESC') $SQL_sort.=' DESC';
		} else if($order->type=='owner'){
			if(!empty($SQL_sort)) $SQL_sort.=' ,'; else $SQL_sort.=' ORDER BY ';
			$SQL_sort.=' ro.owner_id';
			if($order->order=='DESC') $SQL_sort.=' DESC';
		} else if($order->type=='user'){
			if(!empty($SQL_sort)) $SQL_sort.=' ,'; else $SQL_sort.=' ORDER BY ';
			$SQL_sort.=' ro.ro_user';
			if($order->order=='DESC') $SQL_sort.=' DESC';
		} else if($order->type=='creation_date'){
			$include_row=true;
			if(!empty($SQL_sort)) $SQL_sort.=' ,'; else $SQL_sort.=' ORDER BY ';
			$SQL_sort.=' row.creation_date';
			if($order->order=='DESC') $SQL_sort.=' DESC';			
		} else if($order->type=='modified_date'){
			$include_row=true;
			if(!empty($SQL_sort)) $SQL_sort.=' ,'; else $SQL_sort.=' ORDER BY ';
			$SQL_sort.=' row.modified_date';
			if($order->order=='DESC') $SQL_sort.=' DESC';			
		}
	}
	//SQL limit
	$SQL_limit='';	
	if(!empty($start) && !empty($limit)){
		if($start<0) $start=0;
		if($limit<0) $limit=0;
		$SQL_limit=' LIMIT '.$start.','.$limit;
	}
	else if(!empty($limit)){
		if($limit<0) $limit=0;
		$SQL_limit=' LIMIT '.$limit;
	}
	//finaly
	if($only_count){
		//$cnt=' count(*) AS cnt';
		$cnt=' count(DISTINCT ro.row_id) AS cnt';
		$SQL_group='';
		//$SQL_group=" GROUP BY ro.row_id";
	} else {
		$cnt='*';
		$SQL_group=" GROUP BY ro.row_id";
	}
	if(!empty($SQL_zone)) $SQL_zone.=' AND';
	if($return_owners){
		$master_cnt=$cnt;
		$cnt='ro.owner_id';
		$SQL_group=" GROUP BY ro.owner_id";
	}
	$SQL_include_row='';
	if(!empty($include_row)){
		$SQL_include_row=' LEFT JOIN main_row AS row ON row.row_id=ro.row_id';
	}
	$SQL="SELECT ".$cnt." FROM row_owner AS ro".$SQL_leftjoin.$SQL_include_row." WHERE ".$SQL_zone." ro.row_table=".$table.$SQL_enable.$SQL_where.$SQL_group.$SQL_sort.$SQL_limit;
	if($return_owners){
		$master_cnt=str_replace('ro.row_id','row_id',$master_cnt);
		$SQL="SELECT $master_cnt FROM row_owner WHERE row_id IN ($SQL)";
	}
	if($only_count){
		$count=getrowval($SQL,"cnt");
		$include_row=$include_row_backup;
		return $count;
	} else {
		$rs=getall($db,$SQL,2);
		//echo $SQL;
		//echo '<br>'.count($rs).'<br>';
		if(!empty($rs)) foreach($rs AS $row)if(!empty($row["value_row"])) $utpl["row"][$row["value_row"]]=1;
		$rows=rows_to_rlink2($rs);
		if($check_auth){
			$x=get_cmd_link('rows_auth',CMD_ARRAY);
			$rows=$x($rows,$op,$ops);
		}
	}
	$include_row=$include_row_backup;
	return $rows;
}

function get_row_from_rows(&$rows,$process_rows=false){
	if(isset($rows[0])) $row=$rows[0];
	else {
		reset($rows);
		$row=current($rows);
	}
	if(!is_object($row)){
		global $rlink;
		seek_rlink($row);
		$row=$rlink[$row];
		if($process_rows){
			foreach($rows AS $var2=>$value)if(!is_object($rows[$var2])){
				seek_rlink($rows[$var2]);
				$rows[$var2]=$rlink[$rows[$var2]];
			}
		}
	}
	return $row;
}

function allcol($v,$table,$ex,$owner,$rows=Array()){
	global $db,$utpl;
	$var=$v->data;
	$tex=get_tex($owner,$ex,$table);
	$utpl["rows"][$table.'.'.$tex]=1;
	$col_sname=parse_var($v->put);
	$col=getrow($db,"SELECT col_sname, col_id, col_table FROM main_col WHERE col_table=$table AND col_sname='$col_sname'",1,"main_col");
	$col_id=$col["col_id"];
	if(!empty($rows)){
		$ids=collect_rows($rows);
		if(empty($ids)) return Array(); else $ids=implode(',',$ids);
		$query='SELECT rw.value_value, rw.value_row FROM row_owner AS ro LEFT JOIN row_value AS rw ON ro.row_id=rw.value_row AND rw.value_col='.$col_id.' WHERE ro.row_id IN ('.$ids.') AND ro.ro_enable=1';
	} else {
		//$query='SELECT ro.*, rw.value_value, rw.value_row, rw.value_col FROM row_owner AS ro LEFT JOIN row_value AS rw ON ro.row_id=rw.value_row AND rw.value_col='.$col_id.' WHERE ro.ro_ex IN ('.get_exes($owner,$tex).') AND ro.owner_id='.$owner.' AND ro.ro_enable=1';
		$exes=get_exes($owner,$tex);
		if(!empty($exes)) $exes_if='ro.ro_ex IN ('.$exes.')';
		else if(!empty($tex)) $exes_if='ro.ro_ex='.$tex;
		else return Array();
		$query='SELECT rw.value_value, rw.value_row FROM row_owner AS ro LEFT JOIN row_value AS rw ON ro.row_id=rw.value_row AND rw.value_col='.$col_id.' WHERE '.$exes_if.' AND ro.owner_id='.$owner.' AND ro.ro_enable=1';
	}
	//echo $query;
	$tmp=getall($db,$query,2);
	$res=Array();
	if($var=='allcol2(' || $var=='allcol_uniq(') $over=1; else $over=0;
	if(!empty($tmp))foreach($tmp AS $t){
		$utpl["row"][$t["value_row"]]=1;
		if(!empty($col["col_force_onshow"])) $t["value_value"]=force_value($t,$col);
		if($over) $res[$t["value_value"]]=$t["value_value"];
		else $res[]=$t["value_value"];
	}
	return $res;
}

function get_count($arr){
	if(!empty($arr) && !empty($arr->rows)) $arr=$arr->rows;
	if(empty($arr) || !is_array($arr)) return 0;
	$res=count($arr);
	foreach($arr AS $ar) if(!empty($ar->sub)) $res+=get_count($ar->sub);
	return $res;
}

function get_ex($id,$table,$ex=0){//ex здесь это tex, т.е. экземпляр таблицы
	global $db,$excache;
	if(isset($excache[$table][$id])) return $excache[$table][$id];
	stf('get_ex');
	if($ex==0){
		global $rlink;
		if(!isset($rlink[$id])) seek_rlink($id);
		$ex=$rlink[$id]->tex;
		/*$zid=$id;//это было сделано из-за косяка с доступом в под-под таблицы разных модулей, не помогло
		while($ex==0 && $rlink[$zid]->owner!=0){
			$zid=$rlink[$zid]->owner;
			if(!isset($rlink[$zid])) seek_rlink($zid);
			$ex=$rlink[$zid]->tex;
		}*/
	}
	if(!empty($ex)){
		global $ex2cache,$ex2cache2;//ex2cache2 нужен т.к. ex2cache используется в другой функции и если он принимает нулевое значение то это ведёт к ошибке
		if(isset($ex2cache2[$table][$ex])){etf('get_ex'); return $ex2cache2[$table][$ex];}
		else {
			getrow($db,"SELECT * FROM ex_group WHERE ex_table=$table AND ex_ex1=$ex",1,"ex_group",__LINE__,__FILE__);
			if(empty($db->Record["ex_ex2"])) $db->Record["ex_ex2"]=0;
			$ex2cache2[$table][$ex]=$db->Record["ex_ex2"];
			if(!empty($db->Record["ex_ex2"])){
	   			$ex2cache[$table][$ex]=$db->Record["ex_ex2"];
	   			$GLOBALS["excache3"][$ex]=$db->Record["ex_ex2"];
	   		}
			if(!empty($db->Record["ex_ex2"])){
   				$excache[$table][$id]=$db->Record["ex_ex2"];
   				etf('get_ex');
   				return $db->Record["ex_ex2"];
			}
		}
	}
	etf('get_ex');
	$excache[$table][$id]=0;
	return 0;
}

function get_ex2($tex){
	global $db,$excache3;
	if(empty($tex)) return 0;
	if(isset($excache3[$tex])) return $excache3[$tex];
	stf('get_ex2');
	getrow($db,"SELECT * FROM ex_group WHERE ex_ex1=$tex",1,"ex_group",__LINE__,__FILE__);
	if(!empty($db->Record["ex_ex2"])){
		$GLOBLAS["ex2cache"][$db->Record["ex_table"]][$db->Record["ex_ex1"]]=$db->Record["ex_ex2"];
		$excache3[$tex]=$db->Record["ex_ex2"];
		etf('get_ex2');
		return $db->Record["ex_ex2"];
	}
	$excache3[$tex]=0;
	etf('get_ex2');
	return 0;
}

function get_ex_all($tex){
	global $db,$excache4;
	if(empty($tex)) return Array();
	if(isset($excache4[$tex])) return $excache4[$tex];
	$exs=getall($db,"SELECT * FROM ex_group WHERE ex_ex1=$tex",1,"ex_group",__LINE__,__FILE__);
	$excache4[$tex]=Array();
	if(!empty($exs))foreach($exs AS $ex){
		$excache4[$tex][$ex["ex_ex2"]]=$ex["ex_ex2"];
	}
	return $excache4[$tex];
}

function get_tex($owner,$ex,$table,$recycle=false){
	global $db,$excache2;	
	if(isset($excache2[$owner][$ex][$table])) return $excache2[$owner][$ex][$table];
	stf('get_tex');
	if(/*empty($ex) && */!empty($owner)){
		// это надо кешировать... и вообще экземпляр лучше передавать не отсюда
		global $rlink;
		if(!isset($rlink[$owner])) seek_rlink($owner);
		$tex=$rlink[$owner]->tex;
		//getrow($db,"SELECT * FROM main_row WHERE row_id=$owner",1,"main_row",__LINE__,__FILE__);
		//if(!empty($db->Record["row_ex"])) $tex=$db->Record["row_ex"];
	} else {
		global $ex2cache,$ex3cache;
		if(isset($ex3cache[$table][$ex])){ $tex=$ex3cache[$table][$ex];}
		else {
			if(!empty($ex2cache) && !empty($ex2cache[$table])) foreach($ex2cache[$table] AS $ex1=>$ex2) if($ex2==$ex){$tex=$ex1; break;}
			if(!isset($tex) && !empty($table)){
				getrow($db,"SELECT * FROM ex_group WHERE ex_table=$table AND ex_ex2=$ex",1,"ex_group",__LINE__,__FILE__);
				if(empty($db->Record["ex_ex1"])) $db->Record["ex_ex1"]=0;
				if(empty($db->Record["ex_ex2"])) $db->Record["ex_ex2"]=0;
				if(empty($db->Record["ex_table"])) $ex3cache[$table][$ex]='';//в этом не уверен
				if(!empty($db->Record["ex_table"])){
					$GLOBLAS["ex2cache"][$db->Record["ex_table"]][$db->Record["ex_ex1"]]=$db->Record["ex_ex2"];
					$ex3cache[$db->Record["ex_table"]][$db->Record["ex_ex2"]]=$db->Record["ex_ex1"];
				}
				if(!empty($db->Record["ex_ex1"])){
					$tex=$db->Record["ex_ex1"];
					$GLOBALS["excache3"][$tex]=$db->Record["ex_ex2"];
				} else $tex='';
			}
		}
	}
	if(empty($tex) && !$recycle){
		$module=getrowval("SELECT ex_module FROM ex_module WHERE ex_id=$ex","ex_module");		
		if(!empty($module)){
			$tables=getall($db,"SELECT table_id FROM main_table WHERE table_module=$module");
			foreach($tables AS $t){
				$stables=get_all_sub_tables(Array(),Array($t["table_id"]));
				if(isset($stables[$table])){
					$tex=get_tex(0,$ex,$t["table_id"],1);
					break;
				}
			}
		}
		//if(!empty($table2)) $tex=get_tex(0,$ex,$table2,1);
		//echo $tex;
	}
	$excache2[$owner][$ex][$table]=$tex;
	etf('get_tex');
	return $tex;
}

function rescan_st($owns,$xtbl,$seek_ena,$seek_ex,$ignore_table,$ex1,$ex,$table,$seek_auth,$rows=Array()){
	global $rlink;
	if(empty($owns)) return '';
	stf('rescan_st');
	
	$s=Array();
	$s2=Array();
	while(!empty($owns)){
		$zowns=Array();
		foreach($owns AS $own){
			$s[$own->id]=$own->id;
			$s2[$own->id]=$own->pos;
			if(!empty($own->sub)) $zowns=array_merge_recursive($zowns,$own->sub);
		}
		$owns=$zowns;
	}
	if(!empty($s)){
		//$rows=get_sub(implode(',',$s),$xtbl,$seek_ena,$seek_ex,$ignore_table,0,/*1*/0,$ex1,$ex,$table,0,0,$seek_auth,$rows);		
		
		$tmp=get_sub(implode(',',$s),$xtbl,$seek_ena,$seek_ex,$ignore_table,0,0,$ex1,$ex,$table,0,0,$seek_auth,$rows);
		$rows=Array();
		//fix position
		$tmp2=Array();
		if(!empty($tmp)) foreach($tmp AS $t){
			if($t->owner!=0 && !empty($s2[$t->owner])){
				$x=$s2[$t->owner];
			} else $x='0';
			$tmp2[$x.'.'.$t->owner.'.'.$t->pos.'.'.$t->id]=$t;
		}		
		if(!empty($tmp2)){
			natksort($tmp2);
			foreach($tmp2 AS $t) $rows[]=$t;
			unset($tmp2);
			unset($tmp);
		}
	}
	
	/*if(!empty($owns))foreach($owns AS $own){	
		$rows=get_sub($own->id,$xtbl,$seek_ena,$seek_ex,$ignore_table,0,1,$ex1,$ex,$table,0,0,$seek_auth,$rows);
		//if(!empty($rows)) foreach($rows AS $rs) $GLOBALS["utpl"]["row"][$rs->id]=1;
		//сюда чек суб надо процедуру, которая проверяла бы if !isset sub -> check_sub и т.д.
		if(!empty($own->sub)) $rows=rescan_st($own->sub,$xtbl,$seek_ena,$seek_ex,$ignore_table,$ex1,$ex,$table,$seek_auth,$rows);
	}*/
	etf('rescan_st');
	return $rows;
}

function rows_to_rlink($rows){//db rows to rlinks
	if(!empty($rows)) foreach($rows AS $row) seek_rlink($row,0,1);
}

function rows_to_rlink2($rows,$ignore_sub=1){//db rows to rlinks (return array in e5)
	global $rlink;
	$res=Array();
	if(!empty($rows)) foreach($rows AS $row){
		seek_rlink($row,0,$ignore_sub);
		if(!empty($rlink[$row["row_id"]])) $res[]=$rlink[$row["row_id"]];
	}
	return $res;
}

function db_to_rows($rows,$type=0){//принимает массив ro_owner и превращает его в rows, type=0 выдаёт строки, type=1 выдаёт родителей
	//в каком месте тут type используется???
	global $rlink;
	rows_to_rlink($rows);
	$res=Array();
	if(!empty($rows)) foreach($rows AS $var=>$row){
		if(!empty($rlink[$row["row_id"]])) $res[$var]=$rlink[$row["row_id"]];
	}
	return seek_row_type($res);
}

function nums_to_rows($nums,$activity=1){
	global $db;
	if(empty($nums)) return Array();
	$sql_add='';
	if($activity==1) $sql_add=' AND ro_enable=1';
	if($activity==2) $sql_add=' AND ro_enable=2';
	return rows_to_rlink2(getall($db,"SELECT * FROM row_owner WHERE row_id IN (".implode(',',$nums).")".$sql_add));
}

function get_all_sub_tables($res,$search){
	global $db;
	if(empty($search)) return Array();
	$tmp=getall3($db,"SELECT * FROM table_sub WHERE sub_table1 IN (".implode(',',$search).")",'sub_table2');
	$new_search=Array();
	foreach($tmp AS $t) if(!isset($res[$t])){
		$res[$t]=$t;
		$new_search[$t]=$t;
	}
	if(!empty($new_search)){
		$res=get_all_sub_tables($res,$new_search);
	}
	return $res;
}

function get_crossmodule_subtable($module_id,$except_self=0,$type=0){//type=0  == возвращает список [table_id]->[table_name], type=1 == возвращает массив урезанного db->Record,      expect_self  0 = все, 1 = все, кроме текущего модуля, 2 - только из текущего модуля
	global $db,$crossmodule_st_cache;
	if(isset($crossmodule_st_cache[$module_id][$except_self][$type])) return $crossmodule_st_cache[$module_id][$except_self][$type];
	$tables_tmp=getall3($db,"SELECT table_id, table_module FROM main_table WHERE table_module=$module_id","table_id");
	$tables_tmp=get_all_sub_tables($tables_tmp,$tables_tmp);
	$all_subtables=Array();
	$except='';
	if($except_self==1) $except=" AND table_module!=$module_id";
	if($except_self==2) $except=" AND table_module=$module_id";
	if(!empty($tables_tmp)){
		if($type==0){
			//$all_subtables=getall6($db,"SELECT table_id, table_module, table_name FROM main_table WHERE table_id IN (".implode(',',$tables_tmp).")".$except." GROUP BY table_module",'table_module','table_name');
			$all_subtables=getall6($db,"SELECT table_id, table_module, table_name FROM main_table WHERE table_id IN (".implode(',',$tables_tmp).")".$except,'table_module','table_name');
		}
		if($type==1){
			//$all_subtables=getall($db,"SELECT table_id, table_module, table_sname FROM main_table WHERE table_id IN (".implode(',',$tables_tmp).")".$except." GROUP BY table_module");
			$all_subtables=getall($db,"SELECT table_id, table_module, table_sname FROM main_table WHERE table_id IN (".implode(',',$tables_tmp).")".$except);
		}
	}
	$crossmodule_st_cache[$module_id][$except_self][$type]=$all_subtables;
	return $all_subtables;
}

function get_table_subtables_m($table_id,$except_self=0){
	global $db;
	$module_id=getrowval("SELECT table_id, table_module FROM main_table WHERE table_id=$table_id","table_module");
	$tables_tmp=get_all_sub_tables(Array($table_id=>$table_id),Array($table_id=>$table_id));
	if($except_self) unset($tables_tmp[$table_id]);
	$all_subtables=Array();
	//if(!empty($tables_tmp)) $all_subtables=getall6($db,"SELECT table_id, table_module, table_name FROM main_table WHERE table_id IN (".implode(',',$tables_tmp).") AND table_module!=$module_id GROUP BY table_module",'table_module','table_name');
	if(!empty($tables_tmp)) $all_subtables=getall6($db,"SELECT table_id, table_module, table_name FROM main_table WHERE table_id IN (".implode(',',$tables_tmp).") GROUP BY table_module",'table_module','table_name');
	return $all_subtables;
}

function get_table_subtables_m2($tables=Array()){//не используется
	global $db;
	if(empty($tables)) return Array();
	foreach($tables AS $table) {$table_id=$table; break;}
	$module_id=getrowval("SELECT table_id, table_module FROM main_table WHERE table_id=$table_id","table_module");
	$tables_tmp=get_all_sub_tables($tables,$tables);
	$all_subtables=Array();
	if(!empty($tables_tmp)) $all_subtables=getall6($db,"SELECT table_id, table_module, table_name FROM main_table WHERE table_id IN (".implode(',',$tables_tmp).") AND table_module!=$module_id GROUP BY table_module",'table_module','table_name');
	return $all_subtables;
}

// Удаляет пустые подмассивы в массиве
function del_empty_array(&$arr){
	if(is_array($arr) && !empty($arr)){
		foreach($arr AS $key=>$a){
			if(is_array($arr[$key]) && !empty($arr[$key])){
				del_empty_array($arr[$key]);
			}
			if(is_array($arr[$key]) && empty($arr[$key])){
				unset($arr[$key]);
			}
		}
	}
}

// Делает доступными для глобального окружения, переменные GET и POST, указанные в Array
function get_globals($arr){
	//global $cmds;
	if(!empty($arr)) foreach($arr AS $a){
		//if(isset($cmds[$a])) echo $a.'<br>';
		//$cmds[$a]=1;
		if(isset($_POST[$a])) $GLOBALS[$a]=$_POST[$a];
		else if(isset($_GET[$a])) $GLOBALS[$a]=$_GET[$a];
		//else if(isset($GLOBALS[$a])){
		//	$_GET[$a]=$GLOBALS[$a];
		//}
	}
}

// Выдаёт простой список options для rows указанной таблицы
function simple_echo_options(&$rows,$table_id,$ex_ex2,$selected=0,$type=0,$check_auth=0,$owner=0,$step=0,$res=Array()){
	if($type==0 && empty($res)) $res='';
	$spacer='';
	if($type==0){
		for($i=0;$i<$step+1;$i++) $spacer.='&nbsp;&nbsp; ';
	} else {
		for($i=0;$i<$step;$i++) $spacer.='- ';
	}
	if(!empty($rows[$owner])) foreach($rows[$owner] AS $r){
		if($type==0) $id=$ex_ex2.':'.$table_id.':'.$r['id'];
		if($type==1) $id=$r['id'];
		if($type==2) $id='uin:'.$r['row_uin'].':\''.$r['value'].'\'';
		if($id==$selected) $selected=' selected'; else $selected='';	
		if(!$check_auth || check_row($id,$table_id,$ex_ex2,'view')){
			if($type==0) $res.='<option value="'.$id.'"'.$selected.'>'.$spacer.$r["value"].'</option>';
			else $res[]=Array($id,$spacer.$r['value']);
			if(!empty($rows[$r['id']])){
				$res=simple_echo_options($rows,$table_id,$ex_ex2,$selected,$type,$check_auth,$r['id'],($step+1),$res);
			}
		}
	}
	return $res;
}

function get_simple_options($table,$ex_ex2,$selected=0,$type=0,$check_auth=0){//type - 0 - выдаёт options, 1 - выдаёт массив (для ajax)
	global $db;
	getrow($db,"SELECT * FROM ex_group WHERE ex_table=".$table." AND ex_ex2=$ex_ex2",1,"ex_group");
	$ex_ex1=$db->Record["ex_ex1"];
	$mcol=seek_major($table);
	if(!empty($mcol) && !empty($ex_ex1)){
		$mcol=$mcol["col_id"];
		$rows=getall5($db,"SELECT t1.value_value AS value, t2.row_id AS id, t3.row_uin, t2.owner_id AS owner FROM row_owner AS t2 LEFT JOIN row_value AS t1 ON t1.value_row=t2.row_id AND t1.value_col=$mcol LEFT JOIN main_row AS t3 ON t3.row_id=t2.row_id WHERE t2.row_table=$table AND t2.ro_ex=$ex_ex1 AND t2.ro_sub=0 ORDER BY t2.ro_pos",'owner');
		$res=simple_echo_options($rows,$table,$ex_ex2,$selected,$type,$check_auth);
	}
	return $res;
}

//Выдаёт объект, содержащий необходимые данные из разного типа таблицы		
function get_normal_table($table){
	$x->id=0;
	$x->owner=0;
	$x->ex=0;
	$x->tex=0;
	if(isset($table->id)){
		if(isset($table->own)) $table->owner=$table->own;
		if(!isset($table->owner)) $table->owner=0;
		if(strstr($table->id,':')){
			$tid=explode(':',$table->id);
			$x->id=$tid[0];
			$x->ex=$tid[1];
		} else {
			$x->id=$table->id;
		}
		if(isset($table->ex)){
			$x->ex=$table->ex;
			$x->tex=get_tex($table->owner,$x->ex,$x->id);
		}
		if(isset($table->tex)){
			$x->tex=$table->tex;
			$x->ex=get_ex2(/*$table_ex*/$x->tex);
		} else $x->tex=0;
		$x->owner=$table->owner;
	} else if(!is_object($table) && strstr($table,':')) {
		$table=explode(':',$table);
		$x->id=$table[0];
		$x->ex=$table[1];
		if(isset($table[4])) $x->tex=$table[4]; else $x->tex=0;
		$x->owner=$table[2];
	}
	return $x;
}

function array_shiftA(&$a){
	$r=current($a);
	if(next($a)===false) $a=Array();
	return $r;
}

/*function array_unshiftA(&$a,$data){
	$tmp=Array();
	while(current($a)) $tmp[]=array_pop($a);
	$a[]=$data;
	for($i=count($tmp);$i>0;$i--) $a[]=$tmp[$i];
	end($a);
	for($i=count($tmp);$i>0;$i--) prev($a);
	return $a;
}*/

function array_unshiftA(&$a,$data){
	$tmp=Array();
	$pos=0;
	while(next($a)) $pos++;
	for($i=0;$i<=$pos;$i++) $tmp[]=array_pop($a);
	$a[]=$data;
	if(!empty($pos)) for($i=count($tmp)-1;$i>=0;$i--) $a[]=$tmp[$i];
	end($a);
	for($i=count($tmp);$i>0;$i--) prev($a);
	return $a;
}

function array_getA(&$a,$index=0){
	if($index==0) return current($a);
	else return $a[key($a)+$index];
}

function check_arrayA(&$a,$index=0){
	return isset($a[key($a)+$index]);
}

function check_arrayB(&$a,$index=0){
	if(isset($a[key($a)+$index])) return !empty($a[key($a)+$index]->data); else return false;
}

function seek_key(&$a,$k,$del_empty=0){
	reset($a);
	while(key($a)!=$k){
		if(!(next($a)===false)){
			if($del_empty) $a=Array();
			break;
		}
	}
}

function get_table_chields_count($row,$table){
	global $db;
	$csubs=getall5($db,"SELECT owner_id, row_table, ro_enable FROM row_owner WHERE owner_id=$row AND row_table=$table","ro_enable");
	if(!empty($csubs)){
		$e=0; $d=0;
		if(!empty($csubs[1])) $e=count($csubs[1]);
		if(!empty($csubs[0])) $d=count($csubs[0]);
		$add='&nbsp;(';
		if(!empty($e)) $add.='<span style="color:#00AA00;">'.$e.'</span>';
		if(!empty($e) && !empty($d)) $add.='/';
		if(!empty($d)) $add.='<span style="color:#AA0000;">'.$d.'</span>';
		$add.=')';
	} else $add='';
	return $add;
}

function get_all_table_ex($table,$module,$module_ex){
	global $db;
	$tables=getall3($db,"SELECT table_id FROM main_table WHERE table_module=$module",'table_id');
	if(!empty($tables[$table])) $r[$table]=$table;
	if(!empty($tables)) foreach($tables AS $t) $sequence[$t][0]=$t;
	$protect=Array();
	while(!empty($tables)){
		$tmp=getall8($db,"SELECT sub_table1, sub_table2 FROM table_sub WHERE sub_table1 IN (".implode(',',$tables).")",'sub_table2','sub_table1');
		if(!empty($tmp[$table])) foreach($tmp[$table] AS $t){
			if(!empty($sequence[$t])) foreach($sequence[$t] AS $st) $r[$st]=$st;
		}
		$tables=Array();
		$old_sequence=$sequence;
		$sequence=Array();
		if(!empty($tmp)) foreach($tmp AS $sub=>$arr){
			foreach($arr AS $up){
				if(!empty($old_sequence[$up])) foreach($old_sequence[$up] AS $os){
					$sequence[$sub][]=$os;
					if(empty($protect[$os.'.'.$sub])) $tables[$sub]=$sub;
					$protect[$os.'.'.$sub]=1;
				}
			}
			//$tables[$sub]=$sub;
		}
	}
	if(!empty($r)) return getall3($db,"SELECT ex_ex1 FROM ex_group WHERE ex_module=$module AND ex_table IN (".implode(',',$r).") AND ex_ex2=$module_ex",'ex_ex1');
	else return Array();
}

function have_owner($row,$owner){
	global $rlink;
	seek_rlink($row);
	if(empty($rlink[$row]->owners) && !empty($rlink[$row]->owner)) $rlink[$row]->owners[$rlink[$row]->rid]=$rlink[$row]->owner;
	if(!empty($rlink[$row]->owners)) foreach($rlink[$row]->owners AS $o){
		if($o==$owner || have_owner($o,$owner)) return true;
	}
	return false;
}

function load_cols_by_table($table_id){
	global $db,$col_table_cache;
	if(!isset($col_table_cache[$table_id])) $col_table_cache[$table_id]=getall($db,'SELECT * FROM main_col WHERE col_table='.$table_id." ORDER BY col_pos, col_bold",1,'main_col',__LINE__,__FILE__);
	return $col_table_cache[$table_id];
}

function get_loadavg(){
	if(!empty($GLOBALS['load_average'])) return $GLOBALS['load_average'];
	$GLOBALS['load_average']='';
	if(function_exists("sys_getloadavg")){
		$load=sys_getloadavg();
		if(!empty($load[1]) && strlen($load[1])>4) $load[1]=substr($load[1],0,4);
		if(!empty($load[1])) $GLOBALS['load_average']=$load[1];
		
	}
	return $GLOBALS['load_average'];
}

function normalize_cur_ex($cur_ex){
	if(strstr($cur_ex,'?')){
		$tmp=explode('?',$cur_ex);
		return $cur_ex[0];
	} else return $cur_ex;
}

function on_col_edit($row,$col,$on_edit,$module_id=0,$ex2=0,$table_id=0){
	$b=backup_globals();
	global $f_type;
	$oldft=$f_type;
	$f_type='onedit';
	$GLOBALS["cur_row"]=$row;
	$GLOBALS["current"]=$row;
	$GLOBALS["cur_col"]->id=$col;
	$GLOBALS["cur_col"]->row=$row;
	$GLOBALS["cur_ex"]=$ex2;
	$GLOBALS["cur_module"]=$module_id;
	$GLOBALS["cur_table"]=$table_id;
	$GLOBALS["url_row"][$GLOBALS["cur_table"]]=$row;
	$res=shell_tpl($on_edit,0);
	return_globals($b);
	$f_type=$oldft;
	return $res;
}
			

?>