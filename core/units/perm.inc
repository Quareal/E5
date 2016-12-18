<?php
		
if(empty($action) && !empty($_GET["action"])) $action=$_GET["action"];
if(empty($action) && !empty($_POST["action"])) $action=$_POST["action"];
$user->id=0;
$user->super=0;
if(empty($su_pwl) && empty($su_login)){ $user->super=1; $user->name='SuperUser'; $user->id=-1; }

if(isset($_POST["action"])) $action=$_POST["action"];
if(isset($_POST["login"])) $login=$_POST["login"];
if(isset($_POST["pwl"])) $pwl=$_POST["pwl"];
if(isset($_POST["remember_me"])) $remember_me=$_POST["remember_me"];

//Безопасная авторизация (начало сессии)
if($zone[$za]["zone_safe"]!=0 && $allow_safe && (empty($_GET["action"]) || $_GET["action"]!='user_exit')){
	//сессия
	//echo $_COOKIE["z-auth"].'<br>'.$_COOKIE["z-login"];
	if(!empty($_COOKIE["z-auth"]) && !empty($_COOKIE["z-login"])){
		getrow($db,"SELECT * FROM main_auth WHERE auth_type=0 AND user_login='".$_COOKIE["z-login"]."' AND auth_enable=1",1,"main_auth",__LINE__,__FILE__);
		if(!empty($db->Record) && !empty($db->Record["user_safe"])){
			$GLOBALS["xn_user_protect_old"]=$db->Record["user_safe"];
			$us=explode('|-|-|',$db->Record["user_safe"]);
			$GLOBALS["xn_user_protect_type"]=$db->Record["auth_id"];
		} else if($_COOKIE["z-login"]==$su_login && !empty($su_safe)){
			$us=explode('|-|-|',$su_safe);
			$GLOBALS["xn_user_protect_old"]=$su_safe;
			$GLOBALS["xn_user_protect_type"]=-1;
		}
		$user_login=$_COOKIE["z-login"];
		$user_safe=$us[0];
		$user_protect=$us[1];
		if(!empty($us[2])) $o_protect=$us[2];
		$user_protect2=md5($user_safe.$user_protect);
		
		$GLOBALS["xn_user_protect2"]=$user_protect2;//опасно
		$GLOBALS["xn_user_protect"]=$user_protect;//опасно
		$GLOBALS["xn_user_safe"]=$user_safe;//опасно
		$GLOBALS["xn_user_login"]=$user_login;//опасно
		
		$user_safe2=$user_safe.'|-|-|'.$user_protect2.'|-|-|'.$user_protect;
		//us[2] - можно сделать временем жизни сессии
		//if(empty($_GET["ajax"])) echo $user_protect.' - '.$_COOKIE["z-auth"];
		if($user_protect==$_COOKIE["z-auth"] || ($o_protect==$_COOKIE["z-auth"] && !empty($o_protect))){
			if(!empty($o_protect) && $o_protect==$_COOKIE["z-auth"]){
				$GLOBALS["xn_user_protect2"]=$o_protect;
				$GLOBALS["xn_user_protect_old"]=$user_safe.'|-|-|'.$o_protect.'|-|-|'.$user_protect;
			}
			if($user_login==$su_login){
				$user->login=$su_login;
				$user->name='SuperUser';
				$user->super=1;
				$user->email=$system_email;
				$user->id=-1;
				global $su_safe;
				$su_safe=$user_safe2;
				save_timing();
			} else {
				$user->id=$db->Record["auth_id"];
				$user->login=$db->Record["user_login"];
				$user->email=$db->Record["user_email"];
				$user->name=$db->Record["user_name"];
				$user->super=0;
				$db->query("UPDATE main_auth SET user_safe='$user_safe2' WHERE auth_id=".$user->id,3,"main_auth");
			}
			//SetCookie('z-auth','',time()-3600 , '/' , '' , 0 );
			
			//SetCookie('z-login','',time()-3600 , '/' , '' , 0 );
			//SetCookie('z-auth2','',time()-3600 , '/' , '' , 0 );
			//SetCookie('z-login2','',time()-3600 , '/' , '' , 0 );
			$GLOBALS["head"]='
<script type="text/javascript" src="'.$GLOBALS["base_root"].'/files/editor/z-auth.js"></script>
<script type="text/javascript" src="'.$GLOBALS["base_root"].'/files/editor/jstorage.js"></script>
<script>start_session();</script>';
			$GLOBALS["sp_head"]=$GLOBALS["head"];
		}
		unset($user_login);
		unset($us);
		unset($user_safe);
		unset($user_protect);
	}
}

// Проверка хука
if(!empty($_COOKIE["user_hook"]) && !empty($_COOKIE["user_id"])){
	global $e5uid,$session_everytime,$session_lifetime;
	// Проверяем наличие сессии
	$session_id=0;
	$session=getrow($db,"SELECT * FROM auth_session WHERE session_hash='".safe_sql_input($_COOKIE["user_hook"])."' AND session_active=1 AND (session_over=0 OR session_over>".$GLOBALS["cur_min"].")");
	if(!empty($session) && md5($session["session_auth"].get_protection_code())==$_COOKIE["user_id"] && $session['session_ip']==$user->ip){
		$session_id=$session["session_id"];
		$user->session=$session_id;
		if($session['session_auth']==-1){
			$user->login=$su_login;
			$user->name='SuperUser';
			$user->super=1;
			$user->email=$system_email;
			$user->id=-1;
		} else {
			$auth=getrow($db,"SELECT * FROM main_auth WHERE auth_id=".$session["session_auth"]." AND auth_enable=1");
			if(!empty($auth)){
				$user->id=$session["session_auth"];
				$user->login=$auth["user_login"];
				$user->email=$auth["user_email"];
				$user->name=$auth["user_name"];
				$user->super=0;
				if($auth['session_lifetime']!='' && $auth['session_lifetime']!='-1') $session_lifetime=$auth['session_lifetime'];
				else {
					global $link_cache;
					$link_cache[$user->id]=getall($db,"SELECT * FROM auth_link WHERE link_user=".$user->id." AND (link_expire='0000-00-00' OR link_expire>'".date('Y-m-d')."')",1,"auth_link",__LINE__,__FILE__);
					$rewrite_lifetime=-1;
					if(!empty($link_cache[$user->id])) foreach($link_cache[$user->id] AS $a){
						if(!isset($a['session_lifetime'])) $a['session_lifetime']=-1;
						if($a['session_lifetime']==0 || $a['session_lifetime']>$rewrite_lifetime && $rewrite_lifetime!=0) $rewrite_lifetime=$a['session_lifetime'];
					}
					if($rewrite_lifetime!=-1) $session_lifetime=$rewrite_lifetime;
				}
			}
		}
	}
	// Обновляем сессию
	if(!empty($user->id) && !empty($session_id)){
		if(!empty($session_lifetime)) $session_over=$GLOBALS['cur_min']+$session_lifetime;
		else $session_over=0;
		if(!empty($session_everytime)){
			$session_hash=uuin();
			if($user->remember_me==0){
				SetCookie("user_hook",$session_hash,0,'/','',false/*true*/,1);
			} else {
				SetCookie("user_hook",$session_hash,time()+3600*24*7,'/','',false/*true*/,1);
			}
			$sql_add=", session_hash='$session_hash'";
		} else $sql_add='';
		$db->query("UPDATE auth_session SET session_last='".$GLOBALS["timestamp"]."', session_over=$session_over".$sql_add." WHERE session_id=$session_id" );
	}
}

/*if(!empty($_COOKIE["user_hook"]) && !empty($_COOKIE["user_login"]) && $zone[$za]["zone_safe"]==0){
	getrow($db,"SELECT * FROM main_auth WHERE auth_type=0 AND user_login='".$_COOKIE["user_login"]."' AND auth_enable=1",1,"main_auth",__LINE__,__FILE__);
	$do_enter=false;
	if(!empty($su_pwl) && $_COOKIE["user_login"]==$su_login && $_COOKIE["user_hook"]==md5(md5($su_pwl).get_protection_code())){
		$user->login=$su_login;
		$user->name='SuperUser';
		$user->super=1;
		$user->email=$system_email;
		$user->id=-1;
		$do_enter=true;
	}
	if((empty($user->id) || !$do_enter) && !empty($db->Record))
	if(md5(md5($db->Record["user_pwl"]).get_protection_code())==$_COOKIE["user_hook"] && check_ip($db->Record["user_fixedip"],$user->ip)){
		$user->id=$db->Record["auth_id"];
		$user->login=$db->Record["user_login"];
		$user->email=$db->Record["user_email"];
		$user->name=$db->Record["user_name"];
		$user->super=0;
	}
}*/

// Выход
if(!empty($action) && $action=='user_exit' && !$cross_site){
	user_out();
	unset($action);
	unset($_POST["action"]);
	if($zone[$za]["zone_safe"]!=0){		
		$h='<script type="text/javascript" src="'.$GLOBALS["base_root"].'/files/editor/z-auth.js"></script>';
		$h.='<script type="text/javascript" src="'.$GLOBALS["base_root"].'/files/editor/jstorage.js"></script>';
		$h.='<script>user_out();</script>';
		$GLOBALS["head"]=$h;
	}
}

// Авторизация
if(!empty($action) && $action=='auth' && !empty($login) && !empty($pwl)){
	if(!empty($user->session)) user_out();
	if(empty($remember_me)) $remember_me=0;
	$auth_err='';
	if(!empty($auth_cooling)){
		if(
			!empty($auth_single_max_try) && !check_visit_ip(1,0,'','login'.safe_sql_input($login,1),$auth_single_max_try,'!'.$auth_cooling)
			||
			!empty($auth_all_max_try) && !check_visit_ip(1,0,'','login',$auth_all_max_try,'!'.$auth_cooling)
		) $auth_err='Израсходовано макс. кол-во попыток входа. Время ожидания следующей попытки: '.$auth_cooling.'мин.';
	}
	if(empty($auth_err)) $auth_err=auth($login,$pwl,$remember_me);
	global $user;
	unset($action);
	unset($_POST["action"]);
}

if($user->super && !empty($superuser_ip) && $user->ip!=$superuser_ip){
	// Завершение работы в случае несовпадения IP адреса с указанным
	exit;
}

// Обновление данных о последнем входе
//if($user->id>0){
//	$db->query("UPDATE main_auth SET user_lastlogin=".$GLOBALS["cur_min"]." WHERE auth_id=".$user->id,3,"main_auth",__LINE__,__FILE__);
//}
// Брать данные из сессии, а сюда записывать при входе

function flush_perm(){
	$unsets=Array('link_cache','mod_rules','zone_rules','table_rules','folder_rules','ex_rules','row_rules1','row_rules2','col_rules','user_rules','group_rules','row_rules3','xgcache','xgcache2','table_rules2','table_rules3','xuser_group','xuser_link');
	unset($GLOBALS['user']->group);
	foreach($unsets AS $u) unset($GLOBALS[$u]);
}

