<?php

function backup_globals($force_auth=false,$return_id=true){
	stf('backgup_globals');
	global $cur_row,$cur_ex,$cur_module,$current,$step,$pass,$break,$continue,$index,$count,$cur_table,$left_url,$module_url,$right_url,$for_min,$for_max,$url_row,$cur_part,$for_value,$cur_col,$cur_type,$up,$up2,$spec_step,$cur_var,$up_step,$is_404,$spec_step2,$last;	
	$res=Array();
	$res[0]=$cur_module;
	$res[1]=$cur_row;
	$res[2]=$cur_ex;
	$res[3]=$current;
	$res[4]=$step;
	$res[5]=$pass;
	$res[6]=$break;
	$res[7]=$continue;
	$res[8]=$index;
	$res[9]=$count;
	$res[10]=$cur_table;
	$res[11]=$left_url;
	$res[12]=$module_url;
	$res[13]=$right_url;
	$res[14]=$for_min;
	$res[15]=$for_max;
	$res[16]=$url_row;//?
	$res[17]=$cur_part;
	$res[18]=$for_value;
	$res[19]=$cur_col;
	$res[20]=$cur_type;
	$res[21]=$up2;
	$res[22]=$spec_step;
	$res[33]=$up_step;
	$res[34]=$up;
	if($force_auth){
		global $user,$mod_rules,$zone_rules,$table_rules,$ex_rules,$row_rules1,$row_rules2,$col_rules;
		global $xgcache, $xgcache2, $table_rules2, $table_rules3, $row_rules3;
		$res[23]=true;
		$res[24]=$user->id;
		$res[25]=$mod_rules;
		$res[26]=$zone_rules;
		$res[27]=$table_rules;
		$res[28]=$ex_rules;
		$res[29]=$row_rules1;
		$res[30]=$row_rules2;
		$res[47]=$row_rules3;
		$res[48]=$table_rules2;
		$res[49]=$table_rules3;
		$res[31]=$user->super;
		if(isset($user->group)) $res[35]=$user->group; else $res[35]=Array();
		$res[36]=$col_rules;
		/*unset($mod_rules);
		unset($zone_rules);
		unset($table_rules);
		unset($folder_rules);
		unset($ex_rules);
		unset($row_rules1);
		unset($row_rules2);
		unset($row_rules3);
		unset($col_rules);
		unset($user_rules);
		unset($group_rules);
		unset($xgcache);
		unset($xgcache2);		
		unset($user->group);*/
		flush_perm();
		if($force_auth=='super'){$user->super=1; $user->id=-1; if($res[24]!=0 && $return_id) $user->id=$res[24];}
		else if($force_auth=='guest'){$user->super=0; $user->id=0; load_base(0);  if($res[24]!=0 && $return_id) $user->id=$res[24];}
		else {
			$user->id=$force_auth;
			$user->super=0;
			load_base($user->id);			
			if($res[24]!=0 && $return_id) $user->id=$res[24];//возвращаем значения user->id
		}
	} else $res[23]=false;
	$res[32]=$cur_var;
	$res[38]=$is_404;
	$res[39]=$spec_step2;
	$res[40]=$last;
	/*global $lvl, $skip_enter, $skip_enter2, $skip_space, $skip_space2, $skip_dblspace;
	$res[41]=$skip_enter;
	$res[42]=$skip_enter2;
	$res[43]=$skip_space;
	$res[44]=$skip_space2;
	$res[45]=$skip_dblspace;
	$res[46]=$lvl;*/
	etf('backgup_globals');
	return $res;
}

