<?php

function seek_user($user){
	global $ucache,$db;
	if(is_object($user) && isset($user->id)){
		$user=$user->id;
	}	
	if(is_object($user) && !isset($user->id)) $user=0;
	if($user>0){
		if(!isset($ucache[$user])){
			getrow($db,"SELECT * FROM main_auth WHERE auth_id=$user",1,'main_auth',__LINE__,__FILE__);
			$ucache[$user]=$db->Record;
		}
		return Array(	'user'=>$ucache[$user],
					'id'=>$user);
	} else if($user==-1){
		$r["auth_id"]=-1;
		$r["user_name"]='super';
		$r["user_email"]='';
		$r["user_login"]='';
		$r["user_pwl"]='';
		$r["user_ip"]='';
		$r["user_lastlogin"]='';
		return Array(	'user'=>$r,
					'id'=>$user);
	} else if($user==0){
		$r["auth_id"]=0;
		$r["user_name"]='guest';
		$r["user_email"]='';
		$r["user_login"]='';
		$r["user_pwl"]='';
		$r["user_ip"]='';
		$r["user_lastlogin"]='';
		return Array(	'user'=>$r,
					'id'=>$user);
	}
}


$cmd['id'][CMD_USER]->process=function(&$data,&$op,&$ops){
		$d=seek_user($data); $uid=$d['id']; $id=$uid; $cuser=$d['user']; global $user, $ucache;
		return $id;
};
$cmd['id'][CMD_USER]->result=CMD_STRING;
$cmd['id'][CMD_USER]->visual=Array('title'=>'Идентификатор');


$cmd['ip'][CMD_USER]->process=function(&$data,&$op,&$ops){
		$d=seek_user($data); $uid=$d['id']; $id=$uid; $cuser=$d['user']; global $user, $ucache;
		if($uid!=$user->id){
			if(check_user(-$id,'view')) return $cuser["user_ip"];
		} else {
			return $_SERVER["REMOTE_ADDR"];
		}
};
$cmd['ip'][CMD_USER]->result=CMD_STRING;
$cmd['ip'][CMD_USER]->visual=Array('title'=>'IP-адрес');


$cmd['browser'][CMD_USER]->process=function(&$data,&$op,&$ops){
		$d=seek_user($data); $uid=$d['id']; $id=$uid; $cuser=$d['user']; global $user, $ucache;
		if($uid!=$user->id) return false;
		return $user->browser;
};
$cmd['browser'][CMD_USER]->result=CMD_STRING;
$cmd['browser'][CMD_USER]->visual=Array('title'=>'Браузер','condition'=>IF_CUR_USER);


$cmd['referer'][CMD_USER]->process=function(&$data,&$op,&$ops){
		$d=seek_user($data); $uid=$d['id']; $id=$uid; $cuser=$d['user']; global $user, $ucache;
		if($uid!=$user->id) return false;
		if(empty($_SERVER["HTTP_REFERER"])) $_SERVER["HTTP_REFERER"]='';
		return $_SERVER["HTTP_REFERER"];
};
$cmd['referer'][CMD_USER]->result=CMD_STRING;
$cmd['referer'][CMD_USER]->visual=Array('title'=>'Страница с которой перешёл пользователь','condition'=>IF_CUR_USER);


$cmd['referer_domain'][CMD_USER]->process=function(&$data,&$op,&$ops){
		$d=seek_user($data); $uid=$d['id']; $id=$uid; $cuser=$d['user']; global $user, $ucache;
		if($uid!=$user->id) return false;
		if(empty($_SERVER["HTTP_REFERER"])) $_SERVER["HTTP_REFERER"]='';
		$x=$_SERVER["HTTP_REFERER"];
		if(substr_count($x,'/')>=3){
			$x=explode('/',$x);
			$x=$x[2];
		} else $x='';
		return $x;
};
$cmd['referer_domain'][CMD_USER]->result=CMD_STRING;
$cmd['referer_domain'][CMD_USER]->visual=Array('title'=>'Сайт с которого перешёл пользователь','condition'=>IF_CUR_USER);


$cmd['just_enter?'][CMD_USER]->process=function(&$data,&$op,&$ops){
		$d=seek_user($data); $uid=$d['id']; $id=$uid; $cuser=$d['user']; global $user, $ucache;
		if($uid!=$user->id) return false;
		if(empty($_SERVER["HTTP_REFERER"])) $_SERVER["HTTP_REFERER"]='';
		$x=strpos($_SERVER["HTTP_REFERER"],$_SERVER["HTTP_HOST"]);
		if($x===false) return true;
		else return false;	
};
$cmd['just_enter?'][CMD_USER]->result=CMD_STRING;
$cmd['just_enter?'][CMD_USER]->visual=Array('title'=>'Это первая страница, открытая пользователем?','condition'=>IF_CUR_USER);