function load_base($u,$force=0){
	global $db,$user,$perm_cache,$link_cache;
	$s='0';
	$user->id=$u;
	if(!empty($user->id) && $user->id!=-1){
		$s.=','.$user->id;
		if(!isset($link_cache[$user->id]) || $force){
			$link_cache[$user->id]=getall($db,"SELECT * FROM auth_link WHERE link_user=".$user->id." AND (link_expire='0000-00-00' OR link_expire>'".date('Y-m-d')."')",1,"auth_link",__LINE__,__FILE__);
			//$link_cache[$user->id]=getall($db,"SELECT * FROM auth_link WHERE link_user=".$user->id,1,"auth_link",__LINE__,__FILE__);
		}
		$links=$link_cache[$user->id];
		$user->group=Array();
		$user->group2=Array();
		if(!empty($links)) foreach($links AS $link){
			$user->group[]=$link["link_group"];
			$user->group2[]=$link;
			$s.=','.$link["link_group"];
		}
	} else $user->group=Array();
	if($user->id!=-1) $tmp=getall($db,"SELECT * FROM auth_perm WHERE perm_auth IN ($s)",1,"auth_perm",__LINE__,__FILE__);
	$perm_cache=Array();
	if(!empty($tmp)) foreach($tmp AS $tm){
		if($tm["perm_target"]==0) 																										$perm_cache[$tm["perm_auth"]][$tm["perm_target"]][$tm["perm_type"]]=to_perm($tm);
		else if(($tm["perm_target"]==1 || $tm["perm_target"]==3 || $tm["perm_target"]==4 || $tm["perm_target"]==5) && $tm["perm_type"]!=5)						$perm_cache[$tm["perm_auth"]][$tm["perm_target"]][$tm["perm_type"]][$tm["perm_object"]]=to_perm($tm);
		else if(($tm["perm_target"]==1 || $tm["perm_target"]==3 || $tm["perm_target"]==4 || $tm["perm_target"]==5 || $tm["perm_target"]==6) && $tm["perm_type"]==5)	$perm_cache[$tm["perm_auth"]][$tm["perm_target"]][$tm["perm_type"]][$tm["perm_folder"]]=to_perm($tm);
		else if($tm["perm_target"]==7)	$perm_cache[$tm["perm_auth"]][$tm["perm_target"]][$tm["perm_type"]][$tm["perm_subtable"]]=to_perm($tm);
	}
}

load_base($user->id);

/*$s='0';
if(!empty($user->id)){
	$s.=','.$user->id;
	$links=getall($db,"SELECT * FROM auth_link WHERE link_user=".$user->id,1,"auth_link",__LINE__,__FILE__);
	$user->group=Array();
	if(!empty($links)) foreach($links AS $link){
		$user->group[]=$link["link_group"];
		$s.=','.$link["link_group"];
	}
}
$tmp=getall($db,"SELECT * FROM auth_perm WHERE perm_auth IN ($s)",1,"auth_perm",__LINE__,__FILE__);
$perm_cache=Array();
if(!empty($tmp)) foreach($tmp AS $tm){
	if($tm["perm_target"]==0) 						$perm_cache[$tm["perm_auth"]][$tm["perm_target"]][$tm["perm_type"]]=to_perm($tm);
	else if(($tm["perm_target"]==1 || $tm["perm_target"]==3 || $tm["perm_target"]==4) && $tm["perm_type"]!=5)	$perm_cache[$tm["perm_auth"]][$tm["perm_target"]][$tm["perm_type"]][$tm["perm_object"]]=to_perm($tm);
	else if(($tm["perm_target"]==1 || $tm["perm_target"]==3 || $tm["perm_target"]==4 || $tm["perm_target"]==6) && $tm["perm_type"]==5)$perm_cache[$tm["perm_auth"]][$tm["perm_target"]][$tm["perm_type"]][$tm["perm_folder"]]=to_perm($tm);
}*/

class perm{
	var $view=-1;
	var $edit=-1;
	var $add=-1;
	var $del=-1;
	var $rules=-1;
	var $upload=-1;
	var $invite=-1;
	var $leave=-1;
	var $reg=-1;
}

function merge_perm($dst2,$src,$rule=0){
	//if(!empty($dst->object) && !empty($src->object) && $src->object!=$dst->object) return $dst;
	$dst=new perm();
	$dst=dc($dst2);//это вообще жесть. без этого (тут сериалайз и унсериалайз) обработчик дуамает что DST указатель на DST2 и соответственно на саму базу данных. дикая ересь
	if($rule==0){
		if($dst->view==	2) $dst->view=		0.5;
		if($dst->edit==		2) $dst->edit=		0.5;
		if($dst->add==		2) $dst->add=		0.5;
		if($dst->del==		2) $dst->del=		0.5;
		if($dst->rules==	2) $dst->rules=		0.5;
		if($dst->upload==	2) $dst->upload=	0.5;
		if($dst->invite==	2) $dst->invite=	0.5;
		if($dst->leave==	2) $dst->leave=	0.5;
		if($dst->reg==		2) $dst->reg=		0.5;
		if($src->view==	2) $src->view=		0.5;
		if($src->edit==		2) $src->edit=		0.5;
		if($src->add==		2) $src->add=		0.5;
		if($src->del==		2) $src->del=		0.5;
		if($src->rules==	2) $src->rules=		0.5;
		if($src->upload==	2) $src->upload=	0.5;
		if($src->invite==	2) $src->invite=	0.5;
		if($src->leave==	2) $src->leave=	0.5;
		if($src->reg==		2) $src->reg=		0.5;
		if($dst->view==	3) $dst->view=		0.6;
		if($dst->edit==		3) $dst->edit=		0.6;
		if($dst->add==		3) $dst->add=		0.6;
		if($dst->del==		3) $dst->del=		0.6;
		if($dst->rules==	3) $dst->rules=		0.6;
		if($dst->upload==	3) $dst->upload=	0.6;
		if($dst->invite==	3) $dst->invite=	0.6;
		if($dst->leave==	3) $dst->leave=	0.6;
		if($dst->reg==		3) $dst->reg=		0.6;
		if($src->view==	3) $src->view=		0.6;
		if($src->edit==		3) $src->edit=		0.6;
		if($src->add==		3) $src->add=		0.6;
		if($src->del==		3) $src->del=		0.6;
		if($src->rules==	3) $src->rules=		0.6;
		if($src->upload==	3) $src->upload=	0.6;
		if($src->invite==	3) $src->invite=	0.6;
		if($src->leave==	3) $src->leave=	0.6;
		if($src->reg==		3) $src->reg=		0.6;
		if($dst->view	< $src->view) $dst->view=$src->view;	
		if($dst->edit	< $src->edit) $dst->edit=$src->edit;
		if($dst->add	< $src->add) $dst->add=$src->add;
		if($dst->del	< $src->del) $dst->del=$src->del;
		if($dst->rules	< $src->rules) $dst->rules=$src->rules;
		if($dst->upload	< $src->upload) $dst->upload=$src->upload;
		if($dst->invite	< $src->invite) $dst->invite=$src->invite;
		if($dst->leave	< $src->leave) $dst->leave=$src->leave;
		if($dst->reg	< $src->reg) $dst->reg=$src->reg;
		if($dst->view==	0.5) $dst->view=	2;
		if($dst->edit==		0.5) $dst->edit=	2;
		if($dst->add==		0.5) $dst->add=	2;
		if($dst->del==		0.5) $dst->del=	2;
		if($dst->rules==	0.5) $dst->rules=	2;
		if($dst->upload==	0.5) $dst->upload=	2;
		if($dst->invite==	0.5) $dst->invite=	2;
		if($dst->leave==	0.5) $dst->leave=	2;
		if($dst->reg==		0.5) $dst->reg=	2;
		if($src->view==	0.5) $src->view=	2;
		if($src->edit==		0.5) $src->edit=	2;
		if($src->add==		0.5) $src->add=	2;
		if($src->del==		0.5) $src->del=	2;
		if($src->rules==	0.5) $src->rules=	2;
		if($src->upload==	0.5) $src->upload=	2;
		if($src->invite==	0.5) $src->invite=	2;
		if($src->leave==	0.5) $src->leave=	2;
		if($src->reg==		0.5) $src->reg=	2;
		if($dst->view==	0.6) $dst->view=	3;
		if($dst->edit==		0.6) $dst->edit=	3;
		if($dst->add==		0.6) $dst->add=	3;
		if($dst->del==		0.6) $dst->del=	3;
		if($dst->rules==	0.6) $dst->rules=	3;
		if($dst->upload==	0.6) $dst->upload=	3;
		if($dst->invite==	0.6) $dst->invite=	3;
		if($dst->leave==	0.6) $dst->leave=	3;
		if($dst->reg==		0.6) $dst->reg=	3;
		if($src->view==	0.6) $src->view=	3;
		if($src->edit==		0.6) $src->edit=	3;
		if($src->add==		0.6) $src->add=	3;
		if($src->del==		0.6) $src->del=	3;
		if($src->rules==	0.6) $src->rules=	3;
		if($src->upload==	0.6) $src->upload=	3;
		if($src->invite==	0.6) $src->invite=	3;
		if($src->leave==	0.6) $src->leave=	3;
		if($src->reg==		0.6) $src->reg=	3;
	} else if($rule==1){
		if(!isset($src->view)) return $dst;
		if($src->view	!=-1) $dst->view=$src->view;	
		if($src->edit	!=-1) $dst->edit=$src->edit;
		if($src->add	!=-1) $dst->add=$src->add;
		if($src->del	!=-1) $dst->del=$src->del;
		if($src->rules	!=-1) $dst->rules=$src->rules;
		if($src->upload	!=-1) $dst->upload=$src->upload;
		if($src->invite	!=-1) $dst->invite=$src->invite;
		if($src->leave	!=-1) $dst->leave=$src->leave;
		if($src->reg	!=-1) $dst->reg=$src->reg;
	}/* else if($rule==2){
		if($dst->view	==-1 && $src->view>0) $dst->view=$src->view;	
		if($dst->edit	==-1 && $src->edit>0) $dst->edit=$src->edit;
		if($dst->add	==-1 && $src->add>0) $dst->add=$src->add;
		if($dst->del	==-1 && $src->del>0) $dst->del=$src->del;
		if($dst->rules	==-1 && $src->rules>0) $dst->rules=$src->rules;
		if($dst->upload	==-1 && $src->upload>0) $dst->upload=$src->upload;
		if($dst->invite	==-1 && $src->invite>0) $dst->invite=$src->invite;
		if($dst->leave	==-1 && $src->leave>0) $dst->leave=$src->leave;
		if($dst->reg	==-1 && $src->reg>0) $dst->reg=$src->reg;
	} else if($rule==3){// этот тип просто как вариант типа №2
		if($dst->view	==-1 || $src->view>0) $dst->view=$src->view;	
		if($dst->edit	==-1 || $src->edit>0) $dst->edit=$src->edit;
		if($dst->add	==-1 || $src->add>0) $dst->add=$src->add;
		if($dst->del	==-1 || $src->del>0) $dst->del=$src->del;
		if($dst->rules	==-1 || $src->rules>0) $dst->rules=$src->rules;
		if($dst->upload	==-1 || $src->upload>0) $dst->upload=$src->upload;
		if($dst->invite	==-1 || $src->invite>0) $dst->invite=$src->invite;
		if($dst->leave	==-1 || $src->leave>0) $dst->leave=$src->leave;
		if($dst->reg	==-1 || $src->reg>0) $dst->reg=$src->reg;
	} else if($rule==4){
		$dst->view=submerge($dst->view,$src->view,'view',1);
		$dst->edit=submerge($dst->edit,$src->edit,'edit',1);
		$dst->add=submerge($dst->add,$src->add,'add',1);
		$dst->del=submerge($dst->del,$src->del,'del',1);
		$dst->rules=submerge($dst->rules,$src->rules,'rules',1);
		$dst->upload=submerge($dst->upload,$src->upload,'upload',1);
		$dst->invite=submerge($dst->invite,$src->invite,'invite',1);
		$dst->leave=submerge($dst->leave,$src->leave,'leave',1);
		$dst->reg=submerge($dst->reg,$src->reg,'reg',1);
	}
	else if($rule==5){// инверсия типа 1
		if($dst->view	==-1) $dst->view=$src->view;	
		if($dst->edit	==-1) $dst->edit=$src->edit;
		if($dst->add	==-1) $dst->add=$src->add;
		if($dst->del	==-1) $dst->del=$src->del;
		if($dst->rules	==-1) $dst->rules=$src->rules;
		if($dst->upload	==-1) $dst->upload=$src->upload;
		if($dst->invite	==-1) $dst->invite=$src->invite;
		if($dst->leave	==-1) $dst->leave=$src->leave;
		if($dst->reg	==-1) $dst->reg=$src->reg;
	}*/
	else if($rule==6){
		$dst->view=submerge($dst->view,$src->view,'view',6);
		$dst->edit=submerge($dst->edit,$src->edit,'edit',6);
		$dst->add=submerge($dst->add,$src->add,'add',6);
		$dst->del=submerge($dst->del,$src->del,'del',6);
		$dst->rules=submerge($dst->rules,$src->rules,'rules',6);
		$dst->upload=submerge($dst->upload,$src->upload,'upload',6);
		$dst->invite=submerge($dst->invite,$src->invite,'invite',6);
		$dst->leave=submerge($dst->leave,$src->leave,'leave',6);
		$dst->reg=submerge($dst->reg,$src->reg,'reg',6);
	}	
	return $dst;
}

