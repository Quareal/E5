<?php

$ctemp=ob_get_contents();
if($ctemp){
	ob_end_clean();	
}
ob_start();
error_reporting(REPORTING_LEVEL);
include_once(DOCUMENT_ROOT.'/core/update/functions.inc');

if(!empty($_GET["type"]) && $_GET["type"]=='cat' && !empty($_GET["id"])){	
	//Получение папки, связанной с частью (по ID части)
	getrow($db,"SELECT * FROM part_cat WHERE cat_id=".$_GET["id"]);
	foreach($db->Record AS $var=>$value) $$var=$value;
	if(empty($cat_uin)){
		$cat_uin=uuin();
		$db->query("UPDATE part_cat SET cat_uin='$cat_uin' WHERE cat_id=$cat_id",3,'part_cat');
	}
	echo $cat_name.'|||'.$cat_type.'|||'.$cat_pre.'|||'.$cat_after.'|||'.$cat_uin.'???';
	$prt=getall($db,"SELECT part_cat, part_id FROM main_part WHERE part_cat=".$_GET["id"]);
	$first=true;
	if(!empty($prt)) foreach($prt AS $pr){
		if(!$first) echo '|||';$first=false;
		echo $pr["part_id"];
	}
}else if(!empty($_GET["type"]) && $_GET["type"]=='part' && !empty($_GET["id"])){
	//Получение части по SNAME+PROC
	$id=$_GET['id'];
	if(strstr($id,'::')){
		$id=explode('::',$id);
		$_GET["id"]=getrowval("SELECT part_id FROM main_part WHERE part_sname='".$id[1]."' AND part_type=2 AND part_proc=".$id[0],"part_id");
	}	
	echo export_part($_GET["id"]);
}else if(!empty($_GET["type"]) && $_GET["type"]=='mod_check' && !empty($_GET["id"])){	
	// Не используется
	// Проверка наличия необходимых компонент и модулей для загрузки модуля - не используется ввиду наличия этой проверки при отгрузки модулей
	$id=$_GET["id"];
	global $need_module,$is_checked;
	$is_checked=Array();
        $need_module=Array();
	check_module($id);
	//echo implode(',',array_flip($need_module));
	$nm='';
	foreach($need_module AS $var=>$value){if(!empty($nm)) $nm.=','; $nm.=$var;}
	echo $nm;
}else if(!empty($_GET["type"]) && $_GET["type"]=='modlinks' && !empty($_GET["uin"])){
	// Проверка связи модулей с другими модулями
	global $need_module,$is_checked;
	$is_checked=Array();
        $need_module=Array();
	$id=getrowval("SELECT * FROM main_module WHERE module_uin='".$_GET["uin"]."'","module_id");
	check_module($id);
	$nm='';
	foreach($need_module AS $var=>$value){
		$nuin=getrowval("SELECT * FROM main_module WHERE module_id=$var","module_uin");
		if($nuin!=$_GET["uin"]){
			if(!empty($nm)) $nm.=',';
			$nm.=$nuin;
		}
	}
	echo $nm;
}else if(!empty($_GET["type"]) && $_GET["type"]=='moddates' && !empty($_GET["uins"])){
	// Вывод информации о модулях для обновления (даты)
	if(strstr($_GET["uins"],',')) $uins=explode(',',$_GET["uins"]); else $uins=Array($_GET["uins"]);
	if(empty($uins)) return '';
	$modules=getall($db,"SELECT * FROM main_module WHERE module_uin IN ('".implode("','",$uins)."')");
	$s='';
	if(!empty($modules)) foreach($modules AS $mdl){
		if(!empty($s)) $s.=',';
		$s.=$mdl["module_uin"].';'.$mdl["module_date2"];
	}
	echo $s;
}else if(!empty($_GET["type"]) && $_GET["type"]=='mods_part_links' && !empty($_GET["uins"])){
	// Вывод связанных с модулями частей
	$uins=explode(',',$_GET['uins']);
	$sql_uin='';
	foreach($uins AS $uin){
		if(!empty($sql_uin)) $sql_uin.=',';
		$sql_uin.="'".$uin."'";
	}	
	global $db;
	$module_parts=getall($db,"SELECT module_parts FROM main_module WHERE module_uin IN ($sql_uin)");
	$res=Array();
	foreach($module_parts AS $mp) $res=get_all_part_links($mp['module_parts'],$res,true);
	echo serialize($res);
}else if(!empty($_GET["type"]) && $_GET["type"]=='mod_part_links' && !empty($_GET["uin"])){
	// Вывод связанных с частью частей
	$module_parts=getrowval("SELECT module_parts FROM main_module WHERE module_uin='".$_GET["uin"]."'",'module_parts');
	echo get_all_part_links($db->Record["module_parts"]);
}else if(!empty($_GET["type"]) && $_GET["type"]=='mod' && (!empty($_GET["id"]) || !empty($_GET["uin"]))){
	//Получение целого модуля
	global $replace_uin;
	$replace_uin=Array();
	prep_tables(); 
	if(!empty($_GET["id"])){
		$id=$_GET["id"];
		if(!is_numeric($id)){
			$id=getrowval("SELECT module_id FROM main_module WHERE module_uin='".$id.'"','module_id');
		}
		if($id<0){
			$id=-$id;
			getrow($db,"SELECT * FROM main_module WHERE module_id=".$id);
			$replace_uin[$db->Record["module_uin"]]=$db->Record["module_uin"].'_update';
		} else {
			getrow($db,"SELECT * FROM main_module WHERE module_id=".$id);
		}
	}
	if(!empty($_GET["uin"])){
		$uin=$_GET["uin"];
		if($uin[0]=='-'){
			$uin=substr($uin,1);
			getrow($db,"SELECT * FROM main_module WHERE module_uin='".$uin."'");
			$replace_uin[$db->Record["module_uin"]]=$db->Record["module_uin"].'_update';
		} else {
			getrow($db,"SELECT * FROM main_module WHERE module_uin='".$uin."'");
		}
		if(!empty($db->Record["module_id"])) $id=$db->Record["module_id"];
	}
	if(empty($db->Record)) return false;
	$odb=$db->Record;
	if(empty($db->Record["module_uin"])){
		$xuid=uuid();
		$odb["module_uin"]=$xuid;
		$db->query("UPDATE main_module SET module_uin='".$xuid."' WHERE module_id=".$odb["module_id"],3,"main_module");
		$db->Record=$odb;
	}
	$mmaj=$db->Record["module_major"];
	$s='<|-|>';
	echo $db->Record["module_name"].$s.$db->Record["module_sname"].$s.module_uin($db->Record["module_uin"]).$s.$db->Record["module_parts"].$s.file_to_data($db->Record['module_icon']).$s;
	//Таблицы
	$tables=getall($db,"SELECT * FROM main_table WHERE table_module=$id");
	$first=true;
	if(!empty($tables)) foreach($tables AS $table){
		if(empty($table["table_uin"])){
			$xuid=uuid();
			$table["table_uin"]=$xuid;
			$db->query("UPDATE main_table SET table_uin='".$xuid."' WHERE table_id=".$table["table_id"],3,"main_table");
		}
		if(!$first) echo '?|?|?';
		$first=false;
		echo export_table($table["table_id"]);
	}
	//Связки таблиц
	echo $s;
	$st=Array();$st2=Array();
	if(!empty($tables)) foreach($tables AS $table){
		$st[]=$table["table_id"];
		$st2[$table["table_id"]]=$table;
	}
	if(!empty($st)) $ts=getall($db,"SELECT * FROM table_sub WHERE sub_table1 IN (".implode(',',$st).")");
	$first=true;
	if(!empty($ts)) foreach($ts AS $t){
		if(!$first) echo '?|?|?';
		$first=false;
		
		getrow($db,"SELECT * FROM main_table WHERE table_id=".$t["sub_table1"]);
		$table_sname=$db->Record["table_uin"];
		$table_sname2=$db->Record["table_sname"];
		getrow($db,"SELECT * FROM main_module WHERE module_id=".$db->Record["table_module"]);
		$module_sname=module_uin($db->Record["module_uin"]);
		$module_sname2=$db->Record["module_sname"];
		$tmp=$module_sname.'::'.$table_sname.'::'.$module_sname2.'::'.$table_sname2;
		//-----
		getrow($db,"SELECT * FROM main_table WHERE table_id=".$t["sub_table2"]);
		$table_sname=$db->Record["table_uin"];
		$table_sname2=$db->Record["table_sname"];
		getrow($db,"SELECT * FROM main_module WHERE module_id=".$db->Record["table_module"]);
		$module_sname=module_uin($db->Record["module_uin"]);
		$module_sname2=$db->Record["module_sname"];
		$tmp.='**'.$module_sname.'::'.$table_sname.'::'.$module_sname2.'::'.$table_sname2;
		
		echo $tmp;
	}	
	//Переменные экземпляра
	echo $s;
	echo export_cols($id,0);
	//Части
	echo $s;	
	$parts=getall($db,"SELECT * FROM main_part WHERE part_module=$id AND part_type!=2");
	$first=true;
	if(!empty($parts)) foreach($parts AS $part){
		if(!$first) echo '?|?|?';
		$first=false;
		export_part($part["part_id"]);
	}
	//Группы доступа
	$groups=getall($db,"SELECT * FROM main_auth WHERE group_module=$id AND auth_type=1");
	$first=true;
	echo $s;
	global $colcache;
	$groups[-1]["group_name"]=0;
	$groups[-1]["group_sname"]=0;
	$groups[-1]["group_uin"]=0;
	$groups[-1]["auth_id"]=0;
	$groups[-1]["auth_date"]=0;
	$azone=getall3($db,"SELECT * FROM main_zone WHERE zone_module=-1 AND zone_active=1","zone_id");
	
	if(!empty($groups)) foreach($groups AS $group){
		if(!$first) echo '?|?|?';
		$gzone=getall3($db,"SELECT * FROM auth_perm WHERE perm_auth=".$group["auth_id"]." AND perm_target=1 AND perm_type=0 AND perm_view=1","perm_object");
		if(!empty($gzone)) foreach($gzone AS $gz) if(isset($azone[$gz])){
			$use_admin='1';
		} else $use_admin='0';
		$first=false;
		echo $group["auth_date"].'**'.$group["group_name"].'**'.$group["group_sname"].'**'.$group["group_uin"].'**'.$use_admin;
		echo '**';
		echo get_root3($group["auth_id"],1,1,$id);//модуль
		echo '**';
		echo get_root3($group["auth_id"],3,3,$id);//все таблицы модуля
		echo '**';
		echo get_root3($group["auth_id"],4,5,$id);//все строки модуля
		echo '**';
		echo get_root3($group["auth_id"],2,3,$id);//все экземпляры модуля
		echo '**';
		echo get_root3($group["auth_id"],6,3,$id);//все пользователи модуля
		echo '**';
		echo get_root3($group["auth_id"],8,3,$id);//все группы модуля
		
		
		//echo '**';
		//echo get_root3($group["auth_id"],5,1,$id);//все папки
		// тут должен быть перечень папок
		//echo '**';		доступы к папкам пока не будут импортироваться
		//getrow($db,"SELECT * FROM auth_perm WHERE perm_auth=".$group["auth_id"]." AND perm_type=5 AND perm_target=6");
		//if(!empty($db->Record)) echo $db->Record["perm_folder"];
		
		global $stables;
		if(!empty($tables)) foreach($tables AS $table){
			echo '**';
			echo $table["table_uin"].'||'.get_root3($group["auth_id"],3,1,$table["table_id"]);//таблица
			echo '**';
			echo $table["table_uin"].'||'.get_root3($group["auth_id"],4,3,$table["table_id"]);//её строки
			if(!empty($colcache[$id][$table["table_id"]]))foreach($colcache[$id][$table["table_id"]] AS $col){
				echo '**';
				echo $col["col_uin"].'||'.get_root3($group["auth_id"],7,1,$col["col_id"]);//столбцы
			}
			echo '**';
			if(!empty($stables[$table["table_id"]])) echo echo_subtables_serv($group["auth_id"],$table["table_id"],Array($table["table_id"]));
		}
	}
	echo $s;
	$first=true;
	if(!empty($groups)) foreach($groups AS $group) foreach($groups AS $grp)if($grp["auth_id"]!=0){
		if(!$first) echo '**';
		$first=false;
		echo $grp["group_uin"].'||'.$group["group_uin"].'||'.get_root3($group["auth_id"],6,1,$grp["auth_id"]);//пользователи
		echo '**';
		echo $grp["group_uin"].'||'.$group["group_uin"].'||'.get_root3($group["auth_id"],8,1,$grp["auth_id"]);//группы
	}
	echo $s.$mmaj;
}else if(!empty($_GET["type"]) && $_GET["type"]=='show_exs'){
	// Отгрузка экземпляров модуля
	global $db;
	$module_id=getrowval("SELECT module_id FROM main_module WHERE module_uin='".$_GET['module_id']."'","module_id");
	$exs=getall($db,"SELECT ex_id, ex_name FROM ex_module WHERE ex_module=".$module_id." AND ex_public=1");
	$res=Array();
	foreach($exs AS $ex){
		$ex_id=$ex['ex_id'];
		$ex_name=$ex['ex_name'];
		$res[$ex_id]->value=$ex_name;
		$res[$ex_id]->id=$ex_id;
		$use_tex=Array();
		$parts=Array();
		$parts=get_row_parts_links($ex_id,$parts,0,true);
		$texs=getall($db,"SELECT * FROM ex_group WHERE ex_module=$module_id AND ex_ex2=$ex_id");
		foreach($texs AS $t){
			$tex=$t['ex_ex1'];
			$table_id=$t['ex_table'];
			if(!empty($use_tex[$tex])) continue;
			$use_tex[$tex]=1;
			$rows=get_sub(0,$table_id,1,1,0,0,1,$tex,$ex_id,$table_id,0,$table_id,0);
			if(!empty($rows)){
				$parts=collect_part_links_from_rows($rows,$parts);
			}
		}
		$res[$ex_id]->components=$parts;
	}
	echo serialize($res);
}else if(!empty($_GET["type"]) && $_GET["type"]=='get_public_ex' && !empty($_GET['uin'])){
	// Отгрузка главного экземпляра модуля (доступного для обновления)
	$ex=getrowval("SELECT module_public_ex FROM main_module WHERE module_uin='".$_GET['uin']."'",'module_public_ex');
	if(!empty($ex)) $ex_uin=getrowval("SELECT ex_uin FROM ex_module WHERE ex_id=$ex","ex_uin");
	if(!empty($ex_uin)) echo $ex_uin;
}else if(!empty($_GET["type"]) && $_GET["type"]=='get_ex' && !empty($_GET['ex'])){
	// Отгрузка экземпляра
	global $rlink, $db, $exp_rows2;	
	include_once(DOCUMENT_ROOT.'/core/update/objects.inc');
	$ex_id=$_GET['ex'];
	if(is_numeric($ex_id)){
		$ex=getrow($db,"SELECT * FROM ex_module WHERE ex_id=$ex_id AND ex_public=1");
	} else {
		$ex=getrow($db,"SELECT * FROM ex_module WHERE ex_uin='$ex_id'");
		$ex_id=$ex['ex_id'];
	}
	if(empty($ex['ex_uin'])){
		$ex['ex_uin']=uuid();
		$db->query("UPDATE ex_module SET ex_uin='".$ex['ex_uin']."' WHERE ex_id=".$ex_id);
	}
	$result=Array();
	$result['ex_uin']=$ex['ex_uin'];
	$result['ex_name']=$ex['ex_name'];
	$result['ex_sname']=$ex['ex_sname'];
	start_export(1);
	
	$texs=getall($db,"SELECT * FROM ex_group WHERE ex_ex2=$ex_id");
	foreach($texs AS $t){
		$table_id=$t['ex_table'];
		$table_uin=getrowval("SELECT table_uin FROM main_table WHERE table_id=$table_id","table_uin");
		$tex=$t['ex_ex1'];
		$tex_db=getrow($db,"SELECT * FROM ex_table WHERE ex_id=$tex");
		$result['rows'][$table_uin]['name']=$tex_db['ex_name'];
		if(empty($_GET['ignore_all_rows'])) $result['rows'][$table_uin]['rows']=rows_to_text(get_sub(0,$table_id,1,1,0,0,1,$tex,$ex_id,$table_id,0,$table_id,0));
		else $result['rows'][$table_uin]['rows']=Array();
	}
	
	$result['ex_param']=params_to_text($ex_id,'ex',0,$ex['ex_module']);
	
	//linked rows (from uin:uin_string)
	$links_tmp=get_row_uin_links($ex_id,true);
	$mod_links=Array();
	if(!empty($links_tmp)){
		$links=Array();
		foreach($links_tmp AS $row) if(empty($exp_rows2[$row])){
			if(strstr($row,':')){
				$tmp=explode(':',$row);
				$mod_links[]=Array('module'=>$tmp[0],'ex'=>$tmp[1]);
			} else $links[$row]=$row;
		}
		if(!empty($links)) $result['links']=rows_to_text($links);
		$result['mod_links']=$mod_links;
	}
	
	if(!empty($GLOBALS['collected_parts'])) $result['parts']=$GLOBALS['collected_parts'];
	else $result['parts']=Array();
	end_export();
	echo serialize($result);
}else if(!empty($_GET["type"]) && $_GET["type"]=='show_rows_exs'){
	// Отображения объектов экземпляра для обновления
	global $db;
	$module_id=getrowval("SELECT module_id FROM main_module WHERE module_uin='".$_GET['module_id']."'","module_id");
	$table_id=getrowval("SELECT table_id FROM main_table WHERE table_uin='".$_GET['table_id']."'","table_id");
	if(!getrowval("SELECT table_id FROM main_table WHERE table_id=$table_id AND table_public=1","table_id")){
		echo serialize(Array());
	} else {
		$exs=getall($db,"SELECT ex_id, ex_name FROM ex_module WHERE ex_module=".$module_id." AND ex_public=1");
		$res=Array();
		$use_tex=Array();
		foreach($exs AS $ex){
			$ex_id=$ex['ex_id'];
			$ex_name=$ex['ex_name'];
			$tex=getrowval("SELECT ex_ex1 FROM ex_group WHERE ex_ex2=$ex_id AND ex_table=$table_id AND ex_module=$module_id","ex_ex1");
			if(empty($tex)) continue;
			if(!empty($use_tex[$tex])) continue;
			$use_tex[$tex]=1;
			$rows=get_sub(0,$table_id,1,1,0,0,1,$tex,$ex_id,$table_id,0,$table_id,0);
			if(!empty($rows)){
				$res[$ex_id]['ex_name']=$ex_name;
				$rows=get_vars2($rows);
				get_rows_part_links($rows);
				$res[$ex_id]['rows']=$rows;//checkbox(get_rows_part_links(get_vars2($rows)),'','upload',0,Array(),0);
			}
		}
		echo serialize($res);
	}
}else if(!empty($_GET["type"]) && $_GET["type"]=='get_rows'){
	// Отгрузка объектов
	global $rlink, $db;
	include_once(DOCUMENT_ROOT.'/core/update/objects.inc');
	$rows=explode(',',$_GET['rows']);
	rows_to_rlink(getall($db,"SELECT * FROM row_owner WHERE row_id IN (".implode(',',$rows).")"));
	$tables=Array();
	$texs=Array();
	foreach($rows AS $row){
		$tables[$rlink[$row]->table]=$rlink[$row]->table;
		$texs[$rlink[$row]->tex]=$rlink[$row]->tex;
	}
	foreach($texs AS $tex){
		$ex=get_ex2($tex);
		$exs[$ex]=$ex;
		$tex_to_ex[$tex]=$ex;
	}
	$except_table=getall3($db,"SELECT table_id FROM main_table WHERE table_public!=1 AND table_id IN (".implode(',',$tables).")","table_id");
	$except_ex=getall3($db,"SELECT ex_id FROM ex_module WHERE ex_public!=1 AND ex_id IN (".implode(',',$exs).")","ex_id");
	$tmp=$rows;
	$rows=Array();
	foreach($tmp AS $row){
		if(empty($except_table[$rlink[$row]->table]) && empty($except_ex[$tex_to_ex[$rlink[$row]->tex]])) $rows[$row]=$row;
	}
	start_export(1);
	$import=rows_to_text($rows);	
	echo $import;
	echo '_!^^!_';
	if(!empty($GLOBALS['collected_parts'])) echo serialize($GLOBALS['collected_parts']);
	end_export();
}else if(!empty($_GET["type"]) && $_GET["type"]=='mod'){
	//Получение списка модулей
	$m=getall($db,"SELECT * FROM main_module WHERE NOT (module_sname LIKE '%for_uninstall%')",1,"main_module");
	$first=true;
	if(!empty($m)) foreach($m AS $mi){
		if(!$first) echo '///';
		$first=false;
		global $need_module,$is_checked;
	        $need_module=Array();
       		$is_checked=Array();
		check_module($mi["module_id"]);
		$nm='';
		foreach($need_module AS $var=>$value){if(!empty($nm)) $nm.=','; $nm.=$var;}
		//$nm=implode(',',array_flip($need_module));		
		echo $mi["module_name"].'|||'.$mi["module_sname"].'|||'.$mi["module_id"].'|||'.$nm.'|||'.get_all_part_links($mi["module_parts"]);
	}
	// Получение частей
	echo '???';
	$p=getall($db,"SELECT * FROM part_cat",1,"part_cat");
	$first=true;
	if(!empty($p)) foreach($p AS $pi){
		if(!$first) echo '///';$first=false;
		echo $pi["cat_name"].'|||'.$pi["cat_id"].'|||'.$pi["cat_type"].'|||'.$pi["cat_uin"].'|||'.get_all_part_links_for_cat($pi['cat_id']);
	}
	// Получение компонентов
	echo '???';
	$p=getall($db,"SELECT * FROM main_part WHERE part_type=2",1,"main_part");
	$first=true;
	if(!empty($p)) foreach($p AS $pi){
		if(!$first) echo '///';
		$first=false;
		echo $pi["part_proc"].'|||'.$pi["part_name"].'|||'.$pi["part_sname"].'|||'.$pi["part_cat"].'|||'.$pi["part_id"].'|||'.$pi["part_uin"].'|||'.$pi["part_date2"].'|||'.get_all_part_links($pi['part_parts']);
	}
	echo '???';
	// Получение шаблонов таблиц
	$p=getall($db,"SELECT * FROM main_table WHERE table_module=0",1,"main_table");
	$first=true;
	if(!empty($p)) foreach($p AS $pi){
		if(!$first) echo '///';
		$first=false;
		echo $pi['table_name'].'|||'.$pi['table_sname'].'|||'.$pi['table_id'].'|||'.$pi['table_uin'].'|||'.$pi['table_date2'].'|||'.get_all_part_links($pi['table_parts']);
	}
	echo '???';
	// Получение шаблонов полей
	$p=getall($db,"SELECT * FROM main_col WHERE col_module=0 AND col_table=0",1,"main_col");
	$first=true;
	if(!empty($p)) foreach($p AS $pi){
		if(!$first) echo '///';
		$first=false;
		echo $pi['col_name'].'|||'.$pi['col_sname'].'|||'.$pi['col_id'].'|||'.$pi['col_uin'].'|||'.$pi['col_date2'].'|||'.get_all_part_links($pi['col_parts']);
	}
}else if(!empty($_GET["type"]) && $_GET["type"]=='part_list'){
	$p=getall($db,"SELECT * FROM main_part WHERE part_type=2",1,"main_part");
	$first=true;
	if(!empty($p)) foreach($p AS $pi){
		if(!$first) echo '///';
		$first=false;
		echo $pi["part_proc"].'|||'.$pi["part_name"].'|||'.$pi["part_sname"].'|||'.$pi["part_cat"].'|||'.$pi["part_id"].'|||'.$pi["part_uin"].'|||'.$pi["part_date2"].'|||'.get_all_part_links($pi['table_parts']);
	}
} else if(!empty($_GET["type"]) && $_GET["type"]=='tbltpl' && !empty($_GET['id'])){
	echo export_table($_GET['id']);
} else if(!empty($_GET["type"]) && $_GET["type"]=='col' && !empty($_GET['id'])){
	echo export_cols(0,0,$_GET['id']);
} else if(!empty($_GET["type"]) && $_GET["type"]=='folder' && !empty($_GET["folder"])){
	$f=$_GET["folder"];
	if(strstr($f,',')) $f2=explode(',',$f); else $f2=Array($f);
	$fp=Array();
	foreach($f2 AS $fold){
		if($fold[0]=='/') $fold=substr($fold,1,strlen($fold)-1);
		if($fold[strlen($fold)-1]=='/') $fold=substr($fold,0,strlen($fold)-1);
		$fp[]=$fold;
		/*if($fold[strlen($fold)-1]!='/') $fold.='/';
		$f=find_file($fold,Array(),1);
		foreach($f AS $cf) echo $cf->url.'='.$cf->size.'|';*/
	}
	echo gzip_folders($fp,Array(),Array(),0,0,'',1);
} else if(empty($_GET["ttf"])){

// получение списка таблиц
$tbl=getall($db,"SHOW TABLES FROM `".$database."`");
$first3=true;
foreach($tbl AS $tb){
	if(!$first3) echo '!-';
	$first3=false;
	foreach($tb AS $var=>$value) $tname=$value;
	echo $tname.'!:';
	$cols=getall($db,"SHOW COLUMNS FROM ".$tname);
	$first2=true;
	foreach($cols AS $col){
		if(!$first2)	echo '!.';
		$first2=false;
		$first=true;
		$is_text=false;
		$is_date=false;
		foreach($col AS $var=>$value){
			if(!$first) echo '!;';
			if($var=='Type' && $value=='text') $is_text=true;
			if($var=='Type' && $value=='longtext') $is_text=true;
			if($var=='Type' && $value=='tinytext') $is_text=true;
			if($var=='Type' && $value=='date') $is_date=true;
			if($var=='Type' && $value=='time') $is_date=true;
			if($var=='Type' && $value=='timestamp') $is_date=true;
			if($var=='Type' && $value=='datetime') $is_date=true;
			$first=false;
			if(($is_text || $is_date) && $var=='Default' && $value!='') $value="'".$value."'";
			if(!$is_text && $var=='Null' && $value=='NO') $value='NOT NULL';
			if($is_text && $var=='Null' && $value=='NO') $value='NOT NULL';
			if($is_text && $var=='Null' && $value=='YES') $value='NULL';
			if(!$is_text && $value=='YES') $value='';
			echo $var.'!='.$value;
		}
	}
	echo '!:';
	$ind=getall($db,"SHOW INDEXES FROM $tname WHERE key_name!='PRIMARY'");
	$keys=Array();
	if(!empty($ind)) foreach($ind AS $i){
		if(!empty($keys[$i['Key_name']])) $keys[$i['Key_name']].=', ';
		else $keys[$i['Key_name']]='';
		$keys[$i['Key_name']].=$i['Column_name'];
		if($i['Sub_part']!='Null' && $i['Sub_part']!='NULL' && !empty($i['Sub_part'])) $keys[$i['Key_name']].='('.$i['Sub_part'].')';
	}
	$first=true;
	foreach($keys AS $key=>$cols){
		if(!$first) echo '!;';
		$first=false;
		echo $key.'!='.$cols;
	}
}

echo '!-!-!';

//получение файлов
   if ($objs = glob(FTEMP."*")) {
       foreach($objs as $obj) {
	if(is_dir($obj)){
 	  $o=explode('/',$obj);
 	  $name=$o[count($o)-1];
	  if($name!='tpl') removeDirRec($obj);
	}
       }
   }
    if ($objs = glob(FTEMP."*")) {
        foreach($objs as $obj) {
	if(!is_dir($obj)) unlink($obj);
        }
    }
    if ($objs = glob(FTEMP."*")) {
        foreach($objs as $obj) {
	if(is_dir($obj)){
		$o=explode('/',$obj);
		$name=$o[count($o)-1];
		if($name=='tpl') removeDirRec($obj);
	}
        }
    }
// Второе перечисление папок находится в /core/editor/deploy.php
$f=list_files($GLOBALS['UPDATE_PATHS'],$GLOBALS['UPDATE_FILES']);
/*$f=Array();
foreach($GLOBALS["UPDATE_PATHS"] AS $path) if(file_exists(DOCUMENT_ROOT.'/'.$path) && is_dir(DOCUMENT_ROOT.'/'.$path)){
	$x=$path;
	if($x!='') $x.='/';
	$f=find_file($x,$f);
}

foreach($GLOBALS["UPDATE_FILES"] AS $path){
	if(file_exists(DOCUMENT_ROOT.'/'.$path) && is_file(DOCUMENT_ROOT.'/'.$path)){
		$f[$path]->url=$path;
		$f[$path]->name=basename($path);
		$f[$path]->size=filesize(DOCUMENT_ROOT.'/'.$path);
	}
}*/

foreach($f AS $cf){
	if(isset($no_update[$cf->url]) && file_exists(DOCUMENT_ROOT.'/core/units/safe/'.md5($cf->url))){
			$cf->size=filesize(DOCUMENT_ROOT.'/core/units/safe/'.md5($cf->url));
	}
	echo $cf->url.'='.$cf->size.'|';
}

} else {
	$ttf=urldecode($_GET["ttf"]);
	if(isset($no_update[$ttf]) && file_exists(DOCUMENT_ROOT.'/core/units/safe/'.md5($ttf))){
		echo implode('',file(DOCUMENT_ROOT.'/core/units/safe/'.md5($ttf)));
	}else if($ttf!='core/config.inc' && !strstr($ttf,'config.inc') && file_exists(DOCUMENT_ROOT.'/'.$ttf)) echo implode('',file(DOCUMENT_ROOT.'/'.$ttf));
}

$ctemp2=ob_get_contents();
ob_end_clean();
if($ctemp){
	ob_start();
	echo $ctemp;
}
error_reporting(REPORTING_LEVEL);
if(function_exists('gzencode') && empty($_GET["nogzip"])){
	echo gzencode($ctemp2,9);
} else echo '!ngz'.$ctemp2;

?>