<?php

function seek_module($module){
	global $db,$smodule;
	$ex=0;
	$part=0;
	$own=0;
	if(is_object($module) && isset($module->id) && !is_numeric($module->id)) $module=$module->id;
	if(!is_object($module) && !is_numeric($module) && is_string($module)){
		if(strstr5($module,'?')){
			$module=explode('?',$module);
			$own=$module[1];
			$module=$module[0];
		}
		if(strstr5($module,':')){
			$module=explode(':',$module);
			$ex=$module[1];
			if(!empty($module[2])) $part=$module[2];
			$module=$module[0];
		}
	}
	if(is_object($module) && isset($module->id)){
		if(isset($module->ex)) $ex=$module->ex;
		if(isset($module->part)) $part=$module->part;
		if(isset($module->own)) $own=$module->own;
		if(isset($module->owner)) $own=$module->owner;
		$module=$module->id;
	}
	if(!isset($smodule[$module])){
		getrow($db,"SELECT * FROM main_module WHERE module_id=$module",1,'main_module',__LINE__,__FILE__);
		$smodule[$module]=$db->Record;
	}
	return Array(	'module'=>$smodule[$module],
				'ex'=>$ex,
				'part'=>$part,
				'own'=>$own,
				'id'=>$module);
}


$cmd['id'][CMD_MODULE]->process=function(&$data,&$op,&$ops){
		$tm=seek_module($data); $ex=$tm['ex']; $module=$tm['id']; $m=$tm['module'];
		return $m["module_id"];
};
$cmd['id'][CMD_MODULE]->result=CMD_STRING;


$cmd['self'][CMD_MODULE]->process=function(&$data,&$op,&$ops){
		$tm=seek_module($data); $cex=$tm['ex']; $module=$tm['id']; $m=$tm['module'];
		$row2=$module.':'.$cex;
		$tmp=shell_module($row2);
		etf('parse_module');
		return $tmp;
};
$cmd['self'][CMD_MODULE]->result=CMD_MODULE;


$cmd['auto'][CMD_MODULE]->process=function(&$data,&$op,&$ops){
		$tm=seek_module($data); $cex=$tm['ex']; $module=$tm['id']; $m=$tm['module'];
		$row2=$module.':'.$cex;
		if(!empty($tm['own'])) $row2.='?'.$tm['own'];
		
		
		//$cache_conf=get_cache_conf();
		// prepend params if exists		
		if(!empty($op->put)){
			global $p_par;
			check_single($op->put,$GLOBALS["lex_func_sep"]);
			$par=prepend_param($op->put,0,0,false);
		}		
		
		if(!empty($tm['part'])) $tmp=shell_module($row2,$tm['part']);
		else $tmp=shell_module($row2);
		
		// clear params if declare
		if(!empty($op->put)){
			$p_par=$par;
		}
		
		/*set_cache_conf($cache_conf);	
		if(!empty($GLOBALS["xbreak"]) && !empty($GLOBALS["clear"]) && !$old_xbreak){
			//затычка для нормальной отработки return через shell
			$GLOBALS["xbreak"]="";
			$GLOBALS["clear"]="";
		}
		if(!empty($GLOBALS["xbreak"]) && !$old_xbreak){
			//затычка для нормальной отработки return через shell
			$GLOBALS["xbreak"]="";
		}*/
		
		return $tmp;
};
$cmd['auto'][CMD_MODULE]->result=CMD_MIXED;
$cmd['auto'][CMD_MODULE]->result_long=CMD_STRING;
$cmd['auto'][CMD_MODULE]->visual=Array('title'=>'Запустить модуль');


$cmd['sel'][CMD_MODULE]->process=function(&$data,&$op,&$ops){
		$tm=seek_module($data); $cex=$tm['ex']; $module=$tm['id']; $m=$tm['module'];
		if(!empty($cex)) $tcex=$cex; else {
			global $mexs;
			if(!empty($mexs[$module])) $tcex=$mexs[$module];
			else $tcex=$GLOBALS["cur_ex"];
		}
		return prepend_type($tcex,'ex');
};
$cmd['sel'][CMD_MODULE]->result=CMD_EX;
$cmd['sel'][CMD_MODULE]->visual=Array('title'=>'Текущий экземпляр');
$cmd['sel_ex'][CMD_MODULE]->alias=&$cmd['sel'][CMD_MODULE];