function submerge($dst,$src,$cmd='',$type=1){//cmd for debug
	if($type==1){
		//dst - права для экземпляра, src - права таблицы
		//if($cmd=='add') echo $src;
		if($dst==-1) return $src;
		if($src==-1) return $dst;
		if($src==0 || $dst==0) return 0;
		if($src==$dst) return $src;
		if($src==0.5 && $dst==1) return 1;//?
		if($src==1 && $dst==0.5) return 1;//?
		return $dst;
	}
	if($type==6){//только для мультиэкземплярных таблиц
		if($src!=$dst) return -1;
		else return $src;
	}
}

function merge_perm2($r1,$r2,$rule=0){
	foreach($r1 AS $var=>$value){
		if(isset($r2[$var])) $r1[$var]=merge_perm($r1[$var],$r2[$var],$rule);
	}
	return $r1;
}

function recheck_perm($rec){//защита от NULL записей
	$pr=explode(",","perm_view,perm_edit,perm_add,perm_del,perm_rules,perm_upload,perm_invite,perm_leave,perm_reg");
	foreach($pr AS $p){
		if(!isset($rec[$p])) $rec[$p]=-1;
		else if($rec[$p]=='') $rec[$p]=-1;
	}
	return $rec;
}

function to_perm($rec){
	$d=new perm();
	$d->view=(int)$rec["perm_view"];
	$d->edit=(int)$rec["perm_edit"];
	$d->add=(int)$rec["perm_add"];
	$d->del=(int)$rec["perm_del"];
	$d->rules=(int)$rec["perm_rules"];
	$d->upload=(int)$rec["perm_upload"];
	$d->invite=(int)$rec["perm_invite"];
	$d->leave=(int)$rec["perm_leave"];
	if(!empty($rec["perm_object"])) $d->object=(int)$rec["perm_object"]; else $d->object=0;
	$d->reg=(int)$rec["perm_reg"];
	return $d;
}

function r_debug($rules,$tmp){
	echo '<br><div>RULE '.$tmp.'</div><ul>';
	foreach($rules AS $var=>$value){
		echo '<li>'.$var.' ->edit = '.$value->edit.'</li>';
	}
	echo '</ul>';
}

function load_perms($auth,$target,$type,$rules2,$rule=0){
	global $db,$perm_cache;
	$rules=dc($rules2);
	if($target==0){
		if(!empty($perm_cache[$auth][$target][$type])){
			$res=$perm_cache[$auth][$target][$type];
			foreach($rules AS $var=>$value) $rules[$var]=merge_perm($rules[$var],$res,$rule);
			//r_debug($rules,'A'.$auth);
			$rules[0]=merge_perm($rules[0],$res); //установка значения по дефолту для категории
		}
	}
	if($target==1 || $target==7){
		if(!empty($perm_cache[$auth][$target][$type])) foreach($perm_cache[$auth][$target][$type] AS $object=>$perm){
			if(!isset($rules[$object])) $rules[$object]=new perm();//??
			$rules[$object]=merge_perm($rules[$object],$perm,$rule);
			//r_debug($rules,'B'.$auth);
		}
	}
	if($target==3){
		if(!empty($perm_cache[$auth][$target][$type])) foreach($perm_cache[$auth][$target][$type] AS $object=>$perm){
			if($type==2 || $type==3 || $type==8 || $type==6){
				foreach($rules AS $var=>$value) if(!empty($value->module) && $value->module==$object){
					if(!isset($rules[$var])) $rules[$var]=new perm();
					$rules[$var]=merge_perm($rules[$var],$perm,$rule);
				}
				if(!empty($rules[-$object])) $rules[-$object]=merge_perm($rules[-$object],$perm,$rule);
			}
			if($type==4){
				if(!isset($rules[$object])) $rules[$object]=new perm();//??
				$rules[$object]=merge_perm($rules[$object],$perm,$rule);
			}
		}
	}
	if($target==4 || $target==6 || $target==8) if(!empty($perm_cache[$auth][$target][$type])) foreach($perm_cache[$auth][$target][$type] AS $object=>$perm){
		if(!isset($rules[$object])) $rules[$object]=new perm();//??
		$rules[$object]=merge_perm($rules[$object],$perm,$rule);
	}
	if($target==5) if(!empty($perm_cache[$auth][$target][$type])) foreach($perm_cache[$auth][$target][$type] AS $object=>$perm){
		if(!isset($rules[-$object]))$rules[-$object]=new perm();
		$rules[-$object]=merge_perm($rules[-$object],$perm,$rule);
	}
	return $rules;
}

function perm_zone(){
	global $user,$zone,$zone_rules;
	$rules=Array();
	foreach($zone AS $z){
		$rules[(int)$z["zone_id"]]=new perm();
		$rules[(int)$z["zone_id"]]->object=$z["zone_id"];
	}
	$rules2=dc($rules);
	$rules[0]=new perm();
	$rules2[0]=new perm();
	
	// Умолчания
	$rules=load_perms(0,0,0,$rules);
	$rules=load_perms(0,1,0,$rules,1);
	// Группы
	if(!empty($user->group)) foreach($user->group AS $gr){
		$rules2=load_perms($gr,0,0,$rules);
		$rules2=load_perms($gr,1,0,$rules2,1);
		$rules=merge_perm2($rules,$rules2);
	}
	// Пользователь
	$rules2=load_perms($user->id,0,0,$rules);
	$rules2=load_perms($user->id,1,0,$rules2,1);
	$rules=merge_perm2($rules,$rules2);

	/*$rules2=dc($rules);
	
	$rules=load_perms(0,0,0,$rules);
	if(!empty($user->group)) foreach($user->group AS $gr){
		$rules=load_perms($gr,0,0,$rules);
	}
	$rules=load_perms($user->id,0,0,$rules);

	$rules3=load_perms(0,1,0,$rules2);
	if(!empty($user->group)) foreach($user->group AS $gr){
		$rules3=load_perms($gr,1,0,$rules3);
	}
	$rules3=load_perms($user->id,1,0,$rules3);
	$rules=merge_perm2($rules,$rules3,5);*/

	$zone_rules=$rules;
}

function perm_mod(){
	global $user,$mod,$mod_rules;
	if(empty($mod)){
		global $db;
		$mod=getall($db,"SELECT * FROM main_module ORDER BY module_name",1,'main_module',__LINE__,__FILE__);
	}
	$rules=Array();
	if(!empty($mod)) foreach($mod AS $m){
		$rules[(int)$m["module_id"]]=new perm();
		$rules[(int)$m["module_id"]]->object=$m["module_id"];
	}
	$rules2=dc($rules);
	$rules[0]=new perm();
	$rules2[0]=new perm();
	
	
	//все модули
	$rules=load_perms(0,0,1,$rules);
	if(!empty($user->group)) foreach($user->group AS $gr){
		$rules=load_perms($gr,0,1,$rules);
	}
	$rules=load_perms($user->id,0,1,$rules);
	
	//конкретный модуль
	$rules3=load_perms(0,1,1,$rules2);
	if(!empty($user->group)) foreach($user->group AS $gr){
		$rules3=load_perms($gr,1,1,$rules3);
	}
	$rules3=load_perms($user->id,1,1,$rules3);
	$rules=merge_perm2($rules,$rules3,1);
	
	/*// Умолчания
	$rules=load_perms(0,0,1,$rules);
	$rules=load_perms(0,1,1,$rules,1);
	// Группы
	if(!empty($user->group)) foreach($user->group AS $gr){
		$rules2=load_perms($gr,0,1,$rules);
		$rules2=load_perms($gr,1,1,$rules2,1);
		$rules=merge_perm2($rules,$rules2);
	}
	// Пользователь
	$rules2=load_perms($user->id,0,1,$rules);
	$rules2=load_perms($user->id,1,1,$rules2,1);
	$rules=merge_perm2($rules,$rules2);*/

	$mod_rules=$rules;
}

function perm_ex(){
	global $user,$exp,$mod,$ex_rules;
	if(empty($exp)){
		global $db;
		$exp=getall($db,"SELECT * FROM ex_module",1,'ex_module',__LINE__,__FILE__);
	}
	if(empty($mod)){
		global $db;
		$mod=getall($db,"SELECT * FROM main_module ORDER BY module_name",1,'main_module',__LINE__,__FILE__);
	}
	$rules=Array();
	foreach($exp AS $e){
		$rules[(int)$e["ex_id"]]=new perm();
		$rules[(int)$e["ex_id"]]->object=$e["ex_id"];
		$rules[(int)$e["ex_id"]]->module=$e["ex_module"];
	}
	if(!empty($mod)) foreach($mod AS $m){
		$rules[-(int)$m["module_id"]]=new perm();
		$rules[-(int)$m["module_id"]]->object=$m["module_id"];
	}
	$rules2=dc($rules);
	$rules[0]=new perm();
	$rules2[0]=new perm();
	
	//все экземпляры
	$rules=load_perms(0,0,2,$rules);
	if(!empty($user->group)) foreach($user->group AS $gr){
		$rules=load_perms($gr,0,2,$rules);
	}
	$rules=load_perms($user->id,0,2,$rules);
	
	//все экземпляры модуля
	$rules3=load_perms(0,3,2,$rules2);
	if(!empty($user->group)) foreach($user->group AS $gr){
		$rules3=load_perms($gr,3,2,$rules3);
	}
	$rules3=load_perms($user->id,3,2,$rules3);
	$rules=merge_perm2($rules,$rules3,/*2*/1);//почему тут еденица а не двойка для меня очень-очень большой вопрос
	
	//foreach($rules AS $id=>$r)if($id>0){
	//	$rules[$id]=merge_perm($rules[$id],$rules[-$tatomo[$id]],1);
	//}
	
	//отдельные экземпляры
	$rules3=load_perms(0,1,2,$rules2);
	if(!empty($user->group)) foreach($user->group AS $gr){
		$rules3=load_perms($gr,1,2,$rules3);
	}
	$rules3=load_perms($user->id,1,2,$rules3);
	$rules=merge_perm2($rules,$rules3,1);
	
	/* нужно к такому виду привести всё */
	
	/*// Умолчания
	$rules=load_perms(0,0,2,$rules);
	$rules=load_perms(0,3,2,$rules,1);
	$rules=load_perms(0,1,2,$rules,1);
	// Группы
	if(!empty($user->group)) foreach($user->group AS $gr){
		$rules2=load_perms($gr,0,2,$rules);
		$rules2=load_perms($gr,3,2,$rules2,1);
		$rules2=load_perms($gr,1,2,$rules2,1);
		$rules=merge_perm2($rules,$rules2);
	}
	// Пользователь
	$rules2=load_perms($user->id,0,2,$rules);
	$rules2=load_perms($user->id,3,2,$rules2,1);
	$rules2=load_perms($user->id,1,2,$rules2,1);
	$rules=merge_perm2($rules,$rules2);*/
	
	$ex_rules=$rules;
}

