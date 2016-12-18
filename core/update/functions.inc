<?php

include_once('objects.inc');

// Функции экспорта (отгрузка)

function export_part($id){
	global $db, $need_module;
	getrow($db,"SELECT * FROM main_part WHERE part_id=".$id);
	//if(is_numeric($id)) getrow($db,"SELECT * FROM main_part WHERE part_id=".$id);
	//else getrow($db,"SELECT * FROM main_part WHERE part_uin='".$id."'");
	foreach($db->Record AS $var=>$value) $$var=$value;
	if(empty($part_uin)){
		$xuid=uuid();
		$part_uin=$xuid;
		$db->query("UPDATE main_part SET part_uin='".$xuid."' WHERE part_id=$id",3,"main_part");
	}
	if($part_table!=0){
		getrow($db,"SELECT * FROM main_table WHERE table_id=$part_table");
		$part_table=$db->Record["table_uin"];//модуль здесь не нужно указывать потому что часть импортируется в родной модуль
	}
	$s='|/*/|';
	$s2='/@/?/@/';
	$s3='/#/?/#/';
	if(!empty($part_owner)){
		getrow($db,"SELECT * FROM main_part WHERE part_id=".$part_owner);
		$part_owner=$db->Record["part_uin"];
	} else $part_owner='';
	if($part_type==2 && !empty($part_cat) && $part_cat!=0){
		getrow($db,"SELECT * FROM part_cat WHERE cat_id=".$part_cat);
		if(!empty($db->Record)){
			if(empty($db->Record['cat_uin'])){
				$uin=uuid();
				$db->Record['cat_uin']=$uin;
			} else $uin=0;
			$part_module=$part_proc.'^-!-?'.$db->Record["cat_name"].'^-!-?'.$db->Record["cat_uin"].'^-!-?'.$db->Record["cat_pre"].'^-!-?'.$db->Record["cat_after"];
			if(!empty($uin)){
				$db->query("UPDATE part_cat SET cat_uin='".$uin."' WHERE cat_id=$part_cat",3,"part_cat");
			}
		}
	}
	echo $part_name.$s.$part_sname.$s.$part_body.$s.$part_parse.$s.$part_type;
	echo $s.$part_module.$s.$part_proc.$s.$part_ex.$s.$part_file.$s.$part_cat;
	echo $s.$part_access.$s.$part_table.$s.$part_url.$s.$part_iowner.$s.$part_ignore;
	echo $s.$part_major.$s.$part_proc.$s./*$parser_end*/'0'.$s./*$parser_vars*/''.$s./*$parser_vars_back*/'';
	echo $s./*$parser_log*/''.$s./*$parser_err*/''.$s./*$parser_out*/''.$s./*$timer_last*/'0000-00-00'.$s.$timer_act;
	echo $s.$timer_type.$s.$timer_date.$s.$timer_time.$s.$timer_x.$s.$timer_y;
	if($part_auth!=0 && $part_auth!=-1){
		$part_auth=getrowval("SELECT group_uin FROM main_auth WHERE auth_id=".$part_auth,"group_uin");
		if(empty($part_auth)) $part_auth=-1;
	}
	echo $s.$timer_count.$s.$part_sowner.$s.$part_about.$s.$part_owner.$s.$part_folder.$s.$part_404.$s.$part_uin.$s.$part_pic.$s.$part_auth.$s.$part_unsafe.$s.$part_ifcase.$s.$part_ifdetect.$s.$part_ifrow.$s.$part_date2;		
	echo $s2;
	$first=true;
	$params=getall($db,"SELECT * FROM part_param WHERE param_part=".$id);
	if(!empty($params)) foreach($params AS $param){
		if(!$first) echo $s3; $first=false;
		foreach($param AS $var=>$value) $$var=$value;
		if(!empty($param_link)){
			$pm=getrow($db,"SELECT * FROM main_table WHERE table_id=$param_link");
			//$param_link=getrowval("SELECT * FROM main_table WHERE table_id=$param_link","table_uin");			
			$param_link=$db->Record['table_uin'];
			$need_module[$db->Record['table_module']]=1;
		}
		echo $param_id.$s.$param_name.$s.$param_sname.$s.$param_default.$s.$param_array;
		echo $s.$param_get.$s.$param_type.$s.$param_list.$s.$param_link.$s.$param_connect.$s.$param_uin.$s.$param_hide;
	}
}

function export_cols($module,$table,$col=0){
	global $db,$colcache;
	if(empty($col)) $cols=getall($db,"SELECT * FROM main_col WHERE col_module=$module AND col_table=$table");	
	else $cols=getall($db,"SELECT * FROM main_col WHERE col_id=$col");	
	$colcache[$module][$table]=$cols;
	$first=true;
	$res='';
	$s='|-+-|';
	foreach($cols AS $col){
		if(!$first) $res.='|-(-|';
		foreach($col AS $var=>$value) $$var=$value;

		if(empty($col_uin)){		
			$xuid=uuid();
			$col_uin=$xuid;
			$db->query("UPDATE main_col SET col_uin='".$xuid."' WHERE col_id=$col_id",3,"main_col");
		}
		
		if($col_link!=0 && $col_type==1){
			if($col_link>0) $bcol_link=$col_link; else $bcol_link=-$col_link;
			getrow($db,"SELECT * FROM main_table WHERE table_id=$bcol_link");
			$table_sname=$db->Record["table_uin"];
			$table_sname2=$db->Record["table_sname"];
			getrow($db,"SELECT * FROM main_module WHERE module_id=".$db->Record["table_module"]);
			$module_sname=module_uin($db->Record["module_uin"]);
			$module_sname2=module_uin($db->Record["module_sname"]);
			$col_linkB=$module_sname.'::'.$table_sname.'::'.$module_sname2.'::'.$table_sname2;
			if($col_link<0) $col_linkB.='::1';
		} else $col_linkB=0;
		if(!empty($col_part)){
			getrow($db,"SELECT * FROM main_part WHERE part_id=$col_part");
			$part_sname=$db->Record["part_uin"];
			$col_part=$part_sname;
		}
		//$res.=$col_name.$s.$col_sname.$s.$col_type.$s.$col_onshow.$s.$col_onform.$s.$col_onform2.$s.$col_oninsert.$s.$col_onedit.$s.$col_pos.$s.$col_order.$s.$col_default.$s.$col_unique.$s.$col_url.$s.$col_part.$s.$col_pold.$s.$col_required.$s.$col_inform.$s.$col_linkB.$s.$col_link2.$s.$col_link3.$s.$module_url.$s.$module_type.$s.$file_dir.$s.$file_maxsize.$s.$file_totalmax.$s.$file_prefix.$s.$file_types;
		if(!empty($col_paramlink)){
			getrow($db,"SELECT * FROM part_param WHERE param_id=$col_paramlink");
			if(!empty($db->Record)){
				$param=$db->Record["param_sname"];
				getrow($db,"SELECT * FROM main_part WHERE part_id=".$db->Record["param_part"]);
			}
			if(!empty($db->Record)){
				if($db->Record["part_type"]==2){
					$pname=$db->Record["part_uin"];
					$ptype=$db->Record["part_proc"];
					$col_paramlink='0:'.$ptype.':'.$pname.':'.$param.':'.$db->Record['part_sname'];
				} else {
					$pname=$db->Record["part_uin"];
					getrow($db,"SELECT * FROM main_module WHERE module_id=".$db->Record["part_module"]);
					$mname=module_uin($db->Record["module_uin"]);
					$col_paramlink='1:'.$mname.':'.$pname.':'.$param.':'.$db->Record['part_sname'];
				}
			}
		}
		$res.=$col_name.$s.$col_sname.$s.$col_type.$s.$col_onshow.$s.$col_onform.$s.$col_oninsert.$s.$col_pos.$s.$col_order.$s.$col_default.$s.$col_unique.$s.$col_url.$s.$col_part.$s.$col_bold.$s.$col_required.$s.$col_inform.$s.$col_linkB.$s.$col_link2.$s.$col_link3.$s.$module_url.$s.$module_type.$s.$file_dir.$s.$file_maxsize.$s.$file_totalmax.$s.$file_prefix.$s.$file_types.$s.$col_fastedit.$s.$col_uin.$s.$col_deep.$s.$col_filter.$s.$col_paramlink.$s.$col_speclink.$s.$col_link4.$s.$col_index.$s.$col_force_onshow.$s.$file_genname.$s.$col_deflist.$s.$col_cat.$s.$col_type.$s.$col_date2.$s.$col_parts.$s.$col_tpl;
		$first=false;
	}
	return $res;
}

function export_table($id){
	global $db;
	getrow($db,"SELECT * FROM main_table WHERE table_id=$id");
	foreach($db->Record AS $var=>$value) $$var=$value;
	if(empty($table_uin)){		
		$xuid=uuid();
		$table_uin=$xuid;
		$db->query("UPDATE main_table SET table_uin='".$xuid."' WHERE table_id=$id",3,"main_table");
	}
	$s='|%!%|';
	if(!empty($major_col)){
		getrow($db,"SELECT * FROM main_col WHERE col_id=$major_col");
		$major_col=$db->Record["col_uin"];
	}
	$res=$table_multy.$s.$table_name.$s.$table_sname.$s.$table_cansub.$s.$major_col.$s.$table_bold.$s.$table_onedit.$s.$table_extype.$s.$table_bottom.$s.$table_top.$s.$table_uin.$s.file_to_data($table_icon).$s.$table_date2.$s.$table_parts;
	$res.=$s;
	$cols=export_cols($table_module,$table_id);
	$res.=$cols;
	return $res;
}

