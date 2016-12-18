<?php

function seek_zone($zone){
	global $db, $spz;
	if(empty($zone)) return false;
	if(is_object($zone) && isset($zone->id)) $zone=$zone->id;
	if(!isset($spz[$zone])) $spz[$zone]=getrow($db,"SELECT * FROM main_zone WHERE zone_id=$zone");
	return $spz[$zone];
}


$cmd['url'][CMD_ZONE]->process=function(&$data,&$op,&$ops){
		$z=seek_zone($data);
		return $z["zone_domain"];
};
$cmd['url'][CMD_ZONE]->result=CMD_STRING;
$cmd['url'][CMD_ZONE]->visual=Array('title'=>'Домен');


$cmd['name'][CMD_ZONE]->process=function(&$data,&$op,&$ops){
		$z=seek_zone($data);
		return $z["zone_name"];
};
$cmd['name'][CMD_ZONE]->result=CMD_STRING;
$cmd['name'][CMD_ZONE]->visual=Array('title'=>'Название');


$cmd['email'][CMD_ZONE]->process=function(&$data,&$op,&$ops){
		$z=seek_zone($data);
		return $z["zone_email"];
};
$cmd['email'][CMD_ZONE]->result=CMD_STRING;
$cmd['email'][CMD_ZONE]->visual=Array('title'=>'Email');


$cmd['module'][CMD_ZONE]->process=function(&$data,&$op,&$ops){
		$z=seek_zone($data);
		$zm=$z['zone_module'];
		$zm=explode(':',$zm);
		if(!empty($zm[1])){
			return prepend_type($zm[1],'ex');
		} else return false;
};
$cmd['module'][CMD_ZONE]->result=CMD_EX;
$cmd['module'][CMD_ZONE]->visual=Array('title'=>'Экземпляр основного модуля');


$cmd['folder'][CMD_ZONE]->process=function(&$data,&$op,&$ops){
		$z=seek_zone($data);
		return $z["zone_folder"];
};
$cmd['folder'][CMD_ZONE]->result=CMD_STRING;
$cmd['folder'][CMD_ZONE]->visual=Array('title'=>'Папка');


$cmd['type'][CMD_ZONE]->process=function(&$data,&$op,&$ops){
		$z=seek_zone($data);
		return $z["zone_module"];
};
$cmd['type'][CMD_ZONE]->result=CMD_STRING;
$cmd['type'][CMD_ZONE]->visual=Array('title'=>'Тип');


$cmd['url_folder'][CMD_ZONE]->process=function(&$data,&$op,&$ops){
		$z=seek_zone($data);
		$r=$z["zone_domain"];
		if(!empty($z["zone_folder"])) $r.='/'.$z["zone_folder"];
		return $r;
};
$cmd['url_folder'][CMD_ZONE]->result=CMD_STRING;
$cmd['url2'][CMD_ZONE]->alias=&$cmd['url_folder'][CMD_ZONE];
$cmd['url_folder'][CMD_ZONE]->visual=Array('title'=>'Домен с папкой');


$cmd[''][CMD_ZONE]->process=function(&$data,&$op,&$ops){
		global $db;
		$z=seek_zone($data);
		$zone=$z["zone_id"];
		$d=$op->data;
		if(!is_string($op->data)) return false;
		$mid=getrowval("SELECT module_sname,module_id FROM main_module WHERE module_sname='".$op->data."'","module_id");
		if(!empty($mid)){
			$emid=getrowval("SELECT * FROM ex_zone WHERE ex_zone=$zone AND ex_module2=$mid","ex_module");
			if(!empty($emid)){
				return prepend_type($emid,'ex');
			}
		}
		return false;
};
$cmd[''][CMD_ZONE]->result=CMD_EX;
$cmd[''][CMD_ZONE]->visual=Array('title'=>'Связанный экземпляр модуля','special'=>SP_POINT,'put'=>Array(
		0=>Array('title'=>'Модуль', 'type'=>STRING_MODULE_SNAME, 'req'=>1)
));

?>