$cmd['country'][CMD_USER]->process=function(&$data,&$op,&$ops){
		$d=seek_user($data); $uid=$d['id']; $id=$uid; $cuser=$d['user']; global $user, $ucache;
		if($uid!=$user->id) return false;
		if(!isset($user->country)) get_country();
		return $user->country;
};
$cmd['country'][CMD_USER]->result=CMD_STRING;
$cmd['country'][CMD_USER]->visual=Array('title'=>'Страна','condition'=>IF_CUR_USER);


$cmd['town'][CMD_USER]->process=function(&$data,&$op,&$ops){
		$d=seek_user($data); $uid=$d['id']; $id=$uid; $cuser=$d['user']; global $user, $ucache;
		if($uid!=$user->id) return false;
		if(!isset($user->town)) get_country();
		return $user->town;
};
$cmd['town'][CMD_USER]->result=CMD_STRING;
$cmd['town'][CMD_USER]->visual=Array('title'=>'Город','condition'=>IF_CUR_USER);


$cmd['type'][CMD_USER]->process=function(&$data,&$op,&$ops){
		$d=seek_user($data); $uid=$d['id']; $id=$uid; $cuser=$d['user']; global $user, $ucache;
		if($id==0) return 'guest';
		else if($id==-1) return 'super';
		else return 'regular';
};
$cmd['type'][CMD_USER]->result=CMD_STRING;
$cmd['type'][CMD_USER]->visual=Array('title'=>'Тип пользователя');


$cmd['super?'][CMD_USER]->process=function(&$data,&$op,&$ops){
		$d=seek_user($data); $uid=$d['id']; $id=$uid; $cuser=$d['user']; global $user, $ucache;
		return ($id==-1);
};
$cmd['super?'][CMD_USER]->result=CMD_STRING;
$cmd['super?'][CMD_USER]->visual=Array('title'=>'Это суперпользователь?');


$cmd['guest?'][CMD_USER]->process=function(&$data,&$op,&$ops){
		$d=seek_user($data); $uid=$d['id']; $id=$uid; $cuser=$d['user']; global $user, $ucache;
		return ($id==0);
};
$cmd['guest?'][CMD_USER]->result=CMD_STRING;
$cmd['guest?'][CMD_USER]->visual=Array('title'=>'Пользователь не авторизован (гость)?');


$cmd['online?'][CMD_USER]->process=function(&$data,&$op,&$ops){
		$d=seek_user($data); $uid=$d['id']; $id=$uid; $cuser=$d['user']; global $user, $ucache;
		if(check_user(-$id,'view')) return $cuser["user_lastlogin"]>$GLOBALS["cur_min"]-5;
};
$cmd['online?'][CMD_USER]->result=CMD_STRING;
$cmd['online?'][CMD_USER]->visual=Array('title'=>'Пользователь online?');


$cmd['last_enter'][CMD_USER]->process=function(&$data,&$op,&$ops){
		$d=seek_user($data); $uid=$d['id']; $id=$uid; $cuser=$d['user']; global $user, $ucache;
		if(check_user(-$id,'view')){
			return date('Y-m-d H:i:s',$cuser["user_lastlogin"]*60);
		}
};
$cmd['last_enter'][CMD_USER]->result=CMD_STRING;
$cmd['last_enter'][CMD_USER]->visual=Array('title'=>'Дата/время последнего входа пользователя','result'=>STRING_DATETIME);


$cmd['name'][CMD_USER]->process=function(&$data,&$op,&$ops){
		$d=seek_user($data); $uid=$d['id']; $id=$uid; $cuser=$d['user']; global $user, $ucache;
		if(isset($op->put)){
			if(check_user(-$id,'edit')){
				global $db;
				$name=parse_var($op->put);
				$val=prepend_value($name);
				$db->query("UPDATE main_auth SET user_name='$val' WHERE auth_id=$id AND auth_type=0",3,"main_auth");
				unset($ucache[$id]);
			}
		} else {
			if(check_user(-$id,'view')) return $cuser["user_name"];
		}
};
$cmd['name'][CMD_USER]->result=CMD_STRING;
$cmd['name'][CMD_USER]->visual=Array('title'=>'Имя');


