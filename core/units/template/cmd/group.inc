<?php

function seek_group($group){
	global $sgroup,$cur_module;
	if(is_object($group) && isset($group->id)) $group=$group->id;
	if($group==-2){
		return Array(	'group'=>Array(),
					'module'=>0,
					'id'=>0);
	}
	if(!is_object($group) && !is_numeric($group) && is_string($group)){	
		$group_id=getrowval("SELECT auth_id FROM main_auth WHERE auth_type=1 AND group_module=$cur_module AND group_sname='".safe_sql_input($group)."'",'auth_id');
		if(empty($group)){
			$group_id=getrowval("SELECT auth_id FROM main_auth WHERE auth_type=1 AND group_sname='".safe_sql_input($group)."'",'auth_id');
		}
		if(!empty($group_id)) $group=$group_id;
	}	
	if(is_object($part) && isset($part->id)){
		$group=$group->id;
	}
	if(!isset($sgroup[$group])){
		getrow($db,"SELECT * FROM main_auth WHERE auth_type=1 AND auth_id=$group",1,'main_auth',__LINE__,__FILE__);
		$sgroup[$group]=$db->Record;
	}
	return Array(	'group'=>$sgroup[$group],
				'module'=>$sgroup[$group]['group_module'],
				'id'=>$group);
}


$cmd['name'][CMD_GROUP]->process=function(&$data,&$op,&$ops){
		$d=seek_group($data); $group=$d['group']; $id=$d['id']; $module=$d['module'];
		//if(is_object($data) && isset($data->id)) $id=$data->id; else $id=$data;
		return getrowval("SELECT * FROM main_auth WHERE auth_id=$id","group_name");
};
$cmd['name'][CMD_GROUP]->result=CMD_STRING;
$cmd['name'][CMD_GROUP]->visual=Array('title'=>'имя');


$cmd['sname'][CMD_GROUP]->process=function(&$data,&$op,&$ops){
		$d=seek_group($data); $group=$d['group']; $id=$d['id']; $module=$d['module'];
		return getrowval("SELECT * FROM main_auth WHERE auth_id=$id","group_sname");
};
$cmd['sname'][CMD_GROUP]->result=CMD_STRING;
$cmd['sname'][CMD_GROUP]->visual=Array('title'=>'спец.имя');


$cmd['search_login'][CMD_GROUP]->process=function(&$data,&$op,&$ops){
		$d=seek_group($data); $group=$d['group']; $id=$d['id']; $module=$d['module'];
		$login=parse_var($op->put);
		if(empty($id)) $x=getrowval("SELECT * FROM main_auth WHERE user_login='".$login."' AND auth_type=0","auth_id");
		else $x=getrowval("SELECT u.auth_id AS auth_id, a.link_user, a.link_group, u.user_login, u.auth_type FROM auth_link AS a LEFT JOIN main_auth AS u ON u.auth_id=a.link_user WHERE a.link_group=$id AND u.user_login='".$login."' AND u.auth_type=0","auth_id");
		if($x) return prepend_type($x,'usr');
		else return false;
};
$cmd['search_login'][CMD_GROUP]->result=CMD_USER;
$cmd['search_login'][CMD_GROUP]->visual=Array('title'=>'поиск по логину','put'=>Array(
		0=>Array('title'=>'логин','type'=>CMD_STRING,'req'=>1)
));


$cmd['search_email'][CMD_GROUP]->process=function(&$data,&$op,&$ops){
		$d=seek_group($data); $group=$d['group']; $id=$d['id']; $module=$d['module'];
		$email=parse_var($op->put);
		if(empty($id)) $x=getrowval("SELECT * FROM main_auth WHERE user_email='".$email."' AND auth_type=0","auth_id");
		else $x=getrowval("SELECT u.auth_id AS auth_id, a.link_user, a.link_group, u.user_email, u.auth_type FROM auth_link AS a LEFT JOIN main_auth AS u ON u.auth_id=a.link_user WHERE a.link_group=$id AND u.user_email='".$email."' AND u.auth_type=0","auth_id");
		if($x) return prepend_type($x,'usr');
		else return false;
};
$cmd['search_email'][CMD_GROUP]->result=CMD_USER;
$cmd['search_email'][CMD_GROUP]->visual=Array('title'=>'поиск по email','put'=>Array(
		0=>Array('title'=>'email','type'=>CMD_STRING,'req'=>1)
));