function perm_col(){
	global $user,$colp,$mod,$col_rules;
	if(empty($colp)){
		global $db;
		$colp=getall($db,"SELECT * FROM main_col",1,'main_col',__LINE__,__FILE__);
	}
	$rules=Array();
	foreach($colp AS $c){
		$rules[(int)$c["col_id"]]=new perm();
		$rules[(int)$c["col_id"]]->object=$c["col_id"];
		$rules[(int)$c["col_id"]]->module=$c["col_module"];
	}
	$rules2=dc($rules);
	$rules[0]=new perm();
	$rules2[0]=new perm();
	
	// Умолчания
	//$rules=load_perms(0,0,7,$rules);
	//$rules=load_perms(0,3,7,$rules,1);
	$rules=load_perms(0,1,7,$rules);
	// Группы
	if(!empty($user->group)) foreach($user->group AS $gr){
		//$rules=load_perms($gr,0,7,$rules);
		//$rules=load_perms($gr,3,7,$rules,1);
		$rules=load_perms($gr,1,7,$rules);
	}
	// Пользователь
	//$rules=load_perms($user->id,0,7,$rules);
	//$rules=load_perms($user->id,3,7,$rules);
	$rules=load_perms($user->id,1,7,$rules);

	$col_rules=$rules;
}

function perm_folder(){
	global $user,$folder_rules,$afolder;
	
	if(isset($folder_rules)) return;
	if($user->super==1){
		$afolder='files';
		$user->folder='files';
	}
	$rules=Array();
	/*foreach($groupp AS $p){
		$rules[(int)$p["auth_id"]]=new perm();
		$rules[(int)$p["auth_id"]]->object=$p["auth_id"];
	}*/
	$rules2=dc($rules);
	$rules[0]=new perm();
	$rules2[0]=new perm();
	
	
	
	$rules2=dc($rules);
	
	$rules=load_perms(0,1,5,$rules);
	if(!empty($user->group)) foreach($user->group AS $gr){
		$rules=load_perms($gr,1,5,$rules);
	}
	$rules=load_perms($user->id,1,5,$rules);
	
	$rules3=load_perms(0,0,5,$rules2);
	if(!empty($user->group)) foreach($user->group AS $gr){
		$rules3=load_perms($gr,0,5,$rules3);
	}
	$rules3=load_perms($user->id,0,5,$rules3);
	$rules=merge_perm2($rules,$rules3,0);
	
	$trt=load_perms(0,6,5,Array());foreach($trt AS $rt=>$tmp){break;}
	if(!empty($user->group)) foreach($user->group AS $gr){
		$trt=load_perms($gr,6,5,Array());foreach($trt AS $rt2=>$tmp){if($rt2!='') $rt=$rt2; break;}
	}
	$trt=load_perms($user->id,6,5,Array());foreach($trt AS $rt2=>$tmp){if($rt2!='') $rt=$rt2; break;}
	
	
	/*// Умолчания
	$rules=load_perms(0,0,5,$rules);
	$rules=load_perms(0,1,5,$rules,1);
	$rt='';
	$trt=load_perms(0,6,5,Array());foreach($trt AS $rt=>$tmp){break;}
	// Группы
	if(!empty($user->group)) foreach($user->group AS $gr){
		$rules2=load_perms($gr,0,5,$rules);
		$rules2=load_perms($gr,1,5,$rules2,1);
		$rules=merge_perm2($rules,$rules2);
		//По идее надо искать наиболее высшую рут-директорию, которая бы объеденила все текущие диры
		$trt=load_perms($gr,6,5,Array());foreach($trt AS $rt2=>$tmp){if($rt2!='') $rt=$rt2; break;}
	}
	// Пользователь
	$rules2=load_perms($user->id,0,5,$rules);
	$rules2=load_perms($user->id,1,5,$rules2,1);
	$rules=merge_perm2($rules,$rules2);
	$trt=load_perms($user->id,6,5,Array());foreach($trt AS $rt2=>$tmp){if($rt2!='') $rt=$rt2; break;}*/



	/*$rules3=load_perms(0,0,5,$rules2);
	if(!empty($user->group)) foreach($user->group AS $gr){
		$rules3=load_perms($gr,0,5,$rules3);
	}
	$rules3=load_perms($user->id,0,5,$rules3);

	$rules3=load_perms(0,1,5,$rules2);
	if(!empty($user->group)) foreach($user->group AS $gr){
		$rules3=load_perms($gr,1,5,$rules3);
	}
	$rules3=load_perms($user->id,1,5,$rules3);
	$rules=merge_perm2($rules,$rules3,5);*/
	
	if(isset($rt)) $afolder=$rt; else $rt=''/*'files'*/;//не уверен в правильности files
	global $user;
	$user->folder=$afolder;
	$folder_rules=$rules;
}


function perm_group(){
	global $user,$groupp,$group_rules,$group_rules2,$mod;
	
	if(empty($mod)){
		global $db;
		$mod=getall($db,"SELECT * FROM main_module ORDER BY module_name",1,'main_module',__LINE__,__FILE__);
	}
	if(empty($groupp)){
		global $db;
		$groupp=getall($db,"SELECT * FROM main_auth WHERE auth_type=1",1,'main_auth',__LINE__,__FILE__);
	}
	$rules=Array();
	foreach($groupp AS $p){
		$rules[(int)$p["auth_id"]]=new perm();
		$rules[(int)$p["auth_id"]]->object=$p["auth_id"];
	}
	foreach($mod AS $m){
		$rules[-(int)$m["module_id"]]=new perm();
		$rules[-(int)$m["module_id"]]->object=$m["module_id"];
		if(check_mod($m["module_id"],'edit')){
			$r=&$rules[-(int)$m["module_id"]];
			$r->view=1;
			$r->edit=1;
			$r->del=1;
			$r->add=1;
			$r->rules=1;
			$r->invite=1;
			$r->leave=1;
			//остальные доступы давать нельзя по соображениям безопасности, т.к. пользователь может быть в двух группах, что не даёт право админу 1ой удалить его совсем
		}
	}
	$rules2=dc($rules);
	$rules[0]=new perm();
	$rules2[0]=new perm();
	
	// Умолчания
	$rules=load_perms(0,0,8,$rules);
	$rules=load_perms(0,3,8,$rules,1);
	$rules=load_perms(0,1,8,$rules,1);
	// Группы
	if(!empty($user->group)) foreach($user->group AS $gr){
		$rules2=load_perms($gr,0,8,$rules);
		$rules2=load_perms($gr,3,8,$rules2,1);
		$rules2=load_perms($gr,1,8,$rules2,1);
		$rules=merge_perm2($rules,$rules2);
	}
	// Пользователь
	$rules2=load_perms($user->id,0,8,$rules);
	$rules2=load_perms($user->id,3,8,$rules2,1);	
	$rules2=load_perms($user->id,1,8,$rules2,1);
	$rules=merge_perm2($rules,$rules2);
	
	$group_rules=$rules;
	
	//права для общих настроек групп модулей
	$rules=Array();
	foreach($groupp AS $p){
		$rules[(int)$p["auth_id"]]=new perm();
		$rules[(int)$p["auth_id"]]->object=$p["auth_id"];
	}
	$rules2=dc($rules);
	$rules[0]=new perm();
	$rules2[0]=new perm();
	
	// Умолчания
	$rules=load_perms(0,0,8,$rules);
	$rules=load_perms(0,3,8,$rules,1);
	$rules=load_perms(0,1,8,$rules,1);
	// Группы
	if(!empty($user->group)) foreach($user->group AS $gr){
		$rules2=load_perms($gr,0,8,$rules);
		$rules2=load_perms($gr,3,8,$rules2,1);
		$rules2=load_perms($gr,1,8,$rules2,1);
		$rules=merge_perm2($rules,$rules2);
	}
	// Пользователь
	$rules2=load_perms($user->id,0,8,$rules);
	$rules2=load_perms($user->id,3,8,$rules2,1);	
	$rules2=load_perms($user->id,1,8,$rules2,1);
	$rules=merge_perm2($rules,$rules2);
	
	$group_rules2=$rules;	
}

function perm_user(){
	global $user,$groupp,$user_rules,$mod;
	
	if(empty($mod)){
		global $db;
		$mod=getall($db,"SELECT * FROM main_module ORDER BY module_name",1,'main_module',__LINE__,__FILE__);
	}
	
	if(empty($userp)){
		global $db;
		$groupp=getall($db,"SELECT * FROM main_auth WHERE auth_type=1",1,'main_auth',__LINE__,__FILE__);
	}
	$rules=Array();
	foreach($groupp AS $p){
		$rules[(int)$p["auth_id"]]=new perm();
		$rules[(int)$p["auth_id"]]->object=$p["auth_id"];
	}
	if(!empty($mod)) foreach($mod AS $m){
		$rules[-(int)$m["module_id"]]=new perm();
		$rules[-(int)$m["module_id"]]->object=$m["module_id"];
		if(check_mod($m["module_id"],'edit')){
			$r=&$rules[-(int)$m["module_id"]];
			$r->reg=1;
			$r->view=1;
			//остальные доступы давать нельзя по соображениям безопасности, т.к. пользователь может быть в двух группах, что не даёт право админу 1ой удалить его совсем
		}
	}
	$rules2=dc($rules);
	$rules[0]=new perm();
	$rules2[0]=new perm();
	
	// Умолчания
	$rules=load_perms(0,0,6,$rules);
	$rules=load_perms(0,3,6,$rules,1);
	$rules=load_perms(0,1,6,$rules,1);
	// Группы
	if(!empty($user->group)) foreach($user->group AS $gr){
		$rules2=load_perms($gr,0,6,$rules);
		$rules2=load_perms($gr,3,6,$rules2,1);
		$rules2=load_perms($gr,1,6,$rules2,1);
		$rules=merge_perm2($rules,$rules2);
	}
	// Пользователь
	$rules2=load_perms($user->id,0,6,$rules);
	$rules2=load_perms($user->id,3,6,$rules2,1);
	$rules2=load_perms($user->id,1,6,$rules2,1);
	$rules=merge_perm2($rules,$rules2);
	
	$user_rules=$rules;
}