function return_globals($res){
	global $cur_row,$cur_ex,$cur_module,$current,$step,$pass,$break,$index,$count,$cur_table,$left_url,$module_url,$right_url,$for_min,$for_max,$url_row,$cur_part,$for_value,$cur_col,$cur_type,$up,$up2,$spec_step,$cur_var,$up_step,$is_404,$spec_step2,$last;
	stf('return_globals');
	$cur_module=$res[0];
	$cur_row=$res[1];
	$cur_ex=$res[2];
	$current=$res[3];
	$step=$res[4];
	$pass=$res[5];
	$break=$res[6];
	$continue=$res[7];
	$index=$res[8];
	$count=$res[9];
	$cur_table=$res[10];
	$left_url=$res[11];
	$module_url=$res[12];
	$right_url=$res[13];
	$for_min=$res[14];
	$for_max=$res[15];
	//$url_row=$res[16];//?
	$cur_part=$res[17];
	$for_value=$res[18];
	$cur_col=$res[19];
	$cur_type=$res[20];
	$up2=$res[21];
	$spec_step=$res[22];	
	$up_step=$res[33];
	$up=$res[34];
	if($res[23]){
		global $user,$mod_rules,$zone_rules,$table_rules,$ex_rules,$row_rules1,$row_rules2,$col_rules;
		global $table_rules2, $table_rules3, $row_rules3;
		$user->id=$res[24];
		$mod_rules=$res[25];
		$zone_rules=$res[26];
		$table_rules=$res[27];
		$ex_rules=$res[28];
		$row_rules1=$res[29];
		$row_rules2=$res[30];
		$user->super=$res[31];
		$user->group=$res[33];
		$user->group=$res[35];
		$col_rules=$res[36];
		$row_rules3=$res[47];
		$table_rules2=$res[48];
		$table_rules3=$res[49];
	}
	$cur_var=$res[32];
	$is_404=$res[38];//геде 37ая? забыли
	$spec_step2=$res[39];
	$last=$res[40];
	
	/*global $lvl, $skip_enter, $skip_enter2, $skip_space, $skip_space2, $skip_dblspace;
	$skip_enter=$res[41];
	$skip_enter2=$res[42];
	$skip_space=$res[43];
	$skip_space2=$res[44];
	$skip_dblspace=$res[45];
	$lvl=$res[46];*/
	
	etf('return_globals');
}

function parse_args($text){
	$res->name='';
	$res->param=Array();
	$res->arg='';
	$res->left=Array();
	if(is_array($text)){
		$res->left=del1($text);
		$text=$text[0];
	}
	if(strposA($text,'.',0,'(',')','{','}',1)){
		$tmp=explodeA('.',$text,'(',')',0,'{','}',1);
		$text=$tmp[0];
		$res->left=del1($tmp);
	}
	if(strpos($text,'(')){
		$v=explodeA('(',substr($text,0,strlen($text)-1),'[',']',1,'{','}',1);
		$res->name=$v[0];
		if(strposA($v[1],', ',0,'(',')','{','}',1)){
			$res->param=explodeA(', ',$v[1],'(',')',0,'{','}',true);
		} else if(strposA($v[1],',',0,'(',')','{','}',1)){
			$res->param=explodeA(',',$v[1],'(',')',0,'{','}',true);
		} else if(strposA($v[1],' ',0,'(',')','{','}',1)){
			$res->param=explodeA(' ',$v[1],'(',')',0,'{','}',true);
		} else {
			$res->param[0]=$v[1];
		}
	} else {
		$res->name=$text;
	}
	if(!empty($res->param[0])) $res->arg=$res->param[0];
	return $res;
}

function parse_param($text,$separator=''){
	$res->arg='';
	$res->param=Array();
	if(strposA($text,', ',0,'(',')','{','}',1) && ($separator=='' || $separator==', ')){
		$res->param=explodeA(', ',$text,'(',')',0,'{','}',true);
	} else if(strposA($text,',',0,'(',')','{','}',1) && ($separator=='' || $separator==',')){
		$res->param=explodeA(',',$text,'(',')',0,'{','}',true);			
	} else if(strposA($text,' ',0,'(',')','{','}',1) && ($separator=='' || $separator==' ')){
		$res->param=explodeA(' ',$text,'(',')',0,'{','}',true);			
	} else {
		$res->param[0]=$text;
	}
	if(!empty($res->param[0])) $res->arg=$res->param[0];
	return $res;
}

function parse_output($res,$var,$type=''){
	if(empty($var)) return $res;
	if(is_object($res) && isset($res->type) && empty($type)) $type=$res->type;
	if($type=='user') return parse_user($res,$var);
	if($type=='ex') return parse_ex($res,$var);
	if($type=='row') return parse_row($res,$var);
	if($type=='group') return parse_group($res,$var);
	if($type=='module') return parse_module($res,$var);
	if($type=='table') return parse_table($res,$var);		
	if($type=='str') return cmd_str($res,$var);
	if($type=='arr') return cmd_arr($res,$var);
	if(is_array($res)) return cmd_arr($res);
	if(is_string($res) || is_numeric($res)) return cmd_str($res,$var);
	if(is_object($res) && isset($res->rows)) return cmd_arr($res,$var);
	return $res;
}

