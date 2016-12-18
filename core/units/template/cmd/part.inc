<?php

function seek_part($part){
	global $spart,$db,$cur_module;
	$ex=0;
	$module=0;
	if(!is_object($part) && !is_numeric($part) && is_string($part)){
		if(strstr5($part,':')){
			$part=explode(':',$part);
			$module=$part[0];
			$ex=$part[1];
			$part=$part[2];
		} else {
			$part_id=getrowval("SELECT part_id FROM main_part WHERE part_module=$cur_module AND part_sname='".safe_sql_input($part)."'",'part_id');
			if(empty($part)){
				$part_id=getrowval("SELECT part_id FROM main_part WHERE part_sname='".safe_sql_input($part)."'",'part_id');
			}
			if(!empty($part_id)) $part=$part_id;
		}
	}	
	if(is_object($part) && isset($part->id)){
		if(isset($part->ex)) $ex=$part->ex;
		if(isset($part->module)) $module=$part->module;
		$part=$part->id;
	}
	if(empty($part)){
		return Array('part'=>Array(), 'ex'=>0, 'module'=>0, 'id'=>0);
	}
	if(!isset($spart[$part])){
		getrow($db,"SELECT * FROM main_part WHERE part_id=$part",1,'main_part',__LINE__,__FILE__);
		$spart[$part]=$db->Record;
	}
	return Array(	'part'=>$spart[$part],
				'ex'=>$ex,
				'module'=>$module,
				'id'=>$part);
}


$cmd['url'][CMD_PART]->process=function(&$data,&$op,&$ops){
		$d=seek_part($data); $ex=$d['ex']; $part=$d['id']; $p=$d['part'];
		return $p["part_url"];
};
$cmd['url'][CMD_PART]->result=CMD_STRING;
$cmd['url'][CMD_PART]->visual=Array('title'=>'url');


$cmd['id'][CMD_PART]->process=function(&$data,&$op,&$ops){
		$d=seek_part($data); $ex=$d['ex']; $part=$d['id']; $p=$d['part'];
		return $p["part_id"];
};
$cmd['id'][CMD_PART]->result=CMD_STRING;


$cmd['name'][CMD_PART]->process=function(&$data,&$op,&$ops){
		$d=seek_part($data); $ex=$d['ex']; $part=$d['id']; $p=$d['part'];
		return $p["part_name"];
};
$cmd['name'][CMD_PART]->result=CMD_STRING;
$cmd['name'][CMD_PART]->visual=Array('title'=>'имя');


$cmd['self'][CMD_PART]->process=function(&$data,&$op,&$ops){
		$d=seek_part($data); $ex=$d['ex']; $part=$d['id']; $p=$d['part'];
		return prepend_type($part,'prt',Array('ex'=>$ex,'module'=>$p["part_module"]));
};
$cmd['self'][CMD_PART]->result=CMD_PART;


$cmd['run'][CMD_PART]->process=function(&$data,&$op,&$ops){
		$d=seek_part($data); $ex=$d['ex']; $part=$d['id']; $p=$d['part'];
		global $cmd;
		//$x=&$cmd['func'][CMD_BASE]->process;
		$otype='';
		if($p['part_type']==2) switch($p['part_proc']){
			case(0): $otype='func';break;
			case(1): $otype='show';break;
			case(2): $otype='component';break;
			case(3): $otype='form';break;
		}
		if($p['part_type']!=2 && ($op->data=='url_run' || $op->data=='url_shell')){
			$mdata=$p['part_module'].':'.$ex;
			return shell_module($mdata,$p['part_sname'],0,$op);
		}
		$x=get_cmd_link('func',CMD_BASE);
		return $x($data,$op,$ops,$otype,0,$p["part_module"],$ex,$p["part_sname"],$p);
};
$cmd['run'][CMD_PART]->result=CMD_MIXED;
$cmd['run'][CMD_PART]->result_long=CMD_STRING;
$cmd['run'][CMD_PART]->visual=Array('title'=>'запуск','extend_put'=>1);
$cmd['shell'][CMD_PART]->alias=&$cmd['run'][CMD_PART];

$cmd['url_run'][CMD_PART]->visual=Array('title'=>'запуск (с URL разбором модуля)','extend_put'=>1);
$cmd['url_shell'][CMD_PART]->alias=&$cmd['run'][CMD_PART];
$cmd['url_run'][CMD_PART]->alias=&$cmd['run'][CMD_PART];