$cmd['email'][CMD_USER]->process=function(&$data,&$op,&$ops){
		$d=seek_user($data); $uid=$d['id']; $id=$uid; $cuser=$d['user']; global $user, $ucache;
		if(isset($op->put)){
			if(check_user(-$id,'edit')){
				global $db;
				$email=parse_var($op->put);
				$val=prepend_value($email);
				$db->query("UPDATE main_auth SET user_email='$val' WHERE auth_id=$id AND auth_type=0",3,"main_auth");
				unset($ucache[$id]);
			}
		} else {
			if(check_user(-$id,'view')) return $cuser["user_email"];
		}
};
$cmd['email'][CMD_USER]->result=CMD_STRING;
$cmd['email'][CMD_USER]->visual=Array('title'=>'Email');


$cmd['login'][CMD_USER]->process=function(&$data,&$op,&$ops){
		$d=seek_user($data); $uid=$d['id']; $id=$uid; $cuser=$d['user']; global $user, $ucache;
		if(isset($op->put)){
			if(check_user(-$id,'edit')){
				global $db;
				$login=parse_var($op->put);
				$val=prepend_value($login);
				$db->query("UPDATE main_auth SET user_login='$val' WHERE auth_id=$id AND auth_type=0",3,"main_auth");
				unset($ucache[$id]);
			}
		} else {
			if(check_user(-$id,'view')) return $cuser["user_login"];
		}
};
$cmd['login'][CMD_USER]->result=CMD_STRING;
$cmd['login'][CMD_USER]->visual=Array('title'=>'Логин');


$cmd['pwl'][CMD_USER]->process=function(&$data,&$op,&$ops){
		$d=seek_user($data); $uid=$d['id']; $id=$uid; $cuser=$d['user']; global $user, $ucache;
		if(isset($op->put)){
			if(check_user(-$id,'edit')){
				global $db;
				check_single($op->put,$GLOBALS["lex_func_sep"]);
				$pwl=parse_var($op->put[0]);
				$val=prepend_value($pwl);
				if(!empty($op->put[1])){
					$x=parse_var($op->put[1]);
					if(!empty($x)){
						$user_pwlcode=1;
						$val=hash_user_pwl($val);
					} else $user_pwlcode=0;
				} else $user_pwlcode=0;
				$db->query("UPDATE main_auth SET user_pwl='$val', user_pwlcode=$user_pwlcode WHERE auth_id=$id AND auth_type=0",3,"main_auth");
				unset($ucache[$id]);
			}
		} else {
			if(check_user(-$id,'view')) if(empty($cuser["user_pwlcode"])) return $cuser["user_pwl"]; else return '';
		}
};
$cmd['pwl'][CMD_USER]->result=CMD_STRING;
$cmd['pwl'][CMD_USER]->visual=Array('title'=>'Пароль');


$cmd['group'][CMD_USER]->process=function(&$data,&$op,&$ops){
		$d=seek_user($data); $uid=$d['id']; $id=$uid; $cuser=$d['user']; global $user, $ucache;
		global $db;
		$val=parse_var($op->put);
		global $xuser_group;
		if(!isset($xuser_group[$val])){
			getrow($db,"SELECT * FROM main_auth WHERE auth_type=1 AND (group_name='$val' OR group_sname='$val')",1,"main_auth");
			$xuser_group[$val]=$db->Record;
		} else $db->Record=$xuser_group[$val];
		if(empty($db->Record)) return '';
		global $xuser_link;
		$line=$db->Record["auth_id"].'.'.$id;
		if(!isset($xuser_link[$line])){
			getrow($db,"SELECT * FROM auth_link WHERE link_group=".$db->Record["auth_id"]." AND link_user=$id",1,"auth_link");
			$xuser_link[$line]=$db->Record;
		} else $db->Record=$xuser_link[$line];
		if(empty($db->Record)) return '';
		if($db->Record["link_expire"]!='0000-00-00') return $db->Record["link_expire"]; else return 1;		
};
$cmd['group'][CMD_USER]->result=CMD_STRING;
$cmd['group'][CMD_USER]->visual=Array('title'=>'Пользователь присутствует в группе','put'=>Array(
		0=>Array('title'=>'Группа','type'=>STRING_GROUP_NAME,'in_quotes'=>1,'req'=>1)
));