function perm_table(){
	global $user,$tables,$mod,$table_rules,$table_rules2,$table_rules3;
	prep_tables();
	$rules=Array();
	foreach($tables AS $t){
		$rules[(int)$t->id]=new perm();
		$rules[(int)$t->id]->object=$t->id;
		$rules[(int)$t->id]->module=$t->module;
	}
	if(empty($mod)){
		global $db;
		$mod=getall($db,"SELECT * FROM main_module ORDER BY module_name",1,'main_module',__LINE__,__FILE__);
	}
	if(!empty($mod)) foreach($mod AS $m){
		$rules[-(int)$m["module_id"]]=new perm();
		$rules[-(int)$m["module_id"]]->object=$m["module_id"];
	}
	$rules2=dc($rules);
	$rules2[0]=new perm();
	$rules[0]=new perm();
	
	// Умолчания
	$rules=load_perms(0,0,3,$rules);
	$rules=load_perms(0,3,3,$rules,1);
	$rules=load_perms(0,1,3,$rules,1);
	// Группы
	if(!empty($user->group)) foreach($user->group AS $gr){
		$rules2=load_perms($gr,0,3,$rules);
		$rules2=load_perms($gr,3,3,$rules2,1);
		$rules2=load_perms($gr,1,3,$rules2,1);
		$rules=merge_perm2($rules,$rules2);
	}
	// Пользователь
	$rules2=load_perms($user->id,0,3,$rules);
	$rules2=load_perms($user->id,3,3,$rules2,1);
	$rules2=load_perms($user->id,1,3,$rules2,1);
	$rules=merge_perm2($rules,$rules2);

	$table_rules=$rules;
	
	//определение подтаблиц
	$rules3=Array();
	$rules3[0]=new perm();
	$rules3=load_perms(0,7,3,/*$rules2*/$rules3);/* ??? */
	if(!empty($user->group)) foreach($user->group AS $gr){
		$rules3=load_perms($gr,7,3,$rules3);
	}
	$rules3=load_perms($user->id,7,3,$rules3);	
	
	$table_rules2=$rules3;
	
	//определение супердобавления
	$rules4=Array();
	$rules4[0]=new perm();
	$rules4=load_perms(0,3,4,$rules4);
	if(!empty($user->group)) foreach($user->group AS $gr){
		$rules4=load_perms($gr,3,4,$rules4);
	}
	$rules4=load_perms($user->id,3,4,$rules4);
	
	$table_rules3=$rules4;
}

function perm_row1(){ //tables compare
	global $user,$modules,$row_rules1,$row_rules3,$tatomo/*table to module*/;
	prep_tables();
	global $tables;
	/*if(empty($tables)){
		global $db;
		$tables=getall($db,"SELECT * FROM main_table",1,'main_table',__LINE__,__FILE__);
	}*/
	if(empty($modules)){
		global $db;
		$modules=getall($db,"SELECT * FROM main_module",1,'main_module',__LINE__,__FILE__);
	}
	$rules=Array();
	foreach($modules AS $m){
		$rules[-(int)$m["module_id"]]=new perm();
		$rules[-(int)$m["module_id"]]->object=$m["module_id"];
	}
	foreach($tables AS $id=>$t){
		$rules[$id]=new perm();
		$rules[$id]->object=$id;
		$tatomo[$id]=$t->module;
	}
	$rules[0]=new perm();
	$trules[0]=new perm();
	$rules2=dc($rules);
	
	//порядок действий был зачем-то инвертирован
	
	//все строки
	$rules=load_perms(0,0,4,$rules);
	if(!empty($user->group)) foreach($user->group AS $gr){
		$rules=load_perms($gr,0,4,$rules);
	}
	$rules=load_perms($user->id,0,4,$rules);
	
	//все строки модуля
	$rules3=load_perms(0,5,4,$rules2);
	if(!empty($user->group)) foreach($user->group AS $gr){
		$rules3=load_perms($gr,5,4,$rules3);
	}
	$rules3=load_perms($user->id,5,4,$rules3);
	$rules=merge_perm2($rules,$rules3,1);//вместо 1 был 0
	
	/*if($user->ip=='212.164.230.75'){
		var_dump($rules[-33]);
		echo "\n\n\n";
		var_dump($rules[14]);
		echo "\n\n\n";
		var_dump($rules[116]);
		exit;
	}*/
	
	foreach($rules AS $id=>$r)if($id>0){
		$rules[$id]=merge_perm($rules[$id],$rules[-$tatomo[$id]],1);//в общем тут просто поменял местами таблицу и модуль и заработало О_о
	}
	
	//все строки таблицы
	$rules3=load_perms(0,3,4,$rules2);
	if(!empty($user->group)) foreach($user->group AS $gr){
		$rules3=load_perms($gr,3,4,$rules3);
	}
	$rules3=load_perms($user->id,3,4,$rules3);
	$rules=merge_perm2($rules,$rules3,1);//вместо 1 был 0
	$row_rules1=$rules;

	//определение подтаблиц
	$rules3=Array();
	$rules3[0]=new perm();
	$rules3=load_perms(0,7,4,$rules3);
	if(!empty($user->group)) foreach($user->group AS $gr){
		$rules3=load_perms($gr,7,4,$rules3);
	}
	$rules3=load_perms($user->id,7,4,$rules3);
	
	$row_rules3=$rules3;
}

function perm_row2(){ //ex compare
	global $user,$exp,$row_rules2;
	if(empty($exp)){
		global $db;
		$exp=getall($db,"SELECT * FROM ex_module",1,'ex_module',__LINE__,__FILE__);
	}
	$rules=Array();
	foreach($exp AS $e){
		$rules[(int)$e["ex_id"]]=new perm();
		$rules[(int)$e["ex_id"]]->object=$e["ex_id"];
	}
	$rules2=dc($rules);
	$rules[0]=new perm();
	$rules2[0]=new perm();
	
	// Умолчания
	//$rules=load_perms(0,0,4,$rules);  //- эти умолчания не мешало бы убрать, т.к. они уже проверялись в perm_row1 и пользователю придётся дублировать их и в таблице и в экземпляре
	$rules=load_perms(0,4,4,$rules);
	// Группы
	if(!empty($user->group)) foreach($user->group AS $gr){
		//$rules=load_perms($gr,0,4,$rules);
		$rules=load_perms($gr,4,4,$rules);
	}
	// Пользователь
	//$rules=load_perms($user->id,0,4,$rules);
	$rules=load_perms($user->id,4,4,$rules);

	$row_rules2=$rules;
}

function check_zone($zone,$rule){
	global $zone_rules,$user;
	if($user->super==1) return true;
	if(empty($zone_rules)) perm_zone();
	if($rule=='view') return ($zone_rules[$zone]->view!=0);
	if($rule=='edit') return ($zone_rules[$zone]->edit!=0);
	if($rule=='rules') return ($zone_rules[$zone]->rules!=0);
	if($rule=='add') return ($zone_rules[0]->add!=0);
	if($rule=='del') return ($zone_rules[0]->del!=0);
	if($rule=='enter'){
		if($zone_rules[0]->del!=0 || $zone_rules[0]->add!=0) return true;
		foreach($zone_rules AS $zr) if($zr->edit!=0) return true;
	}
	return false;
}

function check_prt($prt,$rule){
	global $db,$user;
	if($user->super==1) return true;
	getrow($db,"SELECT * FROM main_part WHERE part_id=$prt",1,"main_part",__LINE__,__FILE__);
	if($db->Record["part_auth"]==0) return false;
	if($db->Record["part_auth"]==-1) return true;
	else if($db->Record["part_auth"]!=$user->id) return false;
	return false;
}

function check_mod($module,$rule){
	global $mod_rules,$user;
	if($user->super==1) return true;
	if(empty($mod_rules)) perm_mod();
	if($rule=='view') return ($mod_rules[$module]->view!=0);
	if($rule=='edit') return ($mod_rules[$module]->edit!=0);
	if($rule=='add') return ($mod_rules[0]->add!=0);
	if($rule=='del') return ($mod_rules[0]->del!=0);
	return false;
}

function check_tbl($table,$rule,$module=0){
	global $table_rules,$table_rules2,$table_rules3,$user;
	if($user->super==1) return true;
	if(empty($table_rules)) perm_table();
	if($table==0 && $module!=0 && $rule=='view') return ($table_rules[-$module]->view!=0);
	if($rule=='view'){
		if(strstr($table,',')){
			if(isset($table_rules2[$table])) return ($table_rules2[$table]->view!=0);
			else {
				$t=explode(',',$table);
				$st='';
				for($i=0;$i<count($t)-1;$i++){
					if($i>0) $st.=',';
					$st.=$t[$i];
					//if(!check_tbl($st,$rule)) return false;
					//if($i==0 && !check_tbl($st,$rule)) echo $st;
					if($i==0 && !check_tbl($st,$rule)) return false;
					if($i>0 && isset($table_rules2[$st]) && $table_rules2[$st]->view==0) return false;
				}
				return true;
			}
		} else return ($table_rules[$table]->view!=0);
	}
	if($rule=='add'){
		return (isset($table_rules3[$table]) && $table_rules3[$table]->add==5);
	}
	return false;
}

function check_ex($ex,$rule,$module=0){
	global $ex_rules,$user;
	if($user->super==1) return true;
	if(empty($ex_rules)) perm_ex();
	if(!isset($ex_rules[$ex])) return true; //??
	if($rule=='view' && $module!=0) return ($ex_rules[-$module]->view!=0);
	if($rule=='edit' && $module!=0) return ($ex_rules[-$module]->edit!=0);
	if($rule=='view') return ($ex_rules[$ex]->view!=0);
	if($rule=='edit') return ($ex_rules[$ex]->edit!=0);
	if($rule=='add') return ($ex_rules[-$module]->add!=0);
	if($rule=='del' && empty($ex)) return ($ex_rules[-$module]->del!=0);
	if($rule=='del' && !empty($ex)) return ($ex_rules[$ex]->del!=0);
	return false;
}

function check_row2($table,$ex,$own,$force_ex){
	global $user;
	$b1=($table==2 && ($user->id==$own/* || $own==0*/)  || $table==1 || $table==-1);
	if($force_ex!=0) $b2=true; else
	$b2=($ex==2 && ($user->id==$own/* || $own==0*/)  || $ex==1 || $ex==-1);
	return ($b1 && $b2);
}

function seek_uchild($u,$target=-2,$collect=Array()){//если target=0 то возвращает предков
	global $db,$ucache2;
	if(isset($ucache2[$u])) $chlds=$ucache2[$u];
	else { $chlds=getall($db,"SELECT * FROM main_auth WHERE auth_owner=$u OR auth_owner LIKE '%,$u,%' OR auth_owner LIKE '$u,%' OR auth_owner LIKE '%,$u'",1,'main_auth'); $ucache2[$u]=$chlds;}
	//эту схему лучше заменить на таблицу родителей-детей
	foreach($chlds AS $ch){
		if(!isset($collect[$ch["auth_id"]])) $collect=seek_uchild($ch["auth_id"],-2,$collect);
		$collect[$ch["auth_id"]]=$ch;
	}
	if($target==-2){
		return $collect;
	} else {
		return isset($collect[$target]);
	}
}