$cmd['search_ip'][CMD_GROUP]->process=function(&$data,&$op,&$ops){
		$d=seek_group($data); $group=$d['group']; $id=$d['id']; $module=$d['module'];
		$ip=parse_var($op->put);
		if(empty($id)) $x=getrowval("SELECT * FROM main_auth WHERE user_ip='".$ip."' AND auth_type=0","auth_id");
		else $x=getrowval("SELECT u.auth_id AS auth_id, a.link_user, a.link_group, u.user_email, u.auth_type FROM auth_link AS a LEFT JOIN main_auth AS u ON u.auth_id=a.link_user WHERE a.link_group=$id AND u.user_ip='".$ip."' AND u.auth_type=0","auth_id");
		if($x) return prepend_type($x,'usr');
		else return false;
};
$cmd['search_ip'][CMD_GROUP]->result=CMD_USER;
$cmd['search_ip'][CMD_GROUP]->visual=Array('title'=>'поиск по ip','put'=>Array(
		0=>Array('title'=>'ip','type'=>CMD_STRING,'req'=>1)
));


$cmd['search_name'][CMD_GROUP]->process=function(&$data,&$op,&$ops){
		$d=seek_group($data); $group=$d['group']; $id=$d['id']; $module=$d['module'];
		$name=parse_var($op->put);
		if(empty($id)) $x=getrowval("SELECT * FROM main_auth WHERE user_name='".$name."' AND auth_type=0","auth_id");
		else $x=getrowval("SELECT u.auth_id AS auth_id, a.link_user, a.link_group, u.user_name, u.auth_type FROM auth_link AS a LEFT JOIN main_auth AS u ON u.auth_id=a.link_user WHERE a.link_group=$id AND u.user_name='".$name."' AND u.auth_type=0","auth_id");
		if($x) return prepend_type($x,'usr');
		else return false;
};
$cmd['search_name'][CMD_GROUP]->result=CMD_USER;
$cmd['seatch_name'][CMD_GROUP]->visual=Array('title'=>'поиск по имени','put'=>Array(
		0=>Array('title'=>'имя','type'=>CMD_STRING,'req'=>1)
));


$cmd['users'][CMD_GROUP]->process=function(&$data,&$op,&$ops){
		$d=seek_group($data); $group=$d['group']; $id=$d['id']; $module=$d['module'];
		global $db;
		if(check_group($id,'view') && check_user($id,'view')){
			$links=getall3($db,"SELECT * FROM auth_link WHERE link_group=$id","link_user");
			if(!empty($links)) $users=getall($db,"SELECT * FROM main_auth WHERE auth_id IN (".implode(',',$links).")",1,"main_auth");
			$res=Array();$cr=0;
			if(!empty($users)) foreach($users AS $usr) if(check_user(-$usr["auth_id"],'view')){
				$res[$cr]->id=$usr["auth_id"];
				$res[$cr]->name=$usr["user_name"];
				$res[$cr]->login=$usr["user_login"];
				$res[$cr]->email=$usr["user_email"];
				$res[$cr]->type='usr';
				$cr++;
			}
			return $res;
		}
};
$cmd['users'][CMD_GROUP]->result=CMD_ARRAY;
$cmd['users'][CMD_GROUP]->result_long=CMD_USER;
$cmd['users'][CMD_GROUP]->visual=Array('title'=>'пользователи');

?>