function check_table($sname,$module=0){
	global $db,$ptex;
	stf('check_table');
	if(!isset($ptex[$sname][$module])){
		$add='';
		if(!empty($module)) $add=' AND table_module='.$module;
		global $tables2;
		prep_tables();
		if(empty($module)){
			if(!empty($tables2)) foreach($tables2 AS $m=>$val)if(!empty($val)) foreach($val AS $sn=>$val2) if($sn==$sname) {$nt=$val2; break;}
		} else {
			if(isset($tables2[$module][$sname])) $nt=$tables2[$module][$sname];
		}
		/*if(!isset($nt)){//ибо нефиг
			echo $sname.'<br>';
			getrow($db,"SELECT table_sname, table_id, table_module, table_name, major_col FROM main_table WHERE table_sname='".$sname."'".$add,1,'main_table',__LINE__,__FILE__);
			if(!empty($db->Record)){
				db_to_table($db->Record);
				if($module!=0) $nt=$tables2[$module][$sname]; else $nt=$tables2[$db->Record["table_module"]][$sname];
			}
		}*/
		if(!empty($nt)) {
			$ptex[$sname][$module]=$nt->id;
			etf('check_table');
			return $nt->id;
		} else {
			$ptex[$sname][$module]=0;
			etf('check_table');
			return 0;
		}
	} else {
		etf('check_table');
		return $ptex[$sname][$module];
	}
	etf('check_table');
}

function check_table2($table,$var){
	global $db,$ptex2;
	stf('check_table2');
	/*if(!isset($ptex2[$table])){
		$tmp=getall($db,"SELECT t1.table_sname,t1.table_id,t2.sub_table2,t2.sub_table1 FROM main_table AS t1 LEFT JOIN table_sub AS t2 ON (t1.table_id=t2.sub_table2 AND t2.sub_table1=$table)",2,'',__LINE__,__FILE__);
		if(!empty($tmp)) foreach($tmp AS $tm) $ptex2[$tm["sub_table1"]][$tm["table_sname"]]=$tm["table_id"];
		if(!isset($ptex2[$table])) $ptex2[$table]=Array();
	}*/
	//верхний вариант исключён, т.к. при каждом вызове checktable2 происходят аналогичные запросы к базе данным (точнее возвращаются аналогичные ответы)
	if(empty($ptex2)){
		$tmp=getall($db,"SELECT t1.table_sname,t1.table_id,t2.sub_table2,t2.sub_table1 FROM main_table AS t1 LEFT JOIN table_sub AS t2 ON (t1.table_id=t2.sub_table2)",2,'',__LINE__,__FILE__);
		foreach($tmp AS $tm){
			$ptex2[$tm["sub_table1"]][$tm["table_sname"]]=$tm["table_id"];
		}
	}
	etf('check_table2');
	if(!empty($ptex2[$table][$var])) return $ptex2[$table][$var]; else return 0;
}

function preparse($var){
	if(empty($var)) return ''; else for($i=0;$i<count($var);$i++)if(!empty($var[$i])){
		if($var[$i]!='' && $var[$i][0]=='{') $var[$i]=parse_var($var[$i]);
	}
	return $var;
}

function get_ival($dst_col_val,$dst_col_sname,$col_id,$module_ex,$table_id){//table_id лишняя
	global $db;
	stf('get_ival');
	$tmp=$dst_col_val;
	if(!is_array($dst_col_val)){
		$tmp2=$dst_col_val;
		$dst_col_val=Array();
		$dst_col_val[$tmp2]=1;
	}
	$res=Array();
	if(strstr5($col_id,'col')) $col_id=str_replace('col','',$col_id);
	getrow($db,"SELECT * FROM main_col WHERE col_id=$col_id",1,"main_col",__LINE__,__FILE__);
	$table=$db->Record["col_link"];
	getrow($db,"SELECT * FROM main_col WHERE col_table=$table AND col_sname='$dst_col_sname'",1,"main_col",__LINE__,__FILE__);
	$col=$db->Record["col_id"];
	$ccol=$db->Record;
	foreach($dst_col_val AS $dst_col_val2=>$none){
		$a=getall($db,"SELECT * FROM row_value WHERE value_table=$table AND value_col=$col AND value_value='$dst_col_val2'",1,"row_value",__LINE__,__FILE__);
		global $vcache;
		foreach($a AS $aa){
			$val=$aa;
			if(!empty($ccol["col_force_onshow"])) $val["value_value"]=force_value($val,$ccol);
			$vcache[$val["value_col"]][$val["value_row"]][$val["value_id"]]=$val;
			$res[$aa["value_row"]]=1;
		}
	}
	etf('get_ival');
	if(count($res)==1){foreach($res AS $var=>$value) return $var;}
	if(!is_array($tmp) && empty($res)) return 0;
	return $res;
}