function check_row2b($table,$own,$self=0){//$own - возможный родитель, $self - пользователь, который проверяется (self ещё не указан в проверках mod_table, но оно там и не нужно скорее всего)
	global $user;
	if(!strstr($own,',')){
		$b=($table==2 && ($user->id==$own)  || $table==1 || $table==-1 || ($table==3 && ($user->id==$self || $user->id==$own || seek_uchild($user->id,$own) )));
	} else {
		if($table==1 || $table==-1) return true;
		$own=explode(',',$own);
		$b=false;
		foreach($own AS $i=>$val){
			$b=(($table==2 && $user->id==$val) || ($table==3 && ($user->id==$self || $user->id==$val || seek_uchild($user->id,$val))));
			if($b) return true;
		}
	}
	return $b;
}

function check_row2c($rule,$owners){
	if(!check_row2b($rule->edit,$owners)) $rule->edit=0;
	if(!check_row2b($rule->view,$owners)) $rule->view=0;
	if(!check_row2b($rule->add,$owners)) $rule->add=0;
	if(!check_row2b($rule->del,$owners)) $rule->del=0;
	return $rule;
}

function check_row_st($st,$rule,$for_rules=0){
	global $row_rules3,$row_rules;
	if(empty($row_rules3) && empty($row_rules)) perm_row1();
	$r=new perm();
	if(isset($row_rules3[$st])){
		$r=$row_rules3[$st];
	}
	if($rule=='view') $res=$r->view;
	if($rule=='add') $res=$r->add;
	if($rule=='edit') $res=$r->edit;
	if($rule=='del') $res=$r->del;
	if($for_rules) return $res;
	else {
		if($res==0) return false;
		else return true;
	}
}

function get_st($owner,$table=0,$table2=0){
	global $rlink;
	$ltable=0;
	$st='';
	$owners=Array();
	while($owner!=0){
		seek_rlink($owner);
		if($rlink[$owner]->table!=$ltable){
			$ltable=$rlink[$owner]->table;
			//if(!empty($st)) $st.=',';
			//$st.=$ltable;
			$owners[]=$ltable;
		}
		$owner=$rlink[$owner]->owner;
	}
	if(!empty($owners)) for($i=count($owners)-1;$i>=0;$i--){
		if(!empty($st)) $st.=',';
		$st.=$owners[$i];
		$ltable=$owners[$i];
	}
	if($table!=$ltable && $table!=0){
		if(!empty($st)) $st.=',';
		$st.=$table;
		$ltable=$table;
	}
	if($table2!=$ltable && $table2!=0){
		if(!empty($st)) $st.=',';
		$st.=$table2;
	}
	return $st;
}

function check_row($row,$table,$ex,$rule,$usr=0,$usrs=Array(),$module=0,$for_rules=0){
	global $row_rules1,$row_rules2,$row_rules3,$user,$tatomo,$rlink,$db,$xgcache,$xgcache2;
	if($user->super==1){
		if($for_rules) return 1;
		return true;
	}
	if(empty($row_rules1) || empty($row_rules2)){
		perm_row1();//tables
		perm_row2();//ex's
	}
	//ищем родителей для строки
	$owners=Array();
	if($rule=='add' || $row==0){
		$owner=$row;
		$ct=$table;
		$row=0;
	} else {
		seek_rlink($row);
		$owner=$row;//$rlink[$row]->owner;
		$ct=$rlink[$row]->table;
	}
	while($owner!=0){
		seek_rlink($owner);
		$i=0;		
		$owners[]=$owner;
		$owner=$rlink[$owner]->owner;
	}
	$np=new perm();
	$first=true;
	$town='';
	for($i=count($owners)-1;$i>=0;$i--){
		//if($rule=='add') echo $rlink[$owners[$i]]->table.'<br>';
		$cr=$rlink[$owners[$i]];
		$cur_table=$cr->table;
		if(empty($ltable) || $cur_table!=$ltable){
			$np2=new perm();
			$ltable=$cur_table;
			if(empty($st)) $st=$ltable; else $st.=','.$ltable;
			if($first){//таблица		
				$table=$cur_table;
				if($table!=0) $module=$tatomo[$table]; else $module=0;
				$np2=dc($row_rules1[$table]);
			} else {
				//подтаблицы
				if(isset($row_rules3[$st])) $np2=dc($row_rules3[$st]);
			}
			//if($cr->id==81899){
			//	echo $np->edit.'<br>';
			//	exit;
			//}
			$np=merge_perm($np,$np2,1);
		}
		$own=$cr->user;
		if(!empty($cr->users)){
			if(!empty($own)) $own.=',';
			$own.=implode(',',$cr->users);
		}
		if(empty($fown)) $fown=$own;
		if(!empty($own)){
			if(!empty($town)) $town.=',';
			$town.=$own;
		}
		$np=check_row2c($np,$own);
		$first=false;
	}
	if($rule=='add' || $row==0){
		$ltable=$ct;
		if(!empty($st)){
			$st.=','.$ltable;
			if(isset($row_rules3[$st])){
				$np2=new perm();
				$np2=dc($row_rules3[$st]);
				$np=merge_perm($np,$np2,1);
			}
		}
	}
	if(empty($owners)){
		if(empty($table) && empty($module)){
			//для всех строк
			$np=$row_rules1[0];
		} else if(empty($table)){
			//для всех строк конкретного модуля
			$np=$row_rules1[-$module];
		} else {
			//дли всех строк конкретной таблицы
			$np=$row_rules1[$table];
		}
	}
	//экземпляр
	$force_ex=$ex==0;
	$table=$ct;
	if($table!=0) $module=$tatomo[$table]; else $module=0;
	if(!$force_ex && isset($row_rules2[$ex])){
		if($table!=0){
			if(!isset($xgcache[$table])){
				$tmp=getall($db,"SELECT * FROM ex_group WHERE ex_table=$table");
				$xgcache[$table]=Array();
				foreach($tmp AS $tm){
					$xgcache[$table][$tm["ex_ex1"]][]=$tm["ex_ex2"];
					$xgcache2[$table][$tm["ex_ex2"]]=$tm["ex_ex1"];
				}
			}
			if(isset($xgcache[$table])  && isset($xgcache2[$table][$ex]) && count($xgcache[$table])<count($xgcache2[$table])){
				$nd=new perm();
				$first=true;
				foreach($xgcache[$table][$xgcache2[$table][$ex]] AS $ex2){
					if($first){$nd=dc($row_rules2[$ex2]); $first=false;}
					else if($ex2!=-1) $nd=merge_perm($nd,$row_rules2[$ex2],6);
				}
				$np=merge_perm($np,$nd,1);
			} else {
				$np=merge_perm($np,$row_rules2[$ex],1);
			}
		} else {
			$np=merge_perm($np,$row_rules2[$ex]);
		}		
		$np=check_row2c($np,$town/*$fown*/);//town - все владельцы объекта и над-объектов, fown - только текущего объекта
		//if($row==43326) echo $fown;
	}
	//результаты
	if($for_rules){
		if($rule=='add')return $np->add;
		if($rule=='edit')return $np->edit;
		if($rule=='del')return $np->del;
		if($rule=='view')return $np->view;
	}
	//if($table==95){
	//	echo $np->add.'<br>';
	//}
	if($rule=='add') $res=$np->add;
	if($rule=='edit') $res=$np->edit;
	if($rule=='del') $res=$np->del;
	if($rule=='view') $res=$np->view;	
	if($for_rules) return $res;
	else {
		if($res==0) return false;
		//else if($res==2)  //if($row==43326){ echo $np->edit; exit;}
		else return true;
	}
	
	return true;
}

function check_rowB($row,$table,$ex,$rule,$own=0,$owns=Array(),$module=0,$for_rules=0){//for_rules - для раздела "права доступа"
	global $row_rules1,$row_rules2,$user,$tatomo,$rlink,$db,$xgcache,$xgcache2;
	if($row!=0) seek_rlink($row);
	if($own==0 && !empty($owns)) $own=implode(',',$owns); else $own.=','.implode(',',$owns);
	if($user->super==1){
		if($for_rules) return 1;
		return true;
	}
	if(empty($row_rules1)){
		perm_row1();//tables
		perm_row2();//ex's
		//perm_row();//extremal mode
	}	
	
	if($table==0 && $row!=0){
		seek_rlink($row);
		$table=$rlink[$row]->table;
	}
	
	//из-за этого участка при проверке кол-ва элементов подтаблиц может возникнуть избыточное кол-во SQL запросов
	//по этой причине, не выполняется проверка доступа при подсчёте кол-ва строк подтаблиц в админке
	if($own!=0){//глубинный поиск родителя
		$aown=Array();
		$aown[$own]=$table;
		$x=$row; if(empty($rlink[$x])) seek_rlink($x);
		while($rlink[$x]->owner!=0){
			$x=$rlink[$x]->owner;
			if(empty($rlink[$x])) seek_rlink($x);
			$aown[$rlink[$x]->user]=$rlink[$x]->table;
		}
	} else $aown[$own]=$table;
	
	/*if($row!=0){
		$aown=Array();
		$aown[0]->user=$row;
		$aown[0]->table=$table;
		$caown=1;
		$x=$row; if(empty($rlink[$x])) seek_rlink($x);
		while($rlink[$x]->owner!=0){
			$x=$rlink[$x]->owner;
			if(empty($rlink[$x])) seek_rlink($x);
			$aown[$caown]->user=$rlink[$x]->user;
			$aown[$caown]->table=$rlink[$x]->table;
			$caown++;
		}
		//arsort($aown);
	} else {
		$aown[0]->user=$own;
		$aown[0]->table=$table;
	}*/
	
	
	foreach($aown AS $own=>$table){
	//foreach($aown AS $cown){		
	//	$own=$cown->user;
	//	$table=$cown->table;
		$force_ex=$ex==0;
		if($module==0){ if($table!=0) $module=$tatomo[$table]; else $module=0; }
		$np=new perm();
		//$np=merge_perm($row_rules1[-$module],$row_rules1[$table],1);
		//if($rule=='add') echo $row_rules1[$table]->add.' '.$table;
		$np=dc($row_rules1[$table]);

		//if(!$force_ex && isset($row_rules2[$ex])) $np=merge_perm($np,$row_rules2[$ex]);
		//echo $table.' - '.$ex.'<br>'; ex это экземпляр модуля
		
		if(!$force_ex && isset($row_rules2[$ex])){
			if($table!=0){
				//$np=merge_perm($row_rules2[$ex],$np,1);//2 -- ??  эта строчка означает то, что доступы ЭКЗЕМПЛЯРОВ перекрывают доступ ТАБЛИЦ  !! впоследствии способ перекрытия изменил с 2 на 1, а после того как стало невозможно добавлять статьи в ньюартиклс поставил 0

				//конструкция ниже призвана отфильтровывать таблицы, которые единичны для всех экземпляров (например шаблоны)
				//её нужно заменить на проверку в таких таблицах доступ на все созданыне экземпляры (отталкиваясь от $ex)
				//чтобы если юзер имеет права на редактирование всех экземпляров в которых присутствует этот экземпляр таблицы то он мог ею пользоваться
				if(!isset($xgcache[$table])){
						//getrow($db,"SELECT count(*) AS x FROM ex_group WHERE ex_table=$table GROUP BY ex_ex1");
						//$xgcache[$table]=$db->Record["x"];
						$tmp=getall($db,"SELECT * FROM ex_group WHERE ex_table=$table");
						$xgcache[$table]=Array();
						foreach($tmp AS $tm){
							$xgcache[$table][$tm["ex_ex1"]][]=$tm["ex_ex2"];
							$xgcache2[$table][$tm["ex_ex2"]]=$tm["ex_ex1"];
							//echo $tm["ex_ex2"].'<br>';
						}
				}
				//if($xgcache[$table]>1) $force_ex=1;
				if(isset($xgcache[$table])  && isset($xgcache2[$table][$ex]) && count($xgcache[$table])<count($xgcache2[$table])){
					$nd=new perm();
					$first=true;
					foreach($xgcache[$table][$xgcache2[$table][$ex]] AS $ex2){
						if($first){$nd=dc($row_rules2[$ex2]); $first=false;}
						else $nd=merge_perm($nd,$row_rules2[$ex2],6);
					}
					$np=merge_perm($np,$nd,1);
				} else {						
					$np=merge_perm($np,$row_rules2[$ex],1);
				}
			} else {
				$np=merge_perm($np,$row_rules2[$ex]);//(помогло) непонятно поможет ли это (это на случай если проверяется возможность изменения данных для конкретного экземпляра
			}
		}
		if($for_rules){
			if($rule=='add')return $np->add;
			if($rule=='edit')return $np->edit;
			if($rule=='del')return $np->del;
			if($rule=='view')return $np->view;
		}
		if($rule=='add') $res=check_row2b($np->add,0);
		if($rule=='edit') $res=check_row2b($np->edit,$own);
		if($rule=='del') $res=check_row2b($np->del,$own);
		if($rule=='view') $res=check_row2b($np->view,$own);
		if($res) return true;
	}
	return false;
	
	/*
	if($rule=='add') return check_row2($row_rules1[$table]->add,$row_rules2[$ex]->add,0,$force_ex);
	if($rule=='edit') return check_row2($row_rules1[$table]->edit,$row_rules2[$ex]->edit,$own,$force_ex);
	if($rule=='del') return check_row2($row_rules1[$table]->del,$row_rules2[$ex]->del,$own,$force_ex);
	if($rule=='view') return check_row2($row_rules1[$table]->view,$row_rules2[$ex]->view,$own,$force_ex);
	*/
}