$cmd['invite'][CMD_USER]->process=function(&$data,&$op,&$ops){
		$d=seek_user($data); $uid=$d['id']; $id=$uid; $cuser=$d['user']; global $user, $ucache;
		global $db;
		if($user->super) return false;
		check_single($op->put,$GLOBALS["lex_func_sep"]);
		$group_name=parse_var($op->put[0]);
		$group_id=getrowval("SELECT * FROM main_auth WHERE group_sname='$group_name' AND auth_type=1","auth_id");
		if(empty($group_id)) $group_id=getrowval("SELECT * FROM main_auth WHERE group_name='$group_name' AND auth_type=1","auth_id");
		if(!empty($op->put[1])) $d=parse_var($op->put[1]); else $d='';
		if(!empty($group_id)){
			invite_user($group_id,$id,($user->id==$id),0,$d);
		}
};
$cmd['invite'][CMD_USER]->result=CMD_NONE;
$cmd['invite'][CMD_USER]->visual=Array('title'=>'Пригласить в группу','put'=>Array(
		0=>Array('title'=>'Группа','type'=>STRING_GROUP_NAME,'in_quotes'=>1,'req'=>1),
		1=>Array('title'=>'Ограничить членство до','type'=>CMD_STRING,'default'=>'0000-00-00','req'=>0)
));


$cmd['leave'][CMD_USER]->process=function(&$data,&$op,&$ops){
		$d=seek_user($data); $uid=$d['id']; $id=$uid; $cuser=$d['user']; global $user, $ucache;
		$group_name=parse_var($op->put);
		$group_id=getrowval("SELECT * FROM main_auth WHERE group_sname='$group_name' AND auth_type=1","auth_id");
		if(empty($group_id)) $group_id=getrowval("SELECT * FROM main_auth WHERE group_name='$group_name' AND auth_type=1","auth_id");
		if(!empty($group_id)) leave_user($group_id,$id,($id==$user->id));
};
$cmd['leave'][CMD_USER]->result=CMD_NONE;
$cmd['leave'][CMD_USER]->visual=Array('title'=>'Исключить из группы','put'=>Array(
		0=>Array('title'=>'Группа','type'=>STRING_GROUP_NAME,'in_quotes'=>1,'req'=>1)
));


$cmd['logout'][CMD_USER]->process=function(&$data,&$op,&$ops){
		$d=seek_user($data); $uid=$d['id']; $id=$uid; $cuser=$d['user']; global $user, $ucache;
		user_out();
};
$cmd['logout'][CMD_USER]->result=CMD_NONE;
$cmd['logout'][CMD_USER]->visual=Array('title'=>'Выход','condition'=>IF_CUR_USER);


$cmd['auth'][CMD_USER]->process=function(&$data,&$op,&$ops){
		$d=seek_user($data); $uid=$d['id']; $id=$uid; $cuser=$d['user']; global $user, $ucache;
		check_single($op->put,$GLOBALS["lex_func_sep"]);
		if(count($op->put)==1){
			if(check_user(-$uid,'edit')){
				return user_force_auth($uid);
			} else return false;
		} else {
			$login=parse_var($op->put[0]);
			$pwl=parse_var($op->put[1]);
			return user_auth($login,$pwl);
		}
};
$cmd['auth'][CMD_USER]->result=CMD_STRING;
$cmd['auth'][CMD_USER]->visual=Array('title'=>'Авторизация','put'=>Array(
		0=>Array('title'=>'Логин','type'=>CMD_STRING,'req'=>0),
		1=>Array('title'=>'Пароль','type'=>CMD_STRING,'req'=>0)
));


$cmd['remind'][CMD_USER]->process=function(&$data,&$op,&$ops){
		$d=seek_user($data); $uid=$d['id']; $id=$uid; $cuser=$d['user']; global $user, $ucache;
		return user_remind($uid);
};
$cmd['remind'][CMD_USER]->result=CMD_STRING;
$cmd['remind'][CMD_USER]->visual=Array('title'=>'Инициализировать процедуру восстановления пароля');


