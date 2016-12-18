<?php

/* вывод последних новостей пользователя */

global $cmd;

$cmd['edit_parts'][CMD_COMPONENT]->process=function(&$data,&$op,&$ops){
	global $fullurl2,$id,$type,$id2,$module,$zone_url,$base_root,$db;
	
	if($module!='mod_part' || (empty($id) && !empty($type))) return '';	
	if(empty($id)) return '';
	$parts_tmp=getall($db,"SELECT part_sname, part_name, part_ignore, part_type, part_id, part_owner FROM main_part WHERE part_module=$id");
	if(empty($parts_tmp)) return '';
	foreach($parts_tmp AS $p){
		if(!$p['part_owner']){
			$key='-';
			if($p['part_type']==0 && !$p['part_ignore']) $key='url';
			if($p['part_type']==0 && $p['part_ignore']) $key='regular';
			if($p['part_type']==1) $key='widget';
			if($p['part_type']==3) $key='action';
			if($p['part_type']==4) $key='import';
			if($p['part_type']==5) $key='row action';
			$parts_all[$key][]=$p;
		} else $parts_child[$p['part_owner']][]=$p;
	}
	
	$result='';
	$result.='<div class="lblock"><a href="'.$zone_url.'/mod_main?id='.$id.'"><img src="'.$base_root.'/files/editor/classic/code3.png" border="0" align="absmiddle" style="margin-right: 5px;"></a> <b style="color: #565656;">Части модуля</b>';
	$i=0;
	foreach($parts_all AS $key=>$parts){
		$i++;
		
	}
	$result.='<div align="center" style="padding-top: 7px; padding-bottom: 7px;" class="normal"><a href="" class="ablue"><img src="'.$base_root.'/files/editor/classic/add.png" border="0" align="absmiddle" style="margin-right: 5px;">Создать</a></div>';
	$result.='</div>';

	return $result;
};
$cmd['edit_parts'][CMD_COMPONENT]->result=CMD_STRING;

?>