function check_operation($cmd,$row,$owner_row,$module_ex,$table){
	global $rlink;
	if($cmd=='edit' || $cmd=='del' || $cmd=='view'){
		seek_rlink($row);
		//return check_row($row,$table,$module_ex,'edit',$rlink[$row]->user,$rlink[$row]->users,0,0);
		return check_row($row,$table,$module_ex,'edit',0,Array(),0,0);
	}
	if($cmd=='add'){
		if(check_tbl($table,'add')) return true;
		if(empty($owner_row)){
			return check_row(0,$table,$module_ex,'add');
		} else {
			seek_rlink($owner_row);
			//return check_row($owner_row,$table,$module_ex,'add',$rlink[$row]->user,$rlink[$row]->users,0,0,$table) || check_row($owner_row,$table,$module_ex,'edit',$rlink[$owner_row]->user,$rlink[$owner_row]->users,0,0,$table);
			return /*check_row($owner_row,$table,$module_ex,'add',0,Array(),0,0) || */ check_row($owner_row,$table,$module_ex,'edit',$rlink[$owner_row]->user,$rlink[$owner_row]->users,0,0);
		}
	}
	return true;
}

function check_col($col,$rule){
	global $col_rules,$user;
	if($user->super==1) return true;
	if(empty($col_rules)) perm_col();
	if($rule=='view') return ($col_rules[$col]->view!=0);
	if($rule=='edit') return ($col_rules[$col]->edit!=0);
	return false;
}

function check_user($group,$rule,$own=0,$force=0,$for_rules=0,$module=0){//force необходима для сверки владеет ли юзер own пользователем -group
	global $user_rules,$user,$link_cache;
	if($user->super==1){
		if($for_rules) return 1;
		return true;
	}
	if(empty($user_rules)) perm_user();
	if($group>0 && !isset($user_rules[$group])) return true;
	if($own==0 && $group>=0){ //как группу
		if($group==0 && $module!=0) $group=-$module;
		if($for_rules){
			if($rule=='reg') return $user_rules[$group]->reg;
			if($rule=='view') return $user_rules[$group]->view;
			if($rule=='del') return $user_rules[$group]->del;
			if($rule=='edit') return $user_rules[$group]->edit;
			if($rule=='rules') return $user_rules[$group]->rules;
		} else if($rule=='reg'){
			$reg_stop=false;
			foreach($user_rules AS $r){
				if($r->reg==1) return true;
				if($r->reg==0) $reg_stop=true;
			}
			return !$reg_stop;
		}
		//if($rule=='reg') return ($user_rules[$group]->reg!=0);
		if($rule=='view') return ($user_rules[$group]->view!=0);
		if($rule=='del') return ($user_rules[$group]->del!=0);
		if($rule=='edit') return ($user_rules[$group]->edit!=0);
		if($rule=='rules') return ($user_rules[$group]->rules!=0);
	} else { // как юзера
		if($group<0){
			if($force){
				return check_row2b(2,$own,-$group);
			}
			global $db;
			//$lnks=getall($db,"SELECT * FROM auth_link WHERE link_user=".(-$group),1,"auth_link",__LINE__,__FILE__);
			if(!isset($link_cache[-$group])) $link_cache[-$group]=getall($db,"SELECT * FROM auth_link WHERE link_user=".(-$group),1,"auth_link",__LINE__,__FILE__);
			$lnks=$link_cache[-$group];
			if(!empty($lnks)) foreach($lnks AS $lnk){
				if($rule=='view' && check_row2b($user_rules[$lnk["link_group"]]->view,$own,-$group)) return true;
				if($rule=='del' && check_row2b($user_rules[$lnk["link_group"]]->del,$own,-$group)) return true;
				if($rule=='edit' && check_row2b($user_rules[$lnk["link_group"]]->edit,$own,-$group)) return true;
				if($rule=='rules' && check_row2b($user_rules[$lnk["link_group"]]->rules,$own,-$group)) return true;
			}
			if(empty($lnks)){
				//return check_row2b(2,$own);
				if($rule=='view') return check_row2b($user_rules[0]->view,$own,-$group);
				if($rule=='del') return check_row2b($user_rules[0]->del,$own,-$group);
				if($rule=='edit') return check_row2b($user_rules[0]->edit,$own,-$group);
				if($rule=='rules') return check_row2b($user_rules[0]->rules,$own,-$group);
			}
			return false;
		}
		if($for_rules){
			if($rule=='view')return $user_rules[$group]->view;
			if($rule=='edit')return $user_rules[$group]->edit;
			if($rule=='del')return $user_rules[$group]->del;
			if($rule=='rules')return $user_rules[$group]->rules;
		}
		if($rule=='view') return check_row2b($user_rules[$group]->view,$own,-$group);
		if($rule=='del') return check_row2b($user_rules[$group]->del,$own,-$group);	
		if($rule=='edit') return check_row2b($user_rules[$group]->edit,$own,-$group);	
		if($rule=='rules') return check_row2b($user_rules[$group]->rules,$own,-$group);
	}
	return false;
}

function check_group($group,$rule,$for_rules=0,$module=0){
	global $group_rules,$group_rules2,$user,$cg_cache,$db;
	if($user->super==1){
		if($for_rules) return 1;
		return true;
	}
	if(empty($group_rules)) perm_group();
	if(empty($group) && !empty($module)){
		$group=-$module;
	}
	if(!isset($group_rules[$group])) return true;
	if(empty($cg_cache)) $cg_cache=getall4($db,"SELECT * FROM main_auth WHERE auth_type=1","auth_id");
	if($for_rules){
		if($rule=='view') $res=$group_rules[$group]->view;
		if($rule=='add') $res=$group_rules[$group]->add;	
		if($rule=='edit') $res=$group_rules[$group]->edit;	
		if($rule=='del') $res=$group_rules[$group]->del;	
		if($rule=='rules') $res=$group_rules[$group]->rules;	
		if($rule=='invite') $res=$group_rules[$group]->invite;
		if($rule=='leave') $res=$group_rules[$group]->leave;
		return $res;
	}
	if($rule=='view') $res=$group_rules[$group]->view;
	if($rule=='add') $res=$group_rules[$group]->add;	
	if($rule=='edit') $res=$group_rules[$group]->edit;	
	if($rule=='del') $res=$group_rules[$group]->del;	
	if($rule=='rules') $res=$group_rules[$group]->rules;	
	if($rule=='invite') $res=$group_rules[$group]->invite;
	if($rule=='leave') $res=$group_rules[$group]->leave;
	//return $res!=0;
	if(isset($cg_cache[$group]["auth_owner"])) $owner=$cg_cache[$group]["auth_owner"]; else $owner=0;
	return check_row2b($res,$owner,$group);
}

function srool($perm,$rule){
	if($rule=='view') return $perm->view;
	if($rule=='edit') return $perm->edit;
	if($rule=='add') return $perm->add;
	if($rule=='del') return $perm->del;
	if($rule=='control') return $perm->control;
	if($rule=='rules') return $perm->rules;
	if($rule=='upload') return $perm->upload;
	if($rule=='reg') return $perm->reg;
	if($rule=='invite') return $perm->invite;
	if($rule=='leave') return $perm->leave;
	if($rule=='unreg') return $perm->unreg;
}

function check_folder($folder,$rule){
	global $folder_rules,$user,$afolder;
	if($user->super==1){
		$afolder='files';
		$user->folder=$afolder;
		return true;
	}
	if(!isset($folder_rules)) perm_folder();
	if(!isset($folder_rules[$folder]) || srool($folder_rules[$folder],$rule)==-1){
		while(strstr($folder,'/')){
			$d=explode('/',$folder);$folder=$d[0];
			for($i=1;$i<count($d)-1;$i++){
				$folder.='/'.$d[$i];
			}
			if(isset($folder_rules[$folder]) && srool($folder_rules[$folder],$rule)!=-1) break;
		}
	}
	if(!isset($folder_rules[$folder]) || srool($folder_rules[$folder],$rule)==-1){
		$folder='0';
	}
	if(!isset($folder_rules[$folder])) return true;//false;
	if($rule=='view') return ($folder_rules[$folder]->view!=0);
	if($rule=='edit') return ($folder_rules[$folder]->edit!=0);
	if($rule=='del') return ($folder_rules[$folder]->del!=0);
}

function del_perm($id,$target,$type,$object=0,$subtable=''){
	global $db;
	$sql_add='';
	if($object!=0) $sql_add=' AND perm_object='.$object;
	if($subtable!=0) $sql_add=' AND perm_subtable="'.$subtable.'"';
	$db->query("DELETE FROM auth_perm WHERE perm_auth=$id AND perm_target=$target AND perm_type=$type".$sql_add,2,"auth_perm");
}