$cmd['groups'][CMD_MODULE]->process=function(&$data,&$op,&$ops){
		$tm=seek_module($data); $cex=$tm['ex']; $module=$tm['id']; $m=$tm['module'];
		global $db;
		$res=Array();
		$gs=getall($db,"SELECT * FROM main_auth WHERE auth_type=1 AND group_module=$m");
		if(!empty($gs)) foreach($gs AS $g){
			$res[$g['auth_id']]=prepend_type($g['auth_id'],'grp');
		}
		return $res;
};
$cmd['groups'][CMD_MODULE]->result=CMD_ARRAY;
$cmd['groups'][CMD_MODULE]->result_long=CMD_GROUP;
$cmd['groups'][CMD_MODULE]->visual=Array('title'=>'Группы пользователей');


$cmd['group'][CMD_MODULE]->process=function(&$data,&$op,&$ops){
		$tm=seek_module($data); $cex=$tm['ex']; $module=$tm['id']; $m=$tm['module'];
		global $db;
		$op=array_shiftA($ops);
		$name=$op->data;
		$group_id=getrowval("SELECT * FROM main_auth WHERE auth_type=1 AND group_sname='$name' AND group_module=$m","auth_id");
		return prepend_type($group_id,'grp');
};
$cmd['group'][CMD_MODULE]->result=CMD_GROUP;
$cmd['group'][CMD_BASE]->visual=Array('title'=>'Группа пользователей','put'=>Array(
		0=>Array('title'=>'группа','type'=>STRING_GROUP_NAME,'req'=>1)
),'special'=>SP_POINT);


$cmd['cow'][CMD_MODULE]->process=function(&$data,&$op,&$ops){
		$tm=seek_module($data); $cex=$tm['ex']; $module=$tm['id']; $m=$tm['module'];
		$op=get_first_op($ops);
		$var[1]=$op->data;
		$pname=$var[1];
		global $ppart;
		if(isset($GLOBALS["parts_cow"][$ppart[$module][$pname]->id])) return $GLOBALS["parts_cow"][$ppart[$module][$pname]->id];
		else return false;
};
$cmd['cow'][CMD_MODULE]->result=CMD_ROW;
$cmd['cow'][CMD_MODULE]->visual=Array('title'=>'Текущий объект','put'=>Array(
		0=>Array('title'=>'Часть','type'=>STRING_PART_NAME,'req'=>1)
),'special'=>SP_POINT);


$cmd['name'][CMD_MODULE]->process=function(&$data,&$op,&$ops){
		$tm=seek_module($data); $cex=$tm['ex']; $module=$tm['id']; $m=$tm['module'];
		return $m["module_name"];
};
$cmd['name'][CMD_MODULE]->result=CMD_STRING;
$cmd['name'][CMD_MODULE]->visual=Array('title'=>'Имя');


$cmd['sname'][CMD_MODULE]->process=function(&$data,&$op,&$ops){
		$tm=seek_module($data); $cex=$tm['ex']; $module=$tm['id']; $m=$tm['module'];
		return $m["module_sname"];
};
$cmd['sname'][CMD_MODULE]->result=CMD_STRING;
$cmd['sname'][CMD_MODULE]->visual=Array('title'=>'Спец.имя');


$cmd['major'][CMD_MODULE]->process=function(&$data,&$op,&$ops){
		$tm=seek_module($data); $cex=$tm['ex']; $module=$tm['id']; $m=$tm['module'];
		global $db;
		getrow($db,"SELECT * FROM main_table WHERE table_module=$module AND table_bold=1",1,"main_table",__LINE__,__FILE__);
		if(!empty($db->Record)) $ctable=$db->Record["table_id"];
		if(!empty($ctable)){
			if(empty($cex) && !empty($GLOBALS["cur_ex"])) $cex=$GLOBALS["cur_ex"];
			return prepend_type($ctable,'tbl',Array('ex'=>$cex));
		} else return false;
};
$cmd['major'][CMD_MODULE]->result=CMD_TABLE;
$cmd['major'][CMD_MODULE]->visual=Array('title'=>'Главная таблица');