function module_uin($uin){
	global $replace_uin;
	if(isset($replace_uin[$uin])) return $replace_uin[$uin];
	else return $uin;
}

function check_module($id){
	global $db,$is_checked,$need_module;
	$new=Array();
	if(!empty($is_checked[$id])) return '';
	$is_checked[$id]=1;	
	$tables=getall($db,"SELECT * FROM main_table WHERE table_module=$id");
	$st=Array();$st2=Array();
	if(!empty($tables)) foreach($tables AS $table){
		$st[]=$table["table_id"];
		$st2[$table["table_id"]]=$table;
	}
	//Проверяем подтаблицы
	if(!empty($st)) $ts=getall($db,"SELECT * FROM table_sub WHERE sub_table1 IN (".implode(',',$st).")");
	if(!empty($ts)) foreach($ts AS $t)if(empty($st2[$t["sub_table2"]])){
		getrow($db,"SELECT * FROM main_table WHERE table_id=".$t["sub_table2"]);
		if(!empty($db->Record["table_module"])) if(empty($need_module[$db->Record["table_module"]])) $new[$db->Record["table_module"]]=1;
	}
	//нижеследующий код не нужен, т.к. новые модули рекурсивно проверяются на связки с модулями
	//if(!empty($st)) $ts=get_crossmodule_subtable($id,1,1);
	//if(!empty($ts)) foreach($ts AS $tmp) if($tmp['table_module']!=$id)  if(empty($need_module[$tmp["table_module"]])) $new[$tmp["table_module"]]=1;
	//Проверяем связи столбцов
	$cols=getall($db,"SELECT * FROM main_col WHERE col_module=$id AND col_type=1 AND col_link!=0");
	if(!empty($cols)) foreach($cols AS $col) if(empty($st2[$col["col_link"]])){
		getrow($db,"SELECT * FROM main_table WHERE table_id=".$col["col_link"]);
		if(!empty($db->Record["table_module"])) if(empty($need_module[$db->Record["table_module"]])) $new[$db->Record["table_module"]]=1;
	}
	//Проверяем связи параметров
	$parts=getall3($db,"SELECT part_id, part_module, part_type FROM main_part WHERE part_type!=2 AND part_module=$id",'part_id');
	if(!empty($parts)) $params=getall3($db,"SELECT * FROM part_param WHERE param_part IN (".implode(',',$parts).") AND param_type=2 AND param_link!=0","param_link");
	if(!empty($params)) $nms=getall3($db,"SELECT table_id, table_module FROM main_table WHERE table_id IN (".implode(',',$params).") AND table_module!=$id","table_module");
	if(!empty($nms)) foreach($nms AS $nm) if(empty($need_module[$nm])) $new[$nm]=1;
	//Завершаем
	foreach($new AS $var=>$value){
		$need_module[$var]=1;
		check_module($var);
	}
}

function get_root3($auth_id,$perm_type,$perm_target,$perm_object){
	global $db;
	if($perm_target!=7) getrow($db,"SELECT * FROM auth_perm WHERE perm_auth=$auth_id AND perm_type=$perm_type AND perm_target=$perm_target AND perm_object=$perm_object",1,"auth_perm");
	else  getrow($db,"SELECT * FROM auth_perm WHERE perm_auth=$auth_id AND perm_type=$perm_type AND perm_target=$perm_target AND perm_subtable='$perm_object'",1,"auth_perm");
	if(!empty($db->Record)){
		$res->add=$db->Record["perm_add"];
		$res->edit=$db->Record["perm_edit"];
		$res->view=$db->Record["perm_view"];
		$res->del=$db->Record["perm_del"];
		$res->invite=$db->Record["perm_invite"];
		$res->upload=$db->Record["perm_upload"];
		$res->leave=$db->Record["perm_leave"];
		$res->rules=$db->Record["perm_rules"];
		$res->reg=$db->Record["perm_reg"];
	} else {
		$res->add=-1;$res->edit=-1;$res->del=-1;$res->view=-1;$res->upload=-1;$res->invite=-1;$res->leave=-1;$res->rules=-1;$res->reg=-1;
	}
	return serialize($res);
}
function echo_subtables_serv($auth_id,$table,$owners=Array(),$exists=Array()){
	global $stables,$tables;
	$exists[$table]=1;
	$res='';
	foreach($stables[$table] AS $tbl)if(empty($exists[$tbl])){
		$owners2=$owners;
		$owners2[]=$tbl;
		$st=implode(',',$owners2);
		$st2='';
		foreach($owners2 AS $own2){
			if(!empty($st2)) $st2.=',';
			$st2.=getrowval("SELECT table_uin, table_id FROM main_table WHERE table_id=".$own2,"table_uin");
		}
		$res.='$-$'.$st2.'$-$'.get_root3($auth_id,3,7,$st).'$-$'.get_root3($auth_id,4,7,$st);
		$tmp=Array();
		if(!empty($stables[$tbl])) foreach($stables[$tbl] AS $tbl2) if(!in_array($tbl2,$owners)) $tmp[]=$tbl;
		if(!empty($tmp)){
			$exists2=$exists;
			$exists2[$tbl]=$tbl;
			$res.=echo_subtables_serv($auth_id,$tbl,$owners2,$exists2);
		}
	}
	return $res;
}

// Функции импорта

function prepend_mod_install(){
	global $todo_part,$todo_col1,$todo_col2,$todo_sub,$todo_paramlink,$table_sub_after,$todo_stables_auth,$todo_param_link,$part_group_link,$todo_ppar_link,$mod_import_started,$todo_import_ex,$todo_row_link,$todo_rows_link,$change_uin;	
	
	$old['todo_stables_auth']=$todo_stables_auth;
	$old['table_sub_after']=$table_sub_after;
	$old['todo_part']=$todo_part;
	$old['todo_col1']=$todo_col1;
	$old['todo_col2']=$todo_col2;
	$old['todo_sub']=$todo_sub;
	$old['todo_paramlink']=$todo_paramlink;
	$old['todo_param_link']=$todo_param_link;
	$old['part_group_link']=$part_group_link;
	$old['todo_ppar_link']=$todo_ppar_link;
	$old['todo_import_ex']=$todo_import_ex;
	$old['mod_import_started']=$mod_import_started;
	$old['todo_row_link']=$todo_row_link;
	$old['todo_rows_link']=$todo_rows_link;
	$old['change_uin']=$change_uin;
		
	$mod_import_started=1;
	$change_uin=Array();
	$todo_stables_auth=Array();
	$table_sub_after=Array();
	$todo_part=Array();
	$todo_col1=Array();
	$todo_col2=Array();
	$todo_sub=Array();
	$todo_paramlink=Array();
	$todo_param_link=Array();
	$part_group_link=Array();
	$todo_ppar_link=Array();
	$todo_import_ex=Array();
	$todo_row_link=Array();
	$todo_rows_link=Array();
	
	return $old;
}

