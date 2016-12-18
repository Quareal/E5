<?php


$cmd['date'][CMD_GLOBAL]->process=function(&$data,&$op,&$ops){
		return date('Y-m-d');
};
$cmd['date'][CMD_GLOBAL]->result=CMD_STRING;
$cmd['date'][CMD_GLOBAL]->visual=Array('title'=>'дата','result'=>STRING_DATE);


$cmd['time'][CMD_GLOBAL]->process=function(&$data,&$op,&$ops){
		return date('H:i');
};
$cmd['time'][CMD_GLOBAL]->result=CMD_STRING;
$cmd['time'][CMD_GLOBAL]->visual=Array('title'=>'время','result'=>STRING_TIME);


$cmd['datetime'][CMD_GLOBAL]->process=function(&$data,&$op,&$ops){
		return date('Y-m-d H:i');
};
$cmd['datetime'][CMD_GLOBAL]->result=CMD_STRING;
$cmd['datetime'][CMD_GLOBAL]->visual=Array('title'=>'дата время','result'=>STRING_DATETIME);


$cmd['rand'][CMD_GLOBAL]->process=function(&$data,&$op,&$ops){
		check_single($op->put,$GLOBALS["lex_func_sep"]);
		return rand(parse_var($op->put[0]),parse_var($op->put[1]));
};
$cmd['rand'][CMD_GLOBAL]->result=CMD_STRING;
$cmd['rand'][CMD_GLOBAL]->visual=Array('title'=>'случайное число','result'=>STRING_NUM,'put'=>Array(
		0=>Array('title'=>'от','type'=>CMD_STRING,'req'=>1),
		1=>Array('title'=>'до','type'=>CMD_STRING,'req'=>1)
));


$cmd['fdate'][CMD_GLOBAL]->process=function(&$data,&$op,&$ops){
		return date(parse_var($op->put));
};
$cmd['fdate'][CMD_GLOBAL]->result=CMD_STRING;
$cmd['fdate'][CMD_GLOBAL]->visual=Array('title'=>'форматировать дату','put'=>Array(
		0=>Array('title'=>'формат','type'=>CMD_STRING,'req'=>1)
));


$cmd['modules'][CMD_GLOBAL]->process=function(&$data,&$op,&$ops){
		global $db;
		$mds=getall($db,'SELECT * FROM main_module',1,'main_module',__LINE__,__FILE__);
		$res=Array();$cr=0;
		foreach($mds AS $md)/*if(check_mod($md['module_id'],'view'))*/{
			$res[$cr]->id=$md['module_id'];
			$res[$cr]->name=$md['module_name'];
			$res[$cr]->sname=$md['module_sname'];
			$res[$cr]->type='md';
			$cr++;
		}
		return $res;
};
$cmd['modules'][CMD_GLOBAL]->result=CMD_ARRAY;
$cmd['modules'][CMD_GLOBAL]->result_long=CMD_MODULE;
$cmd['modules'][CMD_GLOBAL]->visual=Array('title'=>'модули');


$cmd[''][CMD_GLOBAL]->process=function(&$data,&$op,&$ops){
		global $g_par,$cmglob,$db;
		if(isset($g_par[$op->data])){
			$v=$g_par[$op->data];
			return $v;
		} else {
			if(!isset($cmglob[$op->data])){
				getrow($db,"SELECT module_id,module_sname FROM main_module WHERE module_sname='".$op->data."'",1,'main_module',__LINE__,__FILE__);
				$cmglob[$op->data]=$db->Record;
			} else $db->Record=$cmglob[$op->data];
			if(!empty($db->Record)){
				return prepend_type($db->Record['module_id'],'md');
			}
		}
		return false;
};
$cmd[''][CMD_GLOBAL]->result=CMD_MIXED;
$cmd[''][CMD_GLOBAL]->visual=Array('title'=>'глобальные переменные','special'=>SP_POINT,'put'=>Array(
	0=>Array('title'=>'Название переменной','type'=>STRING_PARAM_NAME,'req'=>1)
));


?>