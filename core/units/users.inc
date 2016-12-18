<?php

function user_reg($login,$pwl,$name,$email,$group='',$date_leave='',$auth=0,$long_session=0,$user_fixedip='',$user_pwlcode=0,$module=0,$owners=Array(),$return_user=false,$session_lifetime=-1,$session_multy=-1){
	global $db,$user,$su_login;
	$GLOBALS["lu"]=0;
	$login=safe_sql_input($login);
	$pwl=safe_sql_input($pwl,1);	
	$email=safe_sql_input($email);
	if(!check_user(0,'reg',0,0,0,$module)) return 0;
	getrow($db,"SELECT * FROM main_auth WHERE auth_type=0 AND user_login='$login'",1,"main_auth");
	if(empty($su_login)) $su_login=cfg_extract('su_login');
	if(!empty($db->Record) || (!empty($su_login) && $su_login==$login)){
		unset($su_login);
		return 2;
	} else {
		$own=Array();
		if(!empty($owners)){
			foreach($owners AS $o){
				if(is_object($o)) $own[$o->id]=$o->id;
				else $own[$o]=$o;
			}
		}
		$own=implode(',',$own);
		if(empty($own)) $own=$user->id;
		if($user_pwlcode) $pwl=md5(md5($pwl).get_protection_code());
		$db->query("INSERT INTO main_auth (auth_type, auth_owner, auth_date, user_login, user_pwl, user_fixedip, user_email, user_pwlcode, user_name, user_lastlogin, session_lifetime, session_multy)
				VALUES (0, '".$own."', '".date('Y-m-d')."', '$login', '$pwl', '".$user_fixedip."', '$email', $user_pwlcode, '$name', '".get_min(date('Y-m-d H:i:s'))."', $session_lifetime, $session_multy)",3,'main_auth');
		getrow($db,"SELECT LAST_INSERT_ID() as sid");
		$sid=$db->Record["sid"];
		$GLOBALS["lu"]=$sid;
		if(!empty($group)){
			if(!user_invite($group,$date_leave,'','',$sid)) return 0;
		}
		if($auth) user_auth($login,$pwl,$long_session);
		unset($su_login);
		if($return_user){
			$result->type='usr';
			$result->id=$sid;
			return $result;
		} else return 1;
	}
}

function user_invite($gname,$date_leave='',$login='',$pwl='',$uid=0){
	global $db;
	global $user,$mod_rules,$zone_rules,$table_rules,$ex_rules,$row_rules1,$row_rules2,$col_rules,$user,$link_cache;
	unset($link_cache); unset($mod_rules); unset($zone_rules); unset($table_rules); unset($folder_rules); unset($ex_rules); unset($row_rules1); unset($row_rules2); unset($col_rules); unset($user_rules); unset($group_rules); unset($user->group);
	if($uid==0 && $login=='') $u=$user->id;
	if(!empty($login)){
		getrow($db,"SELECT * FROM main_auth WHERE auth_type=0 AND user_login='$login'",1,"main_auth");
		if(!empty($db->Record)){
			if(($db->Record["user_pwlcode"]==0 && $db->Record["user_pwl"]==$pwl)
			||($db->Record["user_pwlcode"]==1 && $db->Record["user_pwl"]==md5(md5($pwl).get_protection_code()))){
				$u=$db->Record["auth_id"];
			}
		}
	}
	if(!empty($uid)) $u=$uid;
	if(empty($u) || $u==0 || $u==-1) return 0;
	getrow($db,"SELECT * FROM main_auth WHERE auth_type=1 AND (group_name='$gname' OR group_sname='$gname')",1,"main_auth");
	if(empty($db->Record)) return 0;
	$group=$db->Record["auth_id"];
	if(check_group($group,'invite')/* && check_user(-$u,'view')*/){//вторую проверку решено снести, т.к. если юзер только зарегистрировался, то права проверяются от прав по умолчанию
		if($date_leave=='') $date_leave='0000-00-00';
		$db->query("INSERT INTO auth_link (link_user, link_group, link_expire, link_date, link_invite)
				VALUES ($u, $group, '$date_leave', '".date('Y-m-d')."', ".$user->id.")",3,"auth_link");	
		return 1;
	}
	return 0;
}

function invite_user($group_id,$user_id,$skip_user_check=0,$module=0,$date='0000-00-00'){
	global $user,$db;
	if(check_group($group_id,'invite',0,$module) && ($skip_user_check || check_user(-$user_id,'view',0,0,0,$module))){		
		getrow($db,"SELECT * FROM auth_link WHERE link_user=$user_id AND link_group=$group_id");
		if(empty($db->Record))
		$db->query("INSERT INTO auth_link (link_user, link_group, link_expire, link_date, link_invite)
			VALUES ($user_id, $group_id, '$date', '".date('Y-m-d')."', ".$user->id.")",3,"auth_link");
		if($user_id==$user->id){
			flush_perm();
			load_base($user->id,1);
		}
	}
}

function leave_user($group_id, $user_id, $skip_user_check=0, $module=0){
	global $user,$db;
	if((check_group($group_id,'leave',0,$module) && ($skip_user_check || check_user(-$user_id,'view',0,0,0,$module))) || $user->id==$user_id){
		$db->query("DELETE FROM auth_link WHERE link_user=$user_id AND link_group=$group_id");
		if($user_id==$user->id){
			flush_perm();
			load_base($user->id,1);
		}
	}
}

function auth($login,$pwl,$remember_me=0){
	global $db,$su_login,$su_pwl, $link_cache,$system_email;
	global $user,$mod_rules,$zone_rules,$table_rules,$ex_rules,$row_rules1,$row_rules2,$col_rules,$user,$link_cache;
	unset($link_cache); unset($mod_rules); unset($zone_rules); unset($table_rules); unset($folder_rules); unset($ex_rules); unset($row_rules1); unset($row_rules2); unset($col_rules); unset($user_rules); unset($group_rules); unset($user->group);
	if(empty($su_login)) $su_login=cfg_extract('su_login');
	if(empty($su_pwl)) $su_pwl=cfg_extract('su_pwl');
	
	$auth_err=1;//1 - всё ок
	$login=safe_sql_input($login,1);
	//$pwl=safe_sql_input($pwl);
	getrow($db,"SELECT * FROM main_auth WHERE auth_type=0 AND user_login='$login' AND auth_enable=1",1,"main_auth",__LINE__,__FILE__);
	//if(empty($db->Record)){
	$do_auth=false;
	if(!empty($su_pwl) && $su_login==$login && $su_pwl==$pwl/*  && check_ip($db->Record["user_fixedip"],$user->ip)*/){
		/*$user_hook=md5(md5($pwl).get_protection_code());
		if(empty($remember_me)){
			SetCookie("user_hook",$user_hook,0,'/','',false,1);
			SetCookie("user_login",$login,0,'/','',false,1);
		}else{
			SetCookie("user_hook",$user_hook,time()+3600*24*7,'/','',false,1);
			SetCookie("user_login",$login,time()+3600*24*7,'/','',false,1);
		}*/
		$user->login=$login;
		$user->email=$system_email;
		$user->name='SuperUser';
		$user->super=1;
		$user->id=-1;
		$do_auth=true;
	} else if(empty($db->Record)) $auth_err='Пользователь с таким логином не существует или заблокирован';
	//} else {
	if(!empty($db->Record) && (!isset($user->login) || !$do_auth)){
		//echo $pwl.' - '.db->Record["user_pwl"].' 123';
		$auth=$db->Record;
		if(($db->Record["user_pwlcode"]==0 && $db->Record["user_pwl"]==$pwl)
		||($db->Record["user_pwlcode"]==1 && $db->Record["user_pwl"]==md5(md5($pwl).get_protection_code()))){
		if(check_ip($db->Record["user_fixedip"],$user->ip)){
			/*$user_hook=md5(md5($db->Record["user_pwl"]).get_protection_code());
			if(empty($remember_me)){
				SetCookie("user_hook",$user_hook,0,'/','',false,1);
				SetCookie("user_login",$login,0,'/','',false,1);
			}else{
				SetCookie("user_hook",$user_hook,time()+3600*24*7,'/','',false,1);
				SetCookie("user_login",$login,time()+3600*24*7,'/','',false,1);
			}*/
			$do_auth=true;
			$user->id=$db->Record["auth_id"];
			$user->login=$db->Record["user_login"];
			$user->email=$db->Record["user_email"];
			$user->name=$db->Record["user_name"];
			$user->super=0;
		} else $auth_err='Ошибка доступа';
		} else $auth_err='Пароль не верен';
	}
	unset($su_login);
	unset($su_pwl);
	// Запись сессии
	if($do_auth && !empty($user->id)){
		// Обновляем пользователя
		if(!$user->super){
				//if(!empty($_SERVER["REMOTE_ADDR"]) && $db->Record["user_ip"]!=$_SERVER["REMOTE_ADDR"]) $db->query("UPDATE main_auth SET user_ip='".$_SERVER["REMOTE_ADDR"]."' WHERE auth_id=".$db->Record["auth_id"],3,"main_auth");
				$db->query("UPDATE main_auth SET user_ip='".$user->ip."', user_lastlogin=".$GLOBALS["cur_min"]." WHERE auth_id=".$user->id,3,"main_auth");
		}
		// Генерируем хеш
		$session_hash=uuin();
		
		global $session_lifetime, $session_multy, $session_storage;
		if(!empty($auth)){
			global $link_cache;
			$link_cache[$user->id]=getall($db,"SELECT * FROM auth_link WHERE link_user=".$user->id." AND (link_expire='0000-00-00' OR link_expire>'".date('Y-m-d')."')",1,"auth_link",__LINE__,__FILE__);
			if($auth['session_lifetime']!='' && $auth['session_lifetime']!='-1') $session_lifetime=$auth['session_lifetime'];
			else {
				$rewrite_lifetime=-1;
				if(!empty($link_cache[$user->id])) foreach($link_cache[$user->id] AS $a){
					if(!isset($a['session_lifetime'])) $a['session_lifetime']=-1;
					if($a['session_lifetime']==0 || $a['session_lifetime']>$rewrite_lifetime && $rewrite_lifetime!=0) $rewrite_lifetime=$a['session_lifetime'];
				}
				if($rewrite_lifetime!=-1) $session_lifetime=$rewrite_lifetime;
			}
			if($auth['session_multy']!='' && $auth['session_multy']!='-1') $session_multy=$auth['session_multy'];
			else {
				$rewrite_multy=-1;
				if(!empty($link_cache[$user->id])) foreach($link_cache[$user->id] AS $a){
					if(!isset($a['session_multy'])) $a['session_multy']=-1;
					if($a['session_multy']==0 && $rewrite_multy==-1) $rewrite_multy=0;
					if($a['session_multy']==1) $rewrite_multy=1;
				}
				if($rewrite_multy!=-1) $session_multy=$rewrite_multy;
			}
		}
		if(!empty($session_lifetime)) $session_over=$GLOBALS['cur_min']+$session_lifetime;
		else $session_over=0;
		
		// Удаляем остальные сессии, если нет поддержки мультисессии
		if(!$session_multy){
			//$db->query("DELETE FROM auth_session WHERE session_auth=".$user->id);
			global $session_storage;
			if($session_storage==0){
				$db->query("DELETE FROM auth_session WHERE session_active=1 AND session_auth=".$user->id);
			} else {
				$db->query("UPDATE auth_session SET session_active=0 WHERE session_active=1 AND session_auth=".$user->id);
				if($session_storage>0){
					$cnt=getrowval("SELECT count(*) AS cnt FROM auth_session WHERE session_auth=".$user->id." AND session_active=0","cnt");
					$cnt++;
					if($cnt>=$session_storage) $db->query("DELETE FROM auth_session WHERE session_auth=".$user->id." AND session_active=0 ORDER BY session_last LIMIT ".($cnt-$session_storage));
				}
			}
		}
		
		// Удаляем старые сессии
		if($session_storage>=0){
			$sql_add='';
			if($session_storage>0){
				$cnt=getrowval("SELECT count(*) AS cnt FROM auth_session WHERE session_auth=".$user->id." AND session_active=0","cnt");
				$cnt++;
				if($cnt>=$session_storage) $sql_add=" ORDER BY session_last LIMIT ".($cnt-$session_storage);
			}
			if($session_storage==0 || $cnt>=$session_storage){
				$db->query("DELETE FROM auth_session WHERE session_auth=".$user->id." AND (session_active=0 OR (session_over!=0 AND session_over<=".$GLOBALS["cur_min"]."))".$sql_add);
			}
		}
		
		// Создаём сессию
		$db->query("INSERT INTO auth_session (session_auth, session_hash, session_ip, session_start, session_last, session_active, session_over)
								VALUES (".$user->id.", '$session_hash', '".$user->ip."', '".$GLOBALS['timestamp']."', '".$GLOBALS['timestamp']."', 1, $session_over)");
								
		$user->session=getrowval("SELECT LAST_INSERT_ID() as sid","sid");
		
		// Выдаём Cookies
		$uid_hash=md5($user->id.get_protection_code());
		if(empty($remember_me)){
			$user->remember_me=0;
			SetCookie("user_hook",$session_hash,0,'/','',false/*,true*/,1);
			SetCookie("user_id",$uid_hash,0,'/','',false/*,true*/,1);
		}else{
			$user->remember_me=1;
			SetCookie("user_hook",$session_hash,time()+3600*24*7,'/','',false/*,true*/,1);
			SetCookie("user_id",$uid_hash,time()+3600*24*7,'/','',false/*,true*/,1);
		}
	}
	return $auth_err;
}

function user_force_auth($user_id,$long_session=1){
	force_auth($user_id,$long_session);
}

function force_auth($user_id,$long_session=1){
	global $db;
	getrow($db,"SELECT * FROM main_auth WHERE auth_type=0 AND auth_id=$user_id",1,"main_auth");		
	if(empty($db->Record)) return false;
	if($db->Record["user_pwlcode"]==1) return false;
	$login=$db->Record["user_login"];
	$pwl=$db->Record["user_pwl"];
	user_auth($login,$pwl,$long_session);
}

function user_auth($login,$pwl,$long_session=0){
	global $db;
	global $user,$mod_rules,$zone_rules,$table_rules,$ex_rules,$row_rules1,$row_rules2,$col_rules,$user,$link_cache;
	unset($link_cache); unset($mod_rules); unset($zone_rules); unset($table_rules); unset($folder_rules); unset($ex_rules); unset($row_rules1); unset($row_rules2); unset($col_rules); unset($user_rules); unset($group_rules); unset($user->group);
	if(empty($su_login)) $su_login=cfg_extract('su_login');
	$login=safe_sql_input($login,1);
	//$pwl=safe_sql_input($pwl);
	if($su_login==$login){
		$u=-1;
	} else {
		if(!empty($login)){
			getrow($db,"SELECT * FROM main_auth WHERE auth_type=0 AND user_login='$login'",1,"main_auth");
			if(!empty($db->Record)){
				if(($db->Record["user_pwlcode"]==0 && $db->Record["user_pwl"]==$pwl)
				||($db->Record["user_pwlcode"]==1 && $db->Record["user_pwl"]==md5(md5($pwl).get_protection_code()))){
					$u=$db->Record["auth_id"];
				}
			}
		}
	}
	if(empty($u)) return 'Пользователь с таким логином не существует или заблокирован';
	$x=auth($login,$pwl,$long_session/*,$skip_password*/);
	if($x==1) load_base($u,1);
	return $x;
}

function user_out(){
	global $db,$user,$mod_rules,$zone_rules,$table_rules,$ex_rules,$row_rules1,$row_rules2,$col_rules,$user,$link_cache;
	if($user->super){
		global $su_safe;
		if(!empty($su_safe)){
			$su_safe='';
			//save_config();
			save_timing();
		}
	} else {
		$db->query("UPDATE main_auth SET user_safe='' WHERE auth_id=".$user->id,3,"main_auth");
	}
	unset($link_cache); unset($mod_rules); unset($zone_rules); unset($table_rules); unset($folder_rules); unset($ex_rules); unset($row_rules1); unset($row_rules2); unset($col_rules); unset($user_rules); unset($group_rules); unset($user->group);	
	//SetCookie("user_hook","");
	//SetCookie("user_login","");
	
	
	/*SetCookie("user_login","",time()-3600 , '/' , '' , 0 , 1);//был как-то уже геморой из-за использования '/'
	SetCookie("user_hook","",time()-3600, '/' , '' , 0, 1 );
	
	SetCookie("z-auth","",time()-3600 , '/' , '' , 0 , 0);//был как-то уже геморой из-за использования '/'
	SetCookie("z-login","",time()-3600 , '/' , '' , 0 , 0 );	*/
	
	unset_cookie('user_login');
	unset_cookie('user_hook');
	unset_cookie('z-auth');
	unset_cookie('z_login');
	
	//SetCookie("user_hook",0,-3600);
	//SetCookie("user_login",0,-3600);
	if(!empty($user->session)){
		global $session_storage;
		if($session_storage==0){
			$db->query("DELETE FROM auth_session WHERE session_id=".$user->session);
		} else {
			$db->query("UPDATE auth_session SET session_active=0 WHERE session_id=".$user->session);
			if($session_storage>0){
				$cnt=getrowval("SELECT count(*) AS cnt FROM auth_session WHERE session_auth=".$user->id." AND session_active=0","cnt");
				if($cnt>=$session_storage) $db->query("DELETE FROM auth_session WHERE session_auth=".$user->id." AND session_active=0 ORDER BY session_last LIMIT ".($cnt-$session_storage));
			}
		}
	}
	$user->id=0;
	$user->super=0;
	$user->session=0;
	unset($user->group2);
	load_base(0,1);
}

function user_remind($user_id){
	global $db,$e5uid;
	if(empty($user_id)) return false;
	getrow($db,"SELECT * FROM main_auth WHERE auth_id=".$user_id,1,'main_auth');
	if(empty($db->Record)) return 'Пользователь не найден';
	if($db->Record["auth_enable"]==1){
		$email=$db->Record["user_email"];
		$res='<div>На Ваш электронный адрес «'.$email.'» была выслана инструкция по восстановлению</div>';
		$url=$GLOBALS["sforward"];
		if(strstr($url,'?')) $url.='&action=change&uid='; else $url.='?action=change&uid=';
		$verify=md5($db->Record["auth_id"].$e5uid.date('d'));
		$safe='';for($i=0;$i<strlen($verify);$i++)if($i%4==0) $safe.=$verify[$i]; $verify=$safe;
		$url.=$db->Record["auth_id"].'&verify='.$verify;
		simple_mail($GLOBALS["system_email"],$email,'Восстановление пароля','
		<p>Здравствуйте, '.$db->Record["user_name"].'!</p>
		<p><b>Если Вы не восстанавливали пароль, игнорируйте это письмо</b></p>
		<p>Для восстановления пароля перейдите по этой ссылке -> <a href="'.$url.'">восстановить пароль</a>. Данная ссылка работает до конца дня</p>
		<p>Внимание! Восстановление пароля приводит к смене текущего пароля, на новый (если у Вас есть соответствующие привелегии, то Вы сможете поменять его в дальнейшем)</p>
		<p>С уважением, команда разработчиков '.$_SERVER["HTTP_HOST2"].'</p>
		','Utf-8');
	}
	else $res='<div>Пользователь заблокирован администрацией</div>';
	return $res;
}

function user_remind_form($strong=10,$send_new_by_email=false){
	global $uid,$e5uid,$verify,$db,$action;
	$content='';
	if(!empty($_GET['verify'])) $verify=$_GET['verify'];
	if(!empty($_GET['action'])) $action=$_GET['action'];
	if(!empty($_GET['uid'])) $uid=$_GET['uid'];
	if(!empty($action) && $action=='change' && !empty($verify)){
		$content.='<h2>Восстановление пароля. Шаг 2.</h2><br>';
		$tmp=md5($uid.$e5uid.date('d'));
		$safe='';for($i=0;$i<strlen($tmp);$i++)if($i%4==0) $safe.=$tmp[$i];
		if($verify!=$safe){
			$content.='<div>Ошибка восстановленя. Повторите операцию</div>';
		} else {
			$new_pwl=strtolower(get_code3($strong));
			getrow($db,"SELECT * FROM main_auth WHERE auth_id=$uid AND auth_type=0 AND auth_enable=1",1,"main_auth");
			if(empty($db->Record)){
				$content.='<div>К сожалению, ваш пользователь был удалён или заблокирован</div>';
			} else {
				$content.='<div><b>Вам присвоен новый пароль</b></div>';
				$content.='<table id="records" cellpadding="2" cellspacing="3" class="deny_table">
				<tr><td>Логин</td><td>'.$db->Record["user_login"].'</td></tr>
				<tr><td>Новый пароль</td><td>'.$new_pwl.'</td></tr></table>';
				$content.='<div>Храните его в надёжном месте</div><br><div><a href="'.str_replace('action=','xaction=',$GLOBALS["sforward"]).'">Авторизоваться в системе</a></div>';
				if($db->Record["user_pwlcode"]!=0) $new_pwl=md5(md5($new_pwl).get_protection_code());
				$x=$db->Record;
				$db->query("UPDATE main_auth SET user_pwl='$new_pwl' WHERE auth_id=$uid",3,"main_auth");
				if($send_new_by_email && $x['url_pwlcode']==0){
					simple_mail($GLOBALS["system_email"],$x['user_email'],'Восстановление пароля - Новый пароль','
					<p>Здравствуйте, '.$x["user_name"].'!</p>
					<p><b>Если Вы не восстанавливали пароль, игнорируйте это письмо</b></p><br>
					<p>Для Вас создан новый пароль: '.$new_pwl.'</p>
					<p>Ваш логин: '.$x['user_login'].'</p><br>
					<p>Храните его в надёжном месте</p><br><br>
					<p>С уважением, команда разработчиков '.$_SERVER["HTTP_HOST2"].'</p>
					','Utf-8');				
				}
			}
		}
	}
	return $content;
}

function hash_user_pwl($user_pwl){
	return md5(md5($user_pwl).get_protection_code());
}

function seek_mail_count(){
	global $user,$db,$view;
	$gt='';if(!empty($user->group)) foreach($user->group AS $g){	if($gt!='') $gt.=',';	$gt.=$g;}if(!empty($gt)) $gti=" OR mail_to IN (".$gt.")"; else $gti='';
	if(isset($view)) $dw=' AND mail_id!='.$view; else $dw='';
	$ms2=getall($db,"SELECT * FROM main_mail WHERE (mail_to=".$user->id.$gti.")".$dw." AND (mail_read=0 AND !(',".$user->id.",' LIKE mail_read)) ORDER BY mail_date DESC",1,"main_mail");
	return count($ms2);
}

?>