function after_mod_install($old=Array()){
	global $todo_part,$todo_col1,$todo_col2,$todo_sub,$todo_paramlink,$table_sub_after,$todo_stables_auth,$todo_param_link,$part_group_link,$db,$mod_import_started,$todo_import_ex,$todo_row_link,$todo_rows_link,$change_uin;
	if(!empty($todo_param_link)) foreach($todo_param_link AS $param_id=>$table_uin){
		$table_id=getrowval("SELECT * FROM main_table WHERE table_uin='$table_uin'","table_id");
		if(empty($table_id)) $table_id='0';
		$db->query("UPDATE part_param SET param_link=$table_id WHERE param_id=$param_id");
	}
	if(!empty($todo_sub)) foreach($todo_sub AS $var=>$val)foreach($val AS $value){
		$value=explode('**',$value);
		$v1=explode('::',$value[0]);
		$v2=explode('::',$value[1]);
		getrow($db,"SELECT * FROM main_module WHERE module_uin='$v1[0]'");
		if(empty($db->Record) || empty($v1[0])) getrow($db,"SELECT * FROM main_module WHERE module_sname='$v1[2]'");
		$mid=$db->Record["module_id"];
		getrow($db,"SELECT * FROM main_table WHERE table_uin='$v1[1]' AND table_module=".$mid);
		if(empty($db->Record) || empty($v1[1])){
			getrow($db,"SELECT * FROM main_module WHERE module_sname='$v1[2]'");
			$mid=$db->Record["module_id"];
			getrow($db,"SELECT * FROM main_table WHERE table_module=".$mid." AND table_sname='$v1[3]'");
		}
		$ov1=$v1;
		$v1=$db->Record["table_id"];
		//echo $value[0].'='.$v1.'<br>';
		getrow($db,"SELECT * FROM main_module WHERE module_uin='$v2[0]'");
		if(empty($db->Record) || empty($v2[0])) getrow($db,"SELECT * FROM main_module WHERE module_sname='$v2[2]'");
		$mid=$db->Record["module_id"];
		$ov2=$v2;
		if(!empty($mid)){
			getrow($db,"SELECT * FROM main_table WHERE table_uin='$v2[1]' AND table_module=".$mid);
			if(empty($db->Record) || empty($v2[1])){
				getrow($db,"SELECT * FROM main_module WHERE module_sname='$v2[2]'");
				$mid=$db->Record["module_id"];
				getrow($db,"SELECT * FROM main_table WHERE table_module=".$mid." AND table_sname='$v2[3]'");
			}
			$v2=$db->Record["table_id"];
		}
		//echo $value[1].'='.$v2.'<br><br>';	
		//echo "INSERT INTO table_sub (sub_table1, sub_table2) VALUES ($v1,$v2) - ".$ov1[0].' - '.$ov1[1].' - '.$ov1[2].' - '.$ov1[3].' - '.$ov2[0].' - '.$ov2[1].' - '.$ov2[2].' - '.$ov2[3].'<br>';
		if(!empty($v1) && !empty($v2) && is_numeric($v1) && is_numeric($v2)) $db->query("INSERT INTO table_sub (sub_table1, sub_table2) VALUES ($v1,$v2)");
	}
	if(!empty($todo_col1)) foreach($todo_col1 AS $col_id=>$tmp){
		$tmp=explode('::',$tmp);
		getrow($db,"SELECT * FROM main_module WHERE module_uin='$tmp[0]'");
		if(empty($db->Record) || empty($tmp[0])) getrow($db,"SELECT * FROM main_module WHERE module_sname='$tmp[2]'");
		$mid=$db->Record["module_id"];
		getrow($db,"SELECT * FROM main_table WHERE table_uin='$tmp[1]' AND table_module=".$mid);
		if(empty($db->Record) || empty($tmp[1])){
			getrow($db,"SELECT * FROM main_module WHERE module_sname='$tmp[2]'");
			$mid=$db->Record["module_id"];
			getrow($db,"SELECT * FROM main_table WHERE table_module=".$mid." AND table_sname='$tmp[3]'");
		}
		$link_table=$db->Record["table_id"];
		if(empty($link_table)) $link_table=0;
		if(isset($tmp[4]) && !empty($link_table)) $link_table=-$link_table;
		$db->query("UPDATE main_col SET col_link=".$link_table." WHERE col_id=$col_id");
	}
	if(!empty($todo_paramlink)) foreach($todo_paramlink AS $col_id=>$data){
		$data=explode(':',$data);
		$res='';
		if($data[0]=='0'){
			$part_proc=$data[1];
			$part_uin=$data[2];
			$param_sname=$data[3];
			$part_sname=$data[4];
			getrow($db,"SELECT * FROM main_part WHERE part_type=2 AND part_proc=$part_proc AND part_uin='$part_uin'");
			// догрузка части
			if(empty($db->Record)){
				//echo $part_proc.'::'.$part_sname;
				//exit;
				import_part($part_proc.'::'.$part_sname);
				getrow($db,"SELECT * FROM main_part WHERE part_type=2 AND part_proc=$part_proc AND part_uin='$part_uin'");
			}
			if(!empty($db->Record)){
				getrow($db,"SELECT * FROM part_param WHERE param_part=".$db->Record["part_id"]." AND param_sname='$param_sname'");
				if(!empty($db->Record)) $res=$db->Record["param_id"];
			}
		}
		if($data[0]=='1'){
			$module_uin=$data[1];
			$part_uin=$data[2];			
			$param_sname=$data[3];
			getrow($db,"SELECT * FROM main_module WHERE module_uin='$module_uin'");
			// здесь догрузка модуля не производится (хотя можно было бы)
			if(!empty($db->Record)){
				getrow($db,"SELECT * FROM main_part WHERE part_type!=2 AND part_module=".$db->Record["module_id"]." AND part_uin='$part_uin'");
				if(!empty($db->Record)){
					getrow($db,"SELECT * FROM part_param WHERE param_part=".$db->Record["part_id"]." AND param_sname='$param_sname'");
					if(!empty($db->Record)) $res=$db->Record["param_id"];
				}
			}
		}
		if(!empty($res)) $db->query("UPDATE main_col SET col_paramlink=$res WHERE col_id=$col_id",2,"main_col");
	}
	prep_tables(true); global $stables;
	if(!empty($todo_stables_auth))foreach($todo_stables_auth AS $val)if(!empty($val->data) && !empty($stables[$val->tid])){
		parse_subtables_client(1/*0 - пустая*/,$val->data,$val->auth,$val->tid,Array($val->tid));
	}
	if(!empty($table_sub_after)) foreach($table_sub_after AS $tsa){
		//$table_id=getrowval("SELECT table_id, table_uin FROM main_table WHERE table_uin='".$tsa["uin"]."'","table_id");
		if(strpos($tsa["uin"],',')) $tsa["uin"]=explode(',',$tsa["uin"]); else $tsa["uin"]=Array($tsa["uin"]);
		$sto='';
		foreach($tsa["uin"] AS $vts){
			if(!empty($sto)) $sto.=',';
			$sto.=getrowval("SELECT table_id, table_uin FROM main_table WHERE table_uin='".$vts."'","table_id");
		}
		if(!empty($sto)){
			set_root($tsa["auth"],3,7,/*$tsa["owner"]*/$sto,$tsa["data1"]);
			set_root($tsa["auth"],4,7,/*$tsa["owner"]*/$sto,$tsa["data2"]);
		}
	}
	if(!empty($part_group_link)) foreach($part_group_link AS $part_id=>$group_uin){		
		$group_id=getrowval("SELECT auth_id FROM main_auth WHERE group_uin='".$group_uin."'","auth_id");
		if(!empty($group_id)) $db->query("UPDATE main_part SET part_auth=$group_id WHERE part_id=$part_id");
	}
	if(!empty($todo_import_ex)) foreach($todo_import_ex AS $iex){
		$sid=$iex[0];
		$ex_uin=$iex[1];
		$sid2=import_ex(0,$ex_uin,true,$sid);
		SetCookie('cex'.$sid,$sid2, time()+60*60*24*30);
		global $user;
		if(!$user->super){
			$db->query("INSERT INTO auth_perm (perm_target,perm_type,perm_auth,perm_object,perm_view,perm_edit,perm_del)
							VALUES	(1,2,$user->id,$sid2,2,2,2)",3,"auth_perm");
		}
	}
	if(!empty($todo_row_link)) foreach($todo_row_link AS $tdr){	
		$row=text_to_row($tdr['data'],0,0,$tdr['own_tex'],0,true,$tdr['lnk']);
		//echo $tdr['data'].' - '.$row.'<br>';
		if(!empty($row)){
			$db->query("INSERT INTO row_value (value_module, value_table, value_row, value_col, value_value)
								VALUES (".$tdr['module'].", ".$tdr['table'].", ".$tdr['row'].", ".$tdr['col'].", '$row')");
		}
	}
	if(!empty($todo_rows_link)) foreach($todo_rows_link AS $tdr){
		$rows=text_to_rows($tdr['data'],0,0,$tdr['own_tex'],0,true,$tdr['lnk']);
		foreach($rows AS $row) if(!empty($row)){
			$db->query("INSERT INTO row_value (value_module, value_table, value_row, value_col, value_value)
								VALUES (".$tdr['module'].", ".$tdr['table'].", ".$tdr['row'].", ".$tdr['col'].", '$row')");
		}
	}
	if(!empty($change_uin))foreach($change_uin AS $row=>$tmp){
		$db->query("UPDATE main_row SET row_uin='".uuin()."' WHERE row_id=$row");
	}
	$mod_import_started=false;
	if(!empty($old)){
		$todo_stables_auth=$old['todo_stables_auth'];
		$table_sub_after=$old['table_sub_after'];
		$todo_part=$old['todo_part'];
		$todo_col1=$old['todo_col1'];
		$todo_col2=$old['todo_col2'];
		$todo_sub=$old['todo_sub'];
		$todo_paramlink=$old['todo_paramlink'];
		$todo_param_link=$old['todo_param_link'];
		$part_group_link=$old['part_group_link'];
		$todo_ppar_link=$old['todo_ppar_link'];
		$todo_import_ex=$old['todo_import_ex'];
		$mod_import_started=$old['mod_import_started'];
		$todo_row_link=$old['todo_row_link'];
		$todo_rows_link=$old['todo_rows_link'];
		$change_uin=$old['change_uin'];
	} else {
		$change_uin=Array();
		$todo_stables_auth=Array();
		$table_sub_after=Array();
		$todo_part=Array();
		$todo_col1=Array();
		$todo_col2=Array();
		$todo_sub=Array();
		$todo_paramlink=Array();
		$todo_param_link=Array();
		$part_group_link=Array();
		$todo_ppar_link=Array();
		$todo_import_ex=Array();
		$todo_row_link=Array();
		$todo_rows_link=Array();
	}
}

function import_ex($ex_id=0,$ex_uin='',$force_add=false,$module_id=0,$ex_public=0,$ex_name='',$ex_sname='',$import_rows=true){
	global $db, $mod_import_started;
	
	alog('START IMPORT EX '.$ex_id.' '.$ex_uin.' '.$module_id);
	
	$ex_major=getrowval("SELECT table_id FROM main_table WHERE table_bold=1 AND table_module=$module_id","table_id");
	if(empty($ex_major)) $ex_major=0;
	
	if(!$import_rows) $add='&ignore_all_rows=1'; else $add='';
	//if(empty($ex_id)) echo $GLOBALS["update_server"].'?type=get_ex&ex='.$ex_uin.$add;
	//else echo $GLOBALS["update_server"].'?type=get_ex&ex='.$ex_id.$add;
	if(empty($ex_id)) $data=unserialize(loadserv($GLOBALS["update_server"].'?type=get_ex&ex='.$ex_uin.$add));
	else $data=unserialize(loadserv($GLOBALS["update_server"].'?type=get_ex&ex='.$ex_id.$add));

	$ex_uin=$data['ex_uin'];
	
	getrow($db,"SELECT * FROM ex_module WHERE ex_uin='$ex_uin'");
	if(!empty($db->Record) && !$force_add) return $db->Record['ex_id'];
	
	if(empty($ex_name)) $ex_name=$data['ex_name'];
	if(empty($ex_sname)) $ex_sname=$data['ex_sname'];
	$db->query("INSERT INTO ex_module (ex_name, ex_sname, ex_module, ex_major, ex_public, ex_uin)
			VALUES ('$ex_name', '$ex_sname', $module_id, $ex_major, $ex_public, '$ex_uin')",3,'ex_module');
	getrow($db,"SELECT LAST_INSERT_ID() as sid");
	$ex=$db->Record["sid"];
	
	foreach($data['rows'] AS $table_uin=>$tex_data){
		$tex_name=$tex_data['name'];
		$rows=$tex_data['rows'];
		$table=getrow($db,"SELECT * FROM main_table WHERE table_uin='$table_uin'");
		if(empty($table)) continue;
		$table_id=$table['table_id'];
		$global_table_tex=getrowval("SELECT * FROM ex_group WHERE ex_table=$table_id ORDER BY ex_id DESC LIMIT 1","ex_ex1");
		$ignore_create=false;
		if($table['table_extype']==2 && !empty($global_table_tex)/* && !$mod_import_started*/){
			$ignore_create=true;
			$tex=$global_table_tex;
		} else {
			$db->query("INSERT INTO ex_table (ex_name, ex_module, ex_table)
					VALUES ('$tex_name', $module_id, $table_id)",3,'ex_table');
			getrow($db,"SELECT LAST_INSERT_ID() as sid");
			$tex=$db->Record["sid"];
			
			//echo $tex.' - '.getrowval("SELECT table_name FROM main_table WHERE table_id=$table_id","table_name").'<br>';
		}
		$db->query("INSERT INTO ex_group (ex_module, ex_table, ex_ex1, ex_ex2)
				VALUES ($module_id, $table_id, $tex, $ex)",3,'ex_group');
		
		//alog('import tex in IMPORT EX at '.$tex_name.' '.$tex);
		$data['rows'][$table_uin]['tex']=$tex;
		$data['rows'][$table_uin]['ignore_create']=$ignore_create;
		//text_to_rows($rows,$tex,0,0,0,$ignore_create);
	}
	foreach($data['rows'] AS $table_uin=>$tex_data){
		text_to_rows($tex_data['rows'],$tex_data['tex'],0,0,0,$tex_data['ignore_create']);
	}
	// ex param
	if(!empty($data['ex_param'])) text_to_params($data['ex_param'],$ex,0,$module_id,$ex,0);
	
	//import parts
	$links=Array();
	if(!empty($data['parts'])){
			$parts=$data['parts'];
			global $db;
			$tmp=getall($db,"SELECT part_type, part_proc, part_sname FROM main_part WHERE part_type=2",1,"main_part");
			$parts_dst=Array();
			if(!empty($tmp)) foreach($tmp AS $t){
				$parts_dst[$t['part_proc']][$t['part_sname']]=1;
			}
			foreach($parts AS $part=>$components)
			foreach($components AS $part_proc=>$tmp)
			foreach($tmp AS $part_sname=>$t)
			if(empty($parts_dst[$part_proc][$part_sname])){
				alog('import part in IMPORT EX at '.$part_proc.' '.$part_sname);
				import_part($part_proc.'::'.$part_sname);
			}
	}

	if(!empty($data['mod_links'])){
		$mds=$data['mod_links'];
		if(!empty($mds)) foreach($mds AS $m){
			import_modex($m['module'],$m['ex']);
		}
	}
	
	if(!empty($data['links'])){
		text_to_rows($data['links'],0,0,0,0,true);
	}
	
	alog('END IMPORT EX '.$ex_id.' '.$ex_uin.' '.$module_id);
	
	return $ex;
}

function import_col($module,$table,$data){
	global $db,$todo_col1,$todo_col2,$todo_paramlink,$lcol;
	if($data=='load'){
		$data=loadserv($GLOBALS["update_server"].'?type=col&id='.$module);
		$module=0;
		$table=0;
		if(empty($data)) return false;
	}
	$data=explode('|-+-|',$data);
	for($i=0;$i<count($data);$i++)	$data[$i]=str_replace("'","''",$data[$i]);
	$col_name=$data[0];
	if(empty($col_name)) return false;
	$col_sname=$data[1];
	$col_type=$data[2];
	$col_onshow=$data[3];
	$col_onform=$data[4];
	$col_oninsert=$data[5];
	$col_pos=$data[6];
	$col_order=$data[7];
	$col_default=$data[8];
	$col_unique=$data[9];
	$col_url=$data[10];
	$col_partB=$data[11];
	$col_part=0;
	$col_bold=$data[12];
	$col_required=$data[13];
	$col_inform=$data[14];
	$col_linkB=$data[15];
	$col_link=0;
	$col_link2=$data[16];
	$col_link3=$data[17];
	$module_url=$data[18];
	$module_type=$data[19];
	$file_dir=$data[20];
	$file_maxsize=$data[21];
	$file_totalmax=$data[22];
	$file_prefix=$data[23];
	if(!empty($data[24])) $file_types=$data[24]; else $file_types='';
	$col_fastedit=$data[25];
	$col_uin=$data[26];
	$col_deep=$data[27];
	$col_filter=$data[28];
	$col_paramlink=$data[29];
	$col_speclink=$data[30];
	$col_link4=$data[31];
	$col_index=$data[32];
	$col_force_onshow=$data[33];
	$file_genname=$data[34];
	$col_deflist=$data[35];
	$col_cat=$data[36];
	$col_target=$data[37];
	$col_date2=$data[38];
	$col_parts=$data[39];
	$col_tpl=$data[40];
	if(empty($col_filter)) $col_filter='0';
	if(empty($col_index)) $col_index='0';
	if(empty($col_force_onshow)) $col_force_onshow='0';
	if(empty($col_paramlink)) $col_paramlink='0';
	if(empty($col_type)) $col_type='0';
	if(empty($col_link4)) $col_link4='0';
	if(empty($col_fastedit)) $col_fastedit='0';
	if(empty($col_order)) $col_order='0';
	if(empty($col_pos)) $col_pos='0';
	if(empty($col_deep)) $col_deep='0';
	if(empty($col_unique)) $col_unique='0';
	if(empty($col_url)) $col_url='0';
	if(empty($col_bold)) $col_bold='0';
	if(empty($col_required)) $col_required='0';
	if(empty($col_inform)) $col_inform='0';
	if(empty($col_link2)) $col_link2='0';
	if(empty($col_link3)) $col_link3='0';
	if(empty($module_url)) $module_url='0';
	if(empty($module_type)) $module_type='0';
	if(empty($file_maxsize)) $file_maxsize='0';
	if(empty($file_totalmax)) $file_totalmax='0';
	if(empty($file_genname)) $file_genname='0';
	if(empty($col_cat)) $col_cat='0';
	if(empty($col_tpl)) $col_tpl='0';
	if(empty($col_date2)) $col_date2=date('Y-m-d H:i:s');
	$db->query("INSERT INTO main_col (col_module, col_table, col_name, col_sname, col_type, col_onshow, col_onform, col_oninsert, col_pos, col_order, col_default, col_unique, col_url, col_part, col_bold, col_required, col_inform, col_link, col_link2, col_link3, col_link4, module_url, module_type, file_dir, file_maxsize, file_totalmax, file_prefix, file_types, col_fastedit, col_uin, col_deep, col_filter, col_speclink, col_index, col_force_onshow, file_genname, col_deflist, col_cat, col_target, col_date, col_date2, col_parts, col_tpl)
				VALUES($module, $table, '$col_name', '$col_sname', $col_type, '$col_onshow', '$col_onform', '$col_oninsert', $col_pos, $col_order, '$col_default', $col_unique, $col_url, $col_part, $col_bold, $col_required, $col_inform, $col_link, $col_link2, $col_link3, $col_link4, $module_url, $module_type, '$file_dir', $file_maxsize, $file_totalmax, '$file_prefix', '$file_types', $col_fastedit, '$col_uin', '$col_deep', $col_filter, '$col_speclink', $col_index, $col_force_onshow, $file_genname, '$col_deflist', $col_cat, $col_target, '$col_date2', '0000-00-00 00:00:00', '$col_parts', $col_tpl)");
	getrow($db,"SELECT LAST_INSERT_ID() as sid");
	$col_id=$db->Record["sid"];
	if(!empty($col_paramlink)) $todo_paramlink[$col_id]=$col_paramlink;
	$lcol[$table][$col_uin]=$col_id;
	if(!empty($col_linkB)) $todo_col1[$col_id]=$col_linkB;
	if(!empty($col_partB)) $todo_col2[$col_id]=$col_partB;
}

function import_table($module,$data){
	global $db,$ltable;
	if($data=='load'){
		$data=loadserv($GLOBALS["update_server"].'?type=tbltpl&id='.$module);
		$module=0;
		if(empty($data)) return false;
	}
	$data=explode('|%!%|',$data);
	for($i=0;$i<count($data)-1;$i++) $data[$i]=str_replace("'","''",$data[$i]);
	$table_multy=$data[0];
	$table_name=$data[1];
	$table_sname=$data[2];
	$table_cansub=$data[3];
	$major_col=$data[4];
	$table_bold=$data[5];
	$table_onedit=$data[6];
	$table_extype=$data[7];
	$table_bottom=$data[8];
	$table_top=$data[9];
	$table_uin=$data[10];
	$table_icon=$data[11];
	$table_date=$data[12];
	$table_parts=$data[13];
	if($table_icon) $table_icon=data_to_file($table_icon);
	$cols=explode('|-(-|',$data[14]);
	if(empty($table_multy)) $table_multy=0;
	if(empty($table_cansub)) $table_cansub=0;
	if(empty($major_col)){ $major_col=0; $major_colB=0; }else $major_colB=$major_col;
	$major_col=0;
	if(empty($table_bold)) $table_bold=0;
	if(empty($table_extype)) $table_extype=0;
	$db->query("INSERT INTO main_table (table_module, table_multy, table_name, table_sname, table_cansub, major_col, table_bold, table_onedit, table_extype, table_top, table_bottom, table_uin, table_icon, table_date, table_date2, table_parts)
				VALUES($module, $table_multy, '$table_name', '$table_sname', $table_cansub, $major_col, $table_bold, '$table_onedit', $table_extype, '$table_top', '$table_bottom', '$table_uin', '$table_icon', '$table_date', '0000-00-00 00:00:00', '$table_parts')");
	getrow($db,"SELECT LAST_INSERT_ID() as sid");				
	$table_id=$db->Record["sid"];	
	$ltable[$module][$table_uin]=$table_id;
	if(!empty($cols))foreach($cols AS $col){
		import_col($module,$table_id,$col);
	}
	if(!empty($major_colB)){
		getrow($db,"SELECT * FROM main_col WHERE col_table=$table_id AND col_uin='$major_colB'");
		if(!empty($db->Record["col_id"])) $db->query("UPDATE main_table SET major_col=".$db->Record["col_id"]." WHERE table_id=$table_id");
	}
}

function complex_import_module($uin,$create_base_ex=false){
	global $db;
	$mid=getrowval("SELECT module_id FROM main_module WHERE module_uin='$uin'","module_id");
	if(!empty($mid)) return $mid;
	$old=prepend_mod_install();
	$links=loadserv($GLOBALS["update_server"].'?type=modlinks&uin='.$uin);
	alog('START COMPLEX IMPORT AT '.$uin);
	$sid=0;
	//if(!empty($links)){
	if(!empty($links)) $links=explode(',',$links);
	$la=Array();
	$la[$uin]=$uin;
	if(!empty($links)) foreach($links AS $l) if(!getrowval("SELECT module_id FROM main_module WHERE module_uin='$l'",'module_id')) $la[$l]=$l;
	//load linked modules
	foreach($la AS $module_uin){
		alog('import module in COMPLEX IMPORT at '.$module_uin);
		$tmp=import_module(0,$module_uin,($module_uin==$uin));
		if($module_uin==$uin) $sid=$tmp;
		reset_module_json(0,'',$module_uin);
	}
	//load requirements parts
	$parts=unserialize(loadserv($GLOBALS["update_server"].'?type=mods_part_links&uins='.implode(',',$la)));
	if(!empty($parts)){
		$tmp=getall($db,"SELECT part_type, part_proc, part_sname FROM main_part WHERE part_type=2",1,"main_part");
		$parts_dst=Array();
		if(!empty($tmp)) foreach($tmp AS $t){
			$parts_dst[$t['part_proc']][$t['part_sname']]=1;
		}
		foreach($parts AS $part_proc=>$tmp)
		foreach($tmp AS $part_sname=>$t)
		if(empty($parts_dst[$part_proc][$part_sname])){
			alog('import part in COMPLEX IMPORT at '.$part_proc.' '.$part_sname);
			import_part($part_proc.'::'.$part_sname);
		}
	}
	//}
	after_mod_install($old);
	if($create_base_ex) add_base_ex($sid);
	alog('END COMPLEX IMPORT AT '.$uin);
	return $sid;
}

function add_base_ex($module_id){
	global $user,$db,$mod_import_started,$todo_import_ex;
	$sid=$module_id;
	$sid2=0;
	
	$uin=getrowval("SELECT module_uin FROM main_module WHERE module_id=$module_id",'module_uin');
	$public_ex_uin=loadserv($GLOBALS["update_server"].'?type=get_public_ex&uin='.$uin);
	//echo $GLOBALS["update_server"].'?type=get_public_ex&uin='.$uin;
	if(!empty($public_ex_uin)){
		if($mod_import_started){
			$todo_import_ex[]=Array($module_id,$public_ex_uin);
		} else $sid2=import_ex(0,$public_ex_uin,true,$sid);
	} else {
		$db->query("INSERT INTO ex_module (ex_module, ex_name, ex_sname)
				VALUES ($sid, 'основной','base')",3,'ex_module');
		getrow($db,"SELECT LAST_INSERT_ID() as sid2");
		$sid2=$db->Record["sid2"];
		$cex=$sid2;
		$tables=getall($db,"SELECT * FROM main_table WHERE table_module=$module_id");
		if(!empty($tables)) foreach($tables AS $table){
			$db->query("INSERT INTO ex_table (ex_table, ex_module, ex_name)
					VALUES (".$table['table_id'].", $module_id, '".$table['table_name']."')",3,'ex_table');		
			getrow($db,"SELECT LAST_INSERT_ID() as sid3");
			$sid3=$db->Record["sid3"];
			$db->query("INSERT INTO ex_group (ex_module, ex_table, ex_ex1, ex_ex2)
					VALUES ($module_id, ".$table['table_id'].", $sid3, $sid2)",3,'ex_group');		
		}
	}
	
	if(!empty($sid2)){
		SetCookie('cex'.$sid,$sid2, time()+60*60*24*30);
		global $user;
		if(!$user->super){
			$db->query("INSERT INTO auth_perm (perm_target,perm_type,perm_auth,perm_object,perm_view,perm_edit,perm_del)
							VALUES	(1,2,$user->id,$sid2,2,2,2)",3,"auth_perm");
		}
	}
}

function import_module($id,$uin='',$skip_ex_add=false){
	global $db,$todo_sub,$todo_part,$todo_part2,$todo_col2,$todo_part2b/*,$table_sub_after*/;
	$todo_part=Array();
	$todo_part2=Array();
	$todo_part2b=Array();
	$todo_col2=Array();
	//$table_sub_after=Array();
	if(empty($id) && empty($uin)) return false;
	if(!empty($uin))	$tmp=loadserv($GLOBALS["update_server"].'?type=mod&uin='.$uin);
	else $tmp=loadserv($GLOBALS["update_server"].'?type=mod&id='.$id);
	if(empty($tmp)) return 0;
	$s='<|-|>';	
	$tmp=explode($s,$tmp);
	$module_icon=$tmp[4];
	if($module_icon) $module_icon=data_to_file($module_icon);
	
	$db->query("INSERT INTO main_module (module_name,module_sname,module_uin,module_date,module_date2,module_lastcheck,module_parts,module_icon)
				VALUES('$tmp[0]','$tmp[1]','$tmp[2]','".date('Y-m-d H:i:s')."','".date('Y-m-d H:i:s')."','".date('Y-m-d')."','$tmp[3]','$module_icon')");
	getrow($db,"SELECT LAST_INSERT_ID() as sid");
	$module_id=$db->Record["sid"];
	$next=5;
	$tables=explode('?|?|?',$tmp[$next]); $next++;
	if(!empty($tables)) foreach($tables AS $table){
		import_table($module_id,$table);
	}
	$subs=explode('?|?|?',$tmp[$next]); $next++;
	if(!empty($subs)) foreach($subs AS $sub)if(!empty($sub)){
		$todo_sub[$module_id][]=$sub;
	}
	$ex_cols=explode('|-(-|',$tmp[$next]); $next++;
	if(!empty($ex_cols)) foreach($ex_cols AS $ex_col){
		if(!empty($ex_col)) import_col($module_id,0,$ex_col);
	}
	$parts=explode('?|?|?',$tmp[$next]); $next++;
	if(!empty($parts)) foreach($parts AS $part) if($part!='') import_part(0,0,$part,$module_id);
	global $lgrp;
	$groups=explode('?|?|?',$tmp[$next]); $next++;
	if(!empty($groups)) foreach($groups AS $group) if($group!='') import_group($group,$module_id);
	$grp=explode('**',$tmp[$next]); $next++;
	$i=0;
	//if(!empty($groups)) for($i2=0;$i2<count($groups)*count($groups);$i2++){
	//if(!empty($groups)) foreach($groups AS $group) foreach($groups AS $grp){
	if(!empty($grp)) for($i2=0;$i2<floor(count($grp)/2);$i2++){
		$t=explode('||',$grp[$i]);$i++;
		set_root($lgrp[$t[1]],6,1,$lgrp[$t[0]],$t[2]);
		$t=explode('||',$grp[$i]);$i++;
		set_root($lgrp[$t[1]],8,1,$lgrp[$t[0]],$t[2]);
	}
	$module_major=$tmp[/*9*/$next];
	if(!empty($module_major)) $db->query("UPDATE main_module SET module_major=$module_major WHERE module_id=$module_id",2,"main_module");
	
	foreach($todo_part AS $part_id=>$table_uin){
		getrow($db,"SELECT * FROM main_table WHERE table_uin='$table_uin' AND table_module=".$module_id);
		if(!empty($db->Record["table_id"])) $db->query("UPDATE main_part SET part_table=".$db->Record["table_id"]." WHERE part_id=$part_id");
	}
	foreach($todo_part2 AS $part_id=>$tmp){
		if(!empty($todo_part2b[$tmp])){
			$part_owner=$todo_part2b[$tmp];
		} else {
			getrow($db,"SELECT * FROM main_part WHERE part_module=$module_id AND part_uin='$tmp'",1,"main_part");
			$part_owner=$db->Record["part_id"];
		}
		if(empty($part_owner)) $part_owner='0';		
		$db->query("UPDATE main_part SET part_owner=$part_owner WHERE part_id=$part_id");
	}
	foreach($todo_col2 AS $col_id=>$part_uin){
		getrow($db,"SELECT * FROM main_part WHERE part_uin='$part_uin' AND part_module=".$module_id);
		if(!empty($db->Record["part_id"])) $db->query("UPDATE main_col SET col_part=".$db->Record["part_id"]." WHERE col_id=$col_id");
	}
	//Создание экземпляров
	if(!$skip_ex_add){
		add_base_ex($module_id);
	}
	return $module_id;
}

function import_group($data,$module){
	global $db,$ltable,$lcol,$user,$lgrp,$azone;
	$data=explode('**',$data);$i=0;
	$auth_date=$data[$i];$i++;
	$group_name=$data[$i];$i++;
	$group_sname=$data[$i];$i++;
	$group_uin=$data[$i];$i++;
	$use_admin=$data[$i];$i++;
	if($group_name!="0" && $group_name!=""){ 
		getrow($db,"SELECT * FROM main_auth WHERE group_name='$group_name'",1,"main_auth");
		if(!empty($db->Record) && empty($GLOBALS["do_not_addon_groups"])){
			$group_name_old=$group_name;
			$i2=0;
			while(!empty($db->Record)){
				$i2++;
				$group_name=$group_name_old.' '.$i2;
				getrow($db,"SELECT * FROM main_auth WHERE group_name='$group_name'",1,"main_auth");
			}
			if(empty($GLOBALS["silent"])) echo '<p><b>Внимание</b>! Группа "'.$group_name_old.'". Загружена под новым именем "'.$group_name.'". Причина - группа с таким именем уже существует в системе.</p>';
		}
		getrow($db,"SELECT * FROM main_auth WHERE group_sname='$group_sname'",1,"main_auth");
		if(!empty($db->Record) && empty($GLOBALS["do_not_addon_groups"])){
			$group_sname_old=$group_sname;
			$i2=0;
			while(!empty($db->Record)){
				$i2++;
				$group_sname=$group_sname_old.' '.$i2;
				getrow($db,"SELECT * FROM main_auth WHERE group_sname='$group_sname'",1,"main_auth");
			}
			if(empty($GLOBALS["silent"])) echo '<p><b>Внимание</b>! Группа "'.$group_sname_old.'". Загружена под новым именем "'.$group_sname.'". Причина - группа с таким именем уже существует в системе.</p>';
		}
		$db->query("INSERT INTO main_auth (auth_type, auth_owner, auth_date, group_name, group_module, group_uin, group_sname)
				VALUES (1, ".$user->id.", '".$auth_date."','$group_name', $module, '$group_uin', '$group_sname')",3,'main_auth');
		getrow($db,"SELECT LAST_INSERT_ID() as sid");
		$sid=$db->Record["sid"];	
		$lgrp[$group_uin]=$sid;
	} else {
		$sid=0;
		$lgrp[$group_uin]=$sid;
	}
	if(empty($azone)){
		$az=getall3($db,"SELECT * FROM main_zone WHERE zone_module=-1 AND zone_active=1","zone_id");
		if(!empty($az)) foreach($az AS $azz){$azone=$azz; break;}
	}
	if($use_admin && !empty($azone) && !empty($sid)){
		$db->query("INSERT INTO auth_perm (perm_auth, perm_target, perm_type, perm_object, perm_add, perm_edit, perm_view, perm_del, perm_invite, perm_upload, perm_leave, perm_rules, perm_reg)
			VALUES($sid, 1, 0, $azone, -1, -1, 1, -1, -1, -1, -1, -1, -1)",3,"auth_perm");
	}
	set_root($sid,1,1,$module,$data[$i]);$i++;
	set_root($sid,3,3,$module,$data[$i]);$i++;
	set_root($sid,4,5,$module,$data[$i]);$i++;
	set_root($sid,2,3,$module,$data[$i]);$i++;
	set_root($sid,6,3,$module,$data[$i]);$i++;
	set_root($sid,8,3,$module,$data[$i]);$i++;
	if(!empty($ltable[$module])) foreach($ltable[$module] AS $lt){
		$tmp=explode('||',$data[$i]);$i++;
		if(!empty($ltable[$module][$tmp[0]])){
			$tid=$ltable[$module][$tmp[0]];
		} else {
			if(!empty($tmp[0])) $tid=getrowval("SELECT * FROM main_table WHERE table_module=$module AND table_uin='".$tmp[0]."'","table_id");
			else $tid=0;
		}
		if(!empty($tid)) set_root($sid,3,1,$tid,$tmp[1]);
		$tmp=explode('||',$data[$i]);$i++;
		if(!empty($tid)) set_root($sid,4,3,$tid,$tmp[1]);
		if(!empty($tid) && !empty($lcol[$tid])) foreach($lcol[$tid] AS $lc){
			$tmp=explode('||',$data[$i]);$i++;
			if(!empty($tid)) set_root($sid,7,1,$lcol[$tid][$tmp[0]],$tmp[1]);
		}
		global $todo_stables_auth;
		if(empty($todo_stables_auth)) $xx=0; else $xx=count($todo_stables_auth);
		$todo_stables_auth[$xx]=new stdClass();
		$todo_stables_auth[$xx]->data=explode('$-$',$data[$i]);$i++;
		$todo_stables_auth[$xx]->auth=$sid;
		$todo_stables_auth[$xx]->tid=$tid;
		/*if(!empty($stables[$tid])){
			$i=parse_subtables_client($i,$data,$sid,$tid,Array($tid));
		}*/
	}
	
	check_dir('files/pub/new');
	if($sid!=0){
		$db->query("INSERT INTO auth_perm (perm_auth, perm_target, perm_type, perm_object, perm_folder, perm_add, perm_edit, perm_view, perm_del, perm_invite, perm_upload, perm_leave, perm_rules, perm_reg)
			VALUES($sid, 1, 5, 0, 'files/pub/new', -1, 1, 1, 1, -1, 1, -1, -1, -1)",3,"auth_perm");
		$db->query("INSERT INTO auth_perm (perm_auth, perm_target, perm_type, perm_object, perm_folder, perm_add, perm_edit, perm_view, perm_del, perm_invite, perm_upload, perm_leave, perm_rules, perm_reg)
			VALUES($sid, 6, 5, 0, 'files/pub/new', -1, -1, -1, -1, -1, -1, -1, -1, -1)",3,"auth_perm");
	}
}

function parse_subtables_client($i,$data,$auth_id,$table,$owners=Array(),$exists=Array()){
	global $stables,$tables,$table_sub_after;
	$exists[$table]=1;
	$res='';
	foreach($stables[$table] AS $tbl)if(empty($exists[$tbl])){
		$owners2=$owners;
		$owners2[]=$tbl;
		$st=implode(',',$owners2);
		$table_sid=$data[$i];$i++;
		$data1=$data[$i];$i++;
		$data2=$data[$i];$i++;
		if(empty($table_sub_after)) $tsa=0; else $tsa=count($table_sub_after);
		$table_sub_after[$tsa]["uin"]=$table_sid;
		$table_sub_after[$tsa]["data1"]=$data1;
		$table_sub_after[$tsa]["data2"]=$data2;
		$table_sub_after[$tsa]["owner"]=$st;
		$table_sub_after[$tsa]["auth"]=$auth_id;
		//set_root($auth_id,4,7,$st,$data[$i]);$i++;
		//set_root($auth_id,3,7,$st,$data[$i]);$i++;
		$tmp=Array();
		if(!empty($stables[$tbl])) foreach($stables[$tbl] AS $tbl2) if(!in_array($tbl2,$owners)) $tmp[]=$tbl;
		$cbol=false;
		if(!empty($tmp)){
			$exists2=$exists;
			$exists2[$tbl]=$tbl;
			$i=parse_subtables_client($i,$data,$auth_id,$tbl,$owners2,$exists2);
		}
	}
	return $i;
}	

function import_cat($id){
	global $db;
	$tmp=loadserv($GLOBALS["update_server"].'?type=cat&id='.$id);
	$tmp=str_replace("'","''",$tmp);
	$tmp=explode('???',$tmp);
	$t=explode('|||',$tmp[0]);
	for($i=0;$i<count($t);$i++) $t[$i]=str_replace("'","''",$t[$i]);
	getrow($db,"SELECT * FROM part_cat WHERE cat_type=$t[1] AND (cat_uin='$t[4]' OR cat_name='$t[0]')");
	if(empty($db->Record)){
		$db->query("INSERT INTO part_cat (cat_name,cat_type,cat_pre,cat_after,cat_uin)
			VALUES ('$t[0]',$t[1],'$t[2]','$t[3]','$t[4]')",3,'part_cat');
		getrow($db,"SELECT LAST_INSERT_ID() as sid");
		$sid=$db->Record["sid"];
	} else {
		$sid=$db->Record["cat_id"];
	}
	$t=explode('|||',$tmp[1]);
	foreach($t AS $ts) import_part($ts,$sid);
}

function import_part($id,$cat=0,$tmp='',$module=0,$update_part=0){
	global $db,$todo_part,$todo_part2,$todo_part2b,$todo_param_link, $part_group_link;
	global $loaded_parts;
	if(!empty($id)){
		if(!empty($loaded_parts[$id])) return false;
		$loaded_parts[$id]=1;
	}
	if(empty($tmp)) $tmp=loadserv($GLOBALS["update_server"].'?type=part&id='.$id);
	$tmp=str_replace("\\","\\\\",$tmp);
	$tmp=str_replace("'","''",$tmp);
	$s='|/*/|';
	$s2='/@/?/@/';
	$s3='/#/?/#/';	
	$tmp=explode($s2,$tmp);
	$t=explode($s,$tmp[0]);
	//for($i=0;$i<count($t);$i++) $t[$i]=str_replace("'","''",$t[$i]);
	if(empty($t[3])) $t[3]='0';	
	if(empty($t[5])) $t[5]='0';
	if(empty($t[6])) $t[6]='0';
	if(empty($t[7])) $t[7]='0';
	if(empty($t[9])) $t[9]='0';
	if(empty($t[10])) $t[10]='0';
	if(empty($t[11])) $t[11]='0';
	if(!empty($t[11])) $part_tableB=$t[11];
	$t[11]=0;
	if(empty($t[12])) $t[12]='';
	if(empty($t[13])) $t[13]='0';
	if(empty($t[14])) $t[14]='0';
	if(empty($t[15])) $t[15]='0';
	if(empty($t[16])) $t[16]='0';	
	if(empty($t[30])) $t[30]='0';
	if(empty($t[29])) $t[29]='0';
	if(empty($t[28])) $t[28]='0';
	if(empty($t[25])) $t[25]='0';
	if(empty($t[24])) $t[24]='0';
	if(empty($t[31])) $t[31]='0';
	if(empty($t[33])) $t[33]='0';
	if(empty($t[35])) $t[35]='0';
	if(empty($t[37])) $t[37]='';
	if(empty($t[39])) $t[39]='0';
	if(empty($t[40])) $t[40]='';	
	if(empty($t[41])) $t[41]='';
	if(empty($t[42])) $t[42]='';
	if(empty($t[43])) $t[43]='';
	$part_folder=$t[34];
	if(!empty($module)) $t[5]=$module;
	if($module==0 && empty($update_part)){
		getrow($db,"SELECT part_sname, part_proc, part_cat, part_id FROM main_part WHERE part_sname='$t[1]' AND part_type=2 AND part_proc=$t[16]");
		if(!empty($db->Record)) return $db->Record["part_id"];
	}
	if($cat==0 && $module==0 && strstr($t[5],'^-!-?')){
		$tx=explode('^-!-?',$t[5]);
		// $tx[0] - part_proc
		// $tx[1] - cat_name
		// $tx[2] - cat_uin
		// $tx[3] - cat_pre
		// $tx[4] - cat_after
		getrow($db,"SELECT * FROM part_cat WHERE cat_type=".$tx[0]." AND (cat_uin='".$tx[2]."' OR cat_name='".$tx[1]."')");
		if(!empty($db->Record)) $cat=$db->Record["cat_id"];
		else {
			// создаём новый раздел
			$db->query("INSERT INTO part_cat (cat_type, cat_name, cat_uin, cat_owner, cat_pre, cat_after)
				VALUES ($tx[0], '$tx[1]', '$tx[2]', 0, '$tx[3]', '$tx[4]')",3,"part_cat");
			getrow($db,"SELECT LAST_INSERT_ID() as sid");
			$cat=$db->Record["sid"];
		}
		$t[5]=0;
	} else if(strstr($t[5],'^')){
		$t[5]=explode('^',$t[5]);
		$t[5]=$t[5][0];
	}
	if(!empty($cat)) $t[5]=0;
	if($t[38]!=0 && $t[38]!=-1 || strlen($t[38])>4){		
		$link_to_group=$t[38];
		$t[38]=0;
	} else $link_to_group=0;
	if(empty($update_part)){
		if(empty($t[0]) && empty($t[1]) && empty($t[2])) return false;
		$db->query("INSERT INTO main_part (part_cat, part_name, part_sname, part_body, part_parse, part_type, part_module, part_ex, part_file, part_access, part_table, part_url, part_iowner, part_ignore, part_major, part_proc, timer_last, timer_act, timer_type, timer_date, timer_time, timer_x, timer_y, timer_count, part_sowner, part_about,  part_folder, part_404, part_uin, part_pic, part_auth, part_owner, part_unsafe, part_ifcase, part_ifdetect, part_ifrow, part_date, part_date2)
				VALUES ($cat, '$t[0]', '$t[1]', '$t[2]', $t[3], $t[4], $t[5], $t[7], '$t[8]', $t[10], $t[11], '$t[12]', $t[13], $t[14], $t[15], $t[16], '$t[23]', $t[24], $t[25], '$t[26]', '$t[27]', $t[28], $t[29], $t[30], $t[31], '$t[32]', '$t[34]', $t[35], '$t[36]', '$t[37]',$t[38], 0, $t[39],'$t[40]','$t[41]','$t[42]','$t[43]','0000-00-00 00:00:00')",3,'main_part');
		getrow($db,"SELECT LAST_INSERT_ID() as sid");
		$sid=$db->Record["sid"];
	} else {
		$db->query("UPDATE main_part SET
			part_name='$t[0]',
			part_body='$t[2]',
			part_parse=$t[3],
			part_type=$t[4],
			part_module=$t[5],
			part_ex=$t[7],
			part_file='$t[8]',
			part_access=$t[10],
			part_table=$t[11],
			part_url='$t[12]',
			part_iowner=$t[13],
			part_ignore=$t[14],
			part_major=$t[15],
			part_proc=$t[16],			
			timer_last='$t[23]',
			timer_act=$t[24],
			timer_type=$t[25],
			timer_date='$t[26]',
			timer_time='$t[27]',
			timer_x=$t[28],
			timer_y=$t[29],
			timer_count=$t[30],
			part_sowner=$t[31],
			part_about='$t[32]',
			part_folder='$t[34]',
			part_404=$t[35],
			part_uin='$t[36]',
			part_pic='$t[37]',
			part_auth=$t[38],
			part_owner=0,
			part_unsafe=$t[39],
			part_ifcase='$t[40]',
			part_ifdetect='$t[41]',
			part_ifrow='$t[42]',
			part_date='$t[43]',
			part_date2='$t[43]'
		WHERE part_id=$update_part",3,"main_part");
		$sid=$update_part;			
	}
	//убрал PART_OWNER, т.к. нет смысла - там он уже другой
	//PART_PROC публиковалось дважды по этому я убрал $t[6]
	if(!empty($link_to_group)){
		$part_group_link[$sid]=$link_to_group;
	}
	$major=$t[15];
	if(!empty($t[33])) $todo_part2[$sid]=$t[33];//owner части
	$todo_part2b[$t[36]]=$sid;//было t[1] - sname, стало uin
	if(!empty($part_tableB)) $todo_part[$sid]=$part_tableB;
	if(!empty($update_part)){
		$update_params=getall4($db,"SELECT * FROM part_param WHERE param_part=$update_part","param_id");
		$update_params2=getall6($db,"SELECT * FROM part_param WHERE param_part=$update_part","param_uin","param_id");		
		$safe_param=Array();
	}
	if(!empty($tmp[1])){$t=explode($s3,$tmp[1]);foreach($t AS $ts){
		$g=explode($s,$ts);
		if(empty($g[8])) $g[8]=0;
		if(empty($g[9])) $g[9]=0;
		if(!empty($update_part)) $update_param=getrowval("SELECT * FROM part_param WHERE param_part=$update_part AND param_uin='$g[10]' AND param_uin!=''","param_id");
		if(!empty($update_part) && !empty($update_param)){
			$sid2=$update_param;
			$safe_param[$sid2]=$sid2;
			$db->query("UPDATE part_param SET
							param_name='$g[1]',
							param_sname='$g[2]',
							param_default='$g[3]',
							param_array=$g[4],
							param_get=$g[5],
							param_type=$g[6],
							param_list='$g[7]',
							param_connect=$g[9],
							param_hide=$g[11]
					      WHERE param_id=$sid2",3,"part_param");
		} else {
			$db->query("INSERT INTO part_param (param_part, param_name, param_sname, param_default, param_array, param_get, param_type, param_list, param_connect, param_uin, param_hide)
					VALUES ($sid, '$g[1]','$g[2]','$g[3]',$g[4],$g[5],$g[6],'$g[7]',$g[9],'$g[10]',$g[11])",3,'part_param');
			getrow($db,"SELECT LAST_INSERT_ID() as sid");
			$sid2=$db->Record["sid"];
			$safe_param[$sid2]=$sid2;
		}
		if(!empty($g[8])){
			$todo_param_link[$sid2]=$g[8];
		}
		if($g[0]==$major && !empty($sid2)){
			$db->query("UPDATE main_part SET part_major=$sid2 WHERE part_id=$sid");
		}
	}}
	// Удаляем ненужные параметры части
	if(!empty($update_part) && !empty($safe_param)){			
		$db->query("DELETE FROM part_param WHERE param_part=$update_part AND NOT (param_id IN (".implode(',',$safe_param)."))",3,"part_param");
	}
	if(!empty($part_folder)){
		//remote
		$tmp=loadserv($GLOBALS["update_server"].'?type=folder&folder='.$part_folder);
		//parsing
		ungzip_folders($tmp,DOCUMENT_ROOT);
	}
	reset_components_json();
	return $sid;
}

function update_file($var){
	global $no_update;
	if(isset($no_update[$var])){
		$tmp=loadserv($GLOBALS["update_server"].'?ttf='.urlencode($var));
		if(!empty($tmp)){
			check_dir('core/safe');
			$f=fopen(DOCUMENT_ROOT.'/core/safe/'.md5($var),'w');
			fwrite($f,$tmp);
			fclose($f);
			@chmod(DOCUMENT_ROOT.'/core/safe/'.md5($var),DEF_CHMOD);
		}
		return false;
	}
	$tmp=loadserv($GLOBALS["update_server"].'?ttf='.urlencode($var));
	if(!empty($tmp)){
		check_dir(dirname($var));
		if(file_exists(DOCUMENT_ROOT.'/'.$var)) @chmod(DOCUMENT_ROOT.'/'.$var,DEF_CHMOD);
		@$f=fopen(DOCUMENT_ROOT.'/'.$var,'w');
		@fwrite($f,$tmp);
		@fclose($f);
		@chmod(DOCUMENT_ROOT.'/'.$var,DEF_CHMOD);
	}
}

function check_ident($x,$y,$type='table'){
	$except='';
	if($type=='col'){
		//$except='col_date,col_date2,col_pos,col_filter,col_cat,col_part,col_paramlink,col_module,col_table';
		$except='col_date,col_date2,col_pos,col_filter,col_part,col_paramlink,col_module,col_table';
	}
	if($type=='table'){
		$except='table_date,table_date2,table_extype,table_bold,table_module,table_counter,table_icon';//,major_col';			
	}
	if($type=='part'){
		$except='part_date,part_date2,part_module,part_ex,part_cur,part_cat,parser_end_ex,part_shell,timer_last,timer_count';
	}
	if($type=='param'){
		$except='param_part,param_connect';
	}
	$except=explode(',',$except);
	$except[]='_id';
	foreach($x AS $var=>$value){
		$bool=true;
		foreach($except AS $exc) if(strstr($var,$exc)){
			$bool=false;
			break;
		}
		if(!$bool) continue;
		if(!isset($y[$var]) || $x[$var]!=$y[$var]){
			if(empty($x[$var]) && empty($y[$var])) continue;
			return false;
		}
	}
	return true;
}

function do_ident($src,$dest=0,$action='edit',$type='module',$owner_module=0,$owner=0){
	//для вставки таблицы используется owner_module
	//для вставки столбца используется owner_module для модуля и owner для таблицы
	global $var_cache,$db;
	$except='';
	$add1='';
	$add2='';
	if($type=='module'){
		$except='module_date,module_date2,module_lastcheck,module_sname';
		$table="main_module";
		//$wich="module_id=$owner_module";
		$wich="module_id=$dest";
		//$add1="module_id";
		//$add2=$owner_module;
	}
	if($type=='table'){
		$except='table_module,table_counter,table_date2';
		//$except.=',table_major';
		$table="main_table";
		//$wich="table_module=$owner_module";
		$wich="table_id=$dest";
		$add1="table_module";
		$add2=$owner_module;
	}
	if($type=='col'){
		$except='col_part,col_module,col_table,col_date2';
		//$except.=',col_paramlink';
		$table="main_col";
		//$wich="col_module=$owner_module AND col_table=$owner";
		$wich="col_id=$dest";
		$add1="col_module,col_table";
		$add2=$owner_module.','.$owner;
	}
	if($type=='part'){
		$table='main_part';
		$except='part_date,part_date2,part_module,part_ex,part_cur,part_cat,parser_end_ex,part_shell,timer_last,timer_count';
		//$except.=',part_owner,part_table,part_major';
		//$wich="part_module=$owner_module";
		$wich="part_id=$dest";
		$add1="part_module";
		$add2=$owner_module;
	}
	if($type=='param'){
		$table='part_param';
		$except='param_part,param_connect';
		//$except.=',param_link';
		//$wich="param_part=$owner";
		$wich="param_id=$dest";
		$add1="param_part";
		$add2=$owner;
	}
	if($type=='group'){
		$table='main_auth';
		$except='auth_owner,user_login,user_pwl,user_fixedip,user_email,user_pwlcode,user_name,user_lastlogin,user_row,user_safe,group_owner,group_module';
		$wich="auth_id=$dest";
		$add1="group_module";
		$add2=$owner_module;
	}
	$except=explode(',',$except);
	$except[]='_id';
	$except_only_for_update=Array('_uin');
	
	if(empty($var_cache[$table])){
		$cols=getall($db,"SHOW COLUMNS FROM ".$table);
		if(!empty($cols)) foreach($cols AS $col){
			$nn='';
			foreach($col AS $var=>$value){
				$is_text=false; $is_date=false;
				if($var=='Field') $nn=$value;
				if($var=='Type' && $value=='text') $is_text=true;
				if($var=='Type' && $value=='longtext') $is_text=true;
				if($var=='Type' && $value=='tinytext') $is_text=true;
				if($var=='Type' && $value=='date') $is_date=true;
				if($var=='Type' && $value=='time') $is_date=true;
				if($var=='Type' && $value=='datetime') $is_date=true;
				if($is_text || $is_date) $var_cache[$table][$nn]=1;
			}
		}
	}
	
	$edit1='';
	if(!empty($src) && is_array($src)) foreach($src AS $var=>$value){
		$bool=true;
		foreach($except AS $exc) if(strstr($var,$exc)){$bool=false; break;}
		if($action=='edit' && !empty($except_only_for_update)) foreach($except_only_for_update AS $exc) if(strstr($var,$exc)){$bool=false; break;}
		if(!$bool) continue;
				
		if(!empty($var_cache[$table][$var])){
			$value=str_replace("'","''",$value);
			$value=str_replace("\\","\\\\",$value);
			$value="'".$value."'";
		} else if(empty($value)) $value='0';
		
		
		if(!empty($edit1)) $edit1.=',';
		$edit1.=$var.'='.$value;
		if(!empty($add1)) $add1.=',';
		$add1.=$var;
		if(!empty($add2)) $add2.=',';
		$add2.=$value;
		/*if(!empty($var_cache[$table][$var])){
			if(!empty($add2)) $add2.=',';
			$add2.="'".$value."'";			
		} else {
			if(!empty($add2)) $add2.=',';
			$add2.=$value;
		}*/
	}
	if($action=='edit' && !empty($edit1)){
		$sql='UPDATE '.$table.' SET '.$edit1.' WHERE '.$wich;
		$db->query($sql);
		return $dest;
	}
	if($action=='add'){
		$sql='INSERT INTO '.$table.' ('.$add1.') VALUES ('.$add2.')';
		$db->query($sql);
		getrow($db,"SELECT LAST_INSERT_ID() as sid");
		$sid=$db->Record["sid"];
		return $sid;
	}
	return $dest;
}

function save_version(){
	global $version;
	$v=fopen(DOCUMENT_ROOT.'/core/version.inc','w+');
	fwrite($v,"<?php
	\$version=$version;
?>");
	fclose($v);
}

function load_version(){
	global $version;
	if(file_exists(DOCUMENT_ROOT.'/core/version.inc')){
		include(DOCUMENT_ROOT.'/core/version.inc');
	} else {
		global $version;
		$version=1;
		save_version();
	}
}

function update_version(){
	global $version, $version_action;
	if(!file_exists(DOCUMENT_ROOT.'/core/update/migrations.inc')) $version_action=Array(); else include_once('migrations.inc');
	if(empty($version)) load_version();
	$old_version=$version;
	while(isset($version_action[$version])){
		$x=&$version_action[$version];
		$x();
		$version++;
		save_version();
	}
}

function check_update_requirement(){
	global $version, $version_action;
	if(!file_exists(DOCUMENT_ROOT.'/core/update/migrations.inc')) $version_action=Array(); else include_once('migrations.inc');
	if(empty($version)) load_version();
	return isset($version_action[$version]);
}

function get_update_version(){
	global $version_action;
	if(!file_exists(DOCUMENT_ROOT.'/core/update/migrations.inc')) $version_action=Array(); else include_once('migrations.inc');
	end($version_action);
	return key($version_action)+1;
}

?>