$cmd['reg'][CMD_USER]->process=function(&$data,&$op,&$ops){
		$d=seek_user($data); $uid=$d['id']; $id=$uid; $cuser=$d['user']; global $user, $ucache;
		check_single($op->put,$GLOBALS["lex_func_sep"]);
		$login=parse_var($op->put[0]);
		$pwl=parse_var($op->put[1]);
		$auth=0;
		$owners=Array();
		$group='';
		if(!empty($op->put[2])) $name=parse_var($op->put[2]);
		if(!empty($op->put[3])) $email=parse_var($op->put[3]);
		if(!empty($op->put[4])) $auth=parse_var($op->put[4]);
		if(!empty($op->put[5])) $group=parse_var($op->put[5]);
		if(!empty($op->put[6])) $owners=parse_var($op->put[6]);
		return user_reg($login,$pwl,$name,$email,$group,'',$auth,0,'',0,0,$owners,($op->data=='register'));
};
$cmd['reg'][CMD_USER]->result=CMD_MIXED;
$cmd['register'][CMD_USER]->alias=&$cmd['reg'][CMD_USER];
$cmd['reg'][CMD_USER]->visual=Array('title'=>'Зарегистрировать пользователя','condition'=>IF_CUR_USER,'put'=>Array(
		0=>Array('title'=>'Логин','type'=>CMD_STRING,'req'=>1),
		1=>Array('title'=>'Пароль','type'=>CMD_STRING,'req'=>1),
		2=>Array('title'=>'Имя','type'=>CMD_STRING,'req'=>0),
		3=>Array('title'=>'Email','type'=>CMD_STRING,'req'=>0),
		4=>Array('title'=>'Авторизация после регистрации','type'=>CMD_LOGICAL,'default'=>'0','req'=>0),
		5=>Array('title'=>'Вступление в группу после регистрации','type'=>STRING_GROUP_NAME,'in_quotes'=>1,'req'=>0),
		6=>Array('title'=>'Набор владельцев','type'=>CMD_ARRAY,'req'=>0,'hidden'=>1)
));


$cmd['check'][CMD_USER]->process=function(&$data,&$op,&$ops){
		$d=seek_user($data); $uid=$d['id']; $id=$uid; $cuser=$d['user']; global $user, $ucache;
		check_single($op->put,$GLOBALS["lex_func_sep"]);
		$action=parse_var($op->put[0]);
		if(!empty($op->put[1])) $count=parse_var($op->put[1]); else $count=1;
		if(!empty($op->put[2])) $hours=parse_var($op->put[2]); else $hours=0;
		return check_visit_ip(2,0,$uid,$action,$count,$hours); //ZONE подставляется автоматически в самой функции
};
$cmd['check'][CMD_USER]->result=CMD_STRING;
$cmd['check'][CMD_USER]->visual=Array('title'=>'Доступность действия для пользователя','put'=>Array(
		0=>Array('title'=>'название действия','type'=>CMD_STRING,'req'=>1),
		1=>Array('title'=>'макс.кол-во действий','type'=>CMD_STRING,'req'=>1),
		2=>Array('title'=>'период отслеживания (в часах, "!в минутах"), 0 - за весь период сбора статистики','type'=>CMD_STRING,'req'=>0)
));		


$cmd['auth_long'][CMD_USER]->process=function(&$data,&$op,&$ops){
		$d=seek_user($data); $uid=$d['id']; $id=$uid; $cuser=$d['user']; global $user, $ucache;
		if(check_user(-$uid,'edit')){
			return user_force_auth($uid,1);
		} else return false;
};
$cmd['auth_long'][CMD_USER]->result=CMD_STRING;
$cmd['auth_long'][CMD_USER]->visual=Array('title'=>'Долгая авторизация');