function clear_rowval_cache($col,$row){
	//вместо unset-ов, тут по идее лучше присваивать новое значение, вне зависимости от того - было оно или нет
	if(isset($GLOBALS["vcache"][$col][$row])) unset($GLOBALS["vcache"][$col][$row]);
	if(isset($GLOBALS["vcache2"][$col][$row])) unset($GLOBALS["vcache2"][$col][$row]);
	if(isset($GLOBALS["rowval"][$row])){
		global $pccol,$rowval;
		if(isset($pccol[$col]) && isset($rowval[$row][$pccol[$col]["col_sname"]])) unset($rowval[$row][$pccol[$col]["col_sname"]]);
		else unset($rowval[$row]);
	}
	if(!empty($GLOBALS["prow"][$row])){
		global $pccol,$prow,$prow2;
		if(isset($pccol[$col]) && isset($prow[$row][$pccol[$col]["col_sname"]])) unset($prow[$row][$pccol[$col]["col_sname"]]);
		else unset($prow[$row]);
		if(isset($pccol[$col]) && isset($prow2[$row][$pccol[$col]["col_sname"]])) unset($prow2[$row][$pccol[$col]["col_sname"]]);
		else unset($prow2[$row]);
	}
	
	$GLOBALS["risk_cache"]=Array(); //если активируем эту запись, слетает генерация шаблона (непонятно почему), проверить можно на любом предмете в кабинете ученика на экстерн офисе
}

function seek_id($id,$arr,$col='',$cid=0){
	global $db, $rlink;
	if(is_object($arr) && !empty($arr->rows)) $arr=$arr->rows;
	if(empty($arr) || !is_array($arr)) return false;
	if(empty($cid) && !empty($col)){
		foreach($arr AS $a){
			if(is_object($a) && isset($a->id)) $id=$a->id;
			else $id=$a;
			seek_rlink($id);
			$cid=getrowval("SELECT col_id FROM main_col WHERE col_table=".$rlink[$id]->table." AND col_sname='".$col."'",'col_id');
			break;
		}
	}
	$ids=collect_rows($arr);
	if(empty($ids) && !empty($arr)){
		$ids=$arr;
		//$cid=0;
	}
	if(empty($ids)) return false;
	if(!empty($cid)){
		$ids=implode(',',$ids);
		$c=getrowval("SELECT count(value_id) AS c FROM row_value WHERE value_row IN (".$ids.") AND value_col=$cid AND value_value='".$id."'",'c');
		return ($c>0);
	} else {
		if(!empty($ids[$id])) return true;
		else {
			foreach($ids AS $val) if($val==$id) return true;
		}
		return false;
	}
}

