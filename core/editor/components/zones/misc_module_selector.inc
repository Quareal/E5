<?php

/* набор модулей и их экземпляров для привязки их к новому сайту */

global $cmd;

$cmd['misc_module_selector'][CMD_COMPONENT]->process=function(&$data,&$op,&$ops){
	global $db;
	$m=getall($db,"SELECT * FROM main_module ORDER BY module_name",1,"main_module");
	$tmp=getall($db,"SELECT * FROM ex_module",1,"ex_module");
	$em=Array();
	if(!empty($tmp)) foreach($tmp AS $tm){
		if(empty($em[$tm["ex_module"]])) $em[$tm["ex_module"]]=Array();
		$em[$tm["ex_module"]][]=$tm;
	}	
	$res='';
	if(!empty($m)){
		if(!empty($m)) foreach($m AS $cm)if(check_mod($cm["module_id"],'view')){
			$res.='<p>'.$cm["module_name"].'<br><select name="ex_zone['.$cm["module_id"].']"><option value="0">'.lng('No').'</option>';
			if(!empty($em[$cm["module_id"]])) foreach($em[$cm["module_id"]] AS $cem)if(check_ex($cem["ex_id"],'view')){
				$add='';
				if(!empty($zem[$cem["ex_module"]])) $add=' selected';
				$res.='<option value="'.$cem["ex_id"].'"'.$add.'>'.$cem["ex_name"].'</option>';
			}
			$res.='</select>';
		}
	} else $res.='<p>'.lng('No available modules').'</p>';
	return $res;
};
$cmd['misc_module_selector'][CMD_COMPONENT]->result=CMD_STRING;

?>