$cmd[''][CMD_USER]->process=function(&$data,&$op,&$ops){
		$d=seek_user($data); $uid=$d['id']; $id=$uid; $cuser=$d['user']; global $user, $ucache;
		global $tables2,$cur_module,$cur_ex, $db;
		$var[0]=$op->data;
		if(!empty($var[0]) && $var[0][0]=='_'){
			$iowner=true;
			$var[0]=substr($var[0],1);
		} else $iowner=false;
		if(isset($op->put)){
			$col_sname=parse_var($op->put);
		} else $col_sname='';
		if(isset($tables2[$cur_module][$var[0]])) $st=&$tables2[$cur_module][$var[0]];
		else {
			foreach($tables2 AS $mod=>$tbls){
				foreach($tbls AS $tbl_sname=>$tbl) if($tbl_sname==$var[0]){
					$st=&$tbl;
					break;
				}
				if(isset($st)) break;
			}
		}
		if(!isset($st)){
			$tmp=getrowval("SELECT table_id FROM main_table WHERE table_sname='".$var[0]."'",'table_id');
			if(!empty($tmp)) $st->id=$tmp;
		}
		if($uid==0) return $rows;
		$rows=Array();
		if(isset($st)){
			$table_id=$st->id;
			if($col_sname!='id'){
				$ucol=seek_usercols($table_id,$col_sname);
				$ucols=implode($ucol);
				$ucol=current($ucol);
				if(empty($ucol)) return Array();
				if($cur_ex!=0) $tex=get_tex(0,$cur_ex,$table_id); else $tex=0;
				$seek_ex=1;
				if($cur_ex==0) $seek_ex=0;
				if($iowner){
					//$x=getall($db,"SELECT * FROM row_owner WHERE row_id IN (SELECT value_row FROM row_value WHERE  value_col=".$ucol["col_id"]." AND value_value='".$uid."')");
					//вложенные запросы работают медленней
					
					global $user_irows_cache;
					if(!isset($user_irows_cache[$ucol][$uid])){
						$x=implode(',',getall3($db,"SELECT value_row FROM row_value WHERE  value_col=".$ucol." AND value_value='".$uid."'","value_row"));
						if(!empty($x)){
							$x=getall($db,"SELECT * FROM row_owner WHERE row_id IN (".$x.") AND ro_enable=1");
							$rows=db_to_rows($x);
						}
						$user_irows_cache[$ucol][$uid]=$rows;
					} else $rows=$user_irows_cache[$ucol][$uid];
					
					/*$x=implode(',',getall3($db,"SELECT value_row FROM row_value WHERE  value_col IN (".$ucols.") AND value_value='".$uid."'","value_row"));
					if(!empty($x)){
						$x=getall($db,"SELECT * FROM row_owner WHERE row_id IN (".$x.") AND ro_enable=1");
						$rows=db_to_rows($x);
					}*/
				} else {
					$rows=get_sub(0,$table_id,1,$seek_ex,0,0,0,$tex,$cur_ex,$table_id,0,0,/*1*/0,Array(),0);
					$x=collect_rows($rows); $x=implode(',',$x);
					$filter=Array();
					if(!empty($x)){
						global $xuser_cache;
						$line=$x.'.'.$ucol.'.'.$uid;	
						if(empty($ucol)) return Array();
						if(!isset($xuser_cache[$line])) $xuser_cache[$line]=getall($db,"SELECT * FROM row_value WHERE value_row IN ($x) AND value_col IN (".$ucols.") AND value_value=".$uid,1,"row_value",__LINE__,__FILE__);
						$xs=$xuser_cache[$line];
						if(empty($xs)) return Array();
						foreach($xs AS $x) $filter[$x["value_row"]]=$x["value_row"];
						$rows=filter_rows($rows,$filter);
					}					
				}
			} else {
				if($iowner){
					if(!empty($cur_ex))$exa=' AND row_ex='.$cur_ex;
					else $exa='';
					//$x=getall($db,"SELECT * FROM row_owner WHERE row_id IN (SELECT row_id FROM main_row WHERE row_table=".$table_id.$exa." AND row_user=".$uid.")");
					$x=implode(',',getall3($db,"SELECT row_id FROM main_row WHERE row_table=".$table_id.$exa." AND row_user=".$uid." AND row_enable=1","row_id"));
					if(!empty($x)){
						$x=getall($db,"SELECT * FROM row_owner WHERE row_id IN (".$x.") AND ro_enable=1");
						$rows=db_to_rows($x);
					}
				} else {
					// эту конструкцию надо заменить
					//$rows=get_sub(0,$table_id,1,1,0,0,0,get_ex(),$cur_ex,$table_id,0,0,/*1*/0,Array(),$uid);
					// т.к. она долго грузится и весит много памяти
					
					// например на эту
					if(!empty($cur_ex))$exa=' AND ro_ex='.$cur_ex;
					else $exa='';
					$x=getall3($db,"SELECT * FROM row_owner WHERE row_table=".$table_id.$exa." AND ro_user=".$uid." AND ro_enable=1 AND owner_id=0 AND ro_sub=0","row_id");
					if(!empty($x)) $rows=db_to_rows($x);
					
				}
			}
			return $rows;
		}
		return false;
};
$cmd[''][CMD_USER]->result=CMD_MIXED;
$cmd[''][CMD_USER]->visual=Array('title'=>'Связанные объекты','special'=>SP_POINT,'anyway_include'=>1,'put'=>Array(
		0=>Array('title'=>'Поиск в подтаблицах','type'=>CMD_LOGICAL,'special'=>'_','req'=>0),
		1=>Array('title'=>'Таблица','type'=>STRING_TABLE_NAME,'req'=>1),
		2=>Array('title'=>'Поле','type'=>STRING_COLNAME_ANY,'in_quotes'=>1,'req'=>0)
));	

?>