function seek_id_old($id,$arr,$col='',$cid=0){	//col - col_sname , cid - col_id for cache
	global $db,$pccol2,$rlink,$rlink5,$prow,$prow2;
	if(is_object($arr) && !empty($arr->rows)) $arr=$arr->rows;
	if(empty($arr) || !is_array($arr)) return false;
	stf('seek_id');
	if(empty($col) && !empty($cid)){
		$col=getrowval("SELECT col_sname FROM main_col WHERE col_id=$cid","col_sname");
	}
	//echo $id.' - '.count($arr).' - '.$col;
	$bool=false;
	foreach($arr AS $var=>$value){
		if($col==''){
			if(isset($value->id)){
				if($value->id==$id){$bool=true; break;}
				if(!empty($value->sub) && seek_id($id,$value->sub,$col,$cid)){$bool=true; break;}
			} else {
				if($value==$id){$bool=true; break;}
			}
		} else {
			if(isset($value->id)) $rid=$value->id; else $rid=$value;
			if($cid==0){
				if(!empty($arr->table)){$table=$arr->table; $module=$arr->module;} else {
					if(empty($rlink[$rid]) && empty($rlink5[$rid])){
						getrow($db,"SELECT * FROM main_row WHERE row_id=".$rid,1,"main_row",__LINE__,__FILE__);
						$rlink5[$db->Record["row_id"]]=$db->Record;
					}
					if(!empty($rlink[$rid])){$table=$rlink[$rid]->table; $module=$rlink[$rid]->module;}
					if(empty($rlink[$rid]) && !empty($rlink5[$rid])){$table=$rlink5[$rid]["row_table"]; $module=$rlink5[$rid]["row_module"];}
				}
				if(empty($pccol2[$module][$table][$col])){
					getrow($db,"SELECT * FROM main_col WHERE col_module=$module AND col_table=$table AND col_sname='$col'",1,"main_col",__LINE__,__FILE__);
					$pccol2[$module][$table][$col]=$db->Record;
				}
				if(empty($pccol2[$module][$table][$col])) {etf('seek_id');return false;}
				$cid=$pccol2[$module][$table][$col]["col_id"];
			}
			if(empty($prow[$rid][$col])){
				if(!empty($rid)) $tmp=getall($db,"SELECT * FROM row_value WHERE value_row=$rid AND value_col=$cid",1,"row_value",__LINE__,__FILE__);
				if(!empty($tmp)){
					if(count($tmp)==1) $prow[$rid][$col]=$tmp[0]["value_value"];
					else if(!empty($tmp)) foreach($tmp AS $tm) $prow[$rid][$col][]=$tm["value_value"];
					//тут возможно понадобится обработчик force_value
					if(isset($pccol2[$tmp[0]["value_module"]][$tmp[0]["value_table"]][$col])) $prow2[$rid][$col]=$pccol2[$tmp[0]["value_module"]][$tmp[0]["value_table"]][$col];
				} else {$prow[$rid][$col]='';}
			}
			if(is_array($prow[$rid][$col])){
				foreach($prow[$rid][$col] AS $val) if($val==$id){$bool=true; break;}
			} else if($prow[$rid][$col]==$id){$bool=true;break;}
			if(isset($value->id)){
				if(!empty($value->sub) && seek_id($id,$value->sub,$col,$cid)){$bool=true; break;}
			}
		}
	}
	etf('seek_id');
	return $bool;
}

function start_part_c($part){
	global $utpl;
	$utpl["part"][$part]=1;
	$butpl=dc($utpl);
	$utpl["row"]=Array();
	$utpl["rows"]=Array();
	$utpl["part"]=Array();
	$utpl["part"][$part]=1;
	return $butpl;
}

function end_part_c($butpl){
	global $utpl;
	if(!empty($butpl["part"])) foreach($butpl["part"] AS $var=>$value) $utpl["part"][$var]=1;
	if(!empty($butpl["rows"])) foreach($butpl["rows"] AS $var=>$value) $utpl["rows"][$var]=1;
	if(!empty($butpl["row"])) foreach($butpl["row"] AS $var=>$value) $utpl["row"][$var]=1;
}

function concat_result($res,$res2,$del_clear=0){
	if((is_array($res2) || is_object($res2)) && !empty($GLOBALS["clear"])) $res=$res2;
	else if(is_object($res2) && isset($res2->id) && !empty($res)){
		$res2=$res2->id;
	}
	if(!is_array($res2) && !is_object($res2)){
		if(!empty($GLOBALS["clear"]) || empty($res)) $res=$res2; else {
			if(is_array($res)) $res=$res2;
			else $res.=$res2;
		}
	} else if(empty($res)) $res=$res2;
	if(!empty($GLOBALS["clear_once_at_start"])){
		$del_clear=true;
		$GLOBALS["clear_once_at_start"]=false;
	}
	if(!empty($GLOBALS["clear"]) && $del_clear) $GLOBALS["clear"]=false;
	if(!empty($GLOBALS["clear"]) && !$del_clear) $GLOBALS["reset_clear_on_exit"]=true;
	return $res;
}

function ex2m($ex){
	global $e2m;
	if(!isset($e2m[$ex])){
		if(!empty($ex)) $e2m[$ex]=getrowval("SELECT ex_id, ex_module FROM ex_module WHERE ex_id=$ex","ex_module");
		else $e2m[$ex]='';
	}
	return $e2m[$ex];
}

?>