$cmd['ajax'][CMD_PART]->process=function(&$data,&$op,&$ops){
		$d=seek_part($data); $ex=$d['ex']; $part=$d['id']; $p=$d['part'];
		global $cmd;
		//$x=&$cmd['func'][CMD_BASE]->process;
		$x=get_cmd_link('func',CMD_BASE);
		$otype='';
		if($p['part_type']==2) switch($p['part_proc']){
			case(0): $otype='func';break;
			case(1): $otype='show';break;
			case(2): $otype='component';break;
			case(3): $otype='form';break;
		}
		return $x($data,$op,$ops,$otype,1,$p["part_module"],$ex,$p["part_sname"],$p);
};
$cmd['ajax'][CMD_PART]->result=CMD_MIXED;
$cmd['ajax'][CMD_PART]->visual=Array('title'=>'выдать url для ajax','special'=>SP_AJAX);


$cmd['ajax2'][CMD_PART]->process=function(&$data,&$op,&$ops){
		$d=seek_part($data); $ex=$d['ex']; $part=$d['id']; $p=$d['part'];
		global $cmd;
		//$x=&$cmd['func'][CMD_BASE]->process;
		$x=get_cmd_link('func',CMD_BASE);
		$otype='';
		if($p['part_type']==2) switch($p['part_proc']){
			case(0): $otype='func';break;
			case(1): $otype='show';break;
			case(2): $otype='component';break;
			case(3): $otype='form';break;
		}
		return $x($data,$op,$ops,$otype,2,$p["part_module"],$ex,$p["part_sname"],$p);
};
$cmd['ajax2'][CMD_PART]->result=CMD_MIXED;
$cmd['ajax_min'][CMD_PART]->alias=&$cmd['ajax'][CMD_PART];
$cmd['ajax2'][CMD_PART]->visual=Array('title'=>'выдать url для ajax с переменными окружения (длинный URL)','special'=>SP_AJAX);


$cmd['sname'][CMD_PART]->process=function(&$data,&$op,&$ops){
		$d=seek_part($data); $ex=$d['ex']; $part=$d['id']; $p=$d['part'];
		if(!empty($p["part_sname"])) return $p["part_sname"]; else return false;
};
$cmd['sname'][CMD_PART]->result=CMD_STRING;
$cmd['sname'][CMD_PART]->visual=Array('title'=>'спец.имя');


$cmd['cow'][CMD_PART]->process=function(&$data,&$op,&$ops){
		$d=seek_part($data); $ex=$d['ex']; $part=$d['id']; $p=$d['part'];
		if(isset($GLOBALS["parts_cow"][$part])) return $GLOBALS["parts_cow"][$part];
};
$cmd['cow'][CMD_PART]->result=CMD_ROW;
$cmd['cow'][CMD_PART]->visual=Array('title'=>'текущий объект');


$cmd['ex'][CMD_PART]->process=function(&$data,&$op,&$ops){
		$d=seek_part($data); $ex=$d['ex']; $part=$d['id']; $p=$d['part'];
		//if(!empty($d['module'])) return prepend_type($ex,'ex',Array('module'=>$d['module']));
		if(!empty($d['ex'])) return prepend_type($ex,'ex');
		else return false;
};
$cmd['ex'][CMD_PART]->result=CMD_EX;
$cmd['ex'][CMD_PART]->visual=Array('title'=>'экземпляр модуля','condition'=>IF_PART_OF_MODULE);


$cmd['module'][CMD_PART]->process=function(&$data,&$op,&$ops){
		$d=seek_part($data); $ex=$d['ex']; $part=$d['id']; $p=$d['part'];
		if(!empty($d['module'])) return prepend_type($d['module'],'md',Array('ex'=>$ex));
		else return false;
};
$cmd['module'][CMD_PART]->result=CMD_MODULE;
$cmd['module'][CMD_PART]->visual=Array('title'=>'модуль','condition'=>IF_PART_OF_MODULE);


$cmd['folder'][CMD_PART]->process=function(&$data,&$op,&$ops){
		$d=seek_part($data); $ex=$d['ex']; $part=$d['id']; $p=$d['part'];
		return $p["part_folder"];
};
$cmd['folder'][CMD_PART]->result=CMD_STRING;
$cmd['folder'][CMD_PART]->visual=Array('title'=>'Папка с файлами');


?>