$cmd['allexs'][CMD_MODULE]->process=function(&$data,&$op,&$ops){
		$tm=seek_module($data); $cex=$tm['ex']; $module=$tm['id']; $m=$tm['module'];
		global $db;
		$exs=getall($db,"SELECT * FROM ex_module WHERE ex_module=$module",1,"ex_module",__LINE__,__FILE__);
		$res=Array();$cr=0;
		foreach($exs AS $ex){
			$res[$cr]->id=$ex["ex_id"];
			$res[$cr]->name=$ex["ex_name"];
			$res[$cr]->sname=$ex["ex_sname"];
			$res[$cr]->type='ex';
			$cr++;
		}
		return $res;
};
$cmd['allexs'][CMD_MODULE]->result=CMD_ARRAY;
$cmd['allexs'][CMD_MODULE]->result_long=CMD_EX;
$cmd['allexs'][CMD_MODULE]->visual=Array('title'=>'Все экземпляры');


$cmd['tables'][CMD_MODULE]->process=function(&$data,&$op,&$ops){
		$tm=seek_module($data); $cex=$tm['ex']; $module=$tm['id']; $m=$tm['module'];
		global $tbl_cache,$db;
		if(!isset($tbl_cache[$module])){
			global $db;
			$tbls=getall($db,"SELECT * FROM main_table WHERE table_module=$module",1,"main_table",__LINE__,__FILE__);
			$tbl_cache[$module]=$tbls;
		} else $tbls=$tbl_cache[$module];
		$res=Array();$cr=0;
		foreach($tbls AS $tbl)if(check_tbl($tbl["table_id"],'view')){
			$res[$cr]->id=$tbl["table_id"];
			$res[$cr]->name=$tbl["table_name"];
			$res[$cr]->sname=$tbl["table_sname"];
			$res[$cr]->owner=0;
			$res[$cr]->tex=0;
			$res[$cr]->type='tbl';
			$cr++;
		}
		return $res;
};
$cmd['tables'][CMD_MODULE]->result=CMD_ARRAY;
$cmd['tables'][CMD_MODULE]->result_long=CMD_TABLE;
$cmd['tables'][CMD_MODULE]->visual=Array('title'=>'Таблицы');


$cmd['selpart'][CMD_MODULE]->process=function(&$data,&$op,&$ops){
		$tm=seek_module($data); $cex=$tm['ex']; $module=$tm['id']; $m=$tm['module'];
		global $ppart,$cmd;
		if(!isset($ppart[$module])) prepend_part($module);
		if(!empty($GLOBALS["mod_part"][$module])) $selp=$GLOBALS["mod_part"][$module];
		if(!empty($selp) && !empty($ppart[$module])) foreach($ppart[$module] AS $var2=>$value)if($value->id==$selp){
			$part_sname=$var2;
			if(isset($ops[key($ops)]) && $ops[key($ops)]->type==LEXER_CMD && !empty($ops[key($ops)]->data) && isset($cmd[$ops[key($ops)]->data][CMD_PART])){
				global $db,$part_id_cache;
				if(!isset($part_id_cache[$module][$part_sname])) $part_id_cache[$module][$part_sname]=getrowval("SELECT part_id, part_sname, part_module FROM main_part WHERE part_module=$module AND part_sname='".$part_sname."'","part_id");
				$part_id=$part_id_cache[$module][$part_sname];
				if(!empty($tm['own'])) return prepend_type($part_id,'prt',Array('module'=>$module, 'ex'=>$cex, 'own'=>$tm['own']));
				else return prepend_type($part_id,'prt',Array('module'=>$module, 'ex'=>$cex));
			} else {
				if(!empty($tm['own'])) $cex.='?'.$tm['own'];
				global $cmd;
				//$x=&$cmd['func'][CMD_BASE]->process;
				$x=get_cmd_link('func',CMD_BASE);
				return $x($data,$op,$ops,'',0,$module,$cex,$part_sname);
			}
		}
};
$cmd['selpart'][CMD_MODULE]->result=CMD_MIXED;
$cmd['selpart'][CMD_MODULE]->visual=Array('title'=>'Текущая часть','result'=>CMD_PART);