function del_perm_subtables($auth,$fmod,$table,$exists,$owners){
	global $stables,$tables;
	$exists[$table]=1;
	foreach($stables[$table] AS $tbl)if(empty($exists[$tbl])){
		$owners2=$owners;
		$owners2[]=$tbl;
		$st=implode(',',$owners2);
		del_perm($auth,7,4,0,$st);
		del_perm($auth,7,3,0,$st);
		$tmp=Array();
		if(!empty($stables[$tbl])) foreach($stables[$tbl] AS $tbl2) if(!in_array($tbl2,$owners)) $tmp[]=$tbl;
		if(!empty($tmp)){
			del_perm_subtables($auth,$fmod,$tbl,$exists2,$owners2);
		}
	}
}

function del_current_perm($auth,$fmod,$delex=true){
	global $db;
	$id=$auth;
	del_perm($id,1,1,$fmod);
	del_perm($id,3,2,$fmod);
	del_perm($id,3,3,$fmod);
	del_perm($id,3,6,$fmod);
	del_perm($id,3,8,$fmod);
	del_perm($id,5,4,$fmod);	
	//foreach(get_crossmodule_subtable($fmod) AS $tbl=>$tmp){
	foreach(getall3($db,"SELECT table_id, table_module FROM main_table WHERE table_module=$fmod","table_id") AS $tbl){
		del_perm($id,3,4,$tbl);
		del_perm($id,1,3,$tbl);
		del_perm($id,7,4,$tbl);
		del_perm($id,7,3,$tbl);
		del_perm_subtables($auth,$fmod,$tbl,Array(),Array($tbl));	}
	foreach(getall3($db,"SELECT col_id, col_module, col_table FROM main_col WHERE col_module=$fmod AND col_table>0","col_id") AS $col) del_perm($id,1,7,$col);
	if($delex){
		foreach(getall3($db,"SELECT ex_id, ex_module FROM ex_module WHERE ex_module=$fmod","ex_id") AS $ex){
			del_perm($id,1,2,$ex);
			del_perm($id,4,2,$ex);
		}
	}
	foreach(getall3($db,"SELECT auth_id, auth_type, group_module FROM main_auth WHERE auth_type=1 AND group_module=$fmod","auth_id") AS $grpi){
		del_perm($id,1,6,$grpi);
		del_perm($id,1,8,$grpi);
	}
}

function set_root($auth_id,$perm_type,$perm_target,$perm_object,$data){
	global $db;
	if(!is_object($data)) $perm=unserialize($data);
	else /*if(isset($perm->add)) */$perm=$data;
	if(!is_object($perm)) return false;
	if($perm->add!=-1 || $perm->edit!=-1 || $perm->view!=-1 || $perm->del!=-1 || $perm->invite!=-1 || $perm->upload!=-1 || $perm->leave!=-1 || $perm->rules!=-1 || $perm->reg!=-1){
	
		/*if(!isset($perm->add)) $perm->add=-1;
		if(!isset($perm->view)) $perm->view=-1;
		if(!isset($perm->edit)) $perm->edit=-1;
		if(!isset($perm->del)) $perm->del=-1;
		if(!isset($perm->invite)) $perm->invite=-1;
		if(!isset($perm->upload)) $perm->upload=-1;
		if(!isset($perm->leave)) $perm->leave=-1;
		if(!isset($perm->rules)) $perm->rules=-1;
		if(!isset($perm->reg)) $perm->reg=-1;		

		if($perm->add==='') $perm->add=-1;
		if($perm->view==='') $perm->view=-1;
		if($perm->edit==='') $perm->edit=-1;
		if($perm->del==='') $perm->del=-1;
		if($perm->invite==='') $perm->invite=-1;
		if($perm->upload==='') $perm->upload=-1;
		if($perm->leave==='') $perm->leave=-1;
		if($perm->rules==='') $perm->rules=-1;
		if($perm->reg==='') $perm->reg=-1;*/
		
		if(empty($perm->add)) $perm->add='0';
		if(empty($perm->view)) $perm->view='0';
		if(empty($perm->edit)) $perm->edit='0';
		if(empty($perm->del)) $perm->del='0';
		if(empty($perm->invite)) $perm->invite='0';
		if(empty($perm->upload)) $perm->upload='0';
		if(empty($perm->leave)) $perm->leave='0';
		if(empty($perm->rules)) $perm->rules='0';
		if(empty($perm->reg)) $perm->reg='0';		
	
		if($perm_target!=7){
			$db->query("INSERT INTO auth_perm (perm_auth, perm_target, perm_type, perm_object, perm_add, perm_edit, perm_view, perm_del, perm_invite, perm_upload, perm_leave, perm_rules, perm_reg)
				VALUES($auth_id, $perm_target, $perm_type, $perm_object, $perm->add, $perm->edit, $perm->view, $perm->del, $perm->invite, $perm->upload, $perm->leave, $perm->rules, $perm->reg)",3,"auth_perm");
		} else {
			$db->query("INSERT INTO auth_perm (perm_auth, perm_target, perm_type, perm_subtable, perm_add, perm_edit, perm_view, perm_del, perm_invite, perm_upload, perm_leave, perm_rules, perm_reg)
				VALUES($auth_id, $perm_target, $perm_type, '$perm_object', $perm->add, $perm->edit, $perm->view, $perm->del, $perm->invite, $perm->upload, $perm->leave, $perm->rules, $perm->reg)",3,"auth_perm");
		}
	}
}

function replace_perm($from,$to,$target,$type,$auth){
	global $db;
	if(empty($from)) return false;
	getrow($db,"SELECT * FROM auth_perm WHERE perm_auth=$auth AND perm_target=$target AND perm_type=$type AND perm_object=$from");
	$np=to_perm(recheck_perm($db->Record));
	set_root($auth,$type,$target,$to,$np);
}

//В будущем надо объеденить replace_current_perm и add_perms, т.к. это одно и тоже за исключением auth_to

function replace_current_perm($from,$to,$auth){//заменяет доступы у модуля to на доступы модуля from у группы auth
									  //функция требуется только для обновления модулей для синхронизации между ними 0ой группы (по умолчанию)
	global $db;
	if(empty($from)) return false;
	$fmod=$from;
	replace_perm($from,$to,1,1,$auth);
	replace_perm($from,$to,3,2,$auth);
	replace_perm($from,$to,3,3,$auth);
	replace_perm($from,$to,3,6,$auth);
	replace_perm($from,$to,3,8,$auth);
	replace_perm($from,$to,5,4,$auth);
	foreach(getall4($db,"SELECT table_id, table_module, table_uin FROM main_table WHERE table_module=$from","table_id") AS $tbl){
		$tbl2=getrowval("SELECT table_id, table_module, table_sname, table_uin FROM main_table WHERE table_module=$to AND table_uin='".$tbl["table_uin"]."' AND table_id!=".$tbl["table_id"],"table_id");
		replace_perm($tbl["table_id"],$tbl2,3,4,$auth);
		replace_perm($tbl["table_id"],$tbl2,1,3,$auth);
		replace_perm($tbl["table_id"],$tbl2,7,4,$auth);
		replace_perm($tbl["table_id"],$tbl2,7,3,$auth);
	}
	foreach(getall4($db,"SELECT col_id, col_module, col_table, col_uin FROM main_col WHERE col_module=$fmod AND col_table>0","col_id") AS $col){
		$col2=getrowval("SELECT col_id, col_table, col_module, col_uin FROM main_col WHERE col_module=$to AND col_uin='".$col["col_uin"]."' AND col_id!=".$col["col_id"],"col_id");
		replace_perm($col["col_id"],$col2,1,7,$auth);
	}
	/*
	секция замены доступам к отдельным экземплярам по понятным причинам упразднена
	*/
	foreach(getall4($db,"SELECT auth_id, auth_type, group_module, group_uin FROM main_auth WHERE auth_type=1 AND group_module=$fmod","auth_id") AS $grpi){
		$grpi2=getrowval("SELECT auth_id, auth_type, group_module, group_uin FROM main_auth WHERE group_module=$to AND auth_id!=".$grpi["auth_id"]." AND group_uin='".$grpi["group_uin"]."'","auth_id");
		replace_perm($grpi["auth_id"],$grpi2,1,6,$auth);
		replace_perm($grpi["auth_id"],$grpi2,1,8,$auth);
	}
}

function add_perm($from,$to,$target,$type,$auth_from,$auth_to){
	global $db;
	if(empty($auth_from)) return false;
	getrow($db,"SELECT * FROM auth_perm WHERE perm_auth=$auth_from AND perm_target=$target AND perm_type=$type AND perm_object=$from");
	$np=to_perm(recheck_perm($db->Record));
	//var_dump($np);
	//echo '<br><br><br>';
	set_root($auth_to,$type,$target,$to,$np);
}

function add_perms($module_from,$module_to,$auth_from,$auth_to){
	global $db;
	//echo $module_from.' = '.$module_to.'<br>'.$auth_from.' = '.$auth_to.'<br>';
	add_perm($module_from,$module_to,1,1,$auth_from,$auth_to);
	add_perm($module_from,$module_to,3,2,$auth_from,$auth_to);
	add_perm($module_from,$module_to,3,3,$auth_from,$auth_to);
	add_perm($module_from,$module_to,3,6,$auth_from,$auth_to);
	add_perm($module_from,$module_to,3,8,$auth_from,$auth_to);
	add_perm($module_from,$module_to,5,4,$auth_from,$auth_to);
	foreach(getall4($db,"SELECT table_id, table_module, table_uin FROM main_table WHERE table_module=$module_from","table_id") AS $tbl){
		$tbl2=getrowval("SELECT table_id, table_module, table_sname, table_uin FROM main_table WHERE table_module=$module_to AND table_uin='".$tbl["table_uin"]."' AND table_id!=".$tbl["table_id"],"table_id");
		add_perm($tbl["table_id"],$tbl2,3,4,$auth_from,$auth_to);
		add_perm($tbl["table_id"],$tbl2,1,3,$auth_from,$auth_to);
		add_perm($tbl["table_id"],$tbl2,7,4,$auth_from,$auth_to);
		add_perm($tbl["table_id"],$tbl2,7,3,$auth_from,$auth_to);
	}
	foreach(getall4($db,"SELECT col_id, col_module, col_table, col_uin FROM main_col WHERE col_module=$module_from AND col_table>0","col_id") AS $col){
		$col2=getrowval("SELECT col_id, col_table, col_module, col_uin FROM main_col WHERE col_module=$module_to AND col_uin='".$col["col_uin"]."' AND col_id!=".$col["col_id"],"col_id");
		add_perm($col["col_id"],$col2,1,7,$auth_from,$auth_to);
	}
	/*
	секция замены доступам к отдельным экземплярам по понятным причинам упразднена
	*/
	foreach(getall4($db,"SELECT auth_id, auth_type, group_module, group_uin FROM main_auth WHERE auth_type=1 AND group_module=$module_from","auth_id") AS $grpi){
		$grpi2=getrowval("SELECT auth_id, auth_type, group_module, group_uin FROM main_auth WHERE group_module=$module_to AND auth_id!=".$grpi["auth_id"]." AND group_uin='".$grpi["group_uin"]."'","auth_id");
		add_perm($grpi["auth_id"],$grpi2,1,6,$auth_from,$auth_to);
		add_perm($grpi["auth_id"],$grpi2,1,8,$auth_from,$auth_to);
	}
}

unset($su_login);
unset($su_pwl);

?>