$cmd['part'][CMD_MODULE]->process=function(&$data,&$op,&$ops){
		$tm=seek_module($data); $cex=$tm['ex']; $module=$tm['id']; $m=$tm['module'];
		$ajax=0;
		if(shift_first_op($ops,'ajax')) $ajax=1;
		if(shift_first_op($ops,'ajax2')) $ajax=2;
		$part_sname=$ops[key($ops)]->data;
		$op=get_first_op($ops);
		global $cmd;
		if(isset($ops[key($ops)]) && $ops[key($ops)]->type==LEXER_CMD && !empty($ops[key($ops)]->data) && isset($cmd[$ops[key($ops)]->data][CMD_PART])){
			global $db;
			$part_id=getrowval("SELECT part_id, part_sname, part_module FROM main_part WHERE part_module=$module AND part_sname='".$part_sname."'","part_id");
			return prepend_type($part_id,'prt',Array('module'=>$module, 'ex'=>$cex));
		}
		if(!empty($tm['own'])) $cex.='?'.$tm['own'];
		global $cmd;
		//$x=&$cmd['func'][CMD_BASE]->process;
		$x=get_cmd_link('func',CMD_BASE);
		return $x($data,$op,$ops,'',$ajax,$module,$cex,$part_sname);
};
$cmd['part'][CMD_MODULE]->result=CMD_MIXED;
$cmd['part'][CMD_MODULE]->visual=Array('title'=>'части','special'=>SP_PART,'disable_insert'=>1,'pos'=>1);


$cmd['url_part'][CMD_MODULE]->process=function(&$data,&$op,&$ops){
		$tm=seek_module($data); $cex=$tm['ex']; $module=$tm['id']; $m=$tm['module'];
		$mdata=$module.':'.$cex;
		$part_sname=$ops[key($ops)]->data;
		$op=get_first_op($ops);
		if(getrowval("SELECT part_id FROM main_part WHERE part_module=$module AND part_sname='$part_sname'","part_id")){
			return shell_module($mdata,$part_sname,0,$op);
		} else return false;
};
$cmd['url_part'][CMD_MODULE]->result=CMD_MIXED;
$cmd['url_part'][CMD_MODULE]->visual=Array('title'=>'вызвать часть с определением объекта по URL','special'=>SP_POINT,'put'=>Array(
		0=>Array('title'=>'Часть','type'=>STRING_PART_NAME,'req'=>1)
));


$cmd[''][CMD_MODULE]->process=function(&$data,&$op,&$ops){
		$tm=seek_module($data); $cex=$tm['ex']; $module=$tm['id']; $m=$tm['module'];
		$var[0]=$op->data;
		$ctable=check_table($var[0],$module);
		if(!empty($ctable)){
			if(empty($cex) && !empty($GLOBALS["cur_ex"])) $cex=$GLOBALS["cur_ex"];
			return prepend_type($ctable,'tbl',Array('ex'=>$cex));
		} else {
			global $ex_caches;
			if(!isset($ex_caches[$module][$var[0]])){
				global $db;
				getrow($db,"SELECT ex_id,ex_sname,ex_module FROM ex_module WHERE ex_module=".$module." AND ex_sname='".$var[0]."'",1,'ex_module',__LINE__,__FILE__);
				$ex_caches[$module][$var[0]]=$db->Record;
			}
			$db->Record=$ex_caches[$module][$var[0]];
			if(!empty($db->Record)) return prepend_type($db->Record['ex_id'],'ex');
		}
};
$cmd[''][CMD_MODULE]->result=CMD_MIXED;
$cmd[''][CMD_MODULE]->visual=Array('title'=>'','special'=>SP_UNKNOWN,'